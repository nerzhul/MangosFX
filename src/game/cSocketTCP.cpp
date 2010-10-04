#include <Policies/SingletonImp.h>
#include "World.h"
#include "WorldPacket.h"
#include "cSocketTCP.h"
#include "cPacketOpcodes.h"
#include "cClusterSession.h"

cSocketTCP::cSocketTCP() : isConnected(false), m_session(NULL)
{
	m_sock = new SocketTCP();
}

cSocketTCP::~cSocketTCP()
{
	delete m_sock;
	m_sock = NULL;
	delete m_session;
	m_session = NULL;
}

void cSocketTCP::InitConnect(std::string addr, uint16 port)
{
	m_address = addr;
	m_port = port;
	m_session = new cClusterSession(this);
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
	Socket::Status st = m_sock->Send((Packet&)*pkt);
	CheckState(st);
}

void cSocketTCP::Close()
{
	if(m_sock->IsValid())
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

	packet.hexlike();
	// Pointer to keep data alive
	WorldPacket* pkt = new WorldPacket(packet);
	if(m_session)
		m_session->QueuePacket(pkt);
}