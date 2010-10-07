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
		return;

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
		return;

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
		return;
	uint32 team = cBG->GetPlayerTeam(plGuid);

	Packet pkt;
	pkt << uint16(C_SMSG_GET_UINT32) << uint32(team);
	SendPacket(&pkt);
}

void ClusterSession::Handle_Updt_Plr(WorldPacket &pck)
{
	uint64 bgId,plGuid;
	pck >> bgId >> plGuid;

	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
		return;

	uint32 off,team;
	pck >> off >> team;
	cBG->SetPlayerValues(plGuid,off,team);
	Packet pkt;
	pck << uint16(C_CMSG_NULL);
	SendPacket(&pkt);
}

void ClusterSession::Handle_GetBGCommand(WorldPacket &pck)
{
	uint64 id;
	pck >> id;
	std::string command;
	pck >> command;
}

void ClusterSession::Handle_BGGetOfflineTime(WorldPacket &pck)
{
	uint64 bgId, plGuid;
	cBattleGround* cBG = sClusterBGMgr.getBattleGround(bgId);
	if(!cBG)
		return;

	uint32 offTime = cBG->GetPlayerOfflineTime(plGuid);
	Packet pkt;
	pkt << uint16(C_SMSG_GET_UINT32) << uint32(offTime);
	SendPacket(&pkt);

}