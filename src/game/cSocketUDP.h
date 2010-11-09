#ifndef __C_SOCKET_UDP_SND_H_
#define __C_SOCKET_UDP_SND_H_

#include <Common.h>

#include "cIncludes.h"

class cSocketUDP
{
	public:
		cSocketUDP();
		~cSocketUDP();
		
		void InitConnect(Packet buff, std::string addr, uint16 port);
		void SendPacket();
		
	private:
		bool _init;
		Packet m_data;
		std::string m_address;
		uint16 m_port;
};

#endif