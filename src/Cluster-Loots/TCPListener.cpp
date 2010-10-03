#include <Log.h>
#include <cIncludes.h>
#include "TCPListener.h"
#include "ClusterSession.h"

TCPListener::TCPListener(uint16 port): m_port(port)
{
	m_sessions.clear();
}

TCPListener::~TCPListener()
{
	// TODO: disconnect all
	m_sessions.clear();
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
	
	sLog.outBasic("Connection on port %u wait clients !",m_port);
	ClusterSession* tmpsess = NULL;
	while(true)
	{
		IPAddress ClientAddress;
		SocketTCP Client;
		if (Listener.Accept(Client, &ClientAddress) != Socket::Done)
			continue;
			
		// Todo: verify client connection
		sLog.outBasic("Incoming connection from %s accepted",ClientAddress.ToString().c_str());
		tmpsess = new ClusterSession();
		ACE_Based::Thread session(tmpsess);
		session.setPriority(ACE_Based::Highest);
		m_sessions.push_back(tmpsess);
		ACE_Based::Thread::Sleep(100);
	}
}