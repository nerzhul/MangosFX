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

#ifndef MANGOS_INSTANCE_DATA_H
#define MANGOS_INSTANCE_DATA_H

#include "Common.h"

#include "Object.h"
#include "Unit.h"
#include "Creature.h"
#include "Map.h"

class Player;
class GameObject;

#define DATA_NB_BOSS_DOWN 999

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4
};

#define OUT_SAVE_INST_DATA             debug_log("LibDevFS: Saving Instance Data for Instance %s (Map %d, Instance Id %d)", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE    debug_log("LibDevFS: Saving Instance Data for Instance %s (Map %d, Instance Id %d) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a)          debug_log("LibDevFS: Loading Instance Data for Instance %s (Map %d, Instance Id %d). Input is '%s'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE    debug_log("LibDevFS: Instance Data Load for Instance %s (Map %d, Instance Id: %d) is complete.",instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL        error_log("LibDevFS: Unable to load Instance Data for Instance %s (Map %d, Instance Id: %d).",instance->GetMapName(), instance->GetId(), instance->GetInstanceId())


class MANGOS_DLL_SPEC InstanceData
{
    public:

        explicit InstanceData(Map *map) : instance(map), m_dungeonEntry(0) {}
        virtual ~InstanceData() {}

        Map* instance;
		uint32 m_dungeonEntry;

		void SetLFGDungeon(uint32 dungeonEntry) { m_dungeonEntry = dungeonEntry; }
		uint32 GetLFGDungeon() { return m_dungeonEntry; }

        //On creation, NOT load.
        virtual void Initialize() {}

        //On load
        virtual void Load(const char* /*data*/) {}

        //When save is needed, this function generates the data
        virtual const char* Save() { return ""; }

        void SaveToDB();

        //Called every map update
        virtual void Update(uint32 /*diff*/) {}

        //Used by the map's CanEnter function.
        //This is to prevent players from entering during boss encounters.
        virtual bool IsEncounterInProgress() const { return false; };

        //Called when a player successfully enters the instance (after really added to map)
        virtual void OnPlayerEnter(Player *) {}

		//Called when a player dies inside instance
		virtual void OnPlayerDeath(Player *) {}

		//Called when a player leaves the instance (before really removed from map (or possibly world))
		virtual void OnPlayerLeave(Player *) {}

		//called on creature enter combat
		virtual void OnCreatureEnterCombat(Creature * /*creature*/) {}
		
		//called on creature evade
		virtual void OnCreatureEvade(Creature * /*creature*/) {}
		
		//called on creature death
		virtual void OnCreatureDeath(Creature * /*creature*/) {}
		
		//Called when a gameobject is created
        virtual void OnObjectCreate(GameObject *) {}

        //called on creature creation
        virtual void OnCreatureCreate(Creature * /*creature*/) {}

        //All-purpose data storage 64 bit
        virtual uint64 GetData64(uint32 /*Data*/) { return 0; }
        virtual void SetData64(uint32 /*Data*/, uint64 /*Value*/) { }

        //All-purpose data storage 32 bit
        virtual uint32 GetData(uint32 /*Type*/) { return 0; }
        virtual void SetData(uint32 /*Type*/, uint32 /*Data*/) {}

        // Achievement criteria additional requirements check
        // NOTE: not use this if same can be checked existed requirement types from AchievementCriteriaRequirementType
        virtual bool CheckAchievementCriteriaMeet(uint32 /*criteria_id*/, Player const* /*source*/, Unit const* /*target*/ = NULL, uint32 /*miscvalue1*/ = 0);

		// Condition criteria additional requirements check
        // This is used for such things are heroic loot
        virtual bool CheckConditionCriteriaMeet(Player const* source, uint32 map_id, uint32 instance_condition_id);

		// Achievements
		void CompleteAchievementForGroup(uint32 AchId);
		void CompleteAchievementForPlayer(Player* plr, uint32 AchId);

		// World States
		void DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData);
		
		// GameObjects
		void CloseDoor(uint64 guid);
		void OpenDoor(uint64 guid);
		GameObject* GetGoInMap(uint64 guid)	{ return instance ? instance->GetGameObject(guid) : NULL; }
        void DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime = 0, bool bUseAlternativeState = false);
		void DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn = MINUTE);
	
		// Units
		Unit* GetUnitInMap(uint64 guid) { return instance ? instance->GetCreatureOrPetOrVehicle(guid) : NULL; }
		Creature* GetCreatureInMap(uint64 guid)	
		{
			Unit* u = GetUnitInMap(guid);
			if(!u) return NULL;
			return (u->GetTypeId() == TYPEID_UNIT) ? (Creature*)GetUnitInMap(guid) : NULL; 
		}
		void DespawnCreatures(std::vector<uint64> &spVect)
		{
			for (std::vector<uint64>::iterator itr = spVect.begin(); itr != spVect.end();++itr)
			{
				if(Creature* cr = GetCreatureInMap(*itr))
				{
					cr->ForcedDespawn(2000);
					cr->SetPhaseMask(0x2,true);
				}
			}
			spVect.clear();
		}
		void DoSpeak(Unit* pwho, uint32 soundid, std::string text, uint8 type);
		void AutoFreeze(Creature* cr);

		// Player Selection
		Player* GetClosestPlayer(Unit* u, float maxRange = 100.0f);
};
#endif
