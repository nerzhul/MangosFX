#include <Log.h>
#include <cIncludes.h>
#include "TCPListener.h"

TCPListener::TCPListener(uint16 port): m_port(port)
{
}

void TCPListener::run()
{
	sLog.outBasic("Running Listener on port %u...",m_port);
	SocketTCP Listener;
	if (!Listener.Listen(m_port))
	{
		error_log("Couldn't bind port %u, aborted",m_port);
		exit(-1);
	}
	
	sLog.outBasic("Connection on port %u etablished !",m_port);
	while(true)
	{
		IPAddress ClientAddress;
		SocketTCP Client;
		if (Listener.Accept(Client, &ClientAddress) == sf::Socket::Done)
		{
			// TODO: client connection
		}
		ACE_Based::Thread::Sleep(100);
	}
}