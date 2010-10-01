#ifndef __C_SOCKET_TCP_SND_H_
#define __C_SOCKET_TCP_SND_H_

#include <Common.h>
#include <SFML/Network.hpp>

using namespace sf;

class cSocketTCP: public ACE_Based::Runnable
{
	public:
		cSocketTCP();
		~cSocketTCP();
		void InitConnect(std::string addr, uint16 port);
		void Connect();
		void Close();
		void SendPacket(Packet pkt);
		void run();
	private:
		void CheckState(Socket::Status st);

		bool isConnected;

		std::string m_address;
		uint16 m_port;
		SocketTCP m_sock;
};

#endif