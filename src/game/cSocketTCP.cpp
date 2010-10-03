#include <Policies/SingletonImp.h>
#include "World.h"
#include "cSocketTCP.h"

cSocketTCP::cSocketTCP() : isConnected(false)
{
}

cSocketTCP::~cSocketTCP()
{
}

void cSocketTCP::InitConnect(std::string addr, uint16 port)
{
	m_address = addr;
	m_port = port;
}

void cSocketTCP::Connect()
{
	if(m_sock.Connect(m_port,m_address) != 0)
	{
		error_log("Sock_err: Could not bind %s:%u",m_address.c_str(),m_port);
		isConnected = false;
		return;
	}

	isConnected = true;
}

void cSocketTCP::SendPacket(Packet pkt)
{
	if(!isConnected)
	{
		error_log("Socket for %s:%u is closed...",m_address.c_str(),m_port);
		return;
	}
	CheckState(m_sock.Send(pkt));
}

void cSocketTCP::Close()
{
	if(m_sock.IsValid())
		m_sock.Close();
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
		CheckState(m_sock.Receive(pck));

		ACE_Based::Thread::Sleep(50);
	}
	if(m_sock.IsValid())
		m_sock.Close();
}

void cSocketTCP::CheckState(Socket::Status st)
{
	switch(st)
	{
		case 3 /*Socket::Status::Error*/:
			error_log("Socket Error for %s:%u",m_address.c_str(),m_port);
			return;
		case 2 /*Socket::Status::Disconnected*/:
			error_log("Link with %s:%u lost...",m_address.c_str(),m_port);
			ACE_Based::Thread::Sleep(5000);
			isConnected = false;
			return;
		case 0 /*Socket::Done)*/:
			return;
	}
}