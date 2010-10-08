#ifndef __C_CLUSTER_MGR_H_
#define __C_CLUSTER_MGR_H_

#include <Policies/Singleton.h>
#include "cIncludes.h"
#include "cSocketTCP.h"

typedef std::map<ClusterType,cSocketTCP*> ClusterMap;

class ClusterMgr
{
	public:
		ClusterMgr() { m_clustermap.clear(); }
		~ClusterMgr() {};
		void RegisterCluster(cSocketTCP* sock, ClusterType _type);
		cSocketTCP* getCluster(ClusterType _type);

		void getNullValue(const Packet* pck, ClusterType _type);
		bool getBoolValue(const Packet* pck, ClusterType _type);
		int32 getInt32Value(const Packet* pck, ClusterType _type);
		uint32 getUint32Value(const Packet* pck, ClusterType _type);
		uint64 getUint64Value(const Packet* pck, ClusterType _type);

		std::vector<uint64> getUint64Vector(const Packet* pck, ClusterType _type);

		void sendCommand(const Packet* pck, ClusterType _type);
	private:
		ClusterMap m_clustermap;
};

#define sClusterMgr MaNGOS::Singleton<ClusterMgr>::Instance()

#endif