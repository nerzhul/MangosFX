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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "InstanceSaveMgr.h"

void WorldSession::HandleCalendarGetCalendar(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_CALENDAR");     // empty

    time_t cur_time = time(NULL);

    WorldPacket data(SMSG_CALENDAR_SEND_CALENDAR,4+4*0+4+4*0+4+4);

	uint32 invite_count = 0;
	uint32 event_count = 0;
	uint32 raid_reset_count = 0;
	uint32 holiday_count = 0;
    data << (uint32) invite_count                           //invite node count
    for (int i = 0; i < invite_count; i++)
    {
		uint64 inviteId,eventId;
		uint8 unk1,unk2,unk3;
		uint64 creatorGuid;
		data << inviteId << eventId << unk1 << unk2 << unk3 << creatorGuid;		
	}

    data << (uint32) event_count                            //event count
    
    for (int i = 0; i < event_count; i++)
    {
		uint64 eventId,creatorGuid;
		std::string title;
		uint32 type,occurrence,flags,unk1;
		data << eventId << title << type << occurrence << flags << unk1 << creatorGuid;
	}

    data << (uint32) 0;                                     //wtf??
    data << (uint32) secsToTimeBitFields(cur_time);         // current time

    uint32 counter = 0;
    size_t p_counter = data.wpos();
    data << uint32(counter);                                // instance save count

    for(int i = 0; i < MAX_DIFFICULTY; ++i)
    {
        for (Player::BoundInstancesMap::const_iterator itr = _player->m_boundInstances[i].begin(); itr != _player->m_boundInstances[i].end(); ++itr)
        {
            if(itr->second.perm)
            {
                InstanceSave *save = itr->second.save;
                data << uint32(save->GetMapId());
                data << uint32(save->GetDifficulty());
                data << uint32(save->GetResetTime() - cur_time);
                data << uint64(save->GetInstanceId());      // instance save id as unique instance copy id
                ++counter;
            }
        }
    }
    data.put<uint32>(p_counter,counter);

    data << (uint32) 1135753200;                            //wtf?? (28.12.2005 12:00)
    data << (uint32) raid_reset_count;                      //  unk counter 4
    for (int i = 0; i < raid_reset_count; i++)
    {
		uint32 mapId,resetTime,unkTime;
		data << mapId,resetTime,unkTime;
	}
    data << (uint32) holiday_reset_count;                   // unk counter 5
    for (int i = 0; i < holiday_reset_count; i++)
    {
		uint32 unk1,unk2,unk3,unk4,unk5;
		std::string holidayName;
		data << unk1 << unk3 << unk3 << unk4 << unk5;
		for (int j = 0; j < 26; j++)
			data << uint32(0);
		for (int j = 0; j < 10; j++)
			data << uint32(0);
		for (int j = 0; j < 10; j++)
			data << uint32(0);
			
		data << holidayName;
	}
    sLog.outDebug("Sending calendar");
    SendPacket(&data);
}

void WorldSession::HandleCalendarGetEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_GET_EVENT");
    recv_data.hexlike();
    uint64 eventId = 0;
    recv_data >> eventId;
    // TODO: answer
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
    sLog.outDebug("WORLD: CMSG_CALENDAR_ADD_EVENT");
    recv_data.hexlike();
    
    uint32 flags = ReadCalendarEventCreationValues(recv_data);

    if (!((flags >> 6) & 1))
    {
        recv_data >> (uint32)count;
        if (count > 0)
        {
            uint8 status,rank;
            uint64 guid;
            for (int i=0;i<count;i++)
            {
                recv_data.readPackGUID(guid);
                recv_data >> (uint8)status;
                recv_data >> (uint8)rank;
           }
        }
    }
	
	
	uint8 unk1 = 0;
	uint32 maxInvites = 1;
	WorldPacket data(SMSG_CALENDAR_SEND_EVENT);
	data << unk1;
	data << maxInvites;
	for(int i = 0; i < maxinvites; i++)
	{
		uint64 guid2 = 0,inviteId = 1;
		uint8 unk10 = 0,unk11 = 0,unk12 = 0 ,unk13 = 0;
		uint32 unk14 = 0;
		std::string text = "Salut";
		data << guid2 << unk10 << unk11 << unk12 << unk13 << inviteId << unk14 << text;
	}
}

uint32 Calendar::ReadCalendarEventCreationValues(Worldpacket& data)
{
	if(data.GetOpcode() != CMSG_CALENDAR_ADD_EVENT)
	{
		uint64 eventId,creatorGuid;
		data >> eventId;
		data >> creatorGuid;		
	}
	
	str::string title,description;
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

	ReadCalendarEventCreationValues(Worldpacket& data)
}

void WorldSession::HandleCalendarRemoveEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_REMOVE_EVENT");
    recv_data.hexlike();
	
	uint64 eventId,creatorGuid;
	uint32 unk1;
    recv_data >> eventId;
    recv_data >> creatorGuid;
    recv_data >> unk1;

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

}

void WorldSession::HandleCalendarEventInvite(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_EVENT_INVITE");
    recv_data.hexlike();

	uint64 guid,eventId;
	std::string text;
	uint8 unk1,unk2;
    recv_data >> guid;
    recv_data >> eventId;
    recv_data >> text;
    recv_data >> unk1; // Maybe 1 guid 64
    recv_data >> unk2; //
	
	WorldPacket data(SMSG_CALENDAR_EVENT_INVITE);
	data << guid;
	uint64 inviteId = 0;
	data << inviteId << eventId << unk1 << unk2;
	bool unk3 = false;
	data << unk3;
	if(unk3)
	{
		uint32 unk4 = 0;
		data << uint32(unk4);
	}
	uint8 unk5 = 0;
	data << unk5;
	
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
    uint32 unk4,flags,unk7;
    data << unk4 << flags;
    uint8 unk5,unk6;
    data << unk6 << unk6 << unk7;
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

    WorldPacket data(SMSG_CALENDAR_SEND_NUM_PENDING, 4);
    data << uint32(0);                                      // 0 - no pending invites, 1 - some pending invites
    SendPacket(&data);
}
