#include <Policies/SingletonImp.h>
#include "cSocketTCP.h"
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

uint64 ClusterMgr::getUint64Value(const sf::Packet *pck, ClusterType _type)
{
	if(_type == C_ALL || _type == C_NULL)
		return 0;

	cRPCCommandHandler* rpc = new cRPCCommandHandler(_type);
	Packet* resp = rpc->getResponse(&pck);

	if(!resp)
	{
		delete rpc;
		return 0;
	}

	if(resp->GetDataSize() != (sizeof(uint16) + sizeof(uint64))
	{
		error_log("Packet error on uint64 RPC");
		delete rpc;
		return 0;
	}
	
	uint16 opcode;
	uint64 value;
	*resp >> opcode >> value;

	delete rpc;
	return value;
}