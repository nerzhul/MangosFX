#ifndef __C_CLUSTER_SESSION_
#define __C_CLUSTER_SESSION_

#include "cIncludes.h"

class cSocketTCP;

class cClusterSession
{
	public:
		cClusterSession(cSocketTCP* sock);
		~cClusterSession();

		void Update();
		void QueuePacket(WorldPacket* new_packet);
		void SendPacket(const Packet* pck);

		// Handlers
		void Handle_Null(WorldPacket& pck) {}
		void Handle_ServerSide(WorldPacket &pck) {}

		void Handle_ClusterPing(WorldPacket &pck);
		void Handle_SetClusterType(WorldPacket &pck);

		void Handle_ShowRPCTestResp(WorldPacket &pck);

	private:
		ClusterType m_type;

		cSocketTCP* m_sock;
		ACE_Based::LockedQueue<WorldPacket*, ACE_Thread_Mutex> _recvQueue;
};
#endif