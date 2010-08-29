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

class Player;
class CalendarEvent;

#define MAX_INVITES 100
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
	EVENT_ANN			= 0x00000040,
	EVENT_GUILD			= 0x00000400,
};

enum PveType
{
	PVETYPE_NONE				= -1,
	PVETYPE_ARCHAVON_25			= 240,
	PVETYPE_RUBIS_SANCTUM_10	= 293,
	PVETYPE_RUBIS_SANCTUM_25	= 294,
};

enum State
{
	REFUSED		= 0,
	CONFIRMED	= 1,
	ACCEPTED	= 3,
	EXIT		= 4,
	AWAITING	= 5,
	NOTSURE		= 8,
};

enum State2
{
	REFUSED2	= 0,
	VALID		= 2,
};

typedef std::map<uint64,CalendarEvent*> cEventMap;
class CalendarMgr
{
	public:

		CalendarEvent* CreateEvent(std::string title, std::string desc, EventType type, PveType ptype, uint32 date, CalendarEventFlags flags, uint64 guid);
		
		uint32 ReadCalendarEventCreationValues(WorldPacket& data);
		void Send(Player* plr);
		void SendCalendarFlash(Player* plr);

		cEventMap getAllCalendarEvents() { return m_calendarEvents; }
		cEventMap getAllGuildCalendarEvents() { return m_guildCalendarEvents; }
		cEventMap getGuildEvents(uint32 guild);
		CalendarEvent* getEventById(uint64 id);
		CalendarEvent* getPlayerEventById(uint64 id);
		CalendarEvent* getGuildEventById(uint64 id);
		void LoadCalendarEvents();
		void RemoveCalendarEvent(uint64 eventId);
		void SendEvent(CalendarEvent* cEvent, Player* plr, bool create);
	private:
		cEventMap m_calendarEvents;
		cEventMap m_guildCalendarEvents;
};

struct MemberStatus
{
	State status;
	State2 status2;
};

typedef std::map<uint64,MemberStatus> CalEventMemberList;
class CalendarEvent
{
	public:
		CalendarEvent(std::string title, std::string desc, EventType type, PveType ptype, uint32 date, CalendarEventFlags flags, uint64 guid) : 
					 m_title(title),	m_desc(desc),		m_type(type),	m_ptype(ptype), m_date(date), m_flags(flags), m_creatorGUID(guid), 
							 m_Id(0)
		{
			m_memberList.clear();
		}

		void setId(uint64 id) { m_Id = id; }

		const char* getTitle() { return m_title.c_str(); }
		void setTitle(std::string title) { m_title = title; }
		const char* getDescription() { return m_desc.c_str(); }
		void setDescription(std::string desc) { m_desc = desc; }
		EventType getType() { return m_type; }
		void setEventType(EventType eType) { m_type = eType; }
		PveType getPveType() { return m_ptype; }
		void setPveType(PveType pType) { m_ptype = pType; }
		uint64 getCreator() { return m_creatorGUID; }
		CalendarEventFlags getFlags() { return m_flags; }
		uint64 getId() { return m_Id; }
		uint32 getDate() { return m_date; }
		void setDate(uint32 date) { m_date = date; }
		void AddMember(uint64 guid, State st, State2 st2);
		void DelMember(uint64 guid);
		void UpdateStatus(uint64 guid, State st, State2 st2);
		CalEventMemberList getMemberList() { return m_memberList; }

	private:
		std::string m_title;
		std::string m_desc;
		EventType m_type;
		PveType m_ptype;
		uint64 m_creatorGUID;
		uint64 m_Id;
		CalendarEventFlags m_flags;
		uint32 m_date;
		CalEventMemberList m_memberList;
};

#define sCalendarMgr MaNGOS::Singleton<CalendarMgr>::Instance()
#endif
