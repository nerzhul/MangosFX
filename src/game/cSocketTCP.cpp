#include <Policies/SingletonImp.h>
#include <Config/Config.h>
#include "World.h"
#include "WorldPacket.h"
#include "cSocketTCP.h"
#include "cPacketOpcodes.h"
#include "cClusterSession.h"

cSocketTCP::cSocketTCP(ClusterType _type)
{
	m_sock = new SocketTCP();
	switch(_type)
	{
		case C_LOOT:
			m_address = "localhost";
			m_port = 3695;
			break;
		case C_BG:
			m_address = "localhost";
			m_port = 4238;
			break;
		default:
			m_address = "localhost";
			m_port = 3695;
			break;
	}
	m_session = new cClusterSession(this);
}
cSocketTCP::cSocketTCP(std::string addr, uint16 port) : isConnected(false), m_session(NULL)
{
	m_sock = new SocketTCP();
	m_address = addr;
	m_port = port;
	m_session = new cClusterSession(this);
}

cSocketTCP::~cSocketTCP()
{
	delete m_sock;
	m_sock = NULL;
	delete m_session;
	m_session = NULL;
}

void cSocketTCP::Connect()
{
	if(m_sock->Connect(m_port,m_address) != 0)
	{
		error_log("Sock_err: Could not bind %s:%u",m_address.c_str(),m_port);
		isConnected = false;
		return;
	}
	m_sock->SetBlocking(false);
	isConnected = true;
}

void cSocketTCP::SendPacket(const Packet* pkt)
{
	if(!isConnected)
	{
		error_log("Socket for %s:%u is closed...",m_address.c_str(),m_port);
		return;
	}
	if(!m_sock || !m_sock->IsValid())
	{
		error_log("Socket for %s:%u is invalid...",m_address.c_str(),m_port);
		return;
	}
	Socket::Status st = m_sock->Send((Packet&)*pkt);
	CheckState(st);
}

void cSocketTCP::Close()
{
	m_sock->Close();
}

void cSocketTCP::run()
{
	while(!World::IsStopped())
	{
		if(!isConnected)
			Connect();

		// if connect fail, wait 5 sec
		if(!isConnected)
		{
			ACE_Based::Thread::Sleep(5000);
			continue;
		}

		Packet pck;
		if(CheckState(m_sock->Receive(pck)))
			HandlePacket(&pck);

		if(m_session)
			m_session->Update();

		ACE_Based::Thread::Sleep(100);
	}
	Close();
}

bool cSocketTCP::CheckState(Socket::Status st)
{
	switch(st)
	{
		case 3 /*Socket::Status::Error*/:
			error_log("Socket Error for %s:%u",m_address.c_str(),m_port);
			m_sock->Close();
			isConnected = false;
			return false;
		case 2 /*Socket::Status::Disconnected*/:
			error_log("Link with %s:%u lost...",m_address.c_str(),m_port);
			ACE_Based::Thread::Sleep(5000);
			m_sock->Close();
			isConnected = false;
			return false;
		case 0 /*Socket::Done*/:
			return true;
		case 1:
			//error_log("Socket isn't ready yet !");
			return false;
	}
	return true;
}

void cSocketTCP::HandlePacket(Packet* pck)
{
	if(pck->GetDataSize() < 2)
	{
		error_log("Packet size for Cluster is wrong...");
		return;
	}

	// Get opcode
	uint16 opcode = 0;
	*pck >> opcode;
	if(!opcode)
		return;

	if(opcode >= MAX_C_OPCODES)
	{
		error_log("Cluster receive unhandled opcode %u",opcode);
		return;
	}

	// Recopy data into WorldPacket
	WorldPacket packet(opcode);
	for(uint32 i=2;i<pck->GetDataSize();i++)
		packet << pck->GetData()[i];

	// Pointer to keep data alive
	WorldPacket* pkt = new WorldPacket(packet);
	if(m_session)
		m_session->QueuePacket(pkt);
}

cRPCCommandHandler::cRPCCommandHandler(ClusterType _type)
{
	m_sock = new SocketTCP();
	switch(_type)
	{
		case C_LOOT:
			m_addr = sConfig.GetStringDefault("LootClusterAddr","localhost");
			m_port = sConfig.GetIntDefault("LootClusterPort",3695);
			break;
		case C_BG:
			m_addr = sConfig.GetStringDefault("BGClusterAddr","192.168.1.12");
			m_port = sConfig.GetIntDefault("BGClusterPort",4239);;
		default:
			break;
	}
}

cRPCCommandHandler::~cRPCCommandHandler()
{
}

Packet* cRPCCommandHandler::getResponse(const Packet* pck)
{
	if(!m_sock)
	{
		error_log("Socket error for Cluster %s:%u",m_addr.c_str(),m_port);
		return NULL;
	}

	if(m_sock->Connect(m_port,m_addr) != 0)
	{
		error_log("Connect to Cluster %s:%u failed for RPC",m_addr.c_str(),m_port);
		m_sock->Close();
		return NULL;
	}

	if(m_sock->Send((Packet&)*pck) != 0)
	{
		error_log("Sending Packet to Cluster %s:%u failed for RPC",m_addr.c_str(),m_port);
		m_sock->Close();
		return NULL;
	}

	m_sock->SetBlocking(true);

	Packet resp;

	if(m_sock->Receive(resp) != 0)
	{
		error_log("Receiving Packet from Cluster %s:%u fail on RPC",m_addr.c_str(),m_port);
		m_sock->Close();
		return NULL;
	}

	Packet* resp_ = new Packet();
	for(uint32 i=0;i<pck->GetDataSize();i++)
	{
		error_log("resp %u : %u",i,resp.GetData()[i]);
		*resp_ << uint8(resp.GetData()[i]);
	}
	m_sock->Close();
	return resp_;
}