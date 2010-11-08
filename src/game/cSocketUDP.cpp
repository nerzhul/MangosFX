#include <Log.h>
#include "World.h"
#include "cSocketUDP.h"

cSocketUDP::cSocketUDP()
{
	m_address = "";
	m_port = 0;
	_init = false;
}

cSocketUDP::~cSocketUDP()
{
}

void cSocketUDP::InitConnect(Packet buff, std::string addr, uint16 port)
{
	if(buff.GetDataSize() > 0 || addr == "" || port == 0 || port > 65534)
		return;

	m_data = buff;
	m_address = addr;
	m_port = port;
	_init = true;
}

void cSocketUDP::SendPacket()
{
	if(!_init)
		return;

	// Todo : show opcode
	SocketUDP sock;
	if(sock.Send(m_data,m_address,m_port) != Socket::Done)
	{
		error_log("UDP Transmission to %s:%u failed",m_address.c_str(),m_port);
		return;
	}

	sLog.outDebug("UDP Packet Sent to %s:%u data [..]",m_address.c_str(),m_port);
	sock.Close();
}