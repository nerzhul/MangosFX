#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_SLIME_PUDDLE            = 70343,
    SPELL_UNSTABLE_EXPERIMENT     = 71968, // p1 p2 choose experience
    SPELL_TEAR_GAS                = 71617, // to switch phase
    SPELL_TEAR_GAS_1              = 71618, // triggered, remove if not needed
    SPELL_CREATE_CONCOCTION       = 71621, // ok
    SPELL_MALLEABLE_GOO           = 72296, // ok, to test
    SPELL_GUZZLE_POTIONS          = 73122, // ok, to test
    SPELL_MUTATED_PLAGUE          = 72672, // ok, to test

// p2 p3
	NPC_CHOKING_GAS				  = 38159, // ok
	SPELL_SUMMON_CHOKING_GAS	  = 71273, // ok
    SPELL_CHOKING_GAS             = 71259, // ok
    SPELL_CHOKING_GAS_EXPLODE     = 71280, // ok

// p1 p2 exp 2
    NPC_GAS_CLOUD                 = 37562,
    SPELL_GASEOUS_BLOAT           = 70672, // verify stack decrease
    SPELL_EXPUNGED_GAS            = 70701, // ok
// p1 p2 exp 1
    NPC_VOLATILE_OOZE             = 37697,
    SPELL_OOZE_ADHESIVE           = 70447, // verify if really adhesive and can move
    SPELL_OOZE_ERUPTION           = 70492, // ok
//
    NPC_MUTATED_ABOMINATION       = 37672,
    SPELL_MUTATED_TRANSFORMATION  = 70311,
    SPELL_EAT_OOZE                = 72527,
    SPELL_REGURGITATED_OOZE       = 70539,
    SPELL_MUTATED_SLASH           = 70542,
    SPELL_MUTATED_AURA            = 70405,
};

enum Phases
{
	PHASE_INIT	= 1,
	PHASE_80	= 2,
	PHASE_35	= 3,
};

struct MANGOS_DLL_DECL boss_putricideAI : public LibDevFSAI
{
    boss_putricideAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnCaster(SPELL_MALLEABLE_GOO,20000,20000,0,PHASE_80);
		AddEventOnCaster(SPELL_MALLEABLE_GOO,21000,20000,0,PHASE_80);
		AddEventOnCaster(SPELL_MALLEABLE_GOO,20000,20000,0,PHASE_35);
		AddEventOnCaster(SPELL_MALLEABLE_GOO,21000,20000,0,PHASE_35);
    }

	Phases phase;
	bool experience;
	uint32 summonChoking_Timer;
	uint32 mutatedPlague_Timer;

    void Reset()
    {
		ResetTimers();
		experience = false;
		phase = PHASE_INIT;
		summonChoking_Timer = 30000;
		mutatedPlague_Timer = 20000;
    }

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_PUTRICIDE, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

	void DamageDeal(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= pWho->GetHealth() && pWho->HasAura(SPELL_MUTATED_PLAGUE))
		{
			me->CastStop();
			if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL ||
					m_difficulty == RAID_DIFFICULTY_25MAN_HEROIC)
			{
				if(Aura* aur = pWho->GetAura(SPELL_MUTATED_PLAGUE))
					for(uint8 i=0;i<aur->GetStackAmount();i++)
						DoCastMe(72748,true); // heal
			}
			else
			{
				if(Aura* aur = pWho->GetAura(SPELL_MUTATED_PLAGUE))
					for(uint8 i=0;i<aur->GetStackAmount();i++)
						DoCastMe(72747,true);
			}
		}
	}

	void SpellHit(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == SPELL_UNSTABLE_EXPERIMENT)
		{
			if(experience)
				DoCastMe(71412); // change target to green tuyau
			else
				DoCastMe(71414); // change target to orange tuyau
		}
	}

    void JustDied(Unit* pKiller)
    {
        SetInstanceData(TYPE_PUTRICIDE, DONE);

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
    }

    void JustReachedHome()
    {
        SetInstanceData(TYPE_PUTRICIDE, FAIL);
    }

	void StunAndGo()
	{
		DoCastVictim(SPELL_TEAR_GAS);
		// Todo : cast stun, timed text, special flags...
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(80) && phase == PHASE_INIT)
		{
			StunAndGo();
			phase = PHASE_80;
			me->CastStop();
			DoCastMe(SPELL_CREATE_CONCOCTION);
			return;
		}

		if(CheckPercentLife(35) && phase == PHASE_80)
		{
			StunAndGo();
			phase = PHASE_35;
			me->CastStop();
			DoCastMe(SPELL_GUZZLE_POTIONS,true);
			ModifyAuraStack(SPELL_MUTATED_PLAGUE,1,me->getVictim());
			return;
		}

		if(phase == PHASE_80 || phase == PHASE_35)
		{
			if(summonChoking_Timer <= diff)
			{
				CallCreature(NPC_CHOKING_GAS,23000,ON_ME,NOTHING);
				if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL ||
					m_difficulty == RAID_DIFFICULTY_25MAN_HEROIC)
					CallCreature(NPC_CHOKING_GAS,23000,ON_ME,NOTHING);
				summonChoking_Timer = 60000;
			}
			else
				summonChoking_Timer -= diff;
		}

		if(phase == PHASE_35)
		{
			if(mutatedPlague_Timer <= diff)
			{
				ModifyAuraStack(SPELL_MUTATED_PLAGUE,1,me->getVictim());
				mutatedPlague_Timer = 35000;
			}
			else
				mutatedPlague_Timer -= diff;
		}

		UpdateEvent(diff);
		UpdateEvent(diff,phase);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_putricide(Creature* pCreature)
{
    return new boss_putricideAI(pCreature);
}

struct MANGOS_DLL_DECL ooze_volatileAI : public LibDevFSAI
{
    ooze_volatileAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		target = 0;
		if(Unit* u = GetRandomUnit())
		{
			target = u->GetGUID();
			u->AddThreat(me,1000000.0f);
			DoCast(u,SPELL_OOZE_ADHESIVE);
		}
		CanBeTaunt(false);
    }

	uint32 checkDist_Timer;
	uint64 target;

    void Reset()
    {
		ResetTimers();
		checkDist_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
		if(checkDist_Timer <= diff)
		{
			checkDist_Timer = 1000;
			if(Unit* tar = GetGuidUnit(target))
			{
				if(tar->GetDistance2d(me) < 8.0f || !tar->isAlive())
				{
					DoCast(tar,SPELL_OOZE_ERUPTION);
					me->ForcedDespawn(500);
				}
			}
		}
		else
			checkDist_Timer -= diff;
    }
};

CreatureAI* GetAI_ooze_volatile(Creature* pCreature)
{
    return new ooze_volatileAI(pCreature);
}

struct MANGOS_DLL_DECL gas_cloudAI : public LibDevFSAI
{
    gas_cloudAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		target = 0;
		if(Unit* u = GetRandomUnit())
		{
			target = u->GetGUID();
			u->AddThreat(me,1000000.0f);
			DoCast(u,SPELL_GASEOUS_BLOAT); // verify stacks
		}
		CanBeTaunt(false);
    }

	uint32 checkDist_Timer;
	uint64 target;

    void Reset()
    {
		ResetTimers();
		checkDist_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
		if(checkDist_Timer <= diff)
		{
			checkDist_Timer = 1000;
			if(Unit* tar = GetGuidUnit(target))
			{
				if(tar->GetDistance2d(me) < 8.0f || !tar->isAlive())
				{
					int32 bp0 = 0;
					if(Aura* aur = tar->GetAura(SPELL_GASEOUS_BLOAT))
						bp0 = aur->GetStackAmount() * urand(1219,1281);

					me->CastCustomSpell(tar,SPELL_EXPUNGED_GAS,&bp0,NULL,NULL,true);
					me->ForcedDespawn(500);
				}
			}
		}
		else
			checkDist_Timer -= diff;
    }
};

CreatureAI* GetAI_gas_cloud(Creature* pCreature)
{
    return new gas_cloudAI(pCreature);
}

struct MANGOS_DLL_DECL choking_gasAI : public LibDevFSAI
{
    choking_gasAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
		SetCombatMovement(false);
		AddEventOnMe(SPELL_CHOKING_GAS_EXPLODE,10000,DAY*HOUR);
		DoCastMe(SPELL_CHOKING_GAS,true);
    }

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_choking_gas(Creature* pCreature)
{
    return new choking_gasAI(pCreature);
}

void AddSC_ICC_Putricide()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_putricide";
    NewScript->GetAI = &GetAI_boss_putricide;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "putricide_ooze_volatile";
    NewScript->GetAI = &GetAI_ooze_volatile;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "putricide_gas_cloud";
    NewScript->GetAI = &GetAI_gas_cloud;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "putricide_choking_gas";
    NewScript->GetAI = &GetAI_choking_gas;
    NewScript->RegisterSelf();
}
