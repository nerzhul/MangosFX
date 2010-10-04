#include <Log.h>
#include <WorldPacket.h>
#include "ClusterSession.h"

ClusterSession::ClusterSession()
{
	m_addr = "<error>";
	mustStop = false;
}

ClusterSession::~ClusterSession()
{
	m_sock->Close();
	sLog.outBasic("Closing connection with %s",m_addr.c_str());
}

void ClusterSession::SetParams(SocketTCP* sock, std::string addr)
{
	m_sock = sock;
	m_addr = addr;
}

void ClusterSession::run()
{
	while(!mustStop)
	{
		error_log("TEST");
		Packet pkt;
		Socket::Status st = m_sock->Receive(pkt);
		error_log("Size : %u",pkt.GetDataSize());
		if(CheckState(st))
			HandlePacket(&pkt);
		ACE_Based::Thread::Sleep(100);
	}
}

bool ClusterSession::CheckState(Socket::Status st)
{
	switch(st)
	{
		case 3 /*Socket::Status::Error*/:
			error_log("Socket Error for %s",m_addr.c_str());
			mustStop = true;
			return false;
		case 2 /*Socket::Status::Disconnected*/:
			error_log("Link with %s lost...",m_addr.c_str());
			mustStop = true;
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
	error_log("opcode %u",opcode);
}