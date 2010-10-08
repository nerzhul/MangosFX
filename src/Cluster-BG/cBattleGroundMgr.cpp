#include <Policies/SingletonImp.h>
#include <Log.h>
#include <ObjectDefines.h>
#include <cPacketOpcodes.h>
#include <Opcodes.h>
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

void cBattleGroundMgr::BuildPlaySoundPacket(WorldPacket *data, uint32 soundid)
{
    data->Initialize(SMSG_PLAY_SOUND, 4);
    *data << uint32(soundid);
}

void cBattleGroundMgr::BuildUpdateWorldStatePacket(WorldPacket *data, uint32 field, uint32 value)
{
    data->Initialize(SMSG_UPDATE_WORLD_STATE, 4+4);
    *data << uint32(field);
    *data << uint32(value);
}

void cBattleGroundMgr::BuildPlayerLeftBattleGroundPacket(WorldPacket *data, const uint64& guid)
{
    data->Initialize(SMSG_BATTLEGROUND_PLAYER_LEFT, 8);
    *data << uint64(guid);
}

BattleGroundQueueTypeId cBattleGroundMgr::BGQueueTypeId(BattleGroundTypeId bgTypeId, uint8 arenaType)
{
    switch(bgTypeId)
    {
        case BATTLEGROUND_WS:
            return BATTLEGROUND_QUEUE_WS;
        case BATTLEGROUND_AB:
            return BATTLEGROUND_QUEUE_AB;
        case BATTLEGROUND_AV:
            return BATTLEGROUND_QUEUE_AV;
        case BATTLEGROUND_EY:
            return BATTLEGROUND_QUEUE_EY;
        case BATTLEGROUND_SA:
            return BATTLEGROUND_QUEUE_SA;
        case BATTLEGROUND_IC:
            return BATTLEGROUND_QUEUE_IC;
        case BATTLEGROUND_RB:
            return BATTLEGROUND_QUEUE_RANDOM;
        case BATTLEGROUND_AA:
        case BATTLEGROUND_NA:
        case BATTLEGROUND_RL:
        case BATTLEGROUND_BE:
        case BATTLEGROUND_DS:
        case BATTLEGROUND_RV:
            switch(arenaType)
            {
                case ARENA_TYPE_2v2:
                    return BATTLEGROUND_QUEUE_2v2;
                case ARENA_TYPE_3v3:
                    return BATTLEGROUND_QUEUE_3v3;
                case ARENA_TYPE_5v5:
                    return BATTLEGROUND_QUEUE_5v5;
                default:
                    return BATTLEGROUND_QUEUE_NONE;
            }
        default:
            return BATTLEGROUND_QUEUE_NONE;
    }
}

void cBattleGroundMgr::Update(uint32 diff)
{
}

// Packets

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
		SendInt32(0);
		return;
	}
	uint32 team;
	pck >> team;
	SendInt32(cBG->GetArenaTeamRatingChangeForTeam(team));
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

void ClusterSession::Handle_BGGetTeamStartLoc(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendFloat(0);
		return;
	}
	uint8 pos;
	uint32 team;
	pck >> pos;
	pck >> team;
	float x,y,z,o, toSend=0;
	cBG->GetTeamStartLoc(team,x,y,z,o);
	switch(pos)
	{
		case 0:
			toSend = x;
			break;
		case 1:
			toSend = y;
			break;
		case 2:
			toSend = z;
			break;
		case 3:
			toSend = o;
			break;
	}
	SendFloat(toSend);
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
			cBG->SetMaxPlayers(value);
			break;
		case 1:
			cBG->SetMinPlayers(value);
			break;
		case 2:
			cBG->SetLevelRange(value,cBG->GetMaxLevel());
			break;
		case 3:
			cBG->SetLevelRange(cBG->GetMinLevel(),value);
			break;
		case 4:
			cBG->SetMaxPlayersPerTeam(value);
			break;
		case 5:
			cBG->SetMinPlayersPerTeam(value);
			break;
	}
	SendNullPacket();
}

void ClusterSession::Handle_BGSetTeamStartLoc(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}
	uint32 team;
	float x,y,z,o;
	pck >> team >> x >> y >> z >> o;
	cBG->SetTeamStartLoc(team,x,y,z,o);

	SendNullPacket();
}

void ClusterSession::Handle_BGGetTypeId(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendUint32(0);
		return;
	}
	uint8 random;
	pck >> random;
	SendUint32(cBG->GetTypeID(random > 0 ? true : false));
}

void ClusterSession::Handle_BGSetTypeId(WorldPacket &pck)
{
	uint64 bgId;
	pck >> bgId;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
	{
		SendNullPacket();
		return;
	}
	uint8 data;
	uint32 typeId;
	pck >> data >> typeId;
	if(data == 0)
		cBG->SetTypeID(BattleGroundTypeId(typeId));
	else
		cBG->SetRandomTypeID(BattleGroundTypeId(typeId));
}