#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{
	SPELL_BLADE_TEMPEST		= 75125,
	SPELL_REPELLING_WAVE	= 74509,
	SPELL_ENERVATING_BRAND	= 74502,
	SPELL_ENERVATING_B_TRIG = 74505,
	SPELL_SIPHONED_MIGHT	= 74507,
	SPELL_SUMMON_CLONE		= 74511,
};

struct MANGOS_DLL_DECL trashboss_baltarusAI : public LibDevFSAI
{	
	trashboss_baltarusAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
		AddEventOnTank(SPELL_BLADE_TEMPEST, urand(3000,5000),25000,5000);
		AddEventOnTank(SPELL_REPELLING_WAVE,urand(7000,9000),12000,1000);
		AddEvent(SPELL_ENERVATING_BRAND,urand(30000,50000),45000);
    }

	bool cloned;
	uint64 clone;

    void Reset()
	{
		ResetTimers();
		me->RemoveAurasDueToSpell(SPELL_SIPHONED_MIGHT);
		cloned = false;
		clone = 0;
		SetInstanceData(TYPE_BALTHARUS,NOT_STARTED);
	}

	void Aggro(Unit* pWho)
	{
		SetInstanceData(TYPE_BALTHARUS,IN_PROGRESS);
		Say(17520,"Ah ! Mon petit divertissement est arrivé !");
	}

	void KilledUnit(Unit* pWho)
	{
		if(urand(0,1))
			Say(17521,"Baltharus ne fait jamais de prisonniers !");
		else
			Say(17522,"Le monde a bien assez de héros");
	}

	void JustSummoned(Creature* add)
	{
		clone = add->GetGUID();
	}

	void SpellHitTarget(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == SPELL_ENERVATING_B_TRIG)
			ModifyAuraStack(SPELL_SIPHONED_MIGHT,1);
	}

	void JustDied(Unit* pWho)
	{
		Yell(17523,"Celle la je ne l'ai pas vu venir...");
		SetInstanceData(TYPE_BALTHARUS,DONE);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				break;
		}
	}

	void JustReachedHome()
	{
		SetInstanceData(TYPE_BALTHARUS,FAIL);
		if(Creature* add = GetGuidCreature(clone))
			add->ForcedDespawn(1000);
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(50) && !cloned)
		{
			Yell(17524,"Deux fois plus mal et deux fois moins drôle.");
			cloned = true;
			me->CastStop();
			DoCastMe(SPELL_SUMMON_CLONE);
		}

		UpdateEvent(diff);
	
		DoMeleeAttackIfReady();

	}
};

CreatureAI* GetAI_trashboss_baltarus(Creature* pCreature)
{
    return new trashboss_baltarusAI(pCreature);
}

struct MANGOS_DLL_DECL trashboss_baltarus_cloneAI : public LibDevFSAI
{	
	trashboss_baltarus_cloneAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
		AddEventOnTank(SPELL_BLADE_TEMPEST, urand(3000,5000),15000,5000);
		AddEventOnTank(SPELL_REPELLING_WAVE,urand(7000,9000),12000,1000);
		AddEvent(SPELL_ENERVATING_BRAND,urand(30000,50000),45000);
    }

    void Reset()
	{
		ResetTimers();
		me->RemoveAurasDueToSpell(SPELL_SIPHONED_MIGHT);
		me->SetHealth(me->GetMaxHealth() / 2);
	}

	void SpellHitTarget(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == SPELL_ENERVATING_B_TRIG)
			ModifyAuraStack(SPELL_SIPHONED_MIGHT,1);
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();

	}
};

CreatureAI* GetAI_trashboss_baltarus_clone(Creature* pCreature)
{
    return new trashboss_baltarus_cloneAI(pCreature);
}

void AddSC_trashboss_baltarus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "trashboss_baltarus";
    newscript->GetAI = &GetAI_trashboss_baltarus;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "trashboss_baltarus_clone";
    newscript->GetAI = &GetAI_trashboss_baltarus_clone;
    newscript->RegisterSelf();
}

