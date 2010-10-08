#include <Policies/SingletonImp.h>
#include <Log.h>
#include <ObjectDefines.h>
#include <cPacketOpcodes.h>
#include "cBattleGroundMgr.h"
#include "cObjectMgr.h"
#include "cBattleGround.h"
#include "ClusterSession.h"

INSTANTIATE_SINGLETON_1(cBattleGroundMgr);

uint64 cBattleGroundMgr::CreateBattleGround()
{
	cBattleGround* cBG = new cBattleGround();
	uint64 newId = sClusterObjectMgr.getNewBGId();
	cBG->setId(newId);
	m_BGMap[newId] = cBG;
	sLog.outBasic("Create new BattleGround with id %u",GUID_LOPART(newId));
	return newId;
}

cBattleGround* cBattleGroundMgr::getBattleGround(uint64 id)
{
	cBattleGroundMap::iterator itr = m_BGMap.find(id);
	if(itr == m_BGMap.end())
		return NULL;

	return itr->second;
}

void cBattleGroundMgr::DropBattleGround(uint64 id)
{
	cBattleGround* bg = getBattleGround(id);
	if(!bg)
		return;
	m_BGMap[id] = NULL;
	delete bg;
}

void ClusterSession::Handle_GenerateBGId(WorldPacket &pck)
{
	uint64 id = sClusterBGMgr.CreateBattleGround();
	Packet pkt;
	pkt << uint16(C_SMSG_GET_UINT64) << uint64(id);
	SendPacket(&pkt);
}

void ClusterSession::Handle_GetRewardPlayers(WorldPacket &pck)
{
	uint64 id;
	pck >> id;
	
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(id);
	if(!cBG)
	{
		Packet pkt;
		pkt << uint16(C_SMSG_GET_V_UINT64);
		pkt << uint32(0);
		SendPacket(&pkt);
		return;
	}

	Packet pkt;
	pkt << uint16(C_SMSG_GET_V_UINT64);
	std::vector<uint64> players = cBG->getPlayerList();
	pkt << uint32(players.size());
	for(std::vector<uint64>::iterator itr = players.begin(); itr != players.end(); ++itr)
		pkt << uint64(*itr);

	SendPacket(&pkt);
}

void ClusterSession::Handle_IsInBG(WorldPacket &pck)
{
	uint64 bgId,plGuid;
	pck >> bgId >> plGuid;

	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendBoolPacket(false);
		return;
	}

	bool inBG = cBG->IsPlayerInBattleGround(plGuid);
	Packet pkt;
	pkt << uint16(C_SMSG_GET_BOOL) << uint8(inBG);
	SendPacket(&pkt);
}

void ClusterSession::Handle_GetBgTeam(WorldPacket &pck)
{
	uint64 bgId,plGuid;
	pck >> bgId >> plGuid;

	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendUint32(0);
		return;
	}
	uint32 team = cBG->GetPlayerTeam(plGuid);
	SendUint32(team);
}

void ClusterSession::Handle_Updt_Plr(WorldPacket &pck)
{
	uint64 bgId,plGuid;
	pck >> bgId >> plGuid;

	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}

	uint32 off,team;
	pck >> off >> team;
	cBG->SetPlayerValues(plGuid,off,team);

	SendNullPacket();
}

void ClusterSession::Handle_GetBGCommand(WorldPacket &pck)
{
	uint64 id;
	pck >> id;
	std::string command;
	pck >> command;

	cBattleGround* cBG = sClusterBGMgr.getBattleGround(id);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}

	if(command == "Reset")
		cBG->Reset();
	else if(command == "Drop")
		sClusterBGMgr.DropBattleGround(id);

	SendNullPacket();
}

void ClusterSession::Handle_BGGetOfflineTime(WorldPacket &pck)
{
	uint64 bgId, plGuid;
	pck >> bgId >> plGuid;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendUint32(0);
		return;
	}

	uint32 offTime = cBG->GetPlayerOfflineTime(plGuid);
	SendUint32(offTime);
}

void ClusterSession::Handle_BGGetPlayerNumberByTeam(WorldPacket &pck)
{
	uint64 bgId, plGuid;
	pck >> bgId >> plGuid;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendUint32(0);
		return;
	}
	uint32 team;
	pck >> team;
	uint32 pCount = cBG->GetPlayersCountByTeam(team);
	SendUint32(pCount);
}

void ClusterSession::Handle_BGRemovePlayerAtLeave(WorldPacket &pck)
{
	uint64 bgId, plGuid;
	pck >> bgId >> plGuid;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}
	cBG->RemovePlayerAtLeave(plGuid,false,false);
	SendNullPacket();
}

void ClusterSession::Handle_BGSetArenaTeam(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}
	uint32 team,arenaTeamId;
	pck >> team;
	pck >> arenaTeamId;
	cBG->SetArenaTeamIdForTeam(team,arenaTeamId);
	SendNullPacket();
}

void ClusterSession::Handle_BGSetArenaTeamRatingChange(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}
	uint32 team,rChange;
	pck >> team;
	pck >> rChange;
	cBG->SetArenaTeamRatingChangeForTeam(team,rChange);
	SendNullPacket();
}

void ClusterSession::Handle_BGGetArenaTeam(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendUint32(0);
		return;
	}
	uint32 team;
	pck >> team;
	SendUint32(cBG->GetArenaTeamIdForTeam(team));
}

void ClusterSession::Handle_BGGetArenaTeamRatingChange(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendUint32(0);
		return;
	}
	uint32 team;
	pck >> team;
	SendUint32(cBG->GetArenaTeamRatingChangeForTeam(team));
}

void ClusterSession::Handle_BGHasFreeSlots(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendBoolPacket(false);
		return;
	}
	SendBoolPacket(cBG->HasFreeSlots());
}
void ClusterSession::Handle_BGGetLimit(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendUint32(0);
		return;
	}
	uint8 limit;
	pck >> limit;
	uint32 res = 0;
	switch(limit)
	{
		case 0:
			res = cBG->GetMaxPlayers();
			break;
		case 1:
			res = cBG->GetMinPlayers();
			break;
		case 2:
			res = cBG->GetMinLevel();
			break;
		case 3:
			res = cBG->GetMaxLevel();
			break;
		case 4:
			res = cBG->GetMaxPlayersPerTeam();
			break;
		case 5:
			res = cBG->GetMinPlayersPerTeam();
			break;
	}
	SendUint32(res);
}

void ClusterSession::Handle_BGSetLimit(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}
	uint8 limit;
	pck >> limit;
	uint32 value;
	pck >> value;
	switch(limit)
	{
		case 0:
			res = cBG->SetMaxPlayers(value);
			break;
		case 1:
			res = cBG->SetMinPlayers(value);
			break;
		case 2:
			res = cBG->SetLevelRange(value,cBG->GetMaxLevel());
			break;
		case 3:
			res = cBG->SetLevelRange(cBG->GetMinLevel(),value);
			break;
		case 4:
			res = cBG->GetMaxPlayersPerTeam(value);
			break;
		case 5:
			res = cBG->GetMinPlayersPerTeam(value);
			break;
	}
	SendNullPacket();
}