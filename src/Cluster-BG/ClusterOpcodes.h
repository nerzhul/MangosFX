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
	{"C_SMSG_RPC_COMMAND_TEST",						&ClusterSession::Handle_ClientSide},
	{"C_CMSG_RPC_COMMAND_TEST_RESP",				&ClusterSession::Handle_Null},
	{"C_CMSG_GET_LOOTS",							&ClusterSession::Handle_Null},
	{"C_SMSG_GET_LOOTS",							&ClusterSession::Handle_ClientSide},
	{"C_SMSG_RETRANSMIT_PACKET",					&ClusterSession::Handle_ClientSide},
	{"C_SMSG_RETRANSMIT_PACKET_ON_LIST",			&ClusterSession::Handle_ClientSide},
	{"C_CMSG_BG_M_PLAYERS_MOD",						&ClusterSession::Handle_BG_m_Players_mod},
	{"C_CMSG_BG_SEND_COMMAND",						&ClusterSession::Handle_GetBGCommand},
	{"C_CMSG_GET_BG_ID",							&ClusterSession::Handle_GenerateBGId},
	{"C_SMSG_GET_UINT64",							&ClusterSession::Handle_ClientSide},
	{"C_CMSG_GET_BG_REW_PLAYERS",					&ClusterSession::Handle_GetRewardPlayers},
	{"C_SMSG_GET_V_UINT64",							&ClusterSession::Handle_ClientSide},
	{"C_CMSG_IS_IN_BG",								&ClusterSession::Handle_IsInBG},
	{"C_SMSG_GET_BOOL",								&ClusterSession::Handle_Null},
	{"C_SMSG_GET_UINT32",							&ClusterSession::Handle_Null},
	{"C_CMSG_GET_BG_TEAM",							&ClusterSession::Handle_GetBgTeam},
	{"C_CMSG_BG_REG_PLAYER",						&ClusterSession::Handle_Updt_Plr},
	{"C_CMSG_PLR_GET_OFFLINE_TIME",					&ClusterSession::Handle_BGGetOfflineTime},
	{"C_CMSG_BG_RM_PLR_LEAVE",						&ClusterSession::Handle_BGRemovePlayerAtLeave},
	{"C_GET_PL_NB_TEAM",							&ClusterSession::Handle_BGGetPlayerNumberByTeam},
	{"C_CMSG_BG_SET_ARENA_TEAM",					&ClusterSession::Handle_BGSetArenaTeam},
	{"C_CMSG_BG_GET_ARENA_TEAM",					&ClusterSession::Handle_BGGetArenaTeam},
	{"C_CMSG_BG_SET_ARENA_TEAM_RATING_CHANGE",		&ClusterSession::Handle_BGSetArenaTeamRatingChange},
	{"C_CMSG_BG_GET_ARENA_TEAM_RATING_CHANGE",		&ClusterSession::Handle_BGGetArenaTeamRatingChange},
	{"C_CMSG_BG_GET_LIMIT",							&ClusterSession::Handle_BGGetLimit},
	{"C_CMSG_BG_SET_LIMIT",							&ClusterSession::Handle_BGSetLimit},
};

#endif