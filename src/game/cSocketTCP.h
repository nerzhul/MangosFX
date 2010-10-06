#ifndef __C_SOCKET_TCP_H_
#define __C_SOCKET_TCP_H_

#include <Common.h>

#include "cIncludes.h"
#include "cClusterSession.h"

class cClusterSession;

class cSocketTCP: public ACE_Based::Runnable
{
	public:
		cSocketTCP(ClusterType _type);
		cSocketTCP(std::string addr, uint16 port);
		~cSocketTCP();
		void Connect();
		void Close();
		void SendPacket(const Packet* pkt);
		void run();

		SocketTCP* getSocket() { return m_sock; }
		cClusterSession* getSession() { return m_session; }
	private:
		bool CheckState(Socket::Status st);
		void HandlePacket(Packet* pck);
		bool isConnected;

		std::string m_address;
		uint16 m_port;
		SocketTCP* m_sock;
		cClusterSession* m_session;
};

class cRPCCommandHandler
{
	public:
		cRPCCommandHandler(ClusterType _type);
		~cRPCCommandHandler();
		Packet* getResponse(const Packet* pck);
	private:
		SocketTCP* m_sock;
		std::string m_addr;
		uint16 m_port;
};

#endif