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
    
	//recv_data.print_storage();
    std::string name, desc;
	recv_data >> name;
    recv_data >> desc;

	uint8   type, unk4;
    uint32  maxinvites, unk6, date1, date2, flags, count = 0;
    recv_data >> (uint8)type;
    recv_data >> (uint8)unk4;
    recv_data >> (uint32)maxinvites;
    recv_data >> (uint32)unk6;
    recv_data >> (uint32)date1;
    recv_data >> (uint32)date2;
    recv_data >> (uint32)flags;

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
	
	WorldPacket data(SMSG_CALENDAR_SEND_CALENDAR);
	HandleSendCalendarEvent(data);
	SendPacket(&data);
}

void WorldSession::HandleSendCalendarEvent(WorldPacket &data)
{
		uint8 CreateType = 0;

        data << CreateType;

        uint64 CreatorGUID = 12;
        uint64 EventGUID = 1;
		std::string EventName = "test", EventDescription = "test2";
        uint8 EventType = 1, EventUnk1 = 1;
        uint32 Flags1 = 0, Flags2 = 0, EventFlags = 0;
        uint32 SubInfo1 = 0, SubInfo2 = 0;
        uint32 Unk1 = 0, InvitedCount = 0;

        data << CreatorGUID;
        data << EventGUID;
        data << EventName;
        data << EventDescription;
        data << EventType;
        data << EventUnk1;
        data << Flags1;
        data << Flags2;
        data << EventFlags;
        data << SubInfo1;
        data << SubInfo2;
        data << Unk1;
        data << InvitedCount;

        for (uint32 m = 0; m < InvitedCount; ++m)
        {
                uint64 MemberGUID;
                uint8 mUnk1, mUnk2, mUnk3, mUnk4;
                uint64 mUnkGuid;
                uint32 mUnk5;
				std::string mUnkStr;

                data << MemberGUID;
                data << mUnk1;
                data << mUnk2;
                data << mUnk3;
                data << mUnk4;
                data << mUnkGuid;
                data << mUnk5;
                data << mUnkStr;
        }
}

void WorldSession::HandleCalendarUpdateEvent(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: CMSG_CALENDAR_UPDATE_EVENT");
    recv_data.hexlike();

	uint64 eventId,creatorGuid;
	std::string title,desc;
	uint8 type,unk1;
	uint32 maxInvites,unk2,date1,date2,flags;
    recv_data >> eventId;
    recv_data >> creatorGuid;
    recv_data >> title;
    recv_data >> desc;
    recv_data >> type;
    recv_data >> unk1;
    recv_data >> maxInvites;
    recv_data >> unk2;
    recv_data >> date1;
    recv_data >> date2;
    recv_data >> flags;
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
