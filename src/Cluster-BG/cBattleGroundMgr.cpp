#include <Policies/SingletonImp.h>
#include <Log.h>
#include <ObjectDefines.h>
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
	pkt << uint16(C_SMSG_GET_BG_ID) << uint64(id);
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

	pkt << uint16(C_SMSG_GET_BG_REW_PLAYERS);

	std::vector<uint64> players = cBG->getPlayerList();
	pkt << uint32(players.size());
	for(std::vector<uint64>::iterator itr = players.begin(); itr != players.end(); ++itr)
		pkt << uint64(*itr);

	SendPacket(&pkt);
}

void ClusterSession::Handle_GetBGCommand(WorldPacket &pck)
{
	uint64 id;
	pck >> id;
	std::string command;
	pck >> command;
}