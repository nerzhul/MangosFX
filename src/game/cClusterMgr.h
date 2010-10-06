#ifndef __C_CLUSTER_MGR_H_
#define __C_CLUSTER_MGR_H_

#include <Policies/Singleton.h>
#include "cIncludes.h"
#include "cSocketTCP.h"

typedef std::map<ClusterType,cSocketTCP*> ClusterMap;

class ClusterMgr
{
	public:
		void RegisterCluster(cSocketTCP* sock, ClusterType _type);
		cSocketTCP* getCluster(ClusterType _type);
	private:
		ClusterMap m_clustermap;
};

#define sClusterMgr MaNGOS::Singleton<ClusterMgr>::Instance()

#endif