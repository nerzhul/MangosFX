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

#ifndef MANGOS_CALENDAR_H
#define MANGOS_CALENDAR_H

#include "WorldPacket.h"

class CalendarEvent;

enum EventType
{
	EVENT_RAID			= 0,
	EVENT_DUNGEON		= 1,
	EVENT_PVP			= 2,
	EVENT_MEETING		= 3,
	EVENT_OTHER			= 4,
};

enum CalendarEventFlags
{
	EVENT_UNK1			= 0x00000001,
	EVENT_LOCK			= 0x00000010,
};

enum PveType
{
	PVETYPE_ARCHAVON_25			= 240,
	PVETYPE_RUBIS_SANCTUM_10	= 293,
	PVETYPE_RUBIS_SANCTUM_25	= 294,
};

typedef std::map<uint32,CalendarEvent*> cEventMap;
class CalendarMgr
{
	public:

		CalendarEvent* CreateEvent(std::string title, std::string desc, EventType type, PveType ptype, uint32 date, CalendarEventFlags flags, uint64 guid);
		
		uint32 ReadCalendarEventCreationValues(WorldPacket& data);
		void Send(Player* plr);
		void SendCalendarFlash(Player* plr);

		cEventMap getAllCalendarEvents() { return m_calendarEvents; }
		CalendarEvent* getEventById(uint64 id);
		void LoadCalendarEvents();
	private:
		cEventMap m_calendarEvents;
};

class CalendarEvent
{
	public:
		CalendarEvent(std::string title, std::string desc, EventType type, PveType ptype, uint32 date, CalendarEventFlags flags, uint64 guid) : 
					 m_title(title),	m_desc(desc),		m_type(type),	m_ptype(ptype), m_date(date), m_flags(flags), m_creatorGUID(guid), 
							 m_Id(0)
		{
		}

		void setId(uint64 id) { m_Id = id; }

		const char* getTitle() { return m_title.c_str(); }
		const char* getDescription() { return m_desc.c_str(); }
		EventType getType() { return m_type; }
		PveType getPveType() { return m_ptype; }
		uint64 getCreator() { return m_creatorGUID; }
		CalendarEventFlags getFlags() { return m_flags; }
		uint64 getId() { return m_Id; }
		uint32 getDate() { return m_date; }

	private:
		std::string m_title;
		std::string m_desc;
		EventType m_type;
		PveType m_ptype;
		uint64 m_creatorGUID;
		uint64 m_Id;
		CalendarEventFlags m_flags;
		uint32 m_date;
};

#define sCalendarMgr MaNGOS::Singleton<CalendarMgr>::Instance()
#endif
