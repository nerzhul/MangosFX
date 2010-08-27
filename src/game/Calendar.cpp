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
#include "Player.h"
#include "Guild.h"
#include "Opcodes.h"
#include "Calendar.h"
#include "ObjectMgr.h"
#include "World.h"

INSTANTIATE_SINGLETON_1( CalendarMgr );

void CalendarMgr::Send(Player* plr)
{
	time_t cur_time = time(NULL);

	cEventMap cPlayerEventMap = plr->GetCalendarEvents();
	cEventMap cGuildEventMap;
    if(Guild *guild = plr->getGuild())
		cGuildEventMap = guild->GetCalendarEvents();
	else
		cGuildEventMap.clear();


    WorldPacket data(SMSG_CALENDAR_SEND_CALENDAR,600);

	uint32 invite_count = 0;
	uint32 holiday_count = 0;

    data << (uint32) invite_count;                           //invite node count
    for (int i = 0; i < invite_count; i++)
    {
		uint64 inviteId=0,eventId=0;
		uint8 unk1=0,unk2=0,unk3=0;
		uint64 creatorGuid=0;
		data << inviteId << eventId << unk1 << unk2 << unk3 << creatorGuid;		
	}

	data << uint32(cPlayerEventMap.size() + cGuildEventMap.size());                            //event count

	for (cEventMap::iterator itr = cPlayerEventMap.begin(); itr != cPlayerEventMap.end(); ++itr)
    {
		data << uint64((*itr).first);
		data << std::string((*itr).second->getTitle());
		data << uint32((*itr).second->getType());
		data << uint32((*itr).second->getDate());
		data << uint32((*itr).second->getFlags());
		data << int32((*itr).second->getPveType());
		data.appendPackGUID((*itr).second->getCreator());
	}

	for (cEventMap::iterator itr = cGuildEventMap.begin(); itr != cGuildEventMap.end(); ++itr)
    {
		data << uint64((*itr).first);
		data << std::string((*itr).second->getTitle());
		data << uint32((*itr).second->getType());
		data << uint32((*itr).second->getDate());
		data << uint32((*itr).second->getFlags());
		data << int32((*itr).second->getPveType());
		data.appendPackGUID((*itr).second->getCreator());
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

	counter = 0;
    p_counter = data.wpos();
    data << uint32(counter);  

	// FAIL
    /*ResetTimeByMapDifficultyMap const& resets = sInstanceSaveMgr.GetResetTimeMap();
    for (ResetTimeByMapDifficultyMap::const_iterator itr = resets.begin(); itr != resets.end(); ++itr)
    {
        uint32 mapid = PAIR32_LOPART(itr->first);
        MapEntry const* mapEnt = sMapStore.LookupEntry(mapid);
        if (!mapEnt || !mapEnt->IsRaid())
            continue;

        data << uint32(mapid);
        data << uint32(itr->second - cur_time);
        data << uint32(mapEnt->reset_time);
        ++counter;
    }*/

    data.put<uint32>(p_counter, counter);

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
    plr->GetSession()->SendPacket(&data);
}

void CalendarMgr::SendCalendarFlash(Player* plr)
{
	if(!plr || !plr->GetSession())
		return;

	WorldPacket data(SMSG_CALENDAR_SEND_NUM_PENDING, 4);
    data << uint32(1);
	plr->GetSession()->SendPacket(&data);
}

void CalendarMgr::LoadCalendarEvents()
{
	QueryResult* result = CharacterDatabase.Query("SELECT `id`,`title`,`desc`,`type`,`date`,`ptype`,`flags`,`creator`,`guild` FROM calendar_events");
	if(!result)
		return;

	uint32 nb = 0;
	uint32 nbg = 0;
	do
	{
		Field *fields = result->Fetch();
		uint64 eventId = fields[0].GetUInt64();
		std::string title = fields[1].GetCppString();
		std::string desc = fields[2].GetCppString();
		uint8 type = fields[3].GetUInt8();
		uint32 date = fields[4].GetUInt32();
		int8 ptype = fields[5].GetUInt8();
		uint8 flags = fields[6].GetUInt8();
		uint64 creator = fields[7].GetUInt64();
		uint32 guildid = fields[8].GetUInt32();

		CalendarEvent* cEvent = new CalendarEvent(title,desc,EventType(type),PveType(ptype),date,CalendarEventFlags(flags),creator);
		cEvent->setId(eventId);
		if(guildid > 0)
		{
			m_guildCalendarEvents[eventId] = cEvent;
			if(Guild* guild = sObjectMgr.GetGuildById(guildid))
			{
				m_guildCalendarEvents[eventId] = cEvent;
				guild->RegisterCalendarEvent(cEvent);
			}
			else
				RemoveCalendarEvent(eventId);
			nbg++;
		}
		else
		{
			m_calendarEvents[eventId] = cEvent;
			nb++;
		}
	}
	while(result->NextRow());

	sLog.outString("Loaded %u Player Calendar Events",nb);
	sLog.outString("Loaded %u Guild Calendar Events",nbg);
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

void CalendarMgr::RemoveCalendarEvent(uint64 eventId)
{
	CalendarEvent* cEvent = sCalendarMgr.getEventById(eventId);
	if(!cEvent)
		return;
	sWorld.RemoveCalendarEventFromActiveSessions(cEvent);
	CharacterDatabase.PExecute("DELETE FROM calendar_events WHERE id = '%u'",eventId);
	CharacterDatabase.PExecute("DELETE FROM character_calendar_events WHERE eventid = '%u'",eventId);
	m_calendarEvents.erase(cEvent->getId());
	m_guildCalendarEvents.erase(cEvent->getId());
	delete cEvent;
}

CalendarEvent* CalendarMgr::getEventById(uint64 id)
{
	cEventMap::iterator itr = m_calendarEvents.find(id);
	if(itr == m_calendarEvents.end())
		return NULL;

	return itr->second;
}