#include <Policies/SingletonImp.h>
#include "cObjectMgr.h"

INSTANTIATE_SINGLETON_1(cObjectMgr);

void cObjectMgr::Initialize()
{
	m_bgId = 0;
	m_plId = 0;
}

uint64 cObjectMgr::getNewBGId()
{
	m_bgId++;
	return m_bgId;
}

uint64 cObjectMgr::getNewPlayerId()
{
	m_plId++;
	return m_plId;
}