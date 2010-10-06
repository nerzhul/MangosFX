#include <Util.h>
#include "cClusterSession.h"
#include "cSocketTCP.h"
#include "cOpcodeHandler.h"
#include "cPacketOpcodes.h"
#include "WorldPacket.h"
#include "ObjectAccessor.h"
#include "Player.h"

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
			//if((m_type & opHandle.cType) || packet->GetOpcode() == C_SMSG_CLUSTER_TYPE)
				(this->*opHandle.handler)(*packet);
			/*else
				error_log("One cluster tries to get non owned packet...");*/
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
	sLog.outDebug("C_SMSG_PING received...");
	uint8 ping;
	pck >> ping;

	Packet packet;
	packet << Uint16(C_CMSG_PING_RESP) << Uint8(ping);
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

void cClusterSession::Handle_RetransmitPacket(WorldPacket &pck)
{
	uint64 plGUID;
	uint16 datasize;

	pck >> plGUID;
	pck >> datasize;

	Player *plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(plGUID, 0, HIGHGUID_PLAYER));
	if(!plr)
		return;
	uint16 opcode;
	pck >> opcode;
	
	WorldPacket data(opcode,300);
	for(uint16 i=0;i<datasize;i++)
	{
		uint8 tmp;
		pck >> tmp;
		data << uint8(tmp);
	}
	// test data
	plr->GetSession()->SendPacket(&data);
}

void cClusterSession::Handle_RetransmitPacketOnList(WorldPacket &pck)
{
	uint16 datasize;
	pck >> datasize;

	uint16 opcode;
	pck >> opcode;
	
	WorldPacket data(opcode,datasize+1);
	for(uint16 j=0;j<datasize;j++)
	{
		uint8 tmp;
		pck >> uint8(tmp);
		data << uint8(tmp);
	}

	uint16 nbpl = 0;
	pck >> nbpl;
	for(uint16 i=0;i<nbpl;i++)
	{
		uint64 plGUID;
		pck >> plGUID;
		Player *plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(plGUID, 0, HIGHGUID_PLAYER));
		if(!plr)
			continue;
		plr->GetSession()->SendPacket(&data);
	}
}

void cClusterSession::SendPing()
{
	Packet pkt;
	pkt << Uint16(C_CMSG_PING) << Uint8(urand(0,5));
	SendPacket(&pkt);
}