#include <Log.h>
#include <WorldPacket.h>
#include <cPacketOpcodes.h>
#include <cIncludes.h>
#include "ClusterOpcodes.h"
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
	///- empty incoming packet queue
    WorldPacket* packet;
    while(_recvQueue.next(packet))
        delete packet;
}

void ClusterSession::QueuePacket(WorldPacket* new_packet)
{
    _recvQueue.add(new_packet);
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
	while(!mustStop && m_sock->IsValid())
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
	
	uint16 opcode = 0;
	*pck >> opcode;
	WorldPacket packet(opcode);
	packet << pck->GetData();
	WorldPacket* pkt = new WorldPacket(packet);
	QueuePacket(pkt);

	delete pck;
}

void ClusterSession::SendClusterIdentity()
{
	Packet pkt;
	pkt << Uint16(C_CMSG_CLUSTER_TYPE);
	pkt << Uint8(C_LOOT);
	SendPacket(pkt);
}

void ClusterSession::SendPacket(const Packet* pck)
{
	if(!m_sock || !m_sock->IsValid())
		return;
	
	Socket::Status st = m_sock->Send((Packet&)*pck);
	CheckState(st);
}

void ClusterSession::Update()
{
	WorldPacket* packet;
	while (_recvQueue.next(packet) && m_sock && m_sock->IsValid())
    {
		PacketOpcodeHandler opHandle = PckOpH[packet->GetOpcode()];
		try
		{
			(this->*opHandle.handler)(*packet);
		}
		catch(ByteBufferException &)
        {
            if(sLog.IsOutDebug())
            {
                sLog.outDebug("Dumping error causing packet:");
                packet->hexlike();
            }
        }
		catch(std::exception &e)
		{
			error_log("FATAL STD ERROR : %s",e.what());
		}
		delete packet;
	}
}

void ClusterSession::Handle_ClusterPing(WorldPacket &pck)
{
	sLog.outDebug("C_CMSG_PING received...");
	uint8 ping;
	pck >> ping;
	Packet packet;
	packet << uint8(C_SMSG_PING_RESP);
	packet << uint8(ping);
	SendPacket(packet);
}