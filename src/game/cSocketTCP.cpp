#include <Policies/SingletonImp.h>
#include "World.h"
#include "WorldPacket.h"
#include "cSocketTCP.h"

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
	isConnected = true;
}

void cSocketTCP::SendPacket(const Packet* pkt)
{
	if(!isConnected)
	{
		error_log("Socket for %s:%u is closed...",m_address.c_str(),m_port);
		return;
	}
	Packet pck;
	pck << pkt->GetData();
	Socket::Status st = m_sock->Send(pck);
	CheckState(st);
}

void cSocketTCP::SendRPCCommand(Packet pkt)
{
	m_sock->Send(pkt);
	Packet pck;
	while(true)
	{
		m_sock->Receive(pck);
		uint16 op,op2;
		uint8 test1;
		pkt >> op;
		pck >> op2;
		pkt >> test1;
		uint8 test2;
		pck >> test2;
		error_log("pkt1 %u: %u pkt2 Mu: %u",op,test1,op2,test2);
		ACE_Based::Thread::Sleep(50);
	}
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
			error_log("Socket isn't ready yet !");
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

	uint16 opcode = 0;
	*pck >> opcode;
	WorldPacket packet(opcode);
	packet << pck->GetData();
}