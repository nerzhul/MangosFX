#ifndef __C_SOCKET_TCP_SND_H_
#define __C_SOCKET_TCP_SND_H_

#include <Common.h>

#include "cIncludes.h"
#include "cClusterSession.h"

class cSocketTCP: public ACE_Based::Runnable
{
	public:
		cSocketTCP();
		~cSocketTCP();
		void InitConnect(std::string addr, uint16 port);
		void Connect();
		void Close();
		void SendPacket(Packet pkt);
		void SendRPCCommand(Packet pkt);
		void run();

		SocketTCP* getSocket() { return m_sock; }
	private:
		bool CheckState(Socket::Status st);
		void HandlePacket(Packet* pck);
		bool isConnected;

		std::string m_address;
		uint16 m_port;
		SocketTCP* m_sock;
		cClusterSession* m_session;
};
#endif