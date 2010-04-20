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
SDName: instance_pinnacle
SD%Complete: 25%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

struct MANGOS_DLL_DECL instance_pinnacle : public ScriptedInstance
{
	uint64 SvalaSorrowgrave;
    uint64 GortokPalehoof;
    uint64 SkadiRuthless;
    uint64 KingYmiron;
    uint64 FrenziedWorgen;
    uint64 RavenousFurbolg;
    uint64 MassiveJormungar;
    uint64 FerociousRhino;
    instance_pinnacle(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;
	uint64 pDoorGuid;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

		pDoorGuid = 0;
		SvalaSorrowgrave = GortokPalehoof = SkadiRuthless = KingYmiron = FrenziedWorgen = RavenousFurbolg = 0;
		MassiveJormungar = FerociousRhino = 0;
    }

	void OnCreatureCreate(Creature *creature)
    {
        switch(creature->GetEntry())
        {
            case 26668:    
				SvalaSorrowgrave = creature->GetGUID(); 
				break;
            case 26687:
				creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				creature->CastSpell(creature,66830,false);
				GortokPalehoof = creature->GetGUID();
				break;
            case 26693:    
				SkadiRuthless = creature->GetGUID();
				break;
            case 26861:    
				KingYmiron = creature->GetGUID();  
				break;
            case 26683:    
				FrenziedWorgen = creature->GetGUID();
				creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				creature->CastSpell(creature,66830,false);
				break;
            case 26684:    
				RavenousFurbolg = creature->GetGUID();
				creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				creature->CastSpell(creature,66830,false);
				break;
            case 26685:    
				MassiveJormungar = creature->GetGUID();
				creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				creature->CastSpell(creature,66830,false);
				break;
            case 26686:    
				FerociousRhino = creature->GetGUID();
				creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        	    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				creature->CastSpell(creature,66830,false);
				break;
        }
    }

	void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
			case 192173:
				pDoorGuid = pGo->GetGUID();
			break;
		}
	}

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_SVALA:
                return m_auiEncounter[0];
            case TYPE_GORTOK:
                return m_auiEncounter[1];
            case TYPE_SKADI:
                return m_auiEncounter[2];
            case TYPE_YMIRON:
                return m_auiEncounter[3];
        }

        return 0;
    }

	uint64 GetData64(uint32 identifier)
    {
        switch(identifier)
        {
            case DATA_SVALA_SORROWGRAVE:          return SvalaSorrowgrave;
            case DATA_GORTOK_PALEHOOF:            return GortokPalehoof;
            case DATA_SKADI_THE_RUTHLESS:         return SkadiRuthless;
            case DATA_KING_YMIRON:                return KingYmiron;
            case DATA_MOB_FRENZIED_WORGEN:        return FrenziedWorgen;
            case DATA_MOB_RAVENOUS_FURBOLG:       return RavenousFurbolg;
            case DATA_MOB_MASSIVE_JORMUNGAR:      return MassiveJormungar;
            case DATA_MOB_FEROCIOUS_RHINO:        return FerociousRhino;
        }
        return 0;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Pinnacle: SetData received for type %u with data %u", uiType, uiData);

        switch(uiType)
        {
            case TYPE_SVALA:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_GORTOK:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_SKADI:
                m_auiEncounter[2] = uiData;
				if(uiData == DONE)
					DoUseDoorOrButton(pDoorGuid);
                break;
            case TYPE_YMIRON:
                m_auiEncounter[3] = uiData;
                break;
            default:
                error_log("SD2: Instance Pinnacle: SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
                break;
        }

        //saving also SPECIAL for this instance
        if (uiData == DONE || uiData == SPECIAL)
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

InstanceData* GetInstanceData_instance_pinnacle(Map* pMap)
{
    return new instance_pinnacle(pMap);
}

void AddSC_instance_pinnacle()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_pinnacle";
    newscript->GetInstanceData = &GetInstanceData_instance_pinnacle;
    newscript->RegisterSelf();
}
