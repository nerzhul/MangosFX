#ifndef __C_CLUSTER_SESSION_H_
#define __C_CLUSTER_SESSION_H_

#include "cIncludes.h"
#include "WorldPacket.h"
#include <ace/Atomic_Op.h>

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
		void Handle_Unhandled(WorldPacket& pck) { error_log("Unhandled Packet opcode 0x%x recvd",pck.GetOpcode()); }

		void Handle_ClusterPing(WorldPacket &pck);
		void Handle_SetClusterType(WorldPacket &pck);
		void Handle_RetransmitPacket(WorldPacket &pck);
		void Handle_RetransmitPacketOnList(WorldPacket &pck);

		void SendPing();

	private:
		ClusterType m_type;

		cSocketTCP* m_sock;
		ACE_Based::LockedQueue<WorldPacket*, ACE_Thread_Mutex> _recvQueue;
};
#endif