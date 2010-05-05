#include "precompiled.h"

enum Spells
{
	SPELL_SNAPKICK         = 15618,
	SPELL_CLEAVE           = 15579,
	SPELL_UPPERCUT         = 10966,
	SPELL_MORTALSTRIKE     = 16856,
	SPELL_PUMMEL           = 15615,
	SPELL_THROWAXE         = 16075
};

struct MANGOS_DLL_DECL boss_warmastervooneAI : public LibDevFSAI
{
    boss_warmastervooneAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_SNAPKICK,8000,6000);
		AddEventOnTank(SPELL_CLEAVE,14000,12000);
		AddEventOnTank(SPELL_UPPERCUT,20000,14000);
		AddEventOnTank(SPELL_MORTALSTRIKE,12000,10000);
		AddEventOnTank(SPELL_PUMMEL,32000,16000);
		AddEventOnTank(SPELL_THROWAXE,1000,8000);
    }

    void Reset()
    {
        ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_warmastervoone(Creature* pCreature)
{
    return new boss_warmastervooneAI(pCreature);
}

void AddSC_boss_warmastervoone()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_warmaster_voone";
    newscript->GetAI = &GetAI_boss_warmastervoone;
    newscript->RegisterSelf();
}
