#include "cClusterSession.h"
#include "cSocketTCP.h"
#include "cPacketOpcodes.h"
#include "WorldPacket.h"

cClusterSession::cClusterSession(cSocketTCP* sock): m_sock(sock),m_type(C_NULL)
{
}

cClusterSession::~cClusterSession()
{
	///- empty incoming packet queue
    WorldPacket* packet;
    while(_recvQueue.next(packet))
        delete packet;
}

void cClusterSession::QueuePacket(WorldPacket* new_packet)
{
    _recvQueue.add(new_packet);
}

void cClusterSession::Update()
{
	WorldPacket* packet;
	while (_recvQueue.next(packet) && m_sock->getSocket() && m_sock->getSocket()->IsValid())
    {
		cPacketOpcodeHandler opHandle = cPckOpH[packet->GetOpcode()];
		try
		{
			if(m_type & opHandle.cType)
				(this->*opHandle.handler)(*packet);
			else
				error_log("One cluster tries to get non owned packet...");
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

void cClusterSession::SendPacket(const Packet* pck)
{
	if(!pck)
		return;

	if(pck->GetDataSize() == 0)
		return;

	if(m_sock->getSocket()->IsValid())
		m_sock->SendPacket(pck);
}

void cClusterSession::Handle_ClusterPing(WorldPacket &pck)
{
	sLog.outDebug("C_CMSG_PING received...");
	uint8 ping;
	pck >> ping;
	Packet packet;
	packet << C_SMSG_PING_RESP;
	packet << ping;
	SendPacket(&packet);
}

void cClusterSession::Handle_SetClusterType(WorldPacket &pck)
{
	uint8 type = m_type;
	if(m_type > C_NULL)
	{
		error_log("Cluster type already sent...");
		return;
	}
	pck >> type;
	m_type = ClusterType(type);
}