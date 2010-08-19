#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{
	SPELL_FLAME_BREATH		= 74403,
	SPELL_ENRAGE			= 78722,
	SPELL_CONFLAGRATION		= 74452,
	SPELL_CONFLAG_TRIG		= 74456,
};

struct MANGOS_DLL_DECL trashboss_ragefireAI : public LibDevFSAI
{	
	trashboss_ragefireAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
		AddEventOnTank(SPELL_FLAME_BREATH,5000,12000,5000);
		AddEventOnMe(SPELL_ENRAGE,18000,30000);
		AddEvent(SPELL_CONFLAGRATION,25000,35000);
    }

    void Reset()
	{
		ResetTimers();
		SetInstanceData(TYPE_RAGEFIRE,NOT_STARTED);
	}

	void SpellHit(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == 74455)
		{
			Say(17532,"");
			me->CastStop();
			DoCastRandom(SPELL_CONFLAG_TRIG,true);
			DoCastRandom(SPELL_CONFLAG_TRIG,true);
		}
	}
	
	void Aggro(Unit* pWho)
	{
		SetInstanceData(TYPE_RAGEFIRE,IN_PROGRESS);
		Yell(17528,"Vous allez souffrir pour cette intrusion.");
	}

	void JustDied(Unit* pWho)
	{
		Say(17531,"Aarrrrrgh !");
		SetInstanceData(TYPE_RAGEFIRE,DONE);
	}

	void KilledUnit(Unit* pWho)
	{
		if(urand(0,1))
			Say(17529,"C'est l'ordre des choses.");
		else
			Say(17530,"Halion sera satisfait !");
	}

	void JustReachedHome()
	{
		SetInstanceData(TYPE_RAGEFIRE,FAIL);
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
	
		DoMeleeAttackIfReady();

	}
};

CreatureAI* GetAI_trashboss_ragefire(Creature* pCreature)
{
    return new trashboss_ragefireAI(pCreature);
} 

void AddSC_trashboss_ragefire()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "trashboss_ragefire";
    newscript->GetAI = &GetAI_trashboss_ragefire;
    newscript->RegisterSelf();
}

