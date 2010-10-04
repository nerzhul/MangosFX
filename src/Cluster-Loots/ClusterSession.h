#ifndef __CLUSTER_SESSION_H_
#define __CLUSTER_SESSION_H_

#include <Common.h>
#include <cIncludes.h>

class ClusterSession: public ACE_Based::Runnable
{
	public:
		ClusterSession();
		~ClusterSession();
		void SetParams(SocketTCP* sock, std::string str);
	protected:
		void run();
	private:
		bool CheckState(Socket::Status st);
		void HandlePacket(Packet* pck);
		bool mustStop;
		SocketTCP* m_sock;
		std::string m_addr;
};
#endif
