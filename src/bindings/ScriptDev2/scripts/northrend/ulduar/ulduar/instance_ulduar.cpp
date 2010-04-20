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
SDName: Instance_Ulduar
SD%Complete: 
SDComment: 
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

void instance_ulduar::Initialize()
{
	IgnisIronAdds.clear();
	ThorimThunderOrbs.clear();
	ThorimAdds.clear();
	KologarnTrashs.clear();
	HodirTrashs.clear();
	HodirAdds.clear();
	FreyaTrashs.clear();
	YoggNuage.clear();
	YoggAdds.clear();
    m_uiLeviathanGUID       = 0;
    m_uiIgnisGUID           = 0;
    m_uiRazorscaleGUID      = 0;
    m_uiXT002GUID           = 0;
    m_uiKologarnGUID        = 0;
    m_uiAuriayaGUID         = 0;
    m_uiMimironGUID         = 0;
    m_uiHodirGUID           = 0;
    m_uiThorimGUID          = 0;
    m_uiFreyaGUID           = 0;
    m_uiVezaxGUID           = 0;
    m_uiYoggSaronGUID       = 0;
    m_uiAlgalonGUID         = 0;
	m_uiRightArmGUID		= 0;
	m_uiLeftArmGUID			= 0;
	m_uiKologarnLootGUID	= 0;
	m_uiKologarnBridgeGUID	= 0;

	m_uiThorimBigAddGUID	= 0;
	m_uiThorimDoor			= 0;
	m_uiThorimLootGUID		= 0;
	m_uiIgnisFireGUID		= 0;
	m_uiYoggSaronSaraGUID	= 0;
	
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    memset(&m_auiAssemblyGUIDs, 0, sizeof(m_auiAssemblyGUIDs));
}

void instance_ulduar::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_LEVIATHAN:
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
			pCreature->CastSpell(pCreature,66830,false);
            m_uiLeviathanGUID = pCreature->GetGUID();
            break;
        case NPC_IGNIS:
            m_uiIgnisGUID = pCreature->GetGUID();
            break;
        case NPC_RAZORSCALE:
            m_uiRazorscaleGUID = pCreature->GetGUID();
            break;
        case NPC_XT002:
            m_uiXT002GUID = pCreature->GetGUID();
            break;
        // Assembly of Iron
        case NPC_STEELBREAKER:
            m_auiAssemblyGUIDs[0] = pCreature->GetGUID();
            break;
        case NPC_MOLGEIM:
            m_auiAssemblyGUIDs[1] = pCreature->GetGUID();
            break;
        case NPC_BRUNDIR:
            m_auiAssemblyGUIDs[2] = pCreature->GetGUID();
            break;
        case NPC_KOLOGARN:
            m_uiKologarnGUID = pCreature->GetGUID();
            break;
		case NPC_RIGHT_ARM:
            m_uiRightArmGUID = pCreature->GetGUID();
            break;
		case NPC_LEFT_ARM:
            m_uiLeftArmGUID = pCreature->GetGUID();
            break;
        case NPC_AURIAYA:
            m_uiAuriayaGUID = pCreature->GetGUID();
            break;
        case NPC_MIMIRON:
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
			pCreature->CastSpell(pCreature,66830,false);
            m_uiMimironGUID = pCreature->GetGUID();
            break;
        case NPC_HODIR:
            m_uiHodirGUID = pCreature->GetGUID();
            break;
        case NPC_THORIM:
			m_uiThorimGUID = pCreature->GetGUID();
            break;
        case NPC_FREYA:
            m_uiFreyaGUID = pCreature->GetGUID();
            break;
        case NPC_VEZAX:
            m_uiVezaxGUID = pCreature->GetGUID();
            break;
        case NPC_YOGGSARON:
            m_uiYoggSaronGUID = pCreature->GetGUID();
            break;
        case NPC_ALGALON:
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
			pCreature->CastSpell(pCreature,66830,false);
            m_uiAlgalonGUID = pCreature->GetGUID();
            break;

		case NPC_IRON_ASSEMBLAGE:
			IgnisIronAdds.push_back(pCreature);
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			pCreature->CastSpell(pCreature,66830,false);
			break;
		case 32882:
			m_uiThorimBigAddGUID = pCreature->GetGUID();
			break;
		case 33378:
			//pCreature->SetVisibility(VISIBILITY_OFF);
			pCreature->SetLevel(83);
			pCreature->SetReactState(REACT_PASSIVE);
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
			ThorimThunderOrbs.push_back(pCreature);
			break;
		case 32874:
		case 32872:
		case 32875:
		case 32873:
			ThorimAdds.push_back(pCreature);
			break;
		case 34196:
		case 34190:
			KologarnTrashs.push_back(pCreature);
			if(GetData(TYPE_KOLOGARN) == DONE)
			{
				pCreature->SetRespawnDelay(7*RESPAWN_ONE_DAY);
				pCreature->ForcedDespawn(1000);
			}
			break;
		case 34134:
		case 34135:
		case 34137:
		case 34133:
			HodirTrashs.push_back(pCreature);
			if(GetData(TYPE_HODIR) == DONE)
			{
				pCreature->SetRespawnDelay(7*RESPAWN_ONE_DAY);
				pCreature->ForcedDespawn(1000);
			}
			break;
		case 33431:
		case 32919:
		case 33527:
		case 33528:
		case 32916:
		case 33430:
		case 33525:
		case 33355:
			FreyaTrashs.push_back(pCreature);
			if(GetData(TYPE_FREYA) == DONE)
			{
				pCreature->SetRespawnDelay(7*RESPAWN_ONE_DAY);
				pCreature->ForcedDespawn(1000);
			}
			break;
		case 34193:
		case 34184:
		case 34183:
		case 34191:
			MimironTrashs.push_back(pCreature);
			if(GetData(TYPE_MIMIRON) == DONE)
			{
				pCreature->SetRespawnDelay(7*RESPAWN_ONE_DAY);
				pCreature->ForcedDespawn(1000);
			}
			break;
		// ALLIANCE
		case 32901:
		case 33325:
		case 32900:
		case 32893:
		case 33326:
		case 33328:
		// HORDE
		case 32950:
		case 32941:
		case 33331:
		case 32948:
		case 33333:
		case 33330:
			HodirAdds.push_back(pCreature);
			break;
		case 33134:
			m_uiYoggSaronSaraGUID = pCreature->GetGUID();
			break;
		case 33292:
			pCreature->SetVisibility(VISIBILITY_ON);
			YoggNuage.push_back(pCreature);
			pCreature->SetRespawnDelay(DAY);
			break;
		case 33136:
			YoggAdds.push_back(pCreature);
			pCreature->SetRespawnDelay(DAY);
			break;
		case 33174:
			pCreature->SetVisibility(VISIBILITY_OFF);
			break;
    }
}

void instance_ulduar::SetData(uint32 uiType, uint32 uiData)
{
	switch(uiType)
    {
		case TYPE_LEVIATHAN:
		case TYPE_IGNIS:
		case TYPE_RAZORSCALE:
		case TYPE_XT002:
		case TYPE_ASSEMBLY:
		case TYPE_AURIAYA:
		case TYPE_MIMIRON:
		case TYPE_VEZAX:
		case TYPE_YOGGSARON:
		case TYPE_ALGALON:
			m_auiEncounter[uiType] = uiData;
				break;
		case TYPE_THORIM:
			m_auiEncounter[uiType] = uiData;
			if (uiData == DONE)
			{
				if (GameObject* pChest = instance->GetGameObject(m_uiThorimLootGUID))
					if (pChest && !pChest->isSpawned())
						pChest->SetRespawnTime(350000000);
			}
			break;
		case TYPE_FREYA:
			m_auiEncounter[uiType] = uiData;
			for (std::vector<Creature*>::iterator itr = FreyaTrashs.begin(); itr != FreyaTrashs.end();++itr)
					if((*itr) && (*itr)->isAlive())
						(*itr)->SetRespawnDelay(7*RESPAWN_ONE_DAY);
			break;
		case TYPE_HODIR:
			m_auiEncounter[uiType] = uiData;
			for (std::vector<Creature*>::iterator itr = HodirTrashs.begin(); itr != HodirTrashs.end();++itr)
					if((*itr) && (*itr)->isAlive())
						(*itr)->SetRespawnDelay(7*RESPAWN_ONE_DAY);
			if(uiData == NOT_STARTED)
			{
				for(std::vector<Creature*>::const_iterator itr = HodirAdds.begin(); itr != HodirAdds.end(); ++itr)
					(*itr)->Respawn();
			}
			break;
		case TYPE_KOLOGARN:
			m_auiEncounter[5] = uiData;
			if (uiData == DONE)
			{
				if (GameObject* pChest = instance->GetGameObject(m_uiKologarnLootGUID))
					if (pChest && !pChest->isSpawned())
						pChest->SetRespawnTime(350000000);
				if (GameObject* pBridge = instance->GetGameObject(m_uiKologarnBridgeGUID))
					pBridge->SetGoState(GO_STATE_READY);

				for (std::vector<Creature*>::iterator itr = KologarnTrashs.begin(); itr != KologarnTrashs.end();++itr)
					if((*itr) && (*itr)->isAlive())
						(*itr)->SetRespawnDelay(7*RESPAWN_ONE_DAY);
			}
			break;
		case DATA_THORIM_ORB:
		{
			Unit* target = NULL;
			if(thisMap)
			{
				Map::PlayerList const& lPlayers = thisMap->GetPlayers();
				if (!lPlayers.isEmpty())
					for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
						if (Player* pPlayer = itr->getSource())
							if(pPlayer->isAlive())
								target = pPlayer;
			}
			for (std::vector<Creature*>::iterator itr = ThorimThunderOrbs.begin(); itr != ThorimThunderOrbs.end();++itr)
				if((*itr) && (*itr)->isAlive())
					(*itr)->CastSpell(target,62470,false);
			break;
		}
		case DATA_THORIM_ADDS:
		{
			for (std::vector<Creature*>::iterator itr = ThorimAdds.begin(); itr != ThorimAdds.end();++itr)
				if((*itr))
					(*itr)->Respawn();
			break;
		}
		case DATA_YOGG_NUAGE:
		{
			for (std::vector<Creature*>::iterator itr = YoggNuage.begin(); itr != YoggNuage.end();++itr)
			{
				if(uiData == 0)
					(*itr)->Respawn();
				else
				{
					(*itr)->SetRespawnDelay(DAY*3600);
					(*itr)->ForcedDespawn(500);
				}
			}
			for (std::vector<Creature*>::iterator itr = YoggAdds.begin(); itr != YoggAdds.end();++itr)
			{
				(*itr)->SetRespawnDelay(DAY*3600);
				(*itr)->ForcedDespawn(500);
			}
			break;
		}
    }


    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            saveStream << m_auiEncounter[i] << " ";

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}
InstanceData* GetInstanceData_instance_ulduar(Map* pMap)
{
    return new instance_ulduar(pMap);
}

void AddSC_instance_ulduar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_ulduar";
    newscript->GetInstanceData = &GetInstanceData_instance_ulduar;
    newscript->RegisterSelf();
}
