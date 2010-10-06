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
	{"C_CMSG_NULL",							&ClusterSession::Handle_Null},
	{"C_CMSG_PING",							&ClusterSession::Handle_ClusterPing},
	{"C_SMSG_PING_RESP",					&ClusterSession::Handle_Null},
	{"C_SMSG_PING",							&ClusterSession::Handle_ClientSide},
	{"C_CMSG_PING_RESP",					&ClusterSession::Handle_Null},
	{"C_SMSG_CLUSTER_TYPE",					&ClusterSession::Handle_ClientSide},
	{"C_SMSG_RPC_COMMAND_TEST",				&ClusterSession::Handle_ClientSide},
	{"C_CMSG_RPC_COMMAND_TEST_RESP",		&ClusterSession::Handle_Null},
	{"C_CMSG_GET_LOOTS",					&ClusterSession::Handle_Null},
	{"C_SMSG_GET_LOOTS",					&ClusterSession::Handle_ClientSide},
	{"C_SMSG_RETRANSMIT_PACKET",			&ClusterSession::Handle_ClientSide},
	{"C_SMSG_RETRANSMIT_PACKET_ON_LIST",	&ClusterSession::Handle_ClientSide},
	{"C_CMSG_BG_M_PLAYERS_MOD",				&ClusterSession::Handle_Null},
};

#endif