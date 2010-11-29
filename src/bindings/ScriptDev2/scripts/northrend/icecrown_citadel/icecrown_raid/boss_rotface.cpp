#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_OOZE_FLOOD				= 69783, // use this
    SPELL_SLIME_SPRAY				= 69508,
    SPELL_MUTATED_INFECTION			= 69674, // ok

	// Little
    SPELL_STICKY_OOZE				= 69774, // ok
    SPELL_STICKY_AURA				= 69776, // ok
    SPELL_MERGE_OOZE				= 69889, // ok
    SPELL_RADIATING_OOZE			= 69750, // ok

	// Big
    SPELL_UNSTABLE_OOZE				= 69644, // ok
    SPELL_UNSTABLE_OOZE_AURA		= 69558, // ok
    SPELL_OOZE_EXPLODE				= 69839, // ok
	SPELL_BIG_RADIATING_OOZE		= 69760, // ok

    NPC_BIG_OOZE					= 36899,
    NPC_SMALL_OOZE					= 36897,
    NPC_STICKY_OOZE					= 37006,
    NPC_OOZE_SPRAY_STALKER			= 37986,
    NPC_OOZE_STALKER				= 37013,
    NPC_OOZE_EXPLODE_STALKER		= 38107,
	NPC_OOZE_ROBINET				= 31245,
};

static float SpawnLoc[8][3] =
{
	{4464.04,	3098.95f,	360.39f}, // 1st
	{4485.12f,	3155.27f,	360.39f}, // 2nd
	{4429.07f,	3175.10f,	360.39f}, // 3rd
	{4408.15f,	3119.775f,	360.39f}, // 4th
	{4482.756f,	3117.05f,	360.39f}, // 1st
	{4465.52f,	3174.39f,	360.39f}, // 2nde
	{4409.20f,	3156.56f,	360.39f}, // 3rd
	{4426.61f,	3101.08f,	360.39f}, // 4th
};

static float RobinetLoc[8][3] =
{
	{4467.836f,	3095.145f,	369.4f},
	{4488.21f,	3159.03f,	369.4f},
	{4424.134f,	3176.317f,	369.4f},
	{4403.838f,	3115.386f,	369.4f},
	{4486.983f,	3115.19f,	369.4f},
	{4467.707f,	3178.774f,	369.4f},
	{4404.147f,	3158.814f,	369.4f},
	{4423.889f,	3095.881f,	369.4f}
};

struct MANGOS_DLL_DECL boss_rotfaceAI : public LibDevFSAI
{
    boss_rotfaceAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint8 pool;
	uint32 pool_Timer;
	uint32 check_Timer;
	uint32 spray_Timer;
	uint32 infection_Timer;
	uint8 infection_nb;
	bool HF;
	std::set<uint64> PlayerSet;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		PlayerSet.clear();
		pool = urand(0,3);
		pool_Timer = 30000;
		check_Timer = 1000;
		spray_Timer = 17000;
		infection_nb = 0;
		infection_Timer = 11000;
		HF = true;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            SetInstanceData(TYPE_ROTFACE, IN_PROGRESS);

		Yell(16986,"Ouiiiiiiiiiii ! Hahahaha !");
		me->SetInCombatWithZone();
    }

	void SpellHitTarget(Unit* who, const SpellEntry* sp)
	{
		if(sp->Id == SPELL_MUTATED_INFECTION)
		{
			check_Timer = 700;
			PlayerSet.insert(who->GetGUID());
		}
	}

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Say(16987,"Oh c'est cassé !");
		else
			Say(16988,"Papa va faire des nouveaux jouets avec toi !");
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            SetInstanceData(TYPE_ROTFACE, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,2);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				GiveEmblemsToGroup(TRIOMPHE,1);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
		}

		if(pInstance && HF)
		{
			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
				case RAID_DIFFICULTY_10MAN_HEROIC:
					pInstance->CompleteAchievementForGroup(4538);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
				case RAID_DIFFICULTY_25MAN_HEROIC:
					pInstance->CompleteAchievementForGroup(4614);
					break;
			}
		}

		Say(16989,"Mauvaises nouvelles papa...");
    }

    void JustReachedHome()
    {
        if (pInstance)
            SetInstanceData(TYPE_ROTFACE, FAIL);
    }

	void CallBigOne(Unit* u)
	{
		CallAggressiveCreature(NPC_BIG_OOZE,TEN_MINS,PREC_COORDS,u->GetPositionX(),u->GetPositionY(),u->GetPositionZ() + 0.5f);
	}

	void HFFail()
	{
		HF = false;
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(infection_Timer <= diff)
		{
			Yell(16991,"Ah c'est tout englué.");
			DoCastRandom(SPELL_MUTATED_INFECTION);
			infection_nb++;
			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
				case RAID_DIFFICULTY_10MAN_HEROIC:
				{
					if(infection_nb > 12)
						infection_Timer = 8000;
					else if(infection_nb > 8)
						infection_Timer = 10000;
					else if(infection_nb > 5)
						infection_Timer = 11000;
					else if(infection_nb > 2)
						infection_Timer = 12500;
					else
						infection_Timer = 14000;
					break;
				}
				case RAID_DIFFICULTY_25MAN_NORMAL:
				case RAID_DIFFICULTY_25MAN_HEROIC:
				{
					if(infection_nb > 12)
						infection_Timer = 9000;
					else if(infection_nb > 8)
						infection_Timer = 11000;
					else if(infection_nb > 5)
						infection_Timer = 12500;
					else if(infection_nb > 2)
						infection_Timer = 15500;
					else
						infection_Timer = 17000;
					break;
				}
			}
		}
		else
			infection_Timer -= diff;

		if(check_Timer <= diff)
		{
			std::set<uint64> removableGuids;
			removableGuids.clear();
			for(std::set<uint64>::iterator itr = PlayerSet.begin(); itr != PlayerSet.end(); ++itr)
			{
				if(Unit* u = GetGuidUnit(*itr))
				{
					bool spawn = false;
					if(u->isAlive())
					{
						if(!u->HasAura(SPELL_MUTATED_INFECTION))
							spawn = true;
					}
					else
						spawn = true;

					if(spawn)
					{
						if(Creature* cr = CallAggressiveCreature(NPC_SMALL_OOZE,TEN_MINS,PREC_COORDS,u->GetPositionX(),u->GetPositionY(),u->GetPositionZ()))
							u->AddThreat(cr,100000.0f);
						removableGuids.insert(*itr);
					}
				}
			}

			for(std::set<uint64>::iterator itr = removableGuids.begin(); itr != removableGuids.end(); ++itr)
				PlayerSet.erase(*itr);

			check_Timer = 300;
		}
		else
			check_Timer = diff;

		if(pool_Timer <= diff)
		{
			if(Creature* cr = GetInstanceCreature(TYPE_PUTRICIDE))
				Yell(17126,"Merveille nouvelle mes amis, la gelée coule à flots !");

			if(Creature* cr1 = CallCreature(NPC_OOZE_STALKER,25000,PREC_COORDS,NOTHING,SpawnLoc[pool][0],SpawnLoc[pool][1],SpawnLoc[pool][2]))
				if(Creature* cr3 = CallCreature(NPC_OOZE_ROBINET,8000,PREC_COORDS,NOTHING,RobinetLoc[pool][0],RobinetLoc[pool][1],RobinetLoc[pool][2]))
					cr1->AddThreat(cr3,100000.0f);

			if(Creature* cr2 = CallCreature(NPC_OOZE_STALKER,25000,PREC_COORDS,NOTHING,SpawnLoc[pool+4][0],SpawnLoc[pool+4][1],SpawnLoc[pool+4][2]))
				if(Creature* cr4 = CallCreature(NPC_OOZE_ROBINET,8000,PREC_COORDS,NOTHING,RobinetLoc[pool+4][0],RobinetLoc[pool+4][1],RobinetLoc[pool+4][2]))
					cr2->AddThreat(cr4,100000.0f);
			pool++;
			if(pool >= 4)
				pool = 0;

			pool_Timer = 20000;
		}
		else
			pool_Timer -= diff;

		if(spray_Timer <= diff)
		{
			if(Unit* u = GetRandomUnit(0))
			{
				if(Creature* cr = CallCreature(NPC_OOZE_SPRAY_STALKER,15000,PREC_COORDS,NOTHING,u->GetPositionX(),u->GetPositionY(),u->GetPositionZ()))
				{
					cr->SetDisplayId(16925);
					cr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
					cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					cr->setFaction(14);
					ModifyAuraStack(66830,1,cr,cr);
				}
				DoCast(u,SPELL_SLIME_SPRAY);
			}
			spray_Timer = 18000;
		}
		else
			spray_Timer -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rotface(Creature* pCreature)
{
    return new boss_rotfaceAI(pCreature);
}

struct MANGOS_DLL_DECL big_limonAI : public LibDevFSAI
{
    big_limonAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_STICKY_OOZE,8000,12000,1500);
    }

	uint32 check_Timer;
	uint32 spawn_explode;
	bool inDespawn;

    void Reset()
    {
		ResetTimers();
		DoCastMe(SPELL_BIG_RADIATING_OOZE);

		check_Timer = 1000;
		inDespawn = false;
		spawn_explode = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(check_Timer <= diff)
		{
			if(Aura* aur = me->GetAura(SPELL_UNSTABLE_OOZE_AURA))
			{
				if(aur->GetStackAmount() >= 5)
				{
					spawn_explode += 500;
					if(!inDespawn)
					{
						me->CastStop();
						if(Creature* cr = GetInstanceCreature(TYPE_ROTFACE))
						{
							((boss_rotfaceAI*)cr->AI())->HFFail();
						}
						DoCastMe(SPELL_OOZE_EXPLODE);
						me->ForcedDespawn(7500);
						inDespawn = true;
					}
				}
			}
			check_Timer = 500;
		}
		else
			check_Timer -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_big_limon(Creature* pCreature)
{
    return new big_limonAI(pCreature);
}

struct MANGOS_DLL_DECL small_limonAI : public LibDevFSAI
{
    small_limonAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_STICKY_OOZE,8000,12000,1500);
    }

	uint32 check_Timer;

    void Reset()
    {
		ResetTimers();
		DoCastMe(SPELL_RADIATING_OOZE);
		CanBeTaunt(false);
		check_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(check_Timer <= diff)
		{
			bool found = false;
			if(Creature* cr = me->GetClosestCreatureWithEntry(NPC_BIG_OOZE,10.0f))
			{
				if(cr->isAlive())
				{
					uint8 stk = 1;
					if(Aura* aur = me->GetAura(SPELL_UNSTABLE_OOZE_AURA))
						stk += aur->GetStackAmount();
					
					ModifyAuraStack(SPELL_UNSTABLE_OOZE_AURA,stk,cr,cr);
					me->ForcedDespawn(500);
					found = true;
				}
			}

			if(Creature* cr = me->GetClosestCreatureWithEntry(NPC_SMALL_OOZE,10.0f))
			{
				if(cr->isAlive() && cr != me && !found)
				{
					//DoCast(cr,SPELL_MERGE_OOZE);
					if(Creature* Rotface = GetInstanceCreature(TYPE_ROTFACE))
					{
						((boss_rotfaceAI*)Rotface->AI())->CallBigOne(me);
					}
					Kill(me);
					Kill(cr);
				}
			}
			check_Timer = 1000;
		}
		else
			check_Timer -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_small_limon(Creature* pCreature)
{
    return new small_limonAI(pCreature);
}

struct MANGOS_DLL_DECL sticky_oozeAI : public LibDevFSAI
{
    sticky_oozeAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		MakeHostileInvisibleStalker();
		DoCastMe(SPELL_STICKY_AURA);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_NORMAL:
				me->ForcedDespawn(60000);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				me->ForcedDespawn(300000);
				break;
		}
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_sticky_ooze(Creature* pCreature)
{
    return new sticky_oozeAI(pCreature);
}

struct MANGOS_DLL_DECL ooze_stalkerAI : public LibDevFSAI
{
    ooze_stalkerAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		MakeHostileInvisibleStalker();
		ModifyAuraStack(SPELL_OOZE_FLOOD);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_ooze_stalker(Creature* pCreature)
{
    return new ooze_stalkerAI(pCreature);
}

struct MANGOS_DLL_DECL ooze_flyingAI : public LibDevFSAI
{
    ooze_flyingAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		SetFlying(true);
		SetCombatMovement(false);
		MakeHostileInvisibleStalker();
		me->setFaction(35);
		DoCastMe(SPELL_OOZE_FLOOD);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_ooze_flying(Creature* pCreature)
{
    return new ooze_flyingAI(pCreature);
}

void AddSC_ICC_Rotface()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_rotface";
    NewScript->GetAI = &GetAI_boss_rotface;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_small_ooze";
    NewScript->GetAI = &GetAI_small_limon;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_big_ooze";
    NewScript->GetAI = &GetAI_big_limon;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_sticky_ooze";
    NewScript->GetAI = &GetAI_sticky_ooze;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_ooze_stalker";
    NewScript->GetAI = &GetAI_ooze_stalker;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_ooze_flying";
    NewScript->GetAI = &GetAI_ooze_flying;
    NewScript->RegisterSelf();
}
