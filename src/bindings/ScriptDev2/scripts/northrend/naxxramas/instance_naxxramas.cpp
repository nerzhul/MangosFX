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
SDName: Instance_Naxxramas
SD%Complete: 10
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

/* Encounters
0 Patchwerk
1 Grobbulus
2 Gluth
3 Thaddius
4 Anub'Rekhan
5 Grand Widow Faerlina
6 Maexxna
7 Instructor Razuvious
8 Gothik the Harvester
9 Four Horsemen
10 Noth the Plaguebringer
11 Heigan the Unclean
12 Loatheb
13 Sapphiron
14 Kel'Thuzad
*/

#define SPELL_ERUPTION 29371 

const float HeiganPos[2] = {2796, -3707};
const float HeiganEruptionSlope[3] =
{
    (-3685 - HeiganPos[1]) /(2724 - HeiganPos[0]),
    (-3647 - HeiganPos[1]) /(2749 - HeiganPos[0]),
    (-3637 - HeiganPos[1]) /(2771 - HeiganPos[0]),
};

// 0  H      x
//  1        ^
//   2       |
//    3  y<--o
inline uint32 GetEruptionSection(float x, float y)
{
    y -= HeiganPos[1];
    if (y < 1.0f)
        return 0;

    x -= HeiganPos[0];
    if (x > -1.0f)
        return 3;

    float slope = y/x;
    for (uint32 i = 0; i < 3; ++i)
        if (slope > HeiganEruptionSlope[i])
            return i;
    return 3;
}

struct MANGOS_DLL_DECL instance_naxxramas : public ScriptedInstance
{
    instance_naxxramas(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
	std::set<GameObject*> HeiganEruption[4];
    std::string strInstData;

    uint64 m_uiAracEyeRampGUID;
    uint64 m_uiPlagEyeRampGUID;
    uint64 m_uiMiliEyeRampGUID;
    uint64 m_uiConsEyeRampGUID;

    uint64 m_uiAracPortalGUID;
    uint64 m_uiPlagPortalGUID;
    uint64 m_uiMiliPortalGUID;
    uint64 m_uiConsPortalGUID;

    uint64 m_uiThaddiusGUID;
    uint64 m_uiStalaggGUID;
    uint64 m_uiFeugenGUID;
	uint64 m_uiHeiganGUID;

    uint64 m_uiAnubRekhanGUID;
    uint64 m_uiFaerlinanGUID;

    uint64 m_uiZeliekGUID;
    uint64 m_uiThaneGUID;
    uint64 m_uiBlaumeuxGUID;
    uint64 m_uiRivendareGUID;

    uint64 m_uiPathExitDoorGUID;
    uint64 m_uiGlutExitDoorGUID;
    uint64 m_uiThadDoorGUID;

    uint64 m_uiAnubDoorGUID;
    uint64 m_uiAnubGateGUID;
    uint64 m_uiFaerDoorGUID;
    uint64 m_uiMaexOuterGUID;
    uint64 m_uiMaexInnerGUID;

    uint64 m_uiGothCombatGateGUID;
    uint64 m_uiGothikEntryDoorGUID;
    uint64 m_uiGothikExitDoorGUID;
    uint64 m_uiHorsemenDoorGUID;
    uint64 m_uiHorsemenChestNGUID;

    uint64 m_uiNothEntryDoorGUID;
    uint64 m_uiNothExitDoorGUID;
    uint64 m_uiHeigEntryDoorGUID;
    uint64 m_uiHeigExitDoorGUID;
    uint64 m_uiLoathebDoorGUID;

    uint64 m_uiKelthuzadDoorGUID;

	Creature* Faerlina;
	uint32 CheckInstance_Timer;

	std::vector<Creature*> FaerlinaAdds;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiAracEyeRampGUID = 0;
        m_uiPlagEyeRampGUID = 0;
        m_uiMiliEyeRampGUID = 0;
        m_uiConsEyeRampGUID = 0;

        m_uiAracPortalGUID = 0;
        m_uiPlagPortalGUID = 0;
        m_uiMiliPortalGUID = 0;
        m_uiConsPortalGUID = 0;

        m_uiThaddiusGUID  = 0;
        m_uiStalaggGUID   = 0;
        m_uiFeugenGUID    = 0;
		m_uiHeiganGUID    = 0;

        m_uiAnubRekhanGUID = 0;
        m_uiFaerlinanGUID  = 0;

        m_uiZeliekGUID     = 0;
        m_uiThaneGUID      = 0;
        m_uiBlaumeuxGUID   = 0;
        m_uiRivendareGUID  = 0;

        m_uiPathExitDoorGUID = 0;
        m_uiGlutExitDoorGUID = 0;
        m_uiThadDoorGUID     = 0;

        m_uiAnubDoorGUID     = 0;
        m_uiAnubGateGUID     = 0;
        m_uiFaerDoorGUID     = 0;
        m_uiMaexOuterGUID    = 0;
        m_uiMaexInnerGUID    = 0;

        m_uiGothCombatGateGUID  = 0;
        m_uiGothikEntryDoorGUID = 0;
        m_uiGothikExitDoorGUID  = 0;
        m_uiHorsemenDoorGUID    = 0;
        m_uiHorsemenChestNGUID  = 0;

        m_uiNothEntryDoorGUID = 0;
        m_uiNothExitDoorGUID  = 0;
        m_uiHeigEntryDoorGUID = 0;
        m_uiHeigExitDoorGUID  = 0;
        m_uiLoathebDoorGUID   = 0;

        m_uiKelthuzadDoorGUID = 0;

		Faerlina = NULL;
		CheckInstance_Timer = 5000;
		SetData(TYPE_FAERLINA,NOT_STARTED);
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_ANUB_REKHAN:
                m_uiAnubRekhanGUID = pCreature->GetGUID();
                break;
            case NPC_FAERLINA:
                m_uiFaerlinanGUID = pCreature->GetGUID();
				Faerlina = pCreature;
                break;
            case NPC_THADDIUS:
				m_uiThaddiusGUID = pCreature->GetGUID();
                break;
            case NPC_STALAGG:
                m_uiStalaggGUID = pCreature->GetGUID();
                break;
            case NPC_FEUGEN:
                m_uiFeugenGUID = pCreature->GetGUID();
                break;
            case NPC_ZELIEK:
                m_uiZeliekGUID = pCreature->GetGUID();
                break;
            case NPC_THANE:
                m_uiThaneGUID = pCreature->GetGUID();
                break;
            case NPC_BLAUMEUX:
                m_uiBlaumeuxGUID = pCreature->GetGUID();
                break;
            case NPC_RIVENDARE:
                m_uiRivendareGUID = pCreature->GetGUID();
                break;
			case NPC_HEIGAN:
				m_uiHeiganGUID = pCreature->GetGUID();
				break;
			case 16506:
				FaerlinaAdds.push_back(pCreature);
				break;
			case 15989: // sapphiron
				break;
			case 16129: // forcing visibilité des adds
				pCreature->SetVisibility(VISIBILITY_ON);
				break;

        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
		if (pGo->GetGOInfo()->displayId == 6785 || pGo->GetGOInfo()->displayId == 1287)
        {
            uint32 section = GetEruptionSection(pGo->GetPositionX(), pGo->GetPositionY());
            HeiganEruption[section].insert(pGo);
            return;
        }

        if (pGo->GetEntry() == 181170)
            m_uiGothCombatGateGUID = pGo->GetGUID();

        switch(pGo->GetEntry())
        {
            case GO_ARAC_ANUB_GATE:
                m_uiAnubGateGUID = pGo->GetGUID();
                if (m_auiEncounter[4] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_ARAC_ANUB_DOOR:
                m_uiAnubDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[4] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            //case GO_ARAC_FAER_WEB:            = pGo->GetGUID(); break;
            //case GO_ARAC_DOOD_DOOR_2:         = pGo->GetGUID(); break;
            case GO_ARAC_FAER_DOOR:
                m_uiFaerDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[5] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_ARAC_MAEX_OUTER_DOOR:
                m_uiMaexOuterGUID = pGo->GetGUID();
                if (m_auiEncounter[5] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_ARAC_MAEX_INNER_DOOR:
                m_uiMaexInnerGUID = pGo->GetGUID();
                break;
            case GO_ARAC_EYE_RAMP:
                m_uiAracEyeRampGUID = pGo->GetGUID();
                if (m_auiEncounter[6] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_ARAC_PORTAL:
                m_uiAracPortalGUID = pGo->GetGUID();
                break;

            //case GO_PLAG_SLIME01_DOOR:        = pGo->GetGUID(); break;
            //case GO_PLAG_SLIME02_DOOR:        = pGo->GetGUID(); break;
            case GO_PLAG_NOTH_ENTRY_DOOR:
                m_uiNothEntryDoorGUID = pGo->GetGUID();
                break;
            case GO_PLAG_NOTH_EXIT_DOOR:
                m_uiNothExitDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[10] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_HEIG_ENTRY_DOOR:
                m_uiHeigEntryDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[10] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_HEIG_EXIT_DOOR:
                m_uiHeigExitDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[11] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_LOAT_DOOR:
                m_uiLoathebDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[11] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_EYE_RAMP:
                m_uiPlagEyeRampGUID = pGo->GetGUID();
                if (m_auiEncounter[12] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_PLAG_PORTAL:
                m_uiPlagPortalGUID = pGo->GetGUID();
                break;

            case GO_MILI_GOTH_ENTRY_GATE:
                m_uiGothikEntryDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[7] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MILI_GOTH_EXIT_GATE:
                m_uiGothikExitDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[8] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MILI_HORSEMEN_DOOR:
                m_uiHorsemenDoorGUID  = pGo->GetGUID();
                if (m_auiEncounter[8] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MILI_EYE_RAMP:
                m_uiMiliEyeRampGUID = pGo->GetGUID();
                if (m_auiEncounter[9] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_MILI_PORTAL:
                m_uiMiliPortalGUID = pGo->GetGUID();
                break;

            case GO_CHEST_HORSEMEN_NORM:
                m_uiHorsemenChestNGUID = pGo->GetGUID();
                break;

            case GO_CHEST_HORSEMEN_HERO: 
				m_uiHorsemenChestNGUID = pGo->GetGUID(); 
				break;

            case GO_CONS_PATH_EXIT_DOOR:
                m_uiPathExitDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[0] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CONS_GLUT_EXIT_DOOR:
                m_uiGlutExitDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CONS_THAD_DOOR:
                m_uiThadDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CONS_EYE_RAMP:
                m_uiConsEyeRampGUID = pGo->GetGUID();
                if (m_auiEncounter[3] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_CONS_PORTAL:
                m_uiConsPortalGUID = pGo->GetGUID();
                break;

            case GO_KELTHUZAD_WATERFALL_DOOR:
                m_uiKelthuzadDoorGUID = pGo->GetGUID();
                if (m_auiEncounter[13] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
			case DATA_HEIGAN_ERUPT:
                HeiganErupt(uiData);
				break;
            case TYPE_PATCHWERK:
                m_auiEncounter[0] = uiData;
                if (uiData == DONE)
                    DoorControl(m_uiPathExitDoorGUID, uiData);
                break;
            case TYPE_GROBBULUS:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_GLUTH:
                m_auiEncounter[2] = uiData;
                if (uiData == DONE)
                {
                    DoorControl(m_uiGlutExitDoorGUID, uiData);
                    DoorControl(m_uiThadDoorGUID, uiData);
                }
                break;
            case TYPE_THADDIUS:
                m_auiEncounter[3] = uiData;
                DoorControl(m_uiThadDoorGUID, uiData);
                if (uiData == DONE)
                {
                    ActiveRamp(m_uiConsEyeRampGUID);
                    if (GameObject* pPortal = instance->GetGameObject(m_uiConsPortalGUID))
                        if (pPortal && !pPortal->isSpawned())
                            pPortal->SetRespawnTime(350000000);
                }
                break;

            case TYPE_ANUB_REKHAN:
                m_auiEncounter[4] = uiData;
                DoorControl(m_uiAnubDoorGUID, uiData);
                if (uiData == DONE)
                    DoorControl(m_uiAnubGateGUID, uiData);
                break;
            case TYPE_FAERLINA:
                m_auiEncounter[5] = uiData;
                if (uiData == DONE)
                {
                    DoorControl(m_uiFaerDoorGUID, uiData);
                    DoorControl(m_uiMaexOuterGUID, uiData);
                }
                break;
            case TYPE_MAEXXNA:
                m_auiEncounter[6] = uiData;
                DoorControl(m_uiMaexOuterGUID, uiData);
                DoorControl(m_uiMaexInnerGUID, uiData);
                if (uiData == DONE)
                {
                    ActiveRamp(m_uiAracEyeRampGUID);
                    if (GameObject* pPortal = instance->GetGameObject(m_uiAracPortalGUID))
                        if (pPortal && !pPortal->isSpawned())
                            pPortal->SetRespawnTime(350000000);
                }
                break;

            case TYPE_RAZUVIOUS:
                m_auiEncounter[7] = uiData;
                if (uiData == DONE)
                     DoorControl(m_uiGothikEntryDoorGUID, uiData);
                break;
            case TYPE_GOTHIK:
                m_auiEncounter[8] = uiData;
                if (uiData == DONE)
                {
                     DoorControl(m_uiGothikExitDoorGUID, uiData);
                     DoorControl(m_uiHorsemenDoorGUID, uiData);
                }
                break;
            case TYPE_FOUR_HORSEMEN:
                m_auiEncounter[9] = uiData;
                DoorControl(m_uiHorsemenDoorGUID, uiData);
                if (uiData == DONE)
                {
                    ActiveRamp(m_uiMiliEyeRampGUID);
                    if (GameObject* pPortal = instance->GetGameObject(m_uiMiliPortalGUID))
                        if (pPortal && !pPortal->isSpawned())
                            pPortal->SetRespawnTime(350000000);
                    if (GameObject* pChest = instance->GetGameObject(m_uiHorsemenChestNGUID)) // how about heroic
                        if (pChest && !pChest->isSpawned())
                            pChest->SetRespawnTime(350000000);
                }
                break;

            case TYPE_NOTH:
                m_auiEncounter[10] = uiData;
                DoorControl(m_uiNothEntryDoorGUID, uiData);
                if (uiData == DONE)
                {
                     DoorControl(m_uiNothExitDoorGUID, uiData);
                     DoorControl(m_uiHeigEntryDoorGUID, uiData);
                }
                break;
            case TYPE_HEIGAN:
                m_auiEncounter[11] = uiData;
                DoorControl(m_uiHeigEntryDoorGUID, uiData);
                if (uiData == DONE)
                {
                     DoorControl(m_uiHeigExitDoorGUID, uiData);
                     DoorControl(m_uiLoathebDoorGUID, uiData);
                }
                break;
            case TYPE_LOATHEB:
                m_auiEncounter[12] = uiData;
                DoorControl(m_uiLoathebDoorGUID, uiData);
                if (uiData == DONE)
                {
                    ActiveRamp(m_uiPlagEyeRampGUID);
                    if (GameObject* pPortal = instance->GetGameObject(m_uiPlagPortalGUID))
                        if (pPortal && !pPortal->isSpawned())
                            pPortal->SetRespawnTime(350000000);
                }
                break;

            case TYPE_SAPPHIRON:
                m_auiEncounter[13] = uiData;
                if (uiData == DONE)
                    ActiveRamp(m_uiKelthuzadDoorGUID);
                break;
            case TYPE_KELTHUZAD:
                m_auiEncounter[14] = uiData;
                break;
			case TYPE_ENR_FAERLINA:
				Faerlina->RemoveAurasDueToSpell(54100);
				Faerlina->RemoveAurasDueToSpell(28798);
				Faerlina->CastSpell(Faerlina,28732,false);
				break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " "
                << m_auiEncounter[12] << " " << m_auiEncounter[13] << " " << m_auiEncounter[14];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    void HeiganErupt(uint32 section)
    {
        for (uint32 i = 0; i < 4; ++i)
        {
            if (i == section)
                continue;
            for (std::set<GameObject*>::iterator itr = HeiganEruption[i].begin(); itr != HeiganEruption[i].end(); ++itr)
            {
				(*itr)->SendGameObjectCustomAnim((*itr)->GetGUID());
                //(*itr)->SummonCreature(15384, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            }
        }
    }
    const char* Save()
    {
        return strInstData.c_str();
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_PATCHWERK:
                return m_auiEncounter[0];
            case TYPE_GROBBULUS:
                return m_auiEncounter[1];
            case TYPE_GLUTH:
                return m_auiEncounter[2];
            case TYPE_THADDIUS:
                return m_auiEncounter[3];
            case TYPE_ANUB_REKHAN:
                return m_auiEncounter[4];
            case TYPE_FAERLINA:
                return m_auiEncounter[5];
            case TYPE_MAEXXNA:
                return m_auiEncounter[6];
            case TYPE_RAZUVIOUS:
                return m_auiEncounter[7];
            case TYPE_GOTHIK:
                return m_auiEncounter[8];
            case TYPE_FOUR_HORSEMEN:
                return m_auiEncounter[9];
            case TYPE_NOTH:
                return m_auiEncounter[10];
            case TYPE_HEIGAN:
                return m_auiEncounter[11];
            case TYPE_LOATHEB:
                return m_auiEncounter[12];
            case TYPE_SAPPHIRON:
                return m_auiEncounter[13];
            case TYPE_KELTHUZAD:
                return m_auiEncounter[14];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_ANUB_REKHAN:
                return m_uiAnubRekhanGUID;
            case DATA_FAERLINA:
                return m_uiFaerlinanGUID;
            case DATA_GOTHIK_GATE: 
				return m_uiGothCombatGateGUID;
            case DATA_THADDIUS:
                return m_uiThaddiusGUID;
            case DATA_STALAGG:
                return m_uiStalaggGUID;
            case DATA_FEUGEN:
                return m_uiFeugenGUID;

            case DATA_ZELIEK:
                return m_uiZeliekGUID;
            case DATA_KORTHAZZ:
                return m_uiThaneGUID;
            case DATA_BLAUMEUX:
                return m_uiBlaumeuxGUID;
            case DATA_RIVENDARE:
                return m_uiRivendareGUID;
        }
        return 0;
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
            >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
            >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11]
            >> m_auiEncounter[12] >> m_auiEncounter[13] >> m_auiEncounter[14];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    void DoorControl(uint64 uiDoor, uint32 uiData)
    {
        if (GameObject* pDoors = instance->GetGameObject(uiDoor))
        {
            if(uiData == IN_PROGRESS)
                pDoors->SetGoState(GO_STATE_READY);
            else
                pDoors->SetGoState(GO_STATE_ACTIVE);
        }
    }
    void ActiveRamp(uint64 uiRamp)
    {
        if (GameObject* pRamp = instance->GetGameObject(uiRamp))
            pRamp->SetGoState(GO_STATE_ACTIVE);
    }

	void Update(uint32 diff)
	{
		if(CheckInstance_Timer <= diff)
		{
			if(GetData(TYPE_FAERLINA) == NOT_STARTED)
			{
				for(std::vector<Creature*>::iterator itr = FaerlinaAdds.begin(); itr!= FaerlinaAdds.end(); ++itr)
				{
					if(Faerlina && Faerlina->isAlive())
					{
						Creature *pAdd = *itr;
						if(pAdd && !pAdd->isAlive())
							pAdd->Respawn();
					}
				}
			}

			CheckInstance_Timer = 5000;
		}
		else
			CheckInstance_Timer -= diff;

	}
};

InstanceData* GetInstanceData_instance_naxxramas(Map* pMap)
{
    return new instance_naxxramas(pMap);
}

void AddSC_instance_naxxramas()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_naxxramas";
    newscript->GetInstanceData = &GetInstanceData_instance_naxxramas;
    newscript->RegisterSelf();
}
