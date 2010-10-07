#ifndef __TCP_LISTENER_H_
#define __TCP_LISTENER_H_

#include <Common.h>
#include "ClusterSession.h"

class TCPListener: public ACE_Based::Runnable
{
	public:
		TCPListener(uint16 port);
		~TCPListener();
		void SetRPC(bool rpc) { isRPC = rpc; }
	private:
		void run();

		bool isRPC;
		std::vector<ClusterSession*> m_sessions;
		uint16 m_port;
};
#endif