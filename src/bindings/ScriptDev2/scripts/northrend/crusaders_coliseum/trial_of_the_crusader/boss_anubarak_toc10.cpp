#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum
{
	SPELL_BERSERK					= 26662,
	SPELL_FREEZING_SLASH			= 66012,
	SPELL_LEECHING_SWARM			= 66118,
	SPELL_PENETRATING_COLD			= 66013,
	SPELL_SUBMERGE_0				= 53421,
	SPELL_SUMMON_BEATLE			    = 66339,
	SPELL_MARK						= 67574,
	SPELL_IMPALE					= 65919,

	// adds
	SPELL_EXPOSE_WEAKNESS			= 67847,
	SPELL_SPIDER_FRENZY				= 66129,
	SPELL_SUBMERGE_1				= 67322,
	SPELL_DETERMINATION				= 66092,
	SPELL_ACID_MANDIBLE				= 65775,
	SPELL_PERMAFROST				= 66193,
};

enum Adds
{
    NPC_FROST_SPHERE = 34606,
    NPC_BURROWER = 34607,
    NPC_SCARAB = 34605,
};

struct MANGOS_DLL_DECL boss_anubarakEdCAI : public ScriptedAI
{
    boss_anubarakEdCAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
		difficulty = me->GetMap()->GetDifficulty();
        Reset();
    }

	MobEventTasks Tasks;
	Difficulty difficulty;
	uint32 Submerge_Timer;
	uint8 phase;
	bool WasAggro;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_BERSERK,600000,60000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_FREEZING_SLASH,20000,20000,10000,TARGET_RANDOM,1);
		Tasks.AddEvent(SPELL_FREEZING_SLASH,20000,20000,10000,TARGET_RANDOM,3);
		Tasks.AddEvent(SPELL_IMPALE,5000,15000,0,TARGET_MAIN,2);
		Tasks.AddSummonEvent(NPC_SCARAB,20000,15000,2,5000,3,TEN_MINS,NEAR_15M);
		Tasks.AddSummonEvent(NPC_FROST_SPHERE,15000,15000,2,0,2,TEN_MINS,NEAR_30M);
		Tasks.AddEvent(SPELL_PENETRATING_COLD,20000,30000,0,TARGET_RANDOM,1);
		Tasks.AddEvent(SPELL_PENETRATING_COLD,20000,30000,0,TARGET_RANDOM,3);
		Tasks.AddSummonEvent(NPC_BURROWER,70000,100000,2);
		Submerge_Timer = 45000;
		phase = 1;
		WasAggro = false;
    }
	void MoveInLineOfSight(Unit* pWho) 
	{
		if(!WasAggro)
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetInCombatWithZone();
		WasAggro = true;
	}

    void KilledUnit(Unit *victim)
    {
		if(urand(0,1))
			Speak(CHAT_TYPE_SAY,16236,"Flat'chir");
		else
			Speak(CHAT_TYPE_SAY,16237,"Encore un âme pour repaître l'armée des morts");

		if(victim->GetTypeId() == TYPEID_PLAYER)
			if(pInstance)
				SetInstanceData(TYPE_TRY,1);
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            SetInstanceData(TYPE_ANUBARAK, DONE);

		switch(difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,4);
				break;
		}
		Yell(16238,"J'ai échoué... maître...");
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
		Speak(CHAT_TYPE_SAY,16234,"Ce terreau sera votre tombeau !");
        if (pInstance)
            SetInstanceData(TYPE_ANUBARAK, IN_PROGRESS);
    }
    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase == 1 || phase == 2)
		{
			if(Submerge_Timer <= diff)
			{
				if(!me->HasAura(SPELL_SUBMERGE_0))
				{
					phase = 2;
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					Yell(16240,"Aoum Na'akish ! DÃ©vorez mes serviteurs !");
					if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
					{
						ModifyAuraStack(SPELL_MARK,1,target);
						if(Creature* spike = Tasks.CallCreature(34660,60000))
						{
							spike->GetMotionMaster()->MoveFollow(target,0.5f,0.0f);
							spike->AddThreat(target,1000000.0f);
						}
					}
					me->CastStop();
					DoCastMe(SPELL_SUBMERGE_0);					
				}
				else
				{
					Speak(CHAT_TYPE_TEXT_EMOTE,0,"Anub'Arak ressort des profondeurs !");
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->RemoveAurasDueToSpell(SPELL_SUBMERGE_0);
					phase = 1;
				}
				Submerge_Timer = 60000;
			}
			else
				Submerge_Timer -= diff;
		}

		if(me->GetHealth() * 100 / me->GetMaxHealth() < 30.0f && phase != 3)
		{
			Yell(16241,"L'essaim va vous submerger !");
			DoCastMe(SPELL_LEECHING_SWARM);
			ModifyAuraStack(SPELL_LEECHING_SWARM);
			phase = 3;
		}

		Tasks.UpdateEvent(diff,phase);

		if(phase == 1 || phase == 3)
			DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_anubarakEdC(Creature* pCreature)
{
    return new boss_anubarakEdCAI(pCreature);
}

struct MANGOS_DLL_DECL mob_swarm_scarabAI : public LibDevFSAI
{
    mob_swarm_scarabAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_ACID_MANDIBLE,5000,20000);
		AddEventOnTank(SPELL_DETERMINATION,5000,10000,20000);
    }

    void Reset()
    {
		ResetTimers();		
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}
};

CreatureAI* GetAI_mob_swarm_scarab(Creature* pCreature)
{
    return new mob_swarm_scarabAI(pCreature);
}

struct MANGOS_DLL_DECL mob_nerubian_borrowerAI : public LibDevFSAI
{
    mob_nerubian_borrowerAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_EXPOSE_WEAKNESS,5000,5000);
		AddEventOnMe(SPELL_SPIDER_FRENZY,9000,10000,1000);
    }

	uint32 Submerge_Timer;
    bool submerged;

    void Reset()
    {
		ResetTimers();
		Submerge_Timer = 2000;
        submerged = false;
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(Submerge_Timer <= diff)
		{
			if (CheckPercentLife(20) && !submerged)
			{
				DoCastMe(SPELL_SUBMERGE_1);
				submerged = true;
				Speak(CHAT_TYPE_TEXT_EMOTE,0,"Fouisseur nérubien s'enfouit");
			}
			Submerge_Timer = urand(20000,30000);
		}
		else
			Submerge_Timer -= diff;

        if (GetPercentLife() > 50.0f && submerged)
        {
             me->RemoveAurasDueToSpell(SPELL_SUBMERGE_1);
             submerged = false;
             Speak(CHAT_TYPE_TEXT_EMOTE,0,"Fouisseur nérubien sort du sol");
         };

		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_nerubian_borrower(Creature* pCreature)
{
    return new mob_nerubian_borrowerAI(pCreature);
}

struct MANGOS_DLL_DECL anub_sphereAI : public ScriptedAI
{
    anub_sphereAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    
    uint32 explode_timer;
    MobEventTasks Tasks;
    void Reset()
    {
		Tasks.SetObjects(this,me);
		explode_timer = DAY*HOUR;
		SetCombatMovement(false);
    }

    void DamageTaken(Unit* u, uint32 &dmg)
    {
		if(dmg >= me->GetHealth())
		{
			 dmg = 0;
			 explode_timer = 1000;
		}
    }

    void UpdateAI(const uint32 diff)
    {
		if(explode_timer <= diff)
		{
			DoCastMe(SPELL_PERMAFROST);
			Kill(me);
		}
		else 
			explode_timer -= diff;
    }

};

CreatureAI* GetAI_anub_sphere(Creature* pCreature)
{
    return new anub_sphereAI(pCreature);
}

struct MANGOS_DLL_DECL anub_spikeAI : public LibDevFSAI
{
    anub_spikeAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_IMPALE,500,1500,500);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
    
    void Reset()
    {
		ResetTimers();
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }

};

CreatureAI* GetAI_anub_spike(Creature* pCreature)
{
    return new anub_spikeAI(pCreature);
}

void AddSC_boss_AA_toc10()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anubarakEdC";
    newscript->GetAI = &GetAI_boss_anubarakEdC;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_swarm_scarab";
    newscript->GetAI = &GetAI_mob_swarm_scarab;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nerubian_borrower";
    newscript->GetAI = &GetAI_mob_nerubian_borrower;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "anub_sphere";
    newscript->GetAI = &GetAI_anub_sphere;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "anub_spike";
    newscript->GetAI = &GetAI_anub_spike;
    newscript->RegisterSelf();
};
