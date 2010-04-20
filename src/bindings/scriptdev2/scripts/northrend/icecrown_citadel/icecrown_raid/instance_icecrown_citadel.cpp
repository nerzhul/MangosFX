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
    uint64 m_uiDeathwhisperGUID;
    uint64 m_uiSaurfangGUID;

    uint64 m_uiMarrowgarIce1GUID;
    uint64 m_uiMarrowgarIce2GUID;
    uint64 m_uiDeathwhisperGateGUID;
    uint64 m_uiDeathwhisperElevatorGUID;
    uint64 m_uiSaurfangDoorGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiMarrowgarGUID               = 0;
        m_uiDeathwhisperGUID            = 0;
        m_uiSaurfangGUID                = 0;

        m_uiMarrowgarIce1GUID           = 0;
        m_uiMarrowgarIce2GUID           = 0;
        m_uiDeathwhisperGateGUID        = 0;
        m_uiDeathwhisperElevatorGUID    = 0;
        m_uiSaurfangDoorGUID            = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_MARROWGAR: m_uiMarrowgarGUID = pCreature->GetGUID(); break;
            case NPC_DEATHWHISPER: m_uiDeathwhisperGUID = pCreature->GetGUID(); break;
            case NPC_SAURFANG: m_uiSaurfangGUID = pCreature->GetGUID(); break;
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
                break;
            case GO_SAURFANG_DOOR:
                m_uiSaurfangDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
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
                m_auiEncounter[0] = uiData;
                if (uiData == DONE)
                {
                    DoUseDoorOrButton(m_uiMarrowgarIce1GUID);
                    DoUseDoorOrButton(m_uiMarrowgarIce2GUID);
                }
                break;
            case TYPE_DEATHWHISPER:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_SAURFANG:
                m_auiEncounter[2] = uiData;
                if (uiData == DONE)
                    DoUseDoorOrButton(m_uiSaurfangDoorGUID);
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

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
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

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
                return m_auiEncounter[0];
            case TYPE_DEATHWHISPER:
                return m_auiEncounter[1];
            case TYPE_SAURFANG:
                return m_auiEncounter[2];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case NPC_MARROWGAR:
                return m_uiMarrowgarGUID;
            case NPC_DEATHWHISPER:
                return m_uiDeathwhisperGUID;
            case NPC_SAURFANG:
                return m_uiSaurfangGUID;
        }
        return 0;
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
