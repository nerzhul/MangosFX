#include <Policies/SingletonImp.h>
#include <SFML/Network.hpp>
#include "World.h"
#include "cSocketTCP_snd.h"

using namespace sf;

cSocketTCP_snd::cSocketTCP_snd() : isConnected(false)
{
}

cSocketTCP_snd::~cSocketTCP_snd()
{
}

void cSocketTCP_snd::InitConnect(std::string addr, uint16 port)
{
	m_address = addr;
	m_port = port;
}

bool cSocketTCP_snd::Connect()
{
	if(!m_sock.Connect(m_port,m_address))
	{
		error_log("Sock_err: Could not bind %s:%u",m_address.c_str(),m_port);
		isConnected = false;
		return false;
	}

	isConnected = true;
	return true;
}

void cSocketTCP_snd::SendPacket(WorldPacket* data)
{

}