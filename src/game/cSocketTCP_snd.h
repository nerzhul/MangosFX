#ifndef __C_SOCKET_TCP_SND_H_
#define __C_SOCKET_TCP_SND_H_

#include <Common.h>
#include <SFML/Network.hpp>

using namespace sf;

class cSocketTCP_snd
{
	public:
		cSocketTCP_snd();
		~cSocketTCP_snd();
		void InitConnect(std::string addr, uint16 port);
		bool Connect();
		void SendPacket(WorldPacket* data);
	private:
		bool isConnected;

		SocketTCP m_sock;
};

#endif