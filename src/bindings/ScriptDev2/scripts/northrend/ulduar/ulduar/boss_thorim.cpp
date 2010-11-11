#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
	SPELL_ENRAGE				=	26662,
	// phase 1
	SPELL_STEALTH_OF_LIGHT		=	62276,
	SPELL_STORMHAMMER_10		=	62042,
	SPELL_STORMHAMMER_25		=	64767,
	SPELL_THUNDER				=	62470,
	SPELL_CHARGE_ORB			=	62016,
	SPELL_BERSERK				=	62560,
	SPELL_SUMMON_ORB			=	62391,

	// phase 2
	SPELL_TOUCH_OF_DOMINION		=	62565,
	SPELL_CHAIN_LIGHTNING_10	=	62131,
	SPELL_CHAIN_LIGHTNING_25	=	64390,
	SPELL_LIGHTNING_CHARGE		=	62279,
	SPELL_STRIKE				=	62130,

	// hardmode
	SPELL_FROSTBOLT_10			=	62580,
	SPELL_FROSTBOLT_25			=	62604,
	SPELL_FROSTNOVA_10			=	62597,
	SPELL_FROSTNOVA_25			=	62605,
	SPELL_BLIZZARD_10			=	62576,
	SPELL_BLIZZARD_25			=	62602,
};

enum ThorimAdds
{
	//prephase
	NPC_BEHEMOT					=	32882,
	NPC_HORDE_CAPTAIN			=	32907,
	NPC_ALLIA_CAPTAIN			=	32908,
	NPC_HORDE_SOLDIER			=	32883,
	NPC_ALLIA_SOLDIER			=	32885,

	//phase 1
	NPC_ACOLYTE					=	33110,
	NPC_CHAMPION				=	32876,
	NPC_COMMONER				=	32904,
	NPC_EVOKER					=	32878,
	NPC_WARBRINGER				=	32877,

};

static const float SpawnLocs[4][3] =
{
	{2095.561f,	-279.494f,	420.146f},
	{2172.044f,	-246.257f,	420.146f},
	{2173.538f,	-278.739f,	420.146f},
	{2095.822f,	-246.827f,	420.146f},
};

struct MANGOS_DLL_DECL boss_thorimAI : public ScriptedAI
{
    boss_thorimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;
	uint8 phase;
	uint32 CheckPlayer_Timer;
	uint32 SpawnTimer;
	uint32 SpawnCount;
	uint32 CheckDistanceTimer;
	uint32 Thunder_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		phase = 0;
		Tasks.AddEvent(SPELL_BERSERK,300000,30000,0,TARGET_ME,1);
		Tasks.AddEvent(SPELL_CHARGE_ORB,18000,18000,0,TARGET_RANDOM,1);
		Tasks.AddEvent(SPELL_LIGHTNING_CHARGE,45000,60000,0,TARGET_MAIN,2);
		Tasks.AddEvent(SPELL_STRIKE,30000,30000,0,TARGET_MAIN,2);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_STORMHAMMER_10,35000,35000,0,TARGET_RANDOM,1);
			Tasks.AddEvent(SPELL_CHAIN_LIGHTNING_10,25000,25000,0,TARGET_RANDOM,2);
		}
		else
		{
			Tasks.AddEvent(SPELL_STORMHAMMER_25,35000,35000,0,TARGET_RANDOM,1);
			Tasks.AddEvent(SPELL_CHAIN_LIGHTNING_25,25000,25000,0,TARGET_RANDOM,2);
		}
		pInstance->SetData(DATA_THORIM_ADDS,0);
		CheckPlayer_Timer = 1000;
		SpawnTimer = 1000;
		SpawnCount = 0;
		Thunder_Timer = 15000;
		if (Creature* pSerpent = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_THORIM_BIGADD))))
			pSerpent->Respawn();

		if (GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(DATA_THORIM_DOOR)))
             pDoor->SetGoState(GO_STATE_ACTIVE);

		CheckDistanceTimer = 2500;
    }

    void KilledUnit(Unit *victim)
    {
		if(urand(0,1))
			Speak(CHAT_TYPE_SAY,15739,"Ne pourriez vous pas au moins essayer de vous défendre ?");
		else
			Speak(CHAT_TYPE_SAY,15740,"Pitoyable...");
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_THORIM, DONE);

		Speak(CHAT_TYPE_SAY,15743,"Retenez vos coups ! Je me rend.");
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE);
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_THORIM, IN_PROGRESS);

		DoCastMe(SPELL_STEALTH_OF_LIGHT);
    }

	void StartEvent(Unit* target)
	{
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE);
		if(target)
			me->AddThreat(target,200.0f);

		AggroAllPlayers(400.0f);

		phase = 1;
		if (GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(DATA_THORIM_DOOR)))
            pDoor->SetGoState(GO_STATE_ACTIVE);
		SetCombatMovement(false);
		Relocate(2134.884f,-290.130f,438.247f);
	}


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase == 1)
		{
			if(CheckPlayer_Timer <= diff)
			{
				Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
				if (!lPlayers.isEmpty())
				{
					for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					{
						if (Player* pPlayer = itr->getSource())
							if(me->GetDistance2d(pPlayer) < 3.0f && me->GetDistanceZ(pPlayer) < 2.0f)
							{
								phase = 2;
								me->RemoveAurasDueToSpell(SPELL_STEALTH_OF_LIGHT);
								SetCombatMovement(true);

								if (Creature* pSerpent = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_THORIM_BIGADD))))
									Relocate(pSerpent);
							}
					}
				}
				CheckPlayer_Timer = 1000;
			}
			else
				CheckPlayer_Timer -= diff;

			if(Thunder_Timer <= diff)
			{
				pInstance->SetData(DATA_THORIM_ORB,0);
				Thunder_Timer = 15000;
			}
			else
				Thunder_Timer -= diff;

			if(SpawnTimer <= diff)
			{
				switch(SpawnCount % 5)
				{
					case 0:
						for(short i=0;i<(m_bIsHeroic ? 4 : 3);i++)
						{
							int _rand = urand(0,3);
							Tasks.CallCreature(NPC_ACOLYTE,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLocs[_rand][0],SpawnLocs[_rand][1],SpawnLocs[_rand][2]);
						}
						break;
					case 1:
						{
							int _rand = urand(0,3);
							Tasks.CallCreature(NPC_CHAMPION,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLocs[_rand][0],SpawnLocs[_rand][1],SpawnLocs[_rand][2]);
							 _rand = urand(0,3);
							Tasks.CallCreature(NPC_CHAMPION,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLocs[_rand][0],SpawnLocs[_rand][1],SpawnLocs[_rand][2]);
							break;
						}
					case 2:
						for(short i=0;i<(m_bIsHeroic ? 10 : 6);i++)
						{
							int _rand = urand(0,3);
							Tasks.CallCreature(NPC_COMMONER,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLocs[_rand][0],SpawnLocs[_rand][1],SpawnLocs[_rand][2]);
						}
						break;
					case 3:
						for(short i=0;i<(m_bIsHeroic ? 5 : 3);i++)
						{
							int _rand = urand(0,3);
							Tasks.CallCreature(NPC_EVOKER,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLocs[_rand][0],SpawnLocs[_rand][1],SpawnLocs[_rand][2]);
						}
						break;
					case 4:
						for(short i=0;i<(m_bIsHeroic ? 3 : 2);i++)
						{
							int _rand = urand(0,3);
							Tasks.CallCreature(NPC_WARBRINGER,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLocs[_rand][0],SpawnLocs[_rand][1],SpawnLocs[_rand][2]);
						}
						break;
					default:
						break;
				}
				SpawnCount++;
				SpawnTimer = 19000;
			}
			else
				SpawnTimer -= diff;
		}

		Tasks.UpdateEvent(diff,phase);
		Tasks.UpdateEvent(diff);

		if(CheckDistanceTimer < diff)
		{
			if(phase == 2 && me->getVictim() && me->getVictim()->GetDistance2d(me) > 10.0f)
			{
				me->GetMotionMaster()->MovePoint(0,me->getVictim()->GetPositionX(),me->getVictim()->GetPositionY(),me->getVictim()->GetPositionZ());
				me->SendMonsterMove(me->getVictim()->GetPositionX(),me->getVictim()->GetPositionY(),me->getVictim()->GetPositionZ(), SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, 0);
			}
			CheckDistanceTimer = 2500;
		}
		else
			CheckDistanceTimer -= diff;
        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_thorim(Creature* pCreature)
{
    return new boss_thorimAI(pCreature);
}


struct MANGOS_DLL_DECL boss_addThorimAI : public ScriptedAI
{
    boss_addThorimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(62415,1000,10000,3000);
			Tasks.AddEvent(62417,2000,10000,1000);
		}
		else
		{
			Tasks.AddEvent(62315,1000,10000,3000);
			Tasks.AddEvent(62316,2000,10000,1000);
		}
    }

    void JustDied(Unit* victim)
    {
      if (Creature* Thorim = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_THORIM))))
		{
			((boss_thorimAI*)Thorim->AI())->StartEvent(victim);
		}
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
		SetCombatMovement(false);
		
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }

};

struct MANGOS_DLL_DECL boss_runegiantAI : public ScriptedAI
{
    boss_runegiantAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(62338,30000,30000,1000,TARGET_ME);
		Tasks.AddEvent(62339,2000,30000,1000,TARGET_MAIN);
		if(m_bIsHeroic)
			Tasks.AddEvent(62613,1000,15000,1000);
		else
			Tasks.AddEvent(62614,1000,15000,1000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }

};

struct MANGOS_DLL_DECL boss_runecolossusAI : public ScriptedAI
{
    boss_runecolossusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(62526,10000,15000,1000);
		Tasks.AddEvent(62942,1000,30000,1000,TARGET_ME);
		if(m_bIsHeroic)
			Tasks.AddEvent(62411,4000,10000,1000);
		else
			Tasks.AddEvent(62413,4000,10000,1000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }

};
CreatureAI* GetAI_add_Thorim(Creature* pCreature)
{
    return new boss_addThorimAI(pCreature);
}

CreatureAI* GetAI_rune_giant(Creature* pCreature)
{
    return new boss_runegiantAI(pCreature);
}

CreatureAI* GetAI_runic_colossus(Creature* pCreature)
{
    return new boss_runecolossusAI(pCreature);
}

void AddSC_boss_thorim()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_thorim";
    newscript->GetAI = &GetAI_boss_thorim;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "thorim_add";
    newscript->GetAI = &GetAI_add_Thorim;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_rune_giant";
    newscript->GetAI = &GetAI_rune_giant;
    newscript->RegisterSelf();
	
	newscript = new Script;
    newscript->Name = "npc_runic_colossus";
    newscript->GetAI = &GetAI_runic_colossus;
    newscript->RegisterSelf();
}
