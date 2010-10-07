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

void ClusterSession::Handle_BG_m_Players_mod(WorldPacket &pck)
{
	uint64 bgId, plGuid, time;
	uint32 team;
	uint8 del;
	pck >> bgId;
	pck >> plGuid;
	pck >> time;
	pck >> team;
	pck >> del;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
		return;
	// @TODO : modify datas on BG
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
		SendNullPacket();
		return;
	}
	Packet pkt;

	pkt << uint16(C_SMSG_GET_V_UINT64);
	std::vector<uint64> players = cBG->getPlayerList();
	error_log("size %u",players.size());
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