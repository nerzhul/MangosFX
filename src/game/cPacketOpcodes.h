#ifndef __C_PACKET_OPCODES_
#define __C_PACKET_OPCODES_

#include "cIncludes.h"

#include "cClusterSession.h"

enum cPckOpcodes
{
	C_CMSG_NULL						= 0x00, //0x00
	C_CMSG_PING,
	C_SMSG_PING_RESP,
	C_SMSG_PING,
	C_CMSG_PING_RESP,
	C_SMSG_CLUSTER_TYPE,
	C_SMSG_RPC_COMMAND_TEST,
	C_CMSG_RPC_COMMAND_TEST_RESP,
	C_CMSG_GET_LOOTS,
	C_SMSG_GET_LOOTS,
	C_SMSG_RETRANSMIT_PACKET,
	C_SMSG_RETRANSMIT_PACKET_ON_LIST,
	C_CMSG_BG_M_PLAYERS_MOD,
	C_CMSG_BG_SEND_COMMAND,
	C_CMSG_GET_BG_ID,
	C_SMSG_GET_BG_ID,
	C_CMSG_GET_BG_REW_PLAYERS,
	C_SMSG_GET_BG_REW_PLAYERS,

	MAX_C_OPCODES,
};

#endif