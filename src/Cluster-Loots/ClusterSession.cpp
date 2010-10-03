#include <Log.h>
#include <WorldPacket.h>
#include "ClusterSession.h"

ClusterSession::ClusterSession()
{
}

void ClusterSession::SetParams(SocketTCP sock, std::string addr)
{
	m_sock = sock;
	m_addr = addr;
}

ClusterSession::~ClusterSession()
{
	m_sock.Close();
}

void ClusterSession::run()
{
	while(m_sock.IsValid())
	{
		error_log("boucle");
		Packet* pkt;
		if (CheckState(m_sock.Receive(*pkt)))
			HandlePacket(pkt);

		ACE_Based::Thread::Sleep(300);
	}
}

bool ClusterSession::CheckState(Socket::Status st)
{
	switch(st)
	{
		case 3 /*Socket::Status::Error*/:
			error_log("Socket Error for %s",m_addr.c_str());
			return false;
		case 2 /*Socket::Status::Disconnected*/:
			error_log("Link with %s lost...",m_addr.c_str());
			ACE_Based::Thread::Sleep(5000);
			m_sock.Close();
			return false;
		case 0 /*Socket::Done)*/:
			return true;
	}
	return true;
}

void ClusterSession::HandlePacket(Packet* pck)
{
	if(pck->GetDataSize() < 2)
	{
		error_log("Packet size for Cluster is wrong...");
		return;
	}

	uint16 opcode = 0;
	*pck >> opcode;
	WorldPacket packet(opcode);
	packet << pck->GetData();
}