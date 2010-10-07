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
	Packet* resp = rpc->getResponse(pck);

	if(!resp)
	{
		delete rpc;
		return 0;
	}
	uint64 value = readUint64(*resp,2);
	//delete rpc; crashfix
	delete resp;
	return value;
}

uint32 ClusterMgr::getUint32Value(const sf::Packet *pck, ClusterType _type)
{
	if(_type == C_ALL || _type == C_NULL)
		return 0;

	cRPCCommandHandler* rpc = new cRPCCommandHandler(_type);
	Packet* resp = rpc->getResponse(pck);

	if(!resp)
	{
		delete rpc;
		return 0;
	}

	uint32 value = 0;
	value += uint8(resp->GetData()[5]);
	value += uint8(resp->GetData()[4])*256;
	value += uint8(resp->GetData()[3])*65536;
	value += uint8(resp->GetData()[2])*16777216;

	delete rpc;
	delete resp;
	return value;
}

void ClusterMgr::getNullValue(const sf::Packet *pck, ClusterType _type)
{
	if(_type == C_ALL || _type == C_NULL)
		return;

	cRPCCommandHandler* rpc = new cRPCCommandHandler(_type);
	rpc->getResponse(pck);
	delete rpc;
}

bool ClusterMgr::getBoolValue(const sf::Packet *pck, ClusterType _type)
{
	if(_type == C_ALL || _type == C_NULL)
		return false;

	cRPCCommandHandler* rpc = new cRPCCommandHandler(_type);
	Packet* resp = rpc->getResponse(pck);

	if(!resp)
	{
		delete rpc;
		return false;
	}
	
	uint8 value = 0;
	value += uint8(resp->GetData()[3]);

	delete rpc;
	delete resp;
	return (value != 0) ? true : false;
}


std::vector<uint64> ClusterMgr::getUint64Vector(const sf::Packet *pck, ClusterType _type)
{
	std::vector<uint64> vValues;
	vValues.clear();

	if(_type == C_ALL || _type == C_NULL)
		return vValues;

	cRPCCommandHandler* rpc = new cRPCCommandHandler(_type);
	Packet* resp = rpc->getResponse(pck);

	if(!resp)
	{
		delete rpc;
		return vValues;
	}
	
	uint32 vSize = 0;
	vSize += uint8(resp->GetData()[5]);
	vSize += uint8(resp->GetData()[4])*256;
	vSize += uint8(resp->GetData()[3])*65536;
	vSize += uint8(resp->GetData()[2])*16777216;

	uint32 pos=6;
	for(uint32 i=0;i<vSize;i++)
	{
		uint32 tmp = readUint64(*resp,pos);
		pos+=8;
		vValues.push_back(tmp);
	}
	delete rpc;
	delete resp;
	return vValues;
}

void ClusterMgr::sendCommand(const sf::Packet *pck, ClusterType _type)
{
	if(_type == C_ALL || _type == C_NULL)
		return;

	cRPCCommandHandler* rpc = new cRPCCommandHandler(_type);
	rpc->getResponse(pck);

	delete rpc;
}