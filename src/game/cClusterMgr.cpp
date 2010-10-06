#include <Policies/SingletonImp.h>
#include "cClusterMgr.h"

INSTANTIATE_SINGLETON_1( ClusterMgr );

void ClusterMgr::RegisterCluster(cSocketTCP *sock, ClusterType _type)
{
	if(!sock || _type == C_NULL)
		return;

	m_clustermap[_type] = sock;
}

cSocketTCP* ClusterMgr::getCluster(ClusterType _type)
{
	ClusterMap::iterator itr = m_clustermap.find(_type);
	if(itr == m_clustermap.end())
		return NULL;

	return itr->second;
}