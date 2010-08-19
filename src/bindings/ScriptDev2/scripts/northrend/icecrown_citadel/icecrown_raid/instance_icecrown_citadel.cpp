/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Instance_Icecrown_Citadel
SD%Complete: 0
SDComment: Written by K
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

struct MANGOS_DLL_DECL instance_icecrown_citadel : public ScriptedInstance
{
    instance_icecrown_citadel(Map* pMap) : ScriptedInstance(pMap) {Initialize();}

    std::string strInstData;
    uint32 m_auiEncounter[MAX_ENCOUNTER];

    uint64 m_uiMarrowgarGUID;
	uint64 m_uiMarrowgarDoorGUID;
    uint64 m_uiDeathwhisperGUID;
    uint64 m_uiSaurfangGUID;
	uint64 m_uiFestergutGUID;
	uint64 m_uiRotfaceGUID;
	uint64 m_uiPutricideGUID;
	uint64 m_uiPrinceKelesethGUID;
	uint64 m_uiPrinceValanarGUID;
	uint64 m_uiPrinceTaldaramGUID;
	uint64 m_uiLanathelGUID;
	uint64 m_uiDreamWalkerGUID;
	uint64 m_uiSindragosaGUID;
	uint64 m_uiLichKingGUID;

	std::vector<uint64> rotfacePoolsGUIDs;

    uint64 m_uiMarrowgarIce1GUID;
    uint64 m_uiMarrowgarIce2GUID;
    uint64 m_uiDeathwhisperGateGUID;
    uint64 m_uiDeathwhisperElevatorGUID;
    uint64 m_uiSaurfangDoorGUID;
	uint64 m_uiFestergutDoorGUID;
	uint64 m_uiRotfaceDoorGUID;
	uint64 m_uiPutricideDoorGUID;
	uint64 m_uiBloodWingDoorGUID;
	uint64 m_uiPrinceCouncilDoorGUID;
	uint64 m_uiLanathelDoorGUID_1;
	uint64 m_uiLanathelDoorGUID_2;
	uint64 m_uiFrostWingDoorGUID;
	uint64 m_uiDreamWalkerDoorGUID;
	uint64 m_uiDreamWalkerExitDoorGUID;
	uint64 m_uiSindragosaDoorGUID;

	uint64 m_uiSaurfangCacheGUID;

	uint64 m_uiPlagueSigilGUID;
	uint64 m_uiBloodSigilGUID;
	uint64 m_uiFrostSigilGUID;

	uint64 m_uiOrangeTubeGUID;
	uint64 m_uiOrangeDoorGUID;

	uint32 checkPlayer_Timer;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiMarrowgarGUID               = 0;
		m_uiMarrowgarDoorGUID			= 0;
        m_uiDeathwhisperGUID            = 0;
        m_uiSaurfangGUID                = 0;
		m_uiFestergutGUID				= 0;
		m_uiRotfaceGUID					= 0;
		m_uiPutricideGUID				= 0;
		m_uiPrinceKelesethGUID			= 0;
		m_uiPrinceValanarGUID			= 0;
		m_uiPrinceTaldaramGUID			= 0;
		m_uiLanathelGUID				= 0;
		m_uiDreamWalkerGUID				= 0;
		m_uiSindragosaGUID				= 0;
		// Other IAs
		m_uiLichKingGUID				= 0;

		rotfacePoolsGUIDs.clear();

        m_uiMarrowgarIce1GUID           = 0;
        m_uiMarrowgarIce2GUID           = 0;
        m_uiDeathwhisperGateGUID        = 0;
        m_uiDeathwhisperElevatorGUID    = 0;
        m_uiSaurfangDoorGUID            = 0;
		m_uiFestergutDoorGUID			= 0;
		m_uiRotfaceDoorGUID				= 0;
		m_uiPutricideDoorGUID			= 0;
		m_uiPrinceCouncilDoorGUID		= 0;
		m_uiBloodWingDoorGUID			= 0;
		m_uiPrinceCouncilDoorGUID		= 0;
		m_uiLanathelDoorGUID_1			= 0;
		m_uiLanathelDoorGUID_2			= 0;
		m_uiFrostWingDoorGUID			= 0;
		m_uiDreamWalkerDoorGUID			= 0;
		m_uiDreamWalkerExitDoorGUID		= 0;
		m_uiSindragosaDoorGUID			= 0;

		m_uiSaurfangCacheGUID			= 0;

		m_uiPlagueSigilGUID				= 0;
		m_uiBloodSigilGUID				= 0;
		m_uiFrostSigilGUID				= 0;

		m_uiOrangeTubeGUID				= 0;
		m_uiOrangeDoorGUID				= 0;

		checkPlayer_Timer = 500;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_MARROWGAR: 
				m_uiMarrowgarGUID = pCreature->GetGUID(); 
				break;
            case NPC_DEATHWHISPER: 
				m_uiDeathwhisperGUID = pCreature->GetGUID(); 
				break;
            case NPC_SAURFANG: 
				m_uiSaurfangGUID = pCreature->GetGUID(); 
				break;
			case NPC_FESTERGUT:
				m_uiFestergutGUID = pCreature->GetGUID();
				break;
			case NPC_ROTFACE:
				m_uiRotfaceGUID = pCreature->GetGUID();
				break;
			case NPC_PUTRICIDE:
				m_uiPutricideGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case NPC_PRINCE_KELESETH:
				m_uiPrinceKelesethGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case NPC_PRINCE_VALANAR:
				m_uiPrinceValanarGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case NPC_PRINCE_TALDARAM:
				m_uiPrinceTaldaramGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case NPC_LANATHEL:
				m_uiLanathelGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case NPC_DREAMWALKER:
				m_uiDreamWalkerGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case NPC_SINDRAGOSA:
				m_uiSindragosaGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case NPC_LICHKING:
				m_uiLichKingGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
			case 37006:
				rotfacePoolsGUIDs.push_back(pCreature->GetGUID());
				break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_MARROWGAR_ICE_1:
                m_uiMarrowgarIce1GUID = pGo->GetGUID();
                if (m_auiEncounter[0] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MARROWGAR_ICE_2:
                m_uiMarrowgarIce2GUID = pGo->GetGUID();
                if (m_auiEncounter[0] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_DEATHWHISPER_GATE:
                m_uiDeathwhisperGateGUID = pGo->GetGUID();
                break;
            case GO_DEATHWHISPER_ELEVATOR:
                m_uiDeathwhisperElevatorGUID = pGo->GetGUID();
				if (m_auiEncounter[TYPE_DEATHWHISPER] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_SAURFANG_DOOR:
                m_uiSaurfangDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[TYPE_SAURFANG] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
			case GO_MARROWGAR_DOOR:
				m_uiMarrowgarDoorGUID = pGo->GetGUID();
				break;
			case GO_FESTERGUT_DOOR:
				m_uiFestergutDoorGUID = pGo->GetGUID();
				CloseDoor(m_uiFestergutDoorGUID);
				break;
			case GO_ROTFACE_DOOR:
				m_uiRotfaceDoorGUID = pGo->GetGUID();
				CloseDoor(m_uiRotfaceDoorGUID);
				break;
			case GO_PUTRICIDE_DOOR:
				m_uiPutricideDoorGUID = pGo->GetGUID();
				OpenDoor(m_uiPutricideDoorGUID);
				break;
			case GO_BLOODWING_DOOR:
				m_uiBloodWingDoorGUID = pGo->GetGUID();
				if (m_auiEncounter[TYPE_PUTRICIDE] == DONE)
					OpenDoor(m_uiBloodWingDoorGUID);
				break;
			case GO_PRINCECOUNCIL_DOOR:
				m_uiPrinceCouncilDoorGUID = pGo->GetGUID();
				OpenDoor(m_uiPrinceCouncilDoorGUID);
				break;
			case GO_LANATHEL_DOOR_1:
				m_uiLanathelDoorGUID_1 = pGo->GetGUID();
				if (m_auiEncounter[TYPE_PRINCE_COUNCIL] == DONE)
                    OpenDoor(m_uiLanathelDoorGUID_1);
				break;
			case GO_LANATHEL_DOOR_2:
				m_uiLanathelDoorGUID_2 = pGo->GetGUID();
				if (m_auiEncounter[TYPE_PRINCE_COUNCIL] == DONE)
                    OpenDoor(m_uiLanathelDoorGUID_2);
				break;
			case GO_FROSTWING_DOOR:
				m_uiFrostWingDoorGUID = pGo->GetGUID();
				if (m_auiEncounter[TYPE_LANATHEL] == DONE)
                    OpenDoor(m_uiFrostWingDoorGUID);
				break;
			case GO_DREAMWALKER_DOOR:
				m_uiDreamWalkerDoorGUID = pGo->GetGUID();
				if (m_auiEncounter[TYPE_LANATHEL] == DONE)
                    OpenDoor(m_uiDreamWalkerDoorGUID);
				break;
			case GO_DREAMWALKER_EXIT_DOOR:
				m_uiDreamWalkerExitDoorGUID = pGo->GetGUID();
				if(m_auiEncounter[TYPE_DREAMWALKER] == DONE)
                    OpenDoor(m_uiDreamWalkerExitDoorGUID);
				break;
			case GO_SINDRAGOSA_DOOR:
				m_uiSindragosaDoorGUID = pGo->GetGUID();
				if(m_auiEncounter[TYPE_DREAMWALKER] == DONE)
                    OpenDoor(m_uiSindragosaDoorGUID);
				break;
			case GO_PLAGUE_SIGIL:
				m_uiPlagueSigilGUID = pGo->GetGUID();
				if(m_auiEncounter[TYPE_PUTRICIDE] == DONE)
                    OpenDoor(m_uiPlagueSigilGUID);
				break;
			case GO_BLOOD_SIGIL:
				m_uiBloodSigilGUID = pGo->GetGUID();
				if(m_auiEncounter[TYPE_LANATHEL] == DONE)
                    OpenDoor(m_uiBloodSigilGUID);
				break;
			case GO_FROST_SIGIL:
				m_uiFrostSigilGUID = pGo->GetGUID();
				if(m_auiEncounter[TYPE_SINDRAGOSA] == DONE)
                    OpenDoor(m_uiFrostSigilGUID);
				break;
			case GO_PLAGUE_ORANGE_TUBE:
				m_uiOrangeTubeGUID = pGo->GetGUID();
				if(m_auiEncounter[TYPE_FESTERGUT] == DONE)
                    OpenDoor(m_uiOrangeTubeGUID);
				break;
			case GO_PLAGUE_ORANGE_DOOR:
				m_uiOrangeDoorGUID = pGo->GetGUID();
				if(m_auiEncounter[TYPE_FESTERGUT] == DONE)
                    OpenDoor(m_uiOrangeDoorGUID);
				break;
			case GO_SAURFANG_CACHE_10:
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
            case GO_SAURFANG_CACHE_25:
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
            case GO_SAURFANG_CACHE_10_H:
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
            case GO_SAURFANG_CACHE_25_H:
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
        }
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_MARROWGAR:
                m_auiEncounter[TYPE_MARROWGAR] = uiData;
                if (uiData == DONE)
                {
                    OpenDoor(m_uiMarrowgarIce1GUID);
                    OpenDoor(m_uiMarrowgarIce2GUID);
					CloseDoor(m_uiMarrowgarDoorGUID);
                }
				else if(uiData == IN_PROGRESS)
					OpenDoor(m_uiMarrowgarDoorGUID);
                break;
            case TYPE_DEATHWHISPER:
                m_auiEncounter[TYPE_DEATHWHISPER] = uiData;
				if(uiData == DONE)
				{
					if(GameObject* go = GetGoInMap(m_uiDeathwhisperElevatorGUID))
						go->SetGoState(GO_STATE_ACTIVE);
				}
                break;
			case TYPE_BATTLE_OF_CANNONS:
				m_auiEncounter[TYPE_BATTLE_OF_CANNONS] = uiData;
				break;
            case TYPE_SAURFANG:
                m_auiEncounter[TYPE_SAURFANG] = uiData;
                if (uiData == DONE)
				{
                    OpenDoor(m_uiSaurfangDoorGUID);
					if (GameObject* pChest = instance->GetGameObject(m_uiSaurfangCacheGUID))
						if (!pChest->isSpawned())
							pChest->SetRespawnTime(7*DAY);
				}
                break;
			case TYPE_FESTERGUT:
				m_auiEncounter[TYPE_FESTERGUT] = uiData;
				if(uiData == DONE)
				{
					CloseDoor(m_uiFestergutDoorGUID);
					OpenDoor(m_uiOrangeTubeGUID);
					OpenDoor(m_uiOrangeDoorGUID);
				}
				else if(uiData == IN_PROGRESS)
					OpenDoor(m_uiFestergutDoorGUID);
				break;
			case TYPE_ROTFACE:
				m_auiEncounter[TYPE_ROTFACE] = uiData;
				if(uiData == DONE)
				{
					CloseDoor(m_uiRotfaceDoorGUID);
				}
				else if(uiData == IN_PROGRESS)
					OpenDoor(m_uiRotfaceDoorGUID);
				else if(uiData == FAIL)
				{
					for(std::vector<uint64>::iterator itr = rotfacePoolsGUIDs.begin(); itr != rotfacePoolsGUIDs.end(); ++itr)
						if(Creature* cr = GetCreatureInMap(*itr))
							cr->ForcedDespawn(1000);
					rotfacePoolsGUIDs.clear();
				}
				break;
			case TYPE_PUTRICIDE:
				m_auiEncounter[TYPE_PUTRICIDE] = uiData;
				if(uiData == DONE)
				{
					OpenDoor(m_uiPutricideDoorGUID);
					OpenDoor(m_uiBloodWingDoorGUID);
					OpenDoor(m_uiPlagueSigilGUID);
				}
				else if(uiData == IN_PROGRESS)
					CloseDoor(m_uiPutricideDoorGUID);
				break;
			case TYPE_PRINCE_COUNCIL:
				m_auiEncounter[TYPE_PRINCE_COUNCIL] = uiData;
				if(uiData == DONE)
				{
					OpenDoor(m_uiPrinceCouncilDoorGUID);
					OpenDoor(m_uiLanathelDoorGUID_1);
					OpenDoor(m_uiLanathelDoorGUID_2);
				}
				else if(uiData == IN_PROGRESS)
					CloseDoor(m_uiPrinceCouncilDoorGUID);
				break;
			case TYPE_LANATHEL:
				m_auiEncounter[TYPE_LANATHEL] = uiData;
				if(uiData == DONE)
				{
					OpenDoor(m_uiLanathelDoorGUID_1);
					OpenDoor(m_uiLanathelDoorGUID_2);
					OpenDoor(m_uiFrostWingDoorGUID);
					OpenDoor(m_uiDreamWalkerDoorGUID);
					OpenDoor(m_uiBloodSigilGUID);
				}
				else if(uiData == IN_PROGRESS)
				{
					CloseDoor(m_uiLanathelDoorGUID_1);
					CloseDoor(m_uiLanathelDoorGUID_2);
				}
				break;
			case TYPE_DREAMWALKER:
				m_auiEncounter[TYPE_DREAMWALKER] = uiData;
				if(uiData == DONE)
				{
					OpenDoor(m_uiDreamWalkerDoorGUID);
					OpenDoor(m_uiDreamWalkerExitDoorGUID);
					OpenDoor(m_uiSindragosaDoorGUID);
				}
				else if(uiData == IN_PROGRESS)
				{
					CloseDoor(m_uiDreamWalkerDoorGUID);
				}
				break;
			case TYPE_SINDRAGOSA:
				m_auiEncounter[TYPE_SINDRAGOSA] = uiData;
				if(uiData == DONE)
				{
					OpenDoor(m_uiSindragosaDoorGUID);
					OpenDoor(m_uiFrostSigilGUID);
				}
				else if(uiData == IN_PROGRESS)
					CloseDoor(m_uiSindragosaDoorGUID);
				break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[TYPE_MARROWGAR] << " " << m_auiEncounter[TYPE_DEATHWHISPER] << " " << m_auiEncounter[TYPE_BATTLE_OF_CANNONS] << " " << m_auiEncounter[TYPE_SAURFANG];
			saveStream << m_auiEncounter[TYPE_FESTERGUT] << " " << m_auiEncounter[TYPE_ROTFACE] << " " << m_auiEncounter[TYPE_PUTRICIDE] << " " << m_auiEncounter[TYPE_PRINCE_COUNCIL];
			saveStream << m_auiEncounter[TYPE_LANATHEL] << " " << m_auiEncounter[TYPE_DREAMWALKER] << " " << m_auiEncounter[TYPE_SINDRAGOSA] << " " << m_auiEncounter[TYPE_LICHKING];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[TYPE_MARROWGAR] >> m_auiEncounter[TYPE_DEATHWHISPER] >> m_auiEncounter[TYPE_BATTLE_OF_CANNONS] >> m_auiEncounter[TYPE_SAURFANG];
		loadStream >> m_auiEncounter[TYPE_FESTERGUT] >> m_auiEncounter[TYPE_ROTFACE] >> m_auiEncounter[TYPE_PUTRICIDE] >> m_auiEncounter[TYPE_PRINCE_COUNCIL];
		loadStream >> m_auiEncounter[TYPE_LANATHEL] >> m_auiEncounter[TYPE_DREAMWALKER] >> m_auiEncounter[TYPE_SINDRAGOSA] >> m_auiEncounter[TYPE_LICHKING];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_MARROWGAR:
                return m_auiEncounter[TYPE_MARROWGAR];
            case TYPE_DEATHWHISPER:
                return m_auiEncounter[TYPE_DEATHWHISPER];
			case TYPE_BATTLE_OF_CANNONS:
                return m_auiEncounter[TYPE_BATTLE_OF_CANNONS];
            case TYPE_SAURFANG:
                return m_auiEncounter[TYPE_SAURFANG];
			case TYPE_FESTERGUT:
                return m_auiEncounter[TYPE_FESTERGUT];
			case TYPE_ROTFACE:
                return m_auiEncounter[TYPE_ROTFACE];
			case TYPE_PUTRICIDE:
                return m_auiEncounter[TYPE_PUTRICIDE];
			case TYPE_PRINCE_COUNCIL:
				return m_auiEncounter[TYPE_PRINCE_COUNCIL];
			case TYPE_LANATHEL:
				return m_auiEncounter[TYPE_LANATHEL];
			case TYPE_DREAMWALKER:
				return m_auiEncounter[TYPE_DREAMWALKER];
			case TYPE_SINDRAGOSA:
				return m_auiEncounter[TYPE_SINDRAGOSA];
			case TYPE_LICHKING:
                return m_auiEncounter[TYPE_LICHKING];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case TYPE_MARROWGAR:
                return m_uiMarrowgarGUID;
            case TYPE_DEATHWHISPER:
                return m_uiDeathwhisperGUID;
            case TYPE_SAURFANG:
                return m_uiSaurfangGUID;
			case GO_MARROWGAR_DOOR:
				return m_uiMarrowgarDoorGUID;
			case TYPE_FESTERGUT:
				return m_uiFestergutGUID;
			case TYPE_ROTFACE:
				return m_uiRotfaceGUID;
			case TYPE_PUTRICIDE:
				return m_uiPutricideGUID;
			case DATA_PRINCE_VALANAR:
				return m_uiPrinceValanarGUID;
			case DATA_PRINCE_KELESETH:
				return m_uiPrinceKelesethGUID;
			case DATA_PRINCE_TALDARAM:
				return m_uiPrinceTaldaramGUID;
			case TYPE_LANATHEL:
				return m_uiLanathelGUID;
			case TYPE_DREAMWALKER:
				return m_uiDreamWalkerGUID;
			case TYPE_SINDRAGOSA:
				return m_uiSindragosaGUID;
			case TYPE_LICHKING:
				return m_uiMarrowgarDoorGUID;
        }
        return 0;
    }

	bool CheckPlayersInMap()
	{
		bool found = false;
		Map::PlayerList const& lPlayers = instance->GetPlayers();

		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(!pPlayer->isAlive())
						pPlayer->RemoveAurasDueToSpell(69065);

					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						found = true;
					
					if(GetData(TYPE_SAURFANG) != IN_PROGRESS)
						pPlayer->RemoveAurasDueToSpell(72293);
				}
		return found;
	}

	void Update(uint32 diff)
	{
		if(checkPlayer_Timer <= diff)
		{
			if(!CheckPlayersInMap())
			{
				CloseDoor(m_uiMarrowgarDoorGUID);
				CloseDoor(m_uiFestergutDoorGUID);
				CloseDoor(m_uiRotfaceDoorGUID);
				OpenDoor(m_uiPutricideDoorGUID);
				OpenDoor(m_uiPrinceCouncilDoorGUID);
				if(GetData(TYPE_PRINCE_COUNCIL) == DONE)
				{
					OpenDoor(m_uiLanathelDoorGUID_1);
					OpenDoor(m_uiLanathelDoorGUID_2);
				}
				if(GetData(TYPE_LANATHEL) == DONE)
				{
					OpenDoor(m_uiFrostWingDoorGUID);
					OpenDoor(m_uiDreamWalkerDoorGUID);
				}
				if(GetData(TYPE_DREAMWALKER) == DONE)
					OpenDoor(m_uiSindragosaDoorGUID);
			}
			checkPlayer_Timer = 500;
		}
		else
			checkPlayer_Timer -= diff;
	}
};

InstanceData* GetInstanceData_instance_icecrown_citadel(Map* pMap)
{
    return new instance_icecrown_citadel(pMap);
}

void AddSC_instance_icecrown_citadel()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_icecrown_citadel";
    pNewScript->GetInstanceData = &GetInstanceData_instance_icecrown_citadel;
    pNewScript->RegisterSelf();
}
