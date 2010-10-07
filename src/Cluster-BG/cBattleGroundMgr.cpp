#include <Policies/SingletonImp.h>
#include <Log.h>
#include <ObjectDefines.h>
#include "cBattleGroundMgr.h"
#include "cObjectMgr.h"
#include "cBattleGround.h"
#include "ClusterSession.h"

INSTANTIATE_SINGLETON_1(cBattleGroundMgr);

void cBattleGroundMgr::CreateBattleGround()
{
	cBattleGround* cBG = new cBattleGround();
	cBG->setId(sClusterObjectMgr.getNewBGId());
	m_BGMap[cBG->getId()] = cBG;
	sLog.outBasic("Create new BattleGround with id %u",GUID_LOPART(cBG->getId()));
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