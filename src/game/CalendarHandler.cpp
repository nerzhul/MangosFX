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

    //sLog.outDebug("WORLD: CMSG_CALENDAR_GET_CALENDAR");     // empty
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
	//if(!GetPlayer())
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

	CharacterDatabase.escape_string(title);
	CharacterDatabase.escape_string(description);

	error_log("Calendar Event unk1 %u pve_type %i unk4 %u flags %u date %u",unk1,pve_type,unk4,flags,date);
	if (((flags >> 6) & 1) != 0 && !(flags & EVENT_ANN))
	{
		error_log("Calendar Event: Unhandled added Event");
		return;
	}

	uint32 count = 0;
	if(!(flags & EVENT_ANN))
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
		if((flags & EVENT_GUILD) || (flags & EVENT_ANN))
		{
			if(Guild* guild = GetPlayer()->getGuild())
			{
				cEvent->setGuild(guild->GetId());
				guild->RegisterCalendarEvent(cEvent->getId());
				guild->BroadcastEventToGuild(cEvent->getId());
				CharacterDatabase.PExecute("UPDATE calendar_events SET guild = %u",GetPlayer()->GetGuildId());
			}
		}
		else
			GetPlayer()->RegisterCalendarEvent(cEvent->getId());

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

	//if(!GetPlayer())
		return;

	uint64 eventId = 0;
	uint64 creatorGUID;
	std::string title,desc;
	uint32 maxInvites,date,unk4,flags;
	uint16 unk3;
	int32 pve_type;

	recv_data >> eventId;
	recv_data >> creatorGUID;
	recv_data >> title;
	recv_data >> desc;
	recv_data >> unk3;
	recv_data >> maxInvites;
	recv_data >> pve_type;
	recv_data >> date;
	recv_data >> flags;
	recv_data >> unk4;
	CharacterDatabase.escape_string(title);
	CharacterDatabase.escape_string(desc);
	error_log("Unk3 : %u Unk4 : %u ",unk3,unk4);
	if(CalendarEvent* cEvent = sCalendarMgr.getEventById(eventId))
	{
		cEvent->setTitle(title);
		cEvent->setDescription(desc);
		//cEvent->setEventType(EventType(type));
		cEvent->setPveType(PveType(pve_type));
		cEvent->setFlags(CalendarEventFlags(flags));
		cEvent->setDate(date);
		CharacterDatabase.PExecute("UPDATE calendar_events SET `title` = '%s', `desc` = '%s', `date` = '%u', `ptype` = '%i' WHERE `id` = '"UI64FMTD"'",
			title.c_str(),desc.c_str(),date,pve_type,eventId);
		sCalendarMgr.Send(GetPlayer());
		sCalendarMgr.SendEvent(cEvent,GetPlayer(),false);
	}
	
}

void WorldSession::HandleCalendarRemoveEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_REMOVE_EVENT");
    recv_data.hexlike();
	
	if(!GetPlayer())
		return;
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
	uint64 guid,eventId;
	std::string playername;
	uint8 status,status2;
    recv_data >> eventId;
    recv_data >> guid;
    recv_data >> playername;
    recv_data >> status;
    recv_data >> status2;
	normalizePlayerName(playername);
	debug_log("Guid : " UI64FMTD"  Event Id : %u",guid,eventId);
	
	uint64 inviteId = 0;
	uint8 err = 0;
	uint32 errId = 0;
	guid = sObjectMgr.GetPlayerGUIDByName(playername);

	WorldPacket data(SMSG_CALENDAR_EVENT_INVITE);
	data.appendPackGUID(guid);
	data << uint64(/*inviteId*/0);
	data << uint64(eventId);
	data << uint8(1);
	data << uint8(0); // status
	data << uint8(0); // status2
	data << uint8(1);
	data.hexlike();
	SendPacket(&data);
}

void WorldSession::HandleCalendarEventRsvp(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_RSVP");
    recv_data.hexlike();

	uint64 guid,eventId;
	uint32 unk1;
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
