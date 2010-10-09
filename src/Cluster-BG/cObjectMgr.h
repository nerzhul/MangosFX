#ifndef _COBJECTMGR_H
#define _COBJECTMGR_H

#include <Common.h>

class cObjectMgr
{
	public:
		void Initialize();
		uint64 getNewBGId();
		uint64 getNewPlayerId();
	private:
		uint64 m_bgId;
		uint64 m_plId;
};

#define sClusterObjectMgr MaNGOS::Singleton<cObjectMgr>::Instance()

#endif