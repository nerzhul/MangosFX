#ifndef __C_CPACKET_OPCODES_
#define __C_CPACKET_OPCODES_

#include <cIncludes.h>

#include "ClusterSession.h"

struct PacketOpcodeHandler
{
	const char* name;
	void (ClusterSession::*handler)(WorldPacket& recv_data);
};

static PacketOpcodeHandler PckOpH[MAX_C_OPCODES] = {
	{"C_CMSG_NULL",									&ClusterSession::Handle_Null},
	{"C_CMSG_PING",									&ClusterSession::Handle_ClusterPing},
	{"C_SMSG_PING_RESP",							&ClusterSession::Handle_Null},
	{"C_SMSG_PING",									&ClusterSession::Handle_ClientSide},
	{"C_CMSG_PING_RESP",							&ClusterSession::Handle_Null},
	{"C_SMSG_CLUSTER_TYPE",							&ClusterSession::Handle_ClientSide},
	{"C_CMSG_GET_LOOTS",							&ClusterSession::Handle_Null},
	{"C_SMSG_GET_LOOTS",							&ClusterSession::Handle_ClientSide},
	{"C_SMSG_RETRANSMIT_PACKET",					&ClusterSession::Handle_ClientSide},
	{"C_SMSG_RETRANSMIT_PACKET_ON_LIST",			&ClusterSession::Handle_ClientSide},
	{"C_SMSG_GET_UINT64",							&ClusterSession::Handle_ClientSide},
	{"C_SMSG_GET_V_UINT64",							&ClusterSession::Handle_ClientSide},
	{"C_SMSG_GET_BOOL",								&ClusterSession::Handle_Null},
	{"C_SMSG_GET_UINT32",							&ClusterSession::Handle_Null},
	{"C_SMSG_GET_FLOAT",							&ClusterSession::Handle_Null},
	{"C_SMSG_GET_INT32",							&ClusterSession::Handle_Null},

	{"C_CMSG_BATTLEMASTER_HELLO",					&ClusterSession::Handle_BattleMasterHello},
	{"C_CMSG_BATTLEMASTER_JOIN",					&ClusterSession::Handle_BattleMasterJoin},
};

#endif