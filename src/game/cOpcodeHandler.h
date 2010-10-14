#ifndef __C_OPCODE_HANDLER_H_
#define __C_OPCODE_HANDLER_H_

#include "cPacketOpcodes.h"

struct cPacketOpcodeHandler
{
	const char* name;
	ClusterType cType;
	void (cClusterSession::*handler)(WorldPacket& recv_data);
};

static cPacketOpcodeHandler cPckOpH[MAX_C_OPCODES] = {
	{"C_CMSG_NULL",									C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_PING",									C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_PING_RESP",							C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_PING",									C_ALL,&cClusterSession::Handle_ClusterPing},
	{"C_SMSG_PING_RESP",							C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_CLUSTER_TYPE",							C_ALL,&cClusterSession::Handle_SetClusterType},
	{"C_CMSG_GET_LOOTS",							C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_GET_LOOTS",							C_LOOT,&cClusterSession::Handle_Null},
	{"C_SMSG_RETRANSMIT_PACKET",					C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_RETRANSMIT_PACKET_ON_LIST",			C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_GET_UINT64",							C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_GET_V_UINT64",							C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_GET_BOOL",								C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_GET_UINT32",							C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_GET_FLOAT",							C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_GET_INT32",							C_ALL,&cClusterSession::Handle_Null},
};

#endif