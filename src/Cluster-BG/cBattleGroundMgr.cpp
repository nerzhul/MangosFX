#include <Policies/SingletonImp.h>
#include "cBattleGroundMgr.h"
#include "cObjectMgr.h"
#include "cBattleGround.h"

INSTANTIATE_SINGLETON_1(cBattleGroundMgr);

void cBattleGroundMgr::CreateBattleGround()
{
	cBattleGround* cBG = new cBattleGround();
	cBG->setId(sClusterObjectMgr.getNewBGId());
	m_BGMap[cBG->getId()] = cBG;
}