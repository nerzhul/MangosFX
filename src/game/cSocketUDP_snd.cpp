#include <Log.h>
#include "World.h"
#include "cSocketUDP_snd.h"

cSocketUDP_snd::cSocketUDP_snd()
{
	m_address = "";
	m_data = "";
	m_port = 0;
	_init = false;
}

cSocketUDP_snd::~cSocketUDP_snd()
{
}

void cSocketUDP_snd::InitConnect(char* buff, std::string addr, uint16 port)
{
	if(!buff || buff == "" || addr == "" || port == 0 || port > 65534)
		return;

	m_data = buff;
	m_address = addr;
	m_port = port;
}

void cSocketUDP_snd::SendPacket()
{
	if(!_init)
		return;

	// Todo : show opcode
	SocketUDP sock;
	if(sock.Send(m_data,sizeof(m_data),m_address,m_port) != Socket::Done)
	{
		error_log("UDP Transmission to %s:%u failed",m_address.c_str(),m_port);
		return;
	}

	sLog.outDebug("UDP Packet Sent to %s:%u data %s",m_address.c_str(),m_port,m_data);
}