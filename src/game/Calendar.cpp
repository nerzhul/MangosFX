/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
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

#include <Policies/SingletonImp.h>
#include "InstanceSaveMgr.h"
#include "Calendar.h"

INSTANTIATE_SINGLETON_1( CalendarMgr );

void CalendarMgr::Send(Player* plr)
{
	time_t cur_time = time(NULL);

    WorldPacket data(SMSG_CALENDAR_SEND_CALENDAR,4+4*0+4+4*0+4+4);

	uint32 invite_count = 0;
	uint32 event_count = plr->GetCalendarEvents().size();
	uint32 raid_reset_count = 0;
	uint32 holiday_count = 0;
    data << (uint32) invite_count;                           //invite node count
    for (int i = 0; i < invite_count; i++)
    {
		uint64 inviteId=0,eventId=0;
		uint8 unk1=0,unk2=0,unk3=0;
		uint64 creatorGuid=0;
		data << inviteId << eventId << unk1 << unk2 << unk3 << creatorGuid;		
	}

    data << (uint32) event_count;                            //event count
    
	CalendarEventSet cEventSet = plr->GetCalendarEvents();
	for (CalendarEventSet::iterator itr = cEventSet.begin(); itr != cEventSet.end(); ++itr)
    {
		data << uint64((*itr)->getId());
		data << (*itr)->getTitle();
		data << uint32((*itr)->getType());
		data << uint32((*itr)->getDate());
		data << uint32(0);
		data << uint32((*itr)->getPveType());
		data.appendPackGUID((*itr)->getCreator());
	}

    data << (uint32) 0;                                     //wtf??
    data << (uint32) secsToTimeBitFields(cur_time);         // current time

    uint32 counter = 0;
    size_t p_counter = data.wpos();
    data << uint32(counter);                                // instance save count

    for(int i = 0; i < MAX_DIFFICULTY; ++i)
    {
        for (Player::BoundInstancesMap::const_iterator itr = plr->m_boundInstances[i].begin(); itr != plr->m_boundInstances[i].end(); ++itr)
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
		uint32 mapId = 530,resetTime = 0,unkTime = 0;
		data << mapId << resetTime << unkTime;
	}
    data << (uint32) holiday_count;                   // unk counter 5
    for (int i = 0; i < holiday_count; i++)
    {
		uint32 unk1=0,unk2=0,unk3=0,unk4=0,unk5=0;
		std::string holidayName ="";
		data << unk1 << unk2 << unk3 << unk4 << unk5;
		for (int j = 0; j < 26; j++)
			data << uint32(0);
		for (int j = 0; j < 10; j++)
			data << uint32(0);
		for (int j = 0; j < 10; j++)
			data << uint32(0);
			
		data << holidayName;
	}
    sLog.outDebug("Sending calendar");
    plr->GetSession()->SendPacket(&data);
}

void CalendarMgr::LoadCalendarEvents()
{
	QueryResult* result = CharacterDatabase.Query("SELECT `id`,`title`,`desc`,`type`,`date`,`ptype`,`flags`,`creator` FROM calendar_events");
	if(!result)
		return;

	do
	{
		Field *fields = result->Fetch();
		uint64 eventId = fields[0].GetUInt64();
		std::string title = fields[1].GetCppString();
		std::string desc = fields[2].GetCppString();
		uint8 type = fields[3].GetUInt8();
		uint32 date = fields[4].GetUInt32();
		uint8 ptype = fields[5].GetUInt8();
		uint8 flags = fields[6].GetUInt8();
		uint64 creator = fields[7].GetUInt64();

		CalendarEvent* cEvent = new CalendarEvent(title,desc,EventType(type),PveType(ptype),date,CalendarEventFlags(flags),creator);
		cEvent->setId(eventId);
		m_calendarEvents[eventId] = cEvent;
	}
	while(result->NextRow());
}

CalendarEvent* CalendarMgr::CreateEvent(std::string title, std::string desc, EventType type, PveType ptype, uint32 date, CalendarEventFlags flags, uint64 guid)
{
	CalendarEvent* cEvent = new CalendarEvent(title,desc,type,ptype,date,flags,guid);
	uint32 eventId = sObjectMgr.GenerateCalendarEventId();
	cEvent->setId(eventId);
	m_calendarEvents[eventId] = cEvent;

	CharacterDatabase.escape_string(title);
	CharacterDatabase.escape_string(desc);
	CharacterDatabase.PExecute("INSERT INTO calendar_events(`id`,`title`,`desc`,`type`,`date`,`ptype`,`flags`,`creator`) VALUES "
		"('%u','%s','%s','%u','%u','%u','%u','" UI64FMTD "')", eventId, title.c_str(), desc.c_str(), type, date, ptype, flags, guid);

	CharacterDatabase.PExecute("INSERT INTO character_calendar_events(`guid`,`eventid`,`status`) VALUES "
		"('" UI64FMTD "','%u',1)", guid, eventId);
	return cEvent;
}

CalendarEvent* CalendarMgr::getEventById(uint64 id)
{
	cEventMap::iterator itr = m_calendarEvents.find(id);
	if(itr == m_calendarEvents.end())
		return NULL;

	return itr->second;
}