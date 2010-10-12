#ifndef _COBJECTMGR_H
#define _COBJECTMGR_H

#include <Common.h>

class cObjectMgr
{
	public:
		void Initialize();
		uint64 getNewBGId();
		uint64 getNewPlayerId();
		uint32 GenerateInstanceId();
	private:
		uint64 m_bgId;
		uint64 m_plId;

		uint32 m_instanceId;
};

#define sClusterObjectMgr MaNGOS::Singleton<cObjectMgr>::Instance()

#endif