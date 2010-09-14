#include "precompiled.h"
#include "trial_of_the_crusader.h"

struct MANGOS_DLL_DECL instance_toc10 : public ScriptedInstance
{
    instance_toc10(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER+1];
	std::string strInstData;

	uint8 champion_down;
	uint64 Fordring_guid;
	uint64 Varian_guid;
	uint64 Garrosh_guid;
	uint64 Announcer_guid;
	uint64 Acidmaw_guid;
	uint64 Dreadscale_guid;
	uint64 Icehowl_guid;
	uint64 Jaraxxus_guid;
	uint64 Gormok_guid;
	uint64 Lightbane_guid;
	uint64 Darkbane_guid;
	uint64 AnubArak_guid;
	uint64 Rune_guid;

	uint64 ChampionChest_guid;
	uint64 floor_guid;
	uint64 door_guid;

	uint64 ArgentChestGUID[4][2];

	bool Raid25;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

		m_auiEncounter[5] = 50;
		Announcer_guid = 0;
		Fordring_guid = 0;
		Varian_guid = 0;
		Garrosh_guid = 0;
		Dreadscale_guid = 0;
		Acidmaw_guid = 0;
		champion_down = 0;
		Lightbane_guid = 0;
		AnubArak_guid = 0;
		Darkbane_guid = 0;
		door_guid = 0;
		Rune_guid = 0;

		ChampionChest_guid = 0;

		Raid25 = false;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
			case 34816:
			case 35766:
				Announcer_guid = pCreature->GetGUID();
				switch(pCreature->GetMap()->GetDifficulty())
				{
					case RAID_DIFFICULTY_10MAN_NORMAL:
					case RAID_DIFFICULTY_10MAN_HEROIC:
						Raid25 = false;
						break;
					case RAID_DIFFICULTY_25MAN_NORMAL:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						Raid25 = true;
						break;
				}
				break;
			case 34996:
				Fordring_guid = pCreature->GetGUID();
				break;
			case 34990:
				Varian_guid = pCreature->GetGUID();
				break;
			case 34995:
				Garrosh_guid = pCreature->GetGUID();
				break;
			case 34799:
				Dreadscale_guid = pCreature->GetGUID();
				break;
			case 35144:
				Acidmaw_guid = pCreature->GetGUID();
				break;
			case 34796:
				Gormok_guid = pCreature->GetGUID();
				break;
			case 34797:
				Icehowl_guid = pCreature->GetGUID();
				break;
			case 34780:
				Jaraxxus_guid = pCreature->GetGUID();
				break;
			case 34496:
				Darkbane_guid = pCreature->GetGUID();
				break;
			case 34497:
				Lightbane_guid = pCreature->GetGUID();
				break;
			case 34564:
				AnubArak_guid = pCreature->GetGUID();
				break;
			case 35651:
				Rune_guid = pCreature->GetGUID();
				break;
			default:
				break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
		switch(pGo->GetEntry())
		{
			case GO_CRUSADERS_CACHE_10:
			case GO_CRUSADERS_CACHE_25:
			case GO_CRUSADERS_CACHE_10_H:
			case GO_CRUSADERS_CACHE_25_H:
				ChampionChest_guid = pGo->GetGUID();
				break;
			case 195527:
				floor_guid = pGo->GetGUID();
				break;
			case 195647:
				door_guid = pGo->GetGUID();
				break;
			case GO_10_TRY_10HM:
				ArgentChestGUID[0][0] = pGo->GetGUID();
				break;
			case GO_25_TRY_10HM:
				ArgentChestGUID[1][0] = pGo->GetGUID();
				break;
			case GO_45_TRY_10HM:
				ArgentChestGUID[2][0] = pGo->GetGUID();
				break;
			case GO_50_TRY_10HM:
				ArgentChestGUID[3][0] = pGo->GetGUID();
				break;
			case GO_10_TRY_25HM:
				ArgentChestGUID[0][1] = pGo->GetGUID();
				break;
			case GO_25_TRY_25HM:
				ArgentChestGUID[1][1] = pGo->GetGUID();
				break;
			case GO_45_TRY_25HM:
				ArgentChestGUID[2][1] = pGo->GetGUID();
				break;
			case GO_50_TRY_25HM:
				ArgentChestGUID[3][1] = pGo->GetGUID();
				break;
		}
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
			case TYPE_EVENT_BEAST:
			case TYPE_JARAXXUS:
			case TYPE_VALKYRS:
				m_auiEncounter[uiType] = uiData;
				break;
			case TYPE_ANUBARAK:
				m_auiEncounter[uiType] = uiData;
				if(uiData == DONE)
				{
					if(instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
					{
						uint64 chestGUID = 0;
						if(m_auiEncounter[5] == 50)
						{
							CompleteAchievementForGroup(3810);
							chestGUID = ArgentChestGUID[3][0];
						}
						else if(m_auiEncounter[5] >= 45)
						{
							CompleteAchievementForGroup(3809);
							chestGUID = ArgentChestGUID[2][0];
						}
						else if(m_auiEncounter[5] >= 25)
						{
							CompleteAchievementForGroup(3808);
							chestGUID = ArgentChestGUID[1][0];
						}
						else if(m_auiEncounter[5] >= 10)
							chestGUID = ArgentChestGUID[0][0];

						if(GameObject* pChest = instance->GetGameObject(chestGUID))
							if (!pChest->isSpawned())
							{
								pChest->SetRespawnTime(7*DAY);
								pChest->UpdateObjectVisibility();
							}	
					}
					else if(instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
					{
						uint64 chestGUID = 0;
						if(m_auiEncounter[5] == 50)
						{
							chestGUID = ArgentChestGUID[3][1];
							CompleteAchievementForGroup(3818);
						}
						else if(m_auiEncounter[5] >= 45)
						{
							CompleteAchievementForGroup(3818);
							chestGUID = ArgentChestGUID[2][1];
						}
						else if(m_auiEncounter[5] >= 25)
						{
							CompleteAchievementForGroup(3817);
							chestGUID = ArgentChestGUID[1][1];
						}
						else if(m_auiEncounter[5] >= 10)
							chestGUID = ArgentChestGUID[0][1];

						if(GameObject* pChest = instance->GetGameObject(chestGUID))
							if (!pChest->isSpawned())
							{
								pChest->SetRespawnTime(7*DAY);
								pChest->UpdateObjectVisibility();
							}
					}
				}
				break;
			case TYPE_CHAMPIONS:
				m_auiEncounter[uiType] = uiData;
				if(uiType == DONE)
				{
					if (GameObject* pChest = instance->GetGameObject(ChampionChest_guid))
						if (!pChest->isSpawned())
						{
							pChest->SetRespawnTime(7*DAY);
							pChest->UpdateObjectVisibility();
						}

					if(instance)
					{
						switch(instance->GetDifficulty())
						{
							case RAID_DIFFICULTY_10MAN_NORMAL:
								CompleteAchievementForGroup(4036);
								break;
							case RAID_DIFFICULTY_10MAN_HEROIC:
								CompleteAchievementForGroup(4037);
								break;
							case RAID_DIFFICULTY_25MAN_NORMAL:
								CompleteAchievementForGroup(4038);
								break;
							case RAID_DIFFICULTY_25MAN_HEROIC:
								CompleteAchievementForGroup(4039);
								break;
						}
					}
				}
				break;
			case TYPE_CHAMPIONWAR:
			case TYPE_CHAMPIONDRUID:
			case TYPE_CHAMPIONHUNT:
			case TYPE_CHAMPIONROGUE:
			case TYPE_CHAMPIONWARLOCK:
			case TYPE_CHAMPIONSHAMAN:
			case TYPE_CHAMPIONPALADIN:
			case TYPE_CHAMPIONPRIEST:
			case TYPE_CHAMPIONDEATHKNIGHT:
			case TYPE_CHAMPIONMAGE:
				if(uiData == DONE)
				{
					champion_down++;
					
					if(champion_down == (Raid25 ? 8 : 6))
						SetData(TYPE_CHAMPIONS,DONE);
				}
				else
					champion_down = 0;
				break;
			case TYPE_TRY:
				if(instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
				{
					if(m_auiEncounter[5] > 0 && uiData == 1)
						m_auiEncounter[5]--;
					DoUpdateWorldState(4390,1);
					DoUpdateWorldState(4389,m_auiEncounter[5]);
				}
				break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

	void OnPlayerEnter(Player* pWho)
	{
		pWho->SendUpdateWorldState(4390,1);
		pWho->SendUpdateWorldState(4389,m_auiEncounter[5]);
	}

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
			case TYPE_EVENT_BEAST:
			case TYPE_JARAXXUS:
			case TYPE_CHAMPIONS:
			case TYPE_VALKYRS:
			case TYPE_ANUBARAK:
			case TYPE_TRY:
				return m_auiEncounter[uiType];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
			case DATA_FORDRING:
				return Fordring_guid;
			case DATA_VARIAN:
				return Varian_guid;
			case DATA_GARROSH:
				return Garrosh_guid;
			case DATA_ANNOUNCER:
				return Announcer_guid;
			case TYPE_Acidmaw:
				return Acidmaw_guid;
			case TYPE_Dreadscale:
				return Dreadscale_guid;
			case TYPE_GormoktheImpaler:
				return Gormok_guid;
			case TYPE_JARAXXUS:
				return Jaraxxus_guid;
			case TYPE_Icehowl:
				return Icehowl_guid;
			case TYPE_Fjola_Lightbane:
				return Lightbane_guid;
			case TYPE_Eydis_Darkbane:
				return Darkbane_guid;
			case TYPE_ANUBARAK:
				return AnubArak_guid;
			case TYPE_FLOOR:
				return floor_guid;
			case TYPE_DOOR:
				return door_guid;
			case TYPE_RUNE:
				return Rune_guid;
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
            >> m_auiEncounter[4] >> m_auiEncounter[5];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

};

InstanceData* GetInstanceData_instance_toc10(Map* pMap)
{
    return new instance_toc10(pMap);
}

void AddSC_instance_toc10()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_toc10";
    newscript->GetInstanceData = &GetInstanceData_instance_toc10;
    newscript->RegisterSelf();
}
