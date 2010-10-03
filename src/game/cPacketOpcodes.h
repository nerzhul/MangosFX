#ifndef __C_PACKET_OPCODES_
#define __C_PACKET_OPCODES_

#include "cIncludes.h"

enum cPckOpcodes
{
	C_CMSG_NULL, //0x00
	C_CMSG_PING,
	C_SMSG_PING_RESP,
	C_SMSG_PING,
	C_CMSG_PING_RESP, //0x04

	MAX_C_OPCODES = 0x05,
};

struct cPacketOpcodeHandler
{
	const char* name;
	void (WorldSession::*handler)(Packet& recv_data);
};

#endif