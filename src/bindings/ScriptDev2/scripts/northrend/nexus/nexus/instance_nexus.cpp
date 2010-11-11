/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: instance_nexus
SD%Complete: 75%
SDComment:
SDCategory: The Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

bool GOHello_go_containment_sphere(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    switch(pGo->GetEntry())
    {
        case GO_CONTAINMENT_SPHERE_TELESTRA: pInstance->SetData(TYPE_TELESTRA, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ANOMALUS: pInstance->SetData(TYPE_ANOMALUS, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ORMOROK:  pInstance->SetData(TYPE_ORMOROK, SPECIAL);  break;
    }

    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
    pGo->SetGoState(GO_STATE_ACTIVE);
    return true;
}

struct MANGOS_DLL_DECL instance_nexus : public ScriptedInstance
{
    instance_nexus(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiAnomalusGUID;
	uint64 m_uiKeristraszaGUID;
	uint64 m_uiTelestraGUID;

    uint64 m_uiTelestrasContainmentSphereGUID;
    uint64 m_uiAnomalusContainmentSphereGUID;
    uint64 m_uiOrmoroksContainmentSphereGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiAnomalusGUID = 0;
		m_uiKeristraszaGUID = 0;
		m_uiTelestraGUID = 0;

        m_uiTelestrasContainmentSphereGUID = 0;
        m_uiAnomalusContainmentSphereGUID = 0;
        m_uiOrmoroksContainmentSphereGUID = 0;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        }

        return false;
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_CONTAINMENT_SPHERE_TELESTRA:
                m_uiTelestrasContainmentSphereGUID = pGo->GetGUID();
                if (m_auiEncounter[0] == DONE)
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                break;
            case GO_CONTAINMENT_SPHERE_ANOMALUS:
                m_uiAnomalusContainmentSphereGUID = pGo->GetGUID();
                if (m_auiEncounter[1] == DONE)
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                break;
            case GO_CONTAINMENT_SPHERE_ORMOROK:
                m_uiOrmoroksContainmentSphereGUID = pGo->GetGUID();
                if (m_auiEncounter[2] == DONE)
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                break;
        }
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_ANOMALUS:
                m_uiAnomalusGUID = pCreature->GetGUID();
                break;
			case NPC_KERISTRASZA:
				m_uiKeristraszaGUID = pCreature->GetGUID();
				pCreature->SetAuraStack(47543,pCreature,1);
				if(GetData(TYPE_TELESTRA) == DONE && GetData(TYPE_ANOMALUS) == DONE && GetData(TYPE_ORMOROK) == DONE)
					if(Creature* cr = GetCreatureInMap(m_uiKeristraszaGUID))
						cr->RemoveAurasDueToSpell(47543);
				break;
			case NPC_TELESTRA:
				m_uiTelestraGUID = pCreature->GetGUID();
				break;
        }
    }

    uint64 GetData64(uint32 uiType)
    {
        switch(uiType)
        {
            case NPC_ANOMALUS:
			case TYPE_ANOMALUS:
                return m_uiAnomalusGUID;
			case TYPE_KERISTRASZA:
				return m_uiKeristraszaGUID;
			case TYPE_TELESTRA:
				return m_uiTelestraGUID;
        }

        return 0;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_TELESTRA:
                return m_auiEncounter[0];
            case TYPE_ANOMALUS:
                return m_auiEncounter[1];
            case TYPE_ORMOROK:
                return m_auiEncounter[2];
            case TYPE_KERISTRASZA:
                return m_auiEncounter[3];
			case DATA_NB_BOSS_DOWN:
				uint32 resp = 0;
				if(GetData(TYPE_ORMOROK) == DONE)
					resp++;
				if(GetData(TYPE_TELESTRA) == DONE)
					resp += 2;
				if(GetData(TYPE_ANOMALUS) == DONE)
					resp += 4;
				if(GetData(TYPE_KERISTRASZA) == DONE)
					resp += 8;
				return resp;
        }

        return 0;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Nexus: SetData received for type %u with data %u", uiType, uiData);

        switch(uiType)
        {
            case TYPE_TELESTRA:
                m_auiEncounter[0] = uiData;
                if (uiData == DONE)
                {
                    if (GameObject* pGo = instance->GetGameObject(m_uiTelestrasContainmentSphereGUID))
                        pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                }
				if(GetData(TYPE_TELESTRA) == DONE && GetData(TYPE_ANOMALUS) == DONE && GetData(TYPE_ORMOROK) == DONE)
					if(Creature* cr = GetCreatureInMap(m_uiKeristraszaGUID))
						cr->RemoveAurasDueToSpell(47543);
                break;
            case TYPE_ANOMALUS:
                m_auiEncounter[1] = uiData;
                if (uiData == DONE)
                {
                    if (GameObject* pGo = instance->GetGameObject(m_uiAnomalusContainmentSphereGUID))
                        pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                }
				if(GetData(TYPE_TELESTRA) == DONE && GetData(TYPE_ANOMALUS) == DONE && GetData(TYPE_ORMOROK) == DONE)
					if(Creature* cr = GetCreatureInMap(m_uiKeristraszaGUID))
						cr->RemoveAurasDueToSpell(47543);
                break;
            case TYPE_ORMOROK:
                m_auiEncounter[2] = uiData;
                if (uiData == DONE)
                {
                    if (GameObject* pGo = instance->GetGameObject(m_uiOrmoroksContainmentSphereGUID))
                        pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_UNK1);
                }
				if(GetData(TYPE_TELESTRA) == DONE && GetData(TYPE_ANOMALUS) == DONE && GetData(TYPE_ORMOROK) == DONE)
					if(Creature* cr = GetCreatureInMap(m_uiKeristraszaGUID))
						cr->RemoveAurasDueToSpell(47543);
                break;
            case TYPE_KERISTRASZA:
                m_auiEncounter[3] = uiData;
                break;
            default:
                error_log("SD2: Instance Nexus: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

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
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_nexus(Map* pMap)
{
    return new instance_nexus(pMap);
}

void AddSC_instance_nexus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_nexus";
    newscript->GetInstanceData = &GetInstanceData_instance_nexus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_containment_sphere";
    newscript->pGOHello = &GOHello_go_containment_sphere;
    newscript->RegisterSelf();
}
