#ifndef _CLUSTERLOOT_H
#define _CLUSTERLOOT_H

#include "Common.h"
#include "Policies/Singleton.h"

class ClusterLoot
{
	public:
		ClusterLoot();
		~ClusterLoot();
};
#define sClusterLoot MaNGOS::Singleton<ClusterLoot>::Instance()
#endif