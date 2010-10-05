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
	for(std::vector<ClusterSession*>::iterator itr = m_sessions.begin();itr != m_sessions.end();)
	{
		std::vector<ClusterSession*>::iterator itr2 = itr+1;
		delete (*itr);
		itr = itr2;
	}

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
	
	while(true)
	{
		IPAddress ClientAddress;
		SocketTCP Client;
		if (Listener.Accept(Client, &ClientAddress) != Socket::Done)
			continue;

		// Todo: verify client connection
		sLog.outBasic("Incoming connection from %s accepted",ClientAddress.ToString().c_str());
		ClusterSession* sess = new ClusterSession;
		sess->SetParams(&Client,ClientAddress.ToString());
		ACE_Based::Thread* session = new ACE_Based::Thread(sess);
		session->setPriority(ACE_Based::Highest);
		//m_sessions.push_back(sess);
		ACE_Based::Thread::Sleep(100);
	}
}