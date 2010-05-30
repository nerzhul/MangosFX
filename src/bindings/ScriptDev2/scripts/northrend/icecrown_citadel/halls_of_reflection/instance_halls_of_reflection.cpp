#include "precompiled.h"
#include "halls_of_reflection.h"

#define MAX_ENCOUNTER 3

/* Forge of Souls encounters:
0- Bronjahm, The Godfather of Souls
1- The Devourer of Souls
*/

struct instance_halls_of_reflection : public ScriptedInstance
{
    instance_halls_of_reflection(Map* pMap) : ScriptedInstance(pMap) {};

    uint64 uiFalric;
    uint64 uiMarwyn;
	uint64 uiLichKing;
	uint64 uiFactionleader;
	uint64 uiEvasionFactionleader;
	uint64 uiJaina;
	uint64 uiSylvanas;
	uint64 uiEvasionJaina;
	uint64 uiEvasionSylvanas;
	std::vector<uint64> WarriorVect;
	std::vector<uint64> MageVect;
	std::vector<uint64> RogueVect;
	std::vector<uint64> PriestVect;
	std::vector<uint64> HuntVect;
	EncounterState FrostMourneEvent;
	EncounterState LichKingEscape;

    uint32 uiEncounter[MAX_ENCOUNTER];
    uint32 uiTeamInInstance;
	bool TeamIsSet;

	uint8 vague;
	uint32 vague_Timer;
	uint32 respawn_Timer;

	uint64 MainDoor;
	uint64 LichKingDoor;
	uint64 Frostmourne;

    void Initialize()
    {
        uiFalric = 0;
        uiMarwyn = 0;
		uiJaina = 0;
		uiLichKing = 0;
		uiFactionleader = 0;
		uiEvasionFactionleader = 0;
		uiJaina = 0;
		uiSylvanas = 0;
		uiEvasionJaina = 0;
		uiEvasionSylvanas = 0;

		FrostMourneEvent = NOT_STARTED;
		LichKingEscape = NOT_STARTED;

		WarriorVect.clear();
		MageVect.clear();
		RogueVect.clear();
		PriestVect.clear();
		HuntVect.clear();

        uiTeamInInstance = 0;
		TeamIsSet = false;
		vague = 0;
		vague_Timer = 2000;

		MainDoor = 0;
		LichKingDoor = 0;
		Frostmourne = 0;

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            uiEncounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (uiEncounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
			case 38112:
				uiFalric = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case 38113:
				uiMarwyn = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case 38173:
				WarriorVect.push_back(pCreature->GetGUID());
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case 38175:
				PriestVect.push_back(pCreature->GetGUID());	
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case 38176:
				HuntVect.push_back(pCreature->GetGUID());
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case 38177:
				RogueVect.push_back(pCreature->GetGUID());
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case 38567:
			case 38172:
				MageVect.push_back(pCreature->GetGUID());
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case 37221:
				uiJaina = pCreature->GetGUID();
				if(TeamIsSet)
					uiFactionleader = pCreature->GetGUID();
				break;
			case 37223:
				uiSylvanas = pCreature->GetGUID();
				if(TeamIsSet)
					uiFactionleader = pCreature->GetGUID();
				break;
			case 36955:
				uiEvasionJaina = pCreature->GetGUID();
				if(TeamIsSet)
					uiEvasionFactionleader = pCreature->GetGUID();
				break;
			case 37554:
				uiEvasionSylvanas = pCreature->GetGUID();
				if(TeamIsSet)
					uiEvasionFactionleader = pCreature->GetGUID();
				break;
			case 37226:
				uiLichKing = pCreature->GetGUID();
				break;
        }
    }

	void OnObjectCreate(GameObject* obj)
	{
		switch(obj->GetEntry())
		{
			case 197341:
				LichKingDoor = obj->GetGUID();
				break;
			case 201976:
				MainDoor = obj->GetGUID();
				break;
			case 202302:
				Frostmourne = obj->GetGUID();
				break;
		}
	}

	void OnPlayerEnter(Player* plr)
	{
		uiTeamInInstance = plr->GetTeam();
		uiFactionleader = uiTeamInInstance == ALLIANCE ? uiJaina : uiSylvanas;
		uiEvasionFactionleader = uiTeamInInstance == ALLIANCE ? uiEvasionJaina : uiEvasionSylvanas;
		TeamIsSet = true;
		if(FrostMourneEvent == IN_PROGRESS)
		{
			DoUpdateWorldState(WS_MAIN,1);
			DoUpdateWorldState(WS_VAGUE,vague);
		}
		else
			DoUpdateWorldState(WS_MAIN,0);		
	}

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
			case TYPE_MARWYN:
				uiEncounter[type] = data;
				if(data == DONE)
					OpenDoor(LichKingDoor);
				if(Creature* TheLichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
					TheLichKing->Relocate(5551.325f,2261.067f,733.5f,3.91f);
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					fLead->ForcedDespawn();
				break;
			case TYPE_FALRIC:
			case TYPE_LICHKING:
				uiEncounter[type] = data;
				break;
			case TYPE_EVENT_FROSTMOURNE:
				if(FrostMourneEvent == DONE)
					return;
				FrostMourneEvent = EncounterState(data);
				if(data == IN_PROGRESS)
					InitFrostmourneEvent();
				break;
			case TYPE_EVENT_ESCAPE:
				LichKingEscape = EncounterState(data);
				break;
        }

        if (data == DONE)
            SaveToDB();
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case TYPE_FALRIC:
			case TYPE_MARWYN:
			case TYPE_LICHKING:
				return uiEncounter[type];
        }
        return 0;
    }

    uint64 GetData64(uint32 type)
    {
        switch(type)
        {
			case DATA_RANDOM_WARRIOR:
				return GetOneTrash(WarriorVect);
			case DATA_RANDOM_MAGE:
				return GetOneTrash(MageVect);
			case DATA_RANDOM_PRIEST:
				return GetOneTrash(PriestVect);
			case DATA_RANDOM_HUNT:
				return GetOneTrash(HuntVect);
			case DATA_RANDOM_ROGUE:
				return GetOneTrash(RogueVect);
			case TYPE_FALRIC:
				return uiFalric;
			case TYPE_MARWYN:
				return uiMarwyn;
			case DATA_DOOR_MAIN:
				return MainDoor;
			case DATA_DOOR_LICHKING:
				return LichKingDoor;
			case TYPE_LICHKING:
				return uiLichKing;
        }
        return 0;
    }

	uint64 GetOneTrash(std::vector<uint64> cr_vect)
	{
		if(!cr_vect.empty())
		{
			uint8 i=0;
			while(i<50)
			{
				uint32 pos = urand(0,cr_vect.size());
				if(Creature* tmpCr = GetCreatureInMap(cr_vect[pos]))
					if(tmpCr->isAlive() && tmpCr->HasAura(66830))
						return cr_vect[pos];
				i++;
			}
		}
		return 0;
	}


	void InitFrostmourneEvent()
	{
		vague = 0;
		if(GetData(TYPE_FALRIC) == DONE)
			vague = 5;

		vague_Timer = 2000;
		CloseDoor(GetData64(DATA_DOOR_LICHKING));
		OpenDoor(GetData64(DATA_DOOR_MAIN));
	}

	void Update(uint32 diff)
	{
		if(!CheckPlayersInMap())
		{
			DoRespawnDeadAdds();
			LichKingEscape = NOT_STARTED;
			if(FrostMourneEvent == DONE)
				/*OpenDoor(GetData64(DATA_DOOR_LICHKING))*/;
			else
			{
				FrostMourneEvent = NOT_STARTED;
				CloseDoor(GetData64(DATA_DOOR_LICHKING));
			}

			CloseDoor(GetData64(DATA_DOOR_MAIN));
			return;
		}

		if(FrostMourneEvent == IN_PROGRESS)
		{
			if(vague_Timer <= diff)
			{
				DoSpawnAddsOrBoss();
				if(vague == 5 || vague == 10)
					vague_Timer = 300000;
				else
					vague_Timer = 90000;
			}
			else
				vague_Timer -= diff;

			if(respawn_Timer <= diff)
			{
				DoRespawnDeadAdds();
				respawn_Timer = 35000;
			}
			else
				respawn_Timer -= diff;
		}
		
		if(LichKingEscape == IN_PROGRESS)
		{
		}
	}

	void DoRespawnDeadAdds()
	{
		for (std::vector<uint64>::iterator itr = WarriorVect.begin(); itr != WarriorVect.end();++itr)
			if(Creature* tmpCr = GetCreatureInMap(*itr))
				if(!tmpCr->isAlive())
				{
					tmpCr->Respawn();
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					tmpCr->CastSpell(tmpCr,66830,false);
				}
		for (std::vector<uint64>::iterator itr = MageVect.begin(); itr != MageVect.end();++itr)
			if(Creature* tmpCr = GetCreatureInMap(*itr))
				if(!tmpCr->isAlive())
				{
					tmpCr->Respawn();
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					tmpCr->CastSpell(tmpCr,66830,false);
				}

		for (std::vector<uint64>::iterator itr = HuntVect.begin(); itr != HuntVect.end();++itr)
			if(Creature* tmpCr = GetCreatureInMap(*itr))
				if(!tmpCr->isAlive())
				{
					tmpCr->Respawn();
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					tmpCr->CastSpell(tmpCr,66830,false);
				}

		for (std::vector<uint64>::iterator itr = RogueVect.begin(); itr != RogueVect.end();++itr)
			if(Creature* tmpCr = GetCreatureInMap(*itr))
				if(!tmpCr->isAlive())
				{
					tmpCr->Respawn();
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					tmpCr->CastSpell(tmpCr,66830,false);
				}

		for (std::vector<uint64>::iterator itr = PriestVect.begin(); itr != PriestVect.end();++itr)
			if(Creature* tmpCr = GetCreatureInMap(*itr))
				if(!tmpCr->isAlive())
				{
					tmpCr->Respawn();
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					tmpCr->CastSpell(tmpCr,66830,false);
				}
	}

	void DoSpawnAddsOrBoss()
	{
		if(vague == 10)
			return;

		vague++;
		DoUpdateWorldState(WS_MAIN,1);
		DoUpdateWorldState(WS_VAGUE,vague);

		uint8 nbr = 0;
		switch(vague)
		{
			case 1:
			case 2:
				nbr = 3;
				break;
			case 3:
			case 4:
				nbr = 4;
				break;
			case 5:
				if(Creature* Falric = GetCreatureInMap(GetData64(TYPE_FALRIC)))
				{
					Falric->RemoveAurasDueToSpell(66830);
					Falric->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Falric->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					AggroPlayersInMap(Falric);
				}
				break;
			case 6:
			case 7:
				nbr = 5;
				break;
			case 8:
			case 9:
				nbr = 6;
				break;
			case 10:
				if(Creature* Marwyn = GetCreatureInMap(GetData64(TYPE_MARWYN)))
				{
					Marwyn->RemoveAurasDueToSpell(66830);
					Marwyn->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Marwyn->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					AggroPlayersInMap(Marwyn);
				}
				break;
		}

		if(nbr > 0)
		{
			if(Creature* War = GetCreatureInMap(GetData64(DATA_RANDOM_WARRIOR)))
			{
				War->RemoveAurasDueToSpell(66830);
				War->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				War->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				AggroPlayersInMap(War);
			}

			if(Creature* Mage = GetCreatureInMap(GetData64(DATA_RANDOM_MAGE)))
			{
				Mage->RemoveAurasDueToSpell(66830);
				Mage->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				Mage->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				AggroPlayersInMap(Mage);
			}

			if(Creature* Priest = GetCreatureInMap(GetData64(DATA_RANDOM_PRIEST)))
			{
				Priest->RemoveAurasDueToSpell(66830);
				Priest->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				Priest->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
				AggroPlayersInMap(Priest);
			}

			if(nbr >= 3)
			{
				if(Creature* Rogue = GetCreatureInMap(GetData64(DATA_RANDOM_HUNT)))
				{
					Rogue->RemoveAurasDueToSpell(66830);
					Rogue->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Rogue->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					AggroPlayersInMap(Rogue);
				}
			}

			if(nbr >= 4)
			{
				if(Creature* Hunt = GetCreatureInMap(GetData64(DATA_RANDOM_HUNT)))
				{
					Hunt->RemoveAurasDueToSpell(66830);
					Hunt->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Hunt->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					AggroPlayersInMap(Hunt);
				}
			}

			if(nbr >= 5)
			{
				if(Creature* tmpCr = GetCreatureInMap(GetData64(DATA_RANDOM_WARRIOR+urand(0,4))))
				{
					tmpCr->RemoveAurasDueToSpell(66830);
					tmpCr->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					tmpCr->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					AggroPlayersInMap(tmpCr);
				}
			}
		}
	}

	bool CheckPlayersInMap()
	{
		Map::PlayerList const& lPlayers = instance->GetPlayers();

		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						return true;
		return false;
	}

	void AggroPlayersInMap(Creature* who)
	{
		Map::PlayerList const& lPlayers = instance->GetPlayers();

		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						who->AddThreat(pPlayer,1.0f);
	}

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << " " << uiEncounter[0] << " " << uiEncounter[1] << " " << uiEncounter[2];

        OUT_SAVE_INST_DATA_COMPLETE;
        return saveStream.str();
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
        loadStream >> uiEncounter[0] >> uiEncounter[1] >> uiEncounter[2];

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (uiEncounter[i] == IN_PROGRESS)
                uiEncounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_halls_of_reflection(Map* pMap)
{
    return new instance_halls_of_reflection(pMap);
}

void AddSC_instance_halls_of_reflection()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_halls_of_reflection";
    newscript->GetInstanceData = &GetInstanceData_instance_halls_of_reflection;
    newscript->RegisterSelf();
}
