/*
 *
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "LFGMgr.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Player.h"
#include "Opcodes.h"

void WorldSession::HandleSetLfgCommentOpcode(WorldPacket & recv_data)
{
    sLog.outDebug("CMSG_SET_LFG_COMMENT");

    std::string comment;
    recv_data >> comment;

    GetPlayer()->m_lookingForGroup.comment = comment;
}

void WorldSession::HandleLfgPlayerLockInfoRequestOpcode(WorldPacket &/*recv_data*/)
{
    sLog.outDebug("CMSG_LFD_PLAYER_LOCK_INFO_REQUEST");
    sLFGMgr.SendLfgPlayerInfo(GetPlayer());
}

void WorldSession::HandleLfgPartyLockInfoRequestOpcode(WorldPacket &/*recv_data*/)
{
    sLog.outDebug("CMSG_LFD_PARTY_LOCK_INFO_REQUEST");
    sLFGMgr.SendLfgPartyInfo(GetPlayer());
}

void WorldSession::SendLfgUpdatePlayer(uint8 updateType, uint32 dungeonEntry /* = 0*/)
{
    bool queued = false;
    bool extrainfo = false;

    switch(updateType)
    {
    case LFG_UPDATETYPE_JOIN_PROPOSAL:
    case LFG_UPDATETYPE_ADDED_TO_QUEUE:
        queued = true;
        extrainfo = true;
        break;
    //case LFG_UPDATETYPE_CLEAR_LOCK_LIST: // TODO: Sometimes has extrainfo - Check ocurrences...
    case LFG_UPDATETYPE_PROPOSAL_FOUND:
        extrainfo = true;
        break;
    }
    sLog.outDebug("SMSG_LFG_UPDATE_PLAYER");
    WorldPacket data(SMSG_LFG_UPDATE_PLAYER, 1 + 1 + (extrainfo ? 1 : 0) * (1 + 1 + 1 + 1 + !dungeonEntry ? 4 : GetPlayer()->m_lookingForGroup.applyDungeons.size() * 4 + GetPlayer()->m_lookingForGroup.comment.length()));
    data << uint8(updateType);                              // Lfg Update type
    data << uint8(extrainfo);                               // Extra info
    if (extrainfo)
    {
        data << uint8(queued);                              // Join the queue
        data << uint8(0);                                   // unk - Always 0
        data << uint8(0);                                   // unk - Always 0

        if (dungeonEntry)
        {
            data << uint8(1);
            data << uint32(dungeonEntry);
        }
        else
        {
            uint8 size = GetPlayer()->m_lookingForGroup.applyDungeons.size();
            data << uint8(size);

            for (LfgDungeonSet::const_iterator it = GetPlayer()->m_lookingForGroup.applyDungeons.begin(); it != GetPlayer()->m_lookingForGroup.applyDungeons.end(); ++it)
                data << uint32(*it);
        }
        data << GetPlayer()->m_lookingForGroup.comment;
    }
    SendPacket(&data);
}

void WorldSession::SendLfgUpdateParty(uint8 updateType, uint32 dungeonEntry /* = 0*/)
{
    bool join = false;
    bool extrainfo = false;
    bool queued = false;

    switch(updateType)
    {
    case LFG_UPDATETYPE_JOIN_PROPOSAL:
        extrainfo = true;
        break;
    case LFG_UPDATETYPE_ADDED_TO_QUEUE:
        extrainfo = true;
        join = true;
        queued = true;
        break;
    case LFG_UPDATETYPE_CLEAR_LOCK_LIST:
        // join = true;  // TODO: Sometimes queued and extrainfo - Check ocurrences...
        queued = true;
        break;
    case LFG_UPDATETYPE_PROPOSAL_FOUND:
        extrainfo = true;
        join = true;
        break;
    }

    sLog.outDebug("SMSG_LFG_UPDATE_PARTY");
    WorldPacket data(SMSG_LFG_UPDATE_PARTY, 1 + 1 + (extrainfo ? 1 : 0) * (1 + 1 + 1 + 1 + 1 + !dungeonEntry ? 4 : GetPlayer()->m_lookingForGroup.applyDungeons.size() * 4 + GetPlayer()->m_lookingForGroup.comment.length()));
    data << uint8(updateType);                              // Lfg Update type
    data << uint8(extrainfo);                               // Extra info
    if (extrainfo)
    {
        data << uint8(join);                                // LFG Join
        data << uint8(queued);                              // Join the queue
        data << uint8(0);                                   // unk - Always 0
        data << uint8(0);                                   // unk - Always 0

        if (dungeonEntry)
        {
            data << uint8(1);
            data << uint32(dungeonEntry);
        }
        else
        {
            uint8 size = GetPlayer()->m_lookingForGroup.applyDungeons.size();
            data << uint8(size);

            for (LfgDungeonSet::const_iterator it = GetPlayer()->m_lookingForGroup.applyDungeons.begin(); it != GetPlayer()->m_lookingForGroup.applyDungeons.end(); ++it)
                data << uint32(*it);
        }
        data << GetPlayer()->m_lookingForGroup.comment;
    }
    SendPacket(&data);
}

// handle finished
void WorldSession::HandleLfgJoin(WorldPacket &recv_data)
{
	sLog.outDebug("CMSG_LFG_JOIN");

	uint8 roles;
	uint8 max_instances, instance_mask;
	
	recv_data >> roles;
	recv_data.read_skip<uint8>();
	recv_data.read_skip<uint32>();
	recv_data >> max_instances;
	recv_data >> instance_mask;
	for(uint8 i=0;i<max_instances;i++)
		recv_data.read_skip<uint32>(); // 3 bytes per instance
	
	recv_data.read_skip<uint32>(); // only zeros, end of packets
	recv_data.hexlike();
	
	if(!GetPlayer())
		return;

	sLFGMgr.AddPlayerToRandomQueue(GetPlayer(),LFG_Role(roles));
}

void WorldSession::HandleLfgLeave(WorldPacket &recv_data)
{
	sLog.outDebug("CMSG_LFG_LEAVE");
	recv_data.hexlike();

	if(!GetPlayer())
		return;

	sLFGMgr.RemovePlayerFromRandomQueue(GetPlayer());
}

void WorldSession::HandleLfgSetRoles(WorldPacket &recv_data)
{
	sLog.outDebug("CMSG_LFG_SET_ROLES");
	recv_data.hexlike();

	uint8 roles;
	recv_data >> roles;
	if(!GetPlayer())
		return;

	GetPlayer()->m_lookingForGroup.roles = roles;
}

void WorldSession::HandleLfgSetBootVote(WorldPacket &recv_data)
{
	sLog.outDebug("CMSG_LFG_SET_BOOT_VOTE");
	recv_data.hexlike();

	bool agree;
	recv_data >> agree;

	// TODO : handle this
}

void WorldSession::HandleLfgTeleport(WorldPacket &recv_data)
{
	sLog.outDebug("CMSG_LFG_TELEPORT");
	recv_data.hexlike();

	bool teleport;
	recv_data >> teleport;
	if(!teleport && !GetPlayer())
		return;

	sLFGMgr.TeleportPlayerToInstance(GetPlayer());
}

void WorldSession::HandleLfgProposalResult(WorldPacket &recv_data)
{
	sLog.outDebug("CMSG_LFG_PROPOSAL_RESULT");

	uint32 groupId;
	bool accept;

	recv_data >> groupId;
	recv_data >> accept;

	recv_data.hexlike();

	if(!GetPlayer())
		return;

	return;

	if(GetPlayer()->m_lookingForGroup.group)
		GetPlayer()->m_lookingForGroup.group->PlayerAccept(GetPlayer(),accept);

	if(!accept)
	{
		sLFGMgr.RemovePlayerFromRandomQueue(GetPlayer());
		SendLfgUpdatePlayer(LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
	}
	else
	{
		// something here ?
	}
}	