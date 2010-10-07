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
	{"C_CMSG_NULL",							C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_PING",							C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_PING_RESP",					C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_PING",							C_ALL,&cClusterSession::Handle_ClusterPing},
	{"C_SMSG_PING_RESP",					C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_CLUSTER_TYPE",					C_ALL,&cClusterSession::Handle_SetClusterType},
	{"C_SMSG_RPC_COMMAND_TEST",				C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_CMSG_RPC_COMMAND_TEST_RESP",		C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_GET_LOOTS",					C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_GET_LOOTS",					C_LOOT,&cClusterSession::Handle_Null},
	{"C_SMSG_RETRANSMIT_PACKET",			C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_RETRANSMIT_PACKET_ON_LIST",	C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_BG_M_PLAYERS_MOD",				C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_CMSG_BG_SEND_COMMAND",				C_ALL,&cClusterSession::Handle_ServerSide},
	{"C_CMSG_GET_BG_ID",					C_BG,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_GET_UINT64",					C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_GET_BG_REW_PLAYERS",			C_BG,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_GET_V_UINT64",					C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_IS_IN_BG",						C_BG,&cClusterSession::Handle_ServerSide},
	{"C_SMSG_GET_BOOL",						C_ALL,&cClusterSession::Handle_Null},
	{"C_SMSG_GET_UINT32",					C_ALL,&cClusterSession::Handle_Null},
	{"C_CMSG_GET_BG_TEAM",					C_BG,&cClusterSession::Handle_Null},
	{"C_CMSG_BG_REG_PLAYER",				C_BG,&cClusterSession::Handle_ServerSide},
	{"C_CMSG_PLR_GET_OFFLINE_TIME",			C_BG,&cClusterSession::Handle_Null},
	{"C_CMSG_BG_RM_PLR_LEAVE",				C_BG,&cClusterSession::Handle_ServerSide},
	{"C_GET_PL_NB_TEAM",					C_BG,&cClusterSession::Handle_ServerSide},
};

#endif