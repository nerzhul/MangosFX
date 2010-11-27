#include "precompiled.h"
#include "halls_of_reflection.h"

#define MAX_ENCOUNTER 3

/* Forge of Souls encounters:
0- Bronjahm, The Godfather of Souls
1- The Devourer of Souls
*/

struct instance_halls_of_reflection : public InstanceData
{
    instance_halls_of_reflection(Map* pMap) : InstanceData(pMap) {};

    uint64 uiFalric;
    uint64 uiMarwyn;
	uint64 uiLichKing;
	uint64 uiFactionleader;
	uint64 uiEvasionFactionleader;
	uint64 uiJaina;
	uint64 uiSylvanas;
	uint64 uiEvasionJaina;
	uint64 uiEvasionSylvanas;
	uint64 uiLichKingEscape;
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
	bool RPFrostmourneDone;

	uint8 vague;
	uint32 vague_Timer;
	uint32 respawn_Timer;
	
	uint32 event_Timer;
	uint8 event_Step;

	uint32 fLead_Timer;
	uint32 LichKing_Timer;
	uint32 checkAdds_Timer;
	uint32 spawn_Timer;
	uint32 bugAbuse_Timer;

	uint8 LichKingStep;
	uint8 fLeadStep;
	uint8 TrashStep;
	uint8 Wall;
	uint8 Movement;
	std::vector<uint64> EscapeLichKingAdds;
	std::vector<uint64> IceWallTargets;
	uint64 AllianceVault;
	uint64 HordeVault;
	uint64 UtherGUID;
	

	uint64 MainDoor;
	uint64 LichKingDoor;
	uint64 Frostmourne;
	uint64 LichKingEventDoor;

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
		uiLichKingEscape = 0;
		UtherGUID = 0;

		FrostMourneEvent = NOT_STARTED;
		LichKingEscape = NOT_STARTED;

		WarriorVect.clear();
		MageVect.clear();
		RogueVect.clear();
		PriestVect.clear();
		HuntVect.clear();

        uiTeamInInstance = 0;
		TeamIsSet = false;
		RPFrostmourneDone = false;
		vague = 0;
		vague_Timer = 2000;
		event_Timer = 1000;
		event_Step = 0;

		MainDoor = 0;
		LichKingDoor = 0;
		Frostmourne = 0;

		EscapeLichKingAdds.clear();
		IceWallTargets.clear();

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
			case 36954:
				uiLichKingEscape = pCreature->GetGUID();
				break;
			case 37014:
				IceWallTargets.push_back(pCreature->GetGUID());
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
			case 201710:
				AllianceVault = obj->GetGUID();
				DoRespawnGameObject(AllianceVault,100);
				break;
			case 202337:
				HordeVault = obj->GetGUID();
				DoRespawnGameObject(AllianceVault,100);
				break;
			case 197343:
				LichKingEventDoor = obj->GetGUID();
				OpenDoor(obj->GetGUID());
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
				{
					OpenDoor(LichKingDoor);
					if(Creature* TheLichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
						TheLichKing->ForcedDespawn();
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
						fLead->ForcedDespawn();
					FrostMourneEvent = DONE;
					DoUpdateWorldState(WS_MAIN,0);
				}
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
				if(data == IN_PROGRESS)
					InitEscapeEvent();
				else if(data == DONE)
				{
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
						LichKing->ForcedDespawn();
					DoRespawnGameObject(uiTeamInInstance == ALLIANCE ? AllianceVault : HordeVault,MINUTE*10);
				}
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
			case DATA_FACTION:
				return uiTeamInInstance;
			case TYPE_DIAL_FROSTMOURNE:
				return RPFrostmourneDone ? DONE : IN_PROGRESS;
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
			case TYPE_FACTIONLEADER_EV1:
				return uiFactionleader;
			case TYPE_FACTIONLEADER_EV2:
				return uiEvasionFactionleader;
			case TYPE_LICHKING_EVENT:
				return uiLichKingEscape;
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

		FrostMourneEvent = IN_PROGRESS;
		vague_Timer = 2000;
		CloseDoor(GetData64(DATA_DOOR_LICHKING));
		OpenDoor(GetData64(DATA_DOOR_MAIN));
	}

	void InitEscapeEvent()
	{
		if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
		{
			((HoR_LichKing_EscapeAI*)LichKing->AI())->Kill(LichKing);
			LichKing->Respawn();
			LichKing->setFaction(35);
		}
		for (std::vector<uint64>::iterator itr = IceWallTargets.begin(); itr != IceWallTargets.end();++itr)
			if(Creature* Target = GetCreatureInMap(*itr))
				Target->Respawn();
		fLead_Timer = 500;
		Wall = 0;
		for (std::vector<uint64>::iterator itr = EscapeLichKingAdds.begin(); itr != EscapeLichKingAdds.end();++itr)
			if(Creature* tmpCr = GetCreatureInMap(*itr))
				tmpCr->ForcedDespawn(500);
		EscapeLichKingAdds.clear();

		bugAbuse_Timer = 120000;
		LichKing_Timer = 500;
		checkAdds_Timer = DAY*HOUR;
		spawn_Timer = 24000;
		LichKingEscape = IN_PROGRESS;
		LichKingStep = 0;
		TrashStep = 0;
		fLeadStep = 0;
		Movement = 1;
		if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
		{
			if(uiTeamInInstance == ALLIANCE)
				DoSpeak(fLead,16606,"Il ne reste rien d'Arthas. Il n'y a plus que le Roi Liche. Il est trop fort, il faut vite partir d'ici, ma magie ne pourra pas le retenir "
				"longtemps. Venez vite héros",CHAT_TYPE_SAY);
			else
				DoSpeak(fLead,17028,"Il est trop fort. Héros, vite à moi. Il faut quitter cet endroit au plus vite. Je vais faire ce que je peux pour le bloquer pendant "
				"notre fuite.",CHAT_TYPE_SAY);
		}
	}

	void Update(uint32 diff)
	{
		if(!CheckPlayersInMap())
		{
			DoRespawnDeadAdds();
			if(FrostMourneEvent == DONE)
				OpenDoor(GetData64(DATA_DOOR_LICHKING));
			else
			{
				FrostMourneEvent = NOT_STARTED;
				CloseDoor(GetData64(DATA_DOOR_LICHKING));
				CloseDoor(Frostmourne);
			}

			if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
			{
				((HoR_LichKing_EscapeAI*)LichKing->AI())->Kill(LichKing);
				LichKing->Respawn();
			}

			if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
			{
				fLead->ForcedDespawn();
				fLead->Respawn();
			}
			
			if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
			{
				fLead->ForcedDespawn();
				fLead->Respawn();
			}
			if(LichKingEscape != DONE)
			{
				LichKingEscape = NOT_STARTED;
				OpenDoor(LichKingEventDoor);
			}
			else
				CloseDoor(LichKingEventDoor);

			CloseDoor(GetData64(DATA_DOOR_MAIN));
			return;
		}

		if(FrostMourneEvent == IN_PROGRESS)
		{
			if(!RPFrostmourneDone)
			{
				DoFrostmourneRP(diff);
			}
			else
			{
				if(vague_Timer <= diff)
				{
					DoSpawnAddsOrBoss();
					if(vague == 5 || vague == 10)
						vague_Timer = 300000;
					else
						vague_Timer = 70000;
				}
				else
					vague_Timer -= diff;

				if(respawn_Timer <= diff)
				{
					DoRespawnDeadAdds();
					respawn_Timer = 8640000;
				}
				else
					respawn_Timer -= diff;
			}
		}
		
		if(LichKingEscape == IN_PROGRESS)
		{
			if(fLead_Timer <= diff)
			{
				DoNextActionForFLead();
			}
			else
				fLead_Timer -= diff;

			if(LichKing_Timer <= diff)
			{
				DoNextActionForLichKing();
			}
			else
				LichKing_Timer -= diff;

			if(spawn_Timer <= diff)
			{
				DoSpawnLichKingEventTrashs();
			}
			else
				spawn_Timer -= diff;

			if(checkAdds_Timer <= diff)
			{
				checkAdds_Timer = 1500;
				CheckEscapeAddsAndfLeadDist();
			}
			else
				checkAdds_Timer -= diff;

			if(bugAbuse_Timer <= diff)
			{
				CheckBugAbuseOnLKEvent();
				bugAbuse_Timer = 1000;
			}
			else
				bugAbuse_Timer -= diff;
		}
	}

	void CheckBugAbuseOnLKEvent()
	{
		Map::PlayerList const& lPlayers = instance->GetPlayers();
		bool foundBugAbuse = false;
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
						if(pPlayer->isAlive() && !pPlayer->isGameMaster() && !LichKing->HasInArc(M_PI,pPlayer))
							foundBugAbuse = true;

		if(foundBugAbuse)
			if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				((HoR_LichKing_EscapeAI*)LichKing->AI())->DoCastMe(SPELL_FROSTMOURNE);
	}

	void DoNextActionForFLead()
	{
		switch(fLeadStep)
		{
			case 0:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					fLead->GetMotionMaster()->MovePoint(0,fLeadEscapePos[1][0],fLeadEscapePos[1][1],fLeadEscapePos[1][2]);
					fLeadStep++;
					fLead_Timer = 12000;
				}
				break;
			case 1:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					fLead->GetMotionMaster()->MovePoint(0,fLeadEscapePos[2][0],fLeadEscapePos[2][1],fLeadEscapePos[2][2]);
					fLeadStep++;
					fLead_Timer = 5000;
				}
				break;
			case 2:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					if(uiTeamInInstance == ALLIANCE)
						DoSpeak(fLead,16607,"Je vais détruire cette barrière, tenez les morts-vivants à distance.",CHAT_TYPE_SAY);
					else
						DoSpeak(fLead,17029,"Aucun mur ne peut résister à la Reine Banshee. Tenez les morts-vivants à distance, je vais abattre cette barrière.",CHAT_TYPE_SAY);
					fLeadStep++;
					fLead_Timer = 10000;
				}
				break;
			case 4:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					fLead->GetMotionMaster()->MovePoint(0,fLeadEscapePos[3][0],fLeadEscapePos[3][1],fLeadEscapePos[3][2]);
					fLeadStep++;
					fLead_Timer = 5000;
				}
				break;
			case 5:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					if(uiTeamInInstance == ALLIANCE)
						DoSpeak(fLead,16608,"Un autre mur de glace. Empechez les morts-vivants d'interrompre mon incantation. Je vais le detruire.",CHAT_TYPE_SAY);
					else
						DoSpeak(fLead,17030,"Encore une barrière ! Tenez bon champions, je vais la détruire.",CHAT_TYPE_SAY);
					fLeadStep++;
					fLead_Timer = 10000;
				}
				break;
			case 7:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					fLead->GetMotionMaster()->MovePoint(0,fLeadEscapePos[4][0],fLeadEscapePos[4][1],fLeadEscapePos[4][2]);
					fLeadStep++;
					fLead_Timer = 5000;
				}
				break;
			case 8:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					if(uiTeamInInstance == ALLIANCE)
						DoSpeak(fLead,16609,"Il s'amuse avec nous. Je vais lui montrer ce qui arrive quand la glace rencontre le feu !",CHAT_TYPE_SAY);
					else
						DoSpeak(fLead,17031,"Ces petits jeux commencent à me fatiguer Arthas. Tes murs ne pourront pas m'arrêter.",CHAT_TYPE_SAY);
					fLeadStep++;
					fLead_Timer = 10000;
				}
				break;
			case 10:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					fLead->GetMotionMaster()->MovePoint(0,fLeadEscapePos[5][0],fLeadEscapePos[5][1],fLeadEscapePos[5][2]);
					fLeadStep++;
					fLead_Timer = 5000;
				}
				break;
			case 11:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					if(uiTeamInInstance == ALLIANCE)
						DoSpeak(fLead,16610,"Tes murs ne tiendront plus très longtemps, monstre. Je vais tous les démolir !",CHAT_TYPE_SAY);
					else
						DoSpeak(fLead,17032,"Tu n'empêchera pas notre fuite démon. Empechez les morts vivants de m'atteindre pendant que j'abats ce mur.",CHAT_TYPE_SAY);
					fLeadStep++;
					fLead_Timer = 10000;
				}
				break;
			case 13:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					fLead->GetMotionMaster()->MovePoint(0,fLeadEscapePos[6][0],fLeadEscapePos[6][1],fLeadEscapePos[6][2]);
					fLeadStep++;
					fLead_Timer = 15000;
				}
				break;
			case 3:
			case 6:
			case 9:
			case 12:
				if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
				{
					if(Creature* target = GetClosestCreatureWithEntry(fLead,37014,50.0f))
					{
						fLead->CastSpell(target,SPELL_BARRER_CHANNEL,false);
						fLead_Timer = 1000;
					}
				}				
				break;
			default:
				break;
		}
	}


	void DoSpawnLichKingEventTrashs()
	{	
		if(TrashStep == MAX_TRASHSTEP)
			return;

		switch(TrashStep)
		{
			case 0: // step 1,2,3,4
			case 2:
			case 5:
			case 8:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					for(uint8 i=0;i<8;i++)
						if(Creature* Ghoul = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_GHOUL,TEN_MINS,NEAR_7M))
						{
							EscapeLichKingAdds.push_back(Ghoul->GetGUID());
							AggroPlayersInMap(Ghoul);
						}
					if(TrashStep == 0)
						DoSpeak(LichKing,17216,"Debout, serviteurs ! Ne les laissez pas passer !",CHAT_TYPE_YELL);
				}
				spawn_Timer = 15000;
				break;
			case 1: // step 1,2
			case 4:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					if(Creature* Witcher = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_WITCHER,TEN_MINS,NEAR_7M))
					{
						EscapeLichKingAdds.push_back(Witcher->GetGUID());
						AggroPlayersInMap(Witcher);
					}
				checkAdds_Timer = 10000;
				spawn_Timer = DAY*HOUR;
				break;
			case 3: // step 2
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					if(Creature* Witcher = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_WITCHER,TEN_MINS,NEAR_7M))
					{
						EscapeLichKingAdds.push_back(Witcher->GetGUID());
						AggroPlayersInMap(Witcher);
					}
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					if(Creature* Abomination = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_ABOMINATION,TEN_MINS,NEAR_7M))
					{
						EscapeLichKingAdds.push_back(Abomination->GetGUID());
						AggroPlayersInMap(Abomination);
						DoSpeak(LichKing,17222,"Rattrapez les, serviteurs. Rapportez moi leurs cadavres !",CHAT_TYPE_YELL);
					}
					spawn_Timer = 15000;
				break;
			case 6: // step 3,4
			case 11:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					for(uint8 i=0;i<2;i++)
						if(Creature* Witcher = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_WITCHER,TEN_MINS,NEAR_7M))
						{
							AggroPlayersInMap(Witcher);
							EscapeLichKingAdds.push_back(Witcher->GetGUID());
						}
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					if(Creature* Abomination = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_ABOMINATION,TEN_MINS,NEAR_7M))
					{
						AggroPlayersInMap(Abomination);
						EscapeLichKingAdds.push_back(Abomination->GetGUID());
					}
				spawn_Timer = 15000;
				if(TrashStep == 11)
				{
					checkAdds_Timer = 10000;
					spawn_Timer = DAY*HOUR;
				}
				break;
			case 7: // step 3,4
			case 10:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					for(uint8 i=0;i<2;i++)
						if(Creature* Abomination = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_WITCHER,TEN_MINS,NEAR_7M))
						{
							AggroPlayersInMap(Abomination);
							EscapeLichKingAdds.push_back(Abomination->GetGUID());
						}
				spawn_Timer = 15000;
				if(TrashStep == 7)
				{
					checkAdds_Timer = 10000;
					spawn_Timer = DAY*HOUR;
				}
				break;
			case 9: // step 4
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					for(uint8 i=0;i<2;i++)
						if(Creature* Witcher = ((HoR_LichKing_EscapeAI*)LichKing->AI())->CallCreature(NPC_WITCHER,TEN_MINS,NEAR_7M))
						{
							AggroPlayersInMap(Witcher);
							EscapeLichKingAdds.push_back(Witcher->GetGUID());
						}
				spawn_Timer = 15000;
				break;
		}
		if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
			LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
		TrashStep++;
	}
	void DoNextActionForLichKing()
	{
		switch(LichKingStep)
		{
			case 0:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					LichKing->RemoveAurasDueToSpell(SPELL_ICEBLOCK);
					LichKing->RemoveAurasDueToSpell(SPELL_DARK_ARROW);
					LichKing_Timer = 3000;
					LichKingStep++;
				}
				break;
			case 1:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					CloseDoor(LichKingEventDoor);
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
					LichKing_Timer = 14000;
					LichKingStep++;
				}
				break;
			case 2:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					LichKing->setFaction(14);
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV2)))
						LichKing->SetFacingToObject(fLead);
					((HoR_LichKing_EscapeAI*)LichKing->AI())->DoCastMe(SPELL_WINTER);
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
					LichKing->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					LichKing_Timer = 3000;
					LichKingStep++;
				}
				break;
			case 3:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					LichKing->GetMotionMaster()->Clear();
					((HoR_LichKing_EscapeAI*)LichKing->AI())->DoCastMe(SPELL_ICEWALL);
					Movement = 2;
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
					LichKing_Timer = 37000;
					LichKingStep++;
				}
				break;
			case 4:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					Movement = 3;
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
				}
				LichKing_Timer = 65000;
				LichKingStep++;
				break;
			case 5:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					Movement = 4;
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
				}
				LichKing_Timer = 54000;
				LichKingStep++;
				break;
			case 6:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					Movement = 5;
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
				}
				LichKing_Timer = 53000;
				LichKingStep++;
				break;
			case 7:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					Movement = 6;
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
				}
				LichKing_Timer = 43000;
				LichKingStep++;
				break;
			case 8:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					Movement = 7;
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
				}
				LichKingStep++;
				LichKing_Timer = 37000;
				break;
			case 9:
				if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
				{
					Movement = 8;
					LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
				}
				LichKing_Timer = DAY*HOUR;
				break;
		}
	}

	void CheckEscapeAddsAndfLeadDist()
	{
		bool CanDoNextPlayerStep = true;
		std::vector<uint64>::iterator itr = EscapeLichKingAdds.begin();
		while(itr != EscapeLichKingAdds.end())
		{
			if(Creature* tmpCr = GetCreatureInMap(*itr))
				if(tmpCr->isAlive())
					CanDoNextPlayerStep = false;
			++itr;
		}
		if(CanDoNextPlayerStep)
		{
			EscapeLichKingAdds.clear();
			if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
			{
				LichKing->GetMotionMaster()->Clear();
				if(Creature* target = GetClosestCreatureWithEntry(LichKing,37014,250.0f))
					target->ForcedDespawn();
				if(GameObject* wall = LichKing->GetClosestGameObjectWithEntry(201385,150.0f))
					OpenDoor(wall->GetGUID());

				Wall++;

				if(Wall > 4)
				{
					SetData(TYPE_EVENT_ESCAPE,DONE);
					DoSpeak(LichKing,17223,"Hahahahaha ! Plus aucune issue, vous êtes à moi, maintenant !",CHAT_TYPE_YELL);
				}
				else
				{
					((HoR_LichKing_EscapeAI*)LichKing->AI())->DoCastMe(SPELL_ICEWALL);
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING_EVENT)))
					{
						LichKing->GetMotionMaster()->MovePoint(0,LichKingEscapePos[Movement][0],LichKingEscapePos[Movement][1],LichKingEscapePos[Movement][2]);
						switch(Wall)
						{
							case 1:
								DoSpeak(LichKing,17217,"Vous ne vous échapperez pas !",CHAT_TYPE_YELL);
								break;
							case 2:
								DoSpeak(LichKing,17218,"Succombez au froid de la tombe !",CHAT_TYPE_YELL);
								break;
							case 3:
								DoSpeak(LichKing,17219,"Encore un cul de sac !",CHAT_TYPE_YELL);
								break;
							case 4:
								DoSpeak(LichKing,17220,"Combien de temps allez vous tenir ?",CHAT_TYPE_YELL);
								break;
						}
					}
				}
				fLeadStep++;
				spawn_Timer = 15000;
				LichKing_Timer = 4000;
				checkAdds_Timer = DAY*HOUR;
			}
		}
	}

	void DoFrostmourneRP(uint32 diff)
	{
		if(event_Timer <= diff)
		{
			switch(event_Step)
			{
				case 0:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
							DoSpeak(fLead,16631,"Qu'est ce qu'il fait froid ici. Ah, je sens mon sang se glacer.",CHAT_TYPE_SAY);
						event_Timer = 4000;
					}					
					break;
				case 1:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						fLead->GetMotionMaster()->MovePoint(0,5304.0f,2000.0f,709.5f);
						if(uiTeamInInstance == ALLIANCE)
							DoSpeak(fLead,16632,"Qu'est ce que c'est... la-bas... est-ce possible ? Heros, venez a moi !",CHAT_TYPE_SAY);
						else
							DoSpeak(fLead,17049,"Je ne peux y croire. Deuillegivre est devant nous, comme l'avait annonce le gnome. Venez, Heros !",CHAT_TYPE_SAY);
						event_Timer = 8000;
					}
					break;
				case 2:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16633,"Deuillegivre. La lame qui a detruit notre royaume.",CHAT_TYPE_SAY);
							event_Timer = 5000;
						}
						else
						{
							DoSpeak(fLead,17050,"Contempler la lame qui m'a ote la vie. La douleur en est... ravivee.",CHAT_TYPE_SAY);
							event_Timer = 6000;
						}						
					}
					break;
				case 3:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16634,"Reculez ! Touchez seulement cette lame, et votre ame en sera meurtrie pour l'eternite. Il faut que j'essaie d'entrer"
							"en contact avec les esprits emprisonnes dans Deuillegivre. Faites moi de la place. Reculez s'il vous plait.",CHAT_TYPE_YELL);
							event_Timer = 12000;
						}
						else
						{
							DoSpeak(fLead,17051,"Je n'ose pas la toucher. Reculez, reculez. Je vais essayer d'entrer en harmonie avec la lame. Elle pourrait renfermer la clef"
							" de notre salut.",CHAT_TYPE_SAY);
							event_Timer = 8000;
						}
					}
					break;
				case 4:
					OpenDoor(Frostmourne);
					event_Timer = 4000;
					break;
				case 5:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(Creature* Uther = ((HoR_fLead_frostmourneAI*)fLead->AI())->CallCreature(37225,TEN_MINS,PREC_COORDS,NOTHING,5303.01f,2005.476f,709.342f))
						{
							UtherGUID = Uther->GetGUID();
							Uther->SetFacingToObject(fLead);
							fLead->SetFacingToObject(Uther);
							if(uiTeamInInstance == ALLIANCE)
							{
								DoSpeak(Uther,16666,"Jaina. Est-ce vraiment toi?",CHAT_TYPE_YELL);
								event_Timer = 3000;
							}
							else
							{
								DoSpeak(Uther,16659,"Attention, mon enfant ce n'est pas la premiere fois que j'entend que cette maudite lame va nous sauver. Regardez autour de vous,"
								"Voyez ce qu'elle a engendré",CHAT_TYPE_SAY);
								event_Timer = 10500;
							}
						}
					}
					break;
				case 6:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16635,"Uther, mon cher Uther, oh, je suis si désolée.",CHAT_TYPE_SAY);
							event_Timer = 5500;
						}
						else
						{
							DoSpeak(fLead,17052,"Uther. Uther le Porte-Lumière ! Comment ?",CHAT_TYPE_SAY);
							event_Timer = 3000;
						}
					}
					break;
				case 7:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
							DoSpeak(Uther,16667,"Jaina, tu n'as pas beaucoup de temps. Le Roi Liche voit ce que voit l'épée. Il ne va pas tarder.",CHAT_TYPE_SAY);
						else
							DoSpeak(Uther,16660,"Vous n'avez pas beaucoup de temps. Le Roi Liche voit ce que voit l'épée. Il ne va pas tarder.",CHAT_TYPE_SAY);

						event_Timer = 6500;
					}
					break;
				case 8:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16636,"Arthas est ici ? Peut être que...",CHAT_TYPE_SAY);
							event_Timer = 1800;
						}
						else
						{
							DoSpeak(fLead,17053,"Le Roi Liche est ici ? Alors c'est aujourd'hui que s'accomplit ma destinée.",CHAT_TYPE_SAY);
							event_Timer = 3800;
						}
					}
					break;
				case 9:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(Uther,16668,"Non, ma fille. Arthas n'est pas là. Arthas n'est qu'une présence dans l'esprit du Roi Liche. Une présence qui s'estompe.",CHAT_TYPE_SAY);
							event_Timer = 9500;
						}
						else
						{
							DoSpeak(Uther,16661,"Vous ne pouvez pas vaincre le Roi Liche. Pas ici. Essayer serait une folie. Il tuera ceux qui vous suivent et les relèvera "
							"comme puissants soldats du fléau ! Mais pour vous Sylvanas, sa récompense sera encore pire que la dernière fois.",CHAT_TYPE_YELL);
							event_Timer = 19000;
						}

					}
					break;
				case 10:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16637,"Mais Uther s'il doit y avoir le moindre moyen d'atteindre Arthas, je... je dois essayer",CHAT_TYPE_SAY);
							event_Timer = 4000;
						}
						else
						{
							DoSpeak(fLead,17054,"Il doit y avoir un moyen !",CHAT_TYPE_YELL);
							event_Timer = 1500;
						}
					}
					break;
				case 11:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(Uther,16669,"Jaina, écoute moi. Il faut détruire le Roi Liche. Il n'est pas possible de discuter avec lui. Il a vous tuer toi et tes alliés "
							"et vous relever comme puissants soldats du fléau.",CHAT_TYPE_SAY);
							event_Timer = 11500;
						}
						else
						{
							DoSpeak(Uther,16662,"Peut être. Mais sachez le bien, il doit toujours y avoir un Roi Liche. Même si vous parvenez à abattre Arthas, il faudrait "
							"qu'un autre prenne sa place. Car vidé du contrôle de son Roi, le fléau déferlerait sur le monde comme une nuée de sauterelles et anéantirait "
							"tout sur son passage.",CHAT_TYPE_SAY);
							event_Timer = 20000;
						}

					}
					break;
				case 12:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16638,"Uther, dites moi comment détruire mon prince.",CHAT_TYPE_SAY);
							event_Timer = 4000;
						}
						else
						{
							DoSpeak(fLead,17055,"Qui pourrait porter un tel fardeau ?",CHAT_TYPE_SAY);
							event_Timer = 2000;
						}
					}
					break;
				case 13:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(Uther,16670,"Réveille toi ma petite. Il faut l'anéantir à l'endroit où il a fusionné avec Ner'zhul, en haut de la tour, devant le trône de glace. "
							"C'est le seul moyen.",CHAT_TYPE_SAY);
							event_Step = 14;
							event_Timer = 12000;
						}
						else
						{
							DoSpeak(Uther,16663,"Je ne sais pas Reine Banshee. Je suspecte le fragment d'Arthas qui reste peut être enfoui dans l'esprit du Roi Liche "
							"d'être tout ce qui retient le fléau d'anihiler Azeroth.",CHAT_TYPE_SAY);
							event_Timer = 13000;
						}
					}
					break;
				case 14:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance != ALLIANCE)
						{
							DoSpeak(Uther,16664,"Mais hélas, le seul moyen de le vaincre est de l'abattre, à l'endroit où il fut créé.",CHAT_TYPE_SAY);
							event_Timer = 8000;
						}
					}
					break;
				case 15:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16639,"Vous avez raison Uther, pardonnez moi. Je... je ne sais pas ce qui m'a pris. Je vais porter ces informations aux "
							"Rois et au chevaliers qui combattent le fléau dans la Citadelle de la Couronne de Glace",CHAT_TYPE_SAY);
							event_Timer = 10500;
						}
						else
						{
							DoSpeak(fLead,17056,"Le Trône de Glace...",CHAT_TYPE_SAY);
							event_Step = 20;
							event_Timer = 2000;
						}

						event_Timer = 15000;
					}
					break;
				case 16:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(Uther,16671,"Il y a autre chose à savoir sur le Roi Liche. Il ne faut jamais que le fléau se retrouve sans contrôle. Même si vous "
							"parvenez à abattre le Roi Liche il faut qu'un autre prenne sa place. Car sans son emprise sur le fléau, il serait lâché sur le monde "
							"et détruirait toute vie.",CHAT_TYPE_SAY);
							event_Timer = 23000;
						}
					}
					break;
				case 17:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16640,"Qui pourrait porter un tel fardeau ?",CHAT_TYPE_SAY);
							event_Timer = 2000;
						}
					}
					break;
				case 18:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(Uther,16672,"Un grand sacrifice, par une âme noble.",CHAT_TYPE_SAY);
							event_Timer = 4000;
						}
					}
					break;
				case 19:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16641,"Alors il reste peut être un espoir.",CHAT_TYPE_SAY);
							event_Timer = 2000;
						}
					}
					break;
				case 20:
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(Uther,16673,"Je ne sais pas Jaina. Je suspecte le fragment d'Arthas, qui reste peut être enfoui dans l'esprit du Roi Liche "
							"d'être tout ce qui le retient d'envahir Azeroth.",CHAT_TYPE_SAY);
							event_Timer = 11000;
						}
					}
					break;
				case 21:
					OpenDoor(GetData64(DATA_DOOR_LICHKING));
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
						LichKing->GetMotionMaster()->MovePoint(0,5320.96f,2017.744f,707.695f);
					event_Timer = 2000;
					break;
				case 22:
					CloseDoor(GetData64(DATA_DOOR_LICHKING));
					if(Creature* Uther = GetCreatureInMap(UtherGUID))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(Uther,16674,"Non Jaina. Nooooooooon ! Arrhhhhhhhhhh ! Il arrive... Tu... tu dois...",CHAT_TYPE_YELL);
							event_Timer = 10000;
						}
						else
						{
							DoSpeak(Uther,16665,"Oui. Arrrghhh. Il arrive. Il faut...",CHAT_TYPE_SAY);
							event_Timer = 11000;
						}
					}
					break;
				case 23:
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
					{
						DoSpeak(LichKing,17225,"Silence, Paladin !",CHAT_TYPE_YELL);
						if(Creature* Uther = GetCreatureInMap(UtherGUID))
							LichKing->CastSpell(Uther,31008,true);
						event_Timer = 4000;
					}
					break;
				case 24:
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
					{
						DoSpeak(LichKing,17226,"Alors comme ca, vous voulez converser avec les morts ? Je vais exaucer votre souhait.",CHAT_TYPE_YELL);
						event_Timer = 10000;
					}
					break;
				case 25:
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
					{
						DoSpeak(LichKing,17227,"Falric, Marwyn, traînez leurs corps jusqu'à mon trône. Quand vous en aurez fini.",CHAT_TYPE_YELL);
						event_Timer = 8500;
					}
					break;
				case 26:
					if(Creature* Falric = GetCreatureInMap(GetData64(TYPE_FALRIC)))
					{
						DoSpeak(Falric,16717,"A vos ordres monseigneur !",CHAT_TYPE_YELL);
						event_Timer = 1500;
					}
					break;
				case 27:
					if(Creature* Marwyn = GetCreatureInMap(GetData64(TYPE_MARWYN)))
					{
						DoSpeak(Marwyn,16741,"A vos ordres, monseigneur !",CHAT_TYPE_YELL);
						event_Timer = 2500;
					}
					break;
				case 28:
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(fLead,16642,"Tu ne m'en empêchera pas Arthas. Il faut que je sache, je dois savoir !",CHAT_TYPE_YELL);
							event_Timer = 5000;
						}
						else
						{
							DoSpeak(fLead,17057,"Tu ne m'échappera pas aussi facilement, Arthas. J'aurais ma vengeance.",CHAT_TYPE_SAY);
							event_Timer = 4000;
						}
					}
					break;
				case 29:
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
					{
						if(uiTeamInInstance == ALLIANCE)
						{
							DoSpeak(LichKing,17229,"Jeune écervelée. Ce que vous cherchez je l'ai tué, il y a longtemps. Il n'est plus qu'un fantôme, "
							"un écho lointain, dans mon esprit.",CHAT_TYPE_YELL);
							event_Timer = 15000;
						}
						else
						{
							DoSpeak(LichKing,17228,"Je ne ferai pas la même erreur une deuxième fois Sylvanas. Cette fois tu ne m'échapperas pas. "
							"Tu as échoué à me servir dans la non-mort. Tout ce qui t'attend désormais, est le néant.",CHAT_TYPE_YELL);
							event_Timer = 20000;
						}
					}
					break;
				case 30:
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
						LichKing->GetMotionMaster()->MovePoint(0,5394.384f,2096.59f,707.7f);
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
						fLead->GetMotionMaster()->MovePoint(0,5394.384f,2096.59f,707.7f);
					event_Timer = 3000;
					break;
				case 31:
					OpenDoor(GetData64(DATA_DOOR_LICHKING));
					event_Timer = 2000;
					break;
				case 32:
					CloseDoor(GetData64(DATA_DOOR_LICHKING));
					event_Timer = 2000;
					break;
				case 33:
					if(Creature* LichKing = GetCreatureInMap(GetData64(TYPE_LICHKING)))
						LichKing->ForcedDespawn(2000);
					if(Creature* fLead = GetCreatureInMap(GetData64(TYPE_FACTIONLEADER_EV1)))
						fLead->ForcedDespawn(2000);
				default:
					RPFrostmourneDone = true;
					SetData(TYPE_DIAL_FROSTMOURNE,DONE);
					break;
			}
			event_Step++;
		}
		else
			event_Timer -= diff;
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
				if(Creature* Falric = GetCreatureInMap(GetData64(TYPE_FALRIC)))
					DoSpeak(Falric,16714,"Soldats de Lordaeron, repondez a l'appel de votre maitre !",CHAT_TYPE_YELL);
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
				if(Creature* Marwyn = GetCreatureInMap(GetData64(TYPE_MARWYN)))
					DoSpeak(Marwyn,16738,"Le maitre s'est penche sur son royaume, et l'a trouve... imparfait. Sa sentence fut sans pitie. La mort, pour tous !",CHAT_TYPE_YELL);
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

			if(nbr > 3)
			{
				if(Creature* Rogue = GetCreatureInMap(GetData64(DATA_RANDOM_HUNT)))
				{
					Rogue->RemoveAurasDueToSpell(66830);
					Rogue->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Rogue->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					AggroPlayersInMap(Rogue);
				}
			}

			if(nbr > 4)
			{
				if(Creature* Hunt = GetCreatureInMap(GetData64(DATA_RANDOM_HUNT)))
				{
					Hunt->RemoveAurasDueToSpell(66830);
					Hunt->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Hunt->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					AggroPlayersInMap(Hunt);
				}
			}

			if(nbr > 5)
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
