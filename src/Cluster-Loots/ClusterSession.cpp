#include <Log.h>
#include <WorldPacket.h>
#include <cPacketOpcodes.h>
#include <cIncludes.h>
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
	m_sock->SetBlocking(false);
	SendClusterIdentity();
	while(!mustStop)
	{
		Packet pkt;
		Socket::Status st = m_sock->Receive(pkt);
		if(CheckState(st))
			HandlePacket(&pkt);
		ACE_Based::Thread::Sleep(100);
	}
	m_sock->Close();
	sLog.outBasic("Closing connection with %s",m_addr.c_str());
}

bool ClusterSession::CheckState(Socket::Status st)
{
	switch(st)
	{
		case 3 /*Socket::Status::Error*/:
			/*error_log("Socket Error for %s",m_addr.c_str());
			mustStop = true;*/
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
		//error_log("Packet size for Cluster is wrong...");
		return;
	}
	
	// TODO: handle with opcode table
	delete pck;
}

void ClusterSession::SendClusterIdentity()
{
	Packet* pkt;
	*pkt << Uint16(C_CMSG_CLUSTER_TYPE);
	*pkt << Uint8(C_LOOT);
	m_sock->Send((Packet&)*pkt);
}