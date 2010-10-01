#include "precompiled.h"

enum Spells
{
	SPELL_PRISMATIC_SHIELD		=	40879, // ok
	SPELL_ANTICAC				=	70109, // ok
	SPELL_SHADOW_MOBS_PROC		=	71861, // ok
	SPELL_FEAR					=	73070, // ok
	SPELL_MYSTIC_BUFFER			=	70128, // ok
	SPELL_HIT_TANK				=	64395, // ok
	SPELL_HIT_TANK_FROST		=	71993, // ok
	SPELL_FIRE_BREATH			=	74525, // ok
	SPELL_CLEAVE				=	74524, // ok
	SPELL_SHADOW_BREATH			=	74806, // ok
	SPELL_SHADOW_ZONE			=	74792, // ok
};

enum Phases
{
	PHASE_NONE		=	0,
	PHASE_INIT		=	1,
	PHASE_CLASSIC	=	2,
	PHASE_RAGE		=	3,
	PHASE_CRAZY		=	4,
};

struct MANGOS_DLL_DECL boss_event_annivAI : public LibDevFSAI
{
    boss_event_annivAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(TEN_MINS);
		
		// feu d'artifice
		AddEvent(11540,11000,10000,1000);

		AddEventOnTank(SPELL_HIT_TANK,3000,3000,3000);
		AddEventOnTank(SPELL_FIRE_BREATH,5000,24000,1000,PHASE_CLASSIC);
		AddEventOnTank(SPELL_FIRE_BREATH,5000,24000,1000,PHASE_INIT);
		AddEventOnTank(SPELL_SHADOW_BREATH,5000,24000,1000,PHASE_CLASSIC);
		AddEventOnTank(SPELL_SHADOW_BREATH,5000,24000,1000,PHASE_INIT);
		AddEvent(SPELL_SHADOW_ZONE,18000,30000,5000,TARGET_RANDOM,PHASE_INIT);
		AddEvent(SPELL_SHADOW_ZONE,18000,30000,5000,TARGET_RANDOM,PHASE_CLASSIC);
		AddEvent(SPELL_FEAR,15000,20000);
		AddEventOnTank(SPELL_CLEAVE,4000,3000,3000,PHASE_RAGE);
		AddEventOnTank(SPELL_CLEAVE,4000,3000,3000,PHASE_CRAZY);
		AddEvent(SPELL_SHADOW_MOBS_PROC,15000,30000,0,TARGET_RANDOM,PHASE_RAGE);
		AddEvent(SPELL_SHADOW_MOBS_PROC,15000,30000,0,TARGET_RANDOM,PHASE_CRAZY);
    }

	uint8 phase;
	uint32 phase_Timer;

    void Reset()
    {
		ResetTimers();
		phase = PHASE_NONE;
		phase_Timer = 60000;
    }

	void SpellHit(Unit* pWho, const SpellEntry* spell)
	{
		if(pWho->GetTypeId() != TYPEID_PLAYER)
			return;

		if(urand(1,100) > 90)
			DoCast(pWho,spell->Id,true);
	}

	void SwitchPhase()
	{
		switch(phase)
		{
			case PHASE_CLASSIC:
				me->RemoveAurasDueToSpell(SPELL_ANTICAC);
				me->RemoveAurasDueToSpell(SPELL_HIT_TANK_FROST);
				me->SetDisplayId(28641);
				break;
			case PHASE_RAGE:
				ModifyAuraStack(SPELL_HIT_TANK_FROST);
				ModifyAuraStack(SPELL_ANTICAC);
				me->SetDisplayId(31952);
				break;
			case PHASE_CRAZY:
				ModifyAuraStack(SPELL_MYSTIC_BUFFER);
				ModifyAuraStack(SPELL_HIT_TANK_FROST);
				ModifyAuraStack(SPELL_ANTICAC);
				me->SetDisplayId(20939);
				break;
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(90) && phase == PHASE_INIT)
		{
			phase = PHASE_RAGE;
			ModifyAuraStack(SPELL_PRISMATIC_SHIELD);
		}

		if(CheckPercentLife(90) && (phase == PHASE_RAGE || phase == PHASE_CLASSIC))
		{
			if(phase_Timer <= diff)
			{
				phase = (phase == PHASE_RAGE) ? PHASE_CLASSIC : PHASE_RAGE;
				SwitchPhase();
				phase_Timer = (phase == PHASE_RAGE) ? 45000 : 60000;
			}
			else
				phase_Timer -= diff;
		}

		if(CheckPercentLife(15))
		{
			phase = PHASE_CRAZY;
			SwitchPhase();
		}

		UpdateEvent(diff);
		UpdateEvent(diff,phase);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_event_anniv(Creature* pCreature)
{
    return new boss_event_annivAI(pCreature);
}

void AddSC_ia_custom()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_event_anniv";
    newscript->GetAI = &GetAI_boss_event_anniv;
    newscript->RegisterSelf();
}