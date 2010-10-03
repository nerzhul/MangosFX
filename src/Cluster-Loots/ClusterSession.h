#ifndef __CLUSTER_SESSION_H_
#define __CLUSTER_SESSION_H_

#include <Common.h>
#include <cIncludes.h>

class ClusterSession: public ACE_Based::Runnable
{
	public:
		ClusterSession();
		~ClusterSession();
		void SetParams(SocketTCP sock, std::string str);
		void run();
	private:
		bool CheckState(Socket::Status st);
		void HandlePacket(Packet* pck);
		SocketTCP m_sock;
		std::string m_addr;
};
#endif
