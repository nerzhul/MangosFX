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
SDName: Instance_Halls_of_Stone
SD%Complete: 0%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"

/* Halls of Lightning encounters:
0 - Krystallus
1 - Maiden of Grief
2 - Brann Bronzebeard
3 - Sjonnir The Ironshaper
*/

struct MANGOS_DLL_DECL instance_halls_of_stone : public ScriptedInstance
{
    instance_halls_of_stone(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];

    uint64 m_uiKrystallusGUID;
    uint64 m_uiGriefGUID;
    uint64 m_uiBrannGUID;
    uint64 m_uiSjonnirGUID;

    uint64 m_uiKaddrakGUID;
    uint64 m_uiAbedneumGUID;
    uint64 m_uiMarnakGUID;

    uint64 m_uiGriefDoorGUID;
    uint64 m_uiBrannDoorGUID;
    uint64 m_uiSjonnirDoorGUID;

    uint64 m_uiGoTribunalConsoleGUID;
    uint64 m_uiGoTribunalChestGUID;
    uint64 m_uiGoTribunalSkyFloorGUID;
    uint64 m_uiGoKaddrakGUID;
    uint64 m_uiGoAbedneumGUID;
    uint64 m_uiGoMarnakGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiKrystallusGUID       = 0;
        m_uiGriefGUID            = 0;
        m_uiBrannGUID            = 0;
        m_uiSjonnirGUID          = 0;

        m_uiKaddrakGUID          = 0;
        m_uiAbedneumGUID         = 0;
        m_uiMarnakGUID           = 0;

        m_uiGriefDoorGUID        = 0;
        m_uiBrannDoorGUID        = 0;
        m_uiSjonnirDoorGUID      = 0;

        m_uiGoTribunalConsoleGUID  = 0;
        m_uiGoTribunalChestGUID    = 0;
        m_uiGoTribunalSkyFloorGUID = 0;
        m_uiGoKaddrakGUID          = 0;
        m_uiGoAbedneumGUID         = 0;
        m_uiGoMarnakGUID           = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_KRYSTALLUS:
                m_uiKrystallusGUID = pCreature->GetGUID();
                break;
            case NPC_GRIEF:
                m_uiGriefGUID = pCreature->GetGUID();
                break;
            case NPC_BRANN:
                m_uiBrannGUID = pCreature->GetGUID();
                break;
            case NPC_SJONNIR:
                m_uiSjonnirGUID = pCreature->GetGUID();
                break;
            case NPC_KADDRAK:
                m_uiKaddrakGUID = pCreature->GetGUID();
                break;
            case NPC_ABEDNEUM:
                m_uiAbedneumGUID = pCreature->GetGUID();
                break;
            case NPC_MARNAK:
                m_uiMarnakGUID = pCreature->GetGUID();
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_GRIEF_DOOR:
                m_uiGriefDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[0] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_BRANN_DOOR:
                m_uiBrannDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[1] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_SJONNIR_DOOR:
                m_uiSjonnirDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_TRIBUNAL_CONSOLE:
                m_uiGoTribunalConsoleGUID = pGo->GetGUID();
                break;
            case GO_TRIBUNAL_CHEST:
                m_uiGoTribunalChestGUID = pGo->GetGUID();
                break;
            case GO_TRIBUNAL_SKY_FLOOR:
                m_uiGoTribunalSkyFloorGUID = pGo->GetGUID();
                break;
            case GO_KADDRAK:
                m_uiGoKaddrakGUID = pGo->GetGUID();
                break;
            case GO_ABEDNEUM:
                m_uiGoAbedneumGUID = pGo->GetGUID();
                break;
            case GO_MARNAK:
                m_uiGoMarnakGUID = pGo->GetGUID();
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_KRYSTALLUS:
                if (uiData == DONE)
                    DoUseDoorOrButton(m_uiGriefDoorGUID);
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_GRIEF:
                if (uiData == DONE)
                    DoUseDoorOrButton(m_uiBrannDoorGUID);
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_BRANN:
                if (uiData == DONE)
                {
                    DoUseDoorOrButton(m_uiSjonnirDoorGUID);
                    if (GameObject* pChest = instance->GetGameObject(m_uiGoTribunalChestGUID)) // how about heroic
                        if (pChest && !pChest->isSpawned())
                            pChest->SetRespawnTime(350000000);
                }
                m_auiEncounter[2] = uiData;
                break;
            case TYPE_SJONNIR:
                m_auiEncounter[3] = uiData;
                break;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_KRYSTALLUS:
                return m_auiEncounter[0];
            case TYPE_GRIEF:
                return m_auiEncounter[1];
            case TYPE_BRANN:
                return m_auiEncounter[2];
            case TYPE_SJONNIR:
                return m_auiEncounter[3];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_KRYSTALLUS:
                return m_uiKrystallusGUID;
            case DATA_GRIEF:
                return m_uiGriefGUID;
            case DATA_BRANN:
                return m_uiBrannGUID;
            case DATA_SJONNIR:
                return m_uiSjonnirGUID;
            case DATA_KADDRAK:
                return m_uiKaddrakGUID;
            case DATA_ABEDNEUM:
                return m_uiAbedneumGUID;
            case DATA_MARNAK:
                return m_uiMarnakGUID;
            case DATA_GO_TRIBUNAL_CONSOLE:
                return m_uiGoTribunalConsoleGUID;
            case DATA_GO_SKY_FLOOR:
                return m_uiGoTribunalSkyFloorGUID;
            case DATA_GO_KADDRAK:
                return m_uiGoKaddrakGUID;
            case DATA_GO_ABEDNEUM:
                return m_uiGoAbedneumGUID;
            case DATA_GO_MARNAK:
                return m_uiGoMarnakGUID;
        }
        return 0;
    }
};

InstanceData* GetInstanceData_instance_halls_of_stone(Map* pMap)
{
    return new instance_halls_of_stone(pMap);
}

void AddSC_instance_halls_of_stone()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_halls_of_stone";
    newscript->GetInstanceData = &GetInstanceData_instance_halls_of_stone;
    newscript->RegisterSelf();
}

