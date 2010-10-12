#include <Policies/SingletonImp.h>
#include "cObjectMgr.h"

INSTANTIATE_SINGLETON_1(cObjectMgr);

void cObjectMgr::Initialize()
{
	m_bgId = 0;
	m_plId = 0;
	m_instanceId = 150000;
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

uint32 cObjectMgr::GenerateInstanceId()
{
	m_instanceId++;
	return m_instanceId;
}