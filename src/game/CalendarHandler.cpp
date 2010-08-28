/*
 * Copyright (C) 2005-2010 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "Calendar.h"
#include "Log.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"

void WorldSession::HandleCalendarGetCalendar(WorldPacket &recv_data)
{
	if(!GetPlayer())
		return;

    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_CALENDAR");     // empty
	for(uint8 i=0;i<10;i++)
	{
	WorldPacket data(CMSG_CALENDAR_GET_CALENDAR);
	SendPacket(&data);
	}
	sCalendarMgr.Send(GetPlayer());
}

void WorldSession::HandleCalendarGetEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_EVENT");
    recv_data.hexlike();
    uint64 eventId = 0;
    recv_data >> eventId;

	CalendarEvent* cEvent = sCalendarMgr.getEventById(eventId);
	if(!cEvent)
	{
		sLog.outError("Player tries to get non existing calendar event !");
		return;
	}
	sCalendarMgr.SendEvent(cEvent,GetPlayer(),false);
}

void WorldSession::HandleCalendarGuildFilter(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GUILD_FILTER");
    recv_data.hexlike();
    uint32 unk1,unk2,unk3;
    recv_data >> unk1;                          // unk1
    recv_data >> unk2;                          // unk2
    recv_data >> unk3;                          // unk3
}

void WorldSession::HandleCalendarArenaTeam(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_ARENA_TEAM");
    recv_data.hexlike();
    uint32 unk1;
    recv_data >> unk1;                         // unk
}

void WorldSession::HandleCalendarAddEvent(WorldPacket &recv_data)
{
	if(!GetPlayer())
		return;
    sLog.outDebug("WORLD: CMSG_CALENDAR_ADD_EVENT");
    recv_data.hexlike();
    
	std::string title,description;
	uint8 type,unk1;
	uint32 maxInvites,date,unk4,flags;
	int32 pve_type;

	recv_data >> title;
	recv_data >> description;
	recv_data >> type;
	recv_data >> unk1;
	recv_data >> maxInvites;
	recv_data >> pve_type;
	recv_data >> date;
	recv_data >> unk4;
	recv_data >> flags;

	error_log("Calendar Event unk1 %u maxInvites %u pve_type %i unk4 %u flags %u date %u",unk1,maxInvites,pve_type,unk4,flags,date);
	if (((flags >> 6) & 1) != 0)
		return;

	uint32 count = 0;
	recv_data >> count;

	CalendarEvent* cEvent = sCalendarMgr.CreateEvent(title,description,EventType(type),PveType(pve_type),date,CalendarEventFlags(flags),GetPlayer()->GetGUID());
	if(cEvent)
	{
		for(uint32 i=0;i<count;i++)
		{
			uint64 invitedGUID;
			uint8 status,status2;
			recv_data.readPackGUID(invitedGUID);
			recv_data >> status;
			recv_data >> status2;
			cEvent->AddMember(invitedGUID,State(status),State2(status2));
		}	
		if(flags & EVENT_GUILD)
		{
			if(Guild* guild = GetPlayer()->getGuild())
			{
				guild->RegisterCalendarEvent(cEvent);
				guild->BroadcastEventToGuild(cEvent->getId());
				CharacterDatabase.PExecute("UPDATE calendar_events SET guild = %u",GetPlayer()->GetGuildId());
			}
		}
		else
			GetPlayer()->RegisterCalendarEvent(cEvent);

		sCalendarMgr.Send(GetPlayer());
		sCalendarMgr.SendEvent(cEvent,GetPlayer(),true);
	}
}

uint32 CalendarMgr::ReadCalendarEventCreationValues(WorldPacket& data)
{
	if(data.GetOpcode() != CMSG_CALENDAR_ADD_EVENT)
	{
		uint64 eventId,creatorGuid;
		data >> eventId;
		data >> creatorGuid;		
	}
	
	std::string title,description;
	data >> title;
	data >> description;
	uint8 type,unk1;
	data >> type;
	data >> unk1;
	uint32 maxInvites,unk2,unk3,unk4,flags;
	data >> maxInvites;
	data >> unk2;
	data >> unk3;
	data >> unk4;
	data >> flags;
	return flags;
}

void WorldSession::HandleCalendarUpdateEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_UPDATE_EVENT");
    recv_data.hexlike();

	sCalendarMgr.ReadCalendarEventCreationValues(recv_data);
}

void WorldSession::HandleCalendarRemoveEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_REMOVE_EVENT");
    recv_data.hexlike();
	
	uint64 eventId,creatorGuid;
	uint32 flags;
    recv_data >> eventId;
    recv_data >> creatorGuid;
    recv_data >> flags;
	sCalendarMgr.RemoveCalendarEvent(eventId);
}

void WorldSession::HandleCalendarCopyEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_COPY_EVENT");
    recv_data.hexlike();
 
	uint64 oldId,newId;
	uint32 unk1;
    recv_data >> oldId;
    recv_data >> newId;
    recv_data >> unk1;

	error_log("CMSG_CALENDAR_COPY_EVENT %u %u %u",oldId,newId,unk1);
}

void WorldSession::HandleCalendarEventInvite(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_INVITE");
    recv_data.hexlike();

	uint64 guid,eventId;
	std::string playername;
	uint8 unk1,unk2;
    recv_data >> guid;
    recv_data >> eventId;
    recv_data >> playername;
    recv_data >> unk1;
    recv_data >> unk2;
	normalizePlayerName(playername);
	debug_log("Guid : %u Event Id : %u unk1 : %u unk2 : %u",guid,eventId,unk1,unk2);
	
	uint64 invitedPlayerGuid = sObjectMgr.GetPlayerGUIDByName(playername);
	if(Player* pl = sObjectMgr.GetPlayer(invitedPlayerGuid))
	{
		uint64 inviteId = 1;
		bool unk3 = false;
		uint32 unk4 = 0;
		uint8 unk5 = 0;

		WorldPacket data(SMSG_CALENDAR_EVENT_INVITE);
		data.appendPackGUID(guid);
		data << uint64(inviteId);
		data << uint64(eventId);
		data << uint8(unk1);
		data << uint8(unk2);
		data << uint8(unk3);
		if(unk3)
		{
			data << uint32(unk4);
		}
		data << uint8(unk5);
		pl->GetSession()->SendPacket(&data);
		//SendPacket(&data);
	}
}

void WorldSession::HandleCalendarEventRsvp(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_RSVP");
    recv_data.hexlike();

	uint64 guid,eventId,unk1;
    recv_data >> guid;
    recv_data >> eventId;
    recv_data >> unk1;

}

void WorldSession::HandleCalendarEventRemoveInvite(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_REMOVE_INVITE");
    recv_data.hexlike();

	uint64 guid,eventId,unk1,unk2;
    recv_data.readPackGUID(guid);
    recv_data >> eventId;
    recv_data >> unk1;
    recv_data >> unk2;
    
    WorldPacket data(SMSG_CALENDAR_EVENT_INVITE_REMOVED);
    data << guid << eventId;
    uint32 flags = 0;
    uint8 unk3 = 0;
    data << flags << unk3;
}

void WorldSession::HandleCalendarEventStatus(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_STATUS");
    recv_data.hexlike();

	uint64 guid,eventId,unk1,unk2;
	uint32 unk3;
    recv_data.readPackGUID(guid);
    recv_data >> eventId;
    recv_data >> unk1;
    recv_data >> unk2;
    recv_data >> unk3;
    
    WorldPacket data(SMSG_CALENDAR_EVENT_STATUS);
    data << guid << eventId;
    uint32 unk4=0,flags=0,unk7=0;
    data << unk4 << flags;
    uint8 unk5=0,unk6=0;
    data << unk5 << unk6 << unk7;
}

void WorldSession::HandleCalendarEventModeratorStatus(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_MODERATOR_STATUS");
    recv_data.hexlike();

	uint64 guid,eventId,unk1,unk2;
	uint32 unk3;
    recv_data.readPackGUID(guid);
    recv_data >> eventId;
    recv_data >> unk1;
    recv_data >> unk2;
    recv_data >> unk3;
}

void WorldSession::HandleCalendarComplain(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_COMPLAIN");
    recv_data.hexlike();

	uint64 eventId,guid;
    recv_data >> eventId;
    recv_data >> guid;
}

void WorldSession::HandleCalendarGetNumPending(WorldPacket & /*recv_data*/)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_NUM_PENDING");  // empty

	// if updates
	sCalendarMgr.SendCalendarFlash(GetPlayer());
}
