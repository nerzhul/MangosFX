#ifndef __C_PACKET_OPCODES_
#define __C_PACKET_OPCODES_

#include "cIncludes.h"

#include "cClusterSession.h"

enum cPckOpcodes
{
	C_CMSG_NULL, //0x00
	C_CMSG_PING,
	C_SMSG_PING_RESP,
	C_SMSG_PING,
	C_CMSG_PING_RESP,
	C_CMSG_CLUSTER_TYPE,
	C_SMSG_RPC_COMMAND_TEST,
	C_CMSG_RPC_COMMAND_TEST_RESP,

	MAX_C_OPCODES,
};

struct cPacketOpcodeHandler
{
	const char* name;
	ClusterType cType;
	void (cClusterSession::*handler)(WorldPacket& recv_data);
};

static cPacketOpcodeHandler cPckOpH[MAX_C_OPCODES] = {
	{"C_CMSG_NULL",						C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_PING",						C_ALL,&cClusterSession::Handle_ClusterPing},
	{"C_SMSG_PING_RESP",				C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_PING",						C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_CMSG_PING_RESP",				C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_CMSG_CLUSTER_TYPE",				C_ALL,&cClusterSession::Handle_SetClusterType},
	{"C_SMSG_RPC_COMMAND_TEST",			C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_CMSG_RPC_COMMAND_TEST_RESP",	C_ALL,&cClusterSession::Handle_Null},
};

#endif