#ifndef __TCP_LISTENER_H_
#define __TCP_LISTENER_H_

#include <Common.h>

class TCPListener: public ACE_Based::Runnable
{
	public:
		TCPListener(uint16 port);
		~TCPListener() {}
	private:
		void run();

		uint16 m_port;
};
#endif