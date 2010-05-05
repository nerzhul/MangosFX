#include "precompiled.h"

enum Spells
{
	SPELL_FIRENOVA         = 23462,
	SPELL_FLAMEBUFFET      = 23341,
	SPELL_PYROBLAST        = 17274
};

struct MANGOS_DLL_DECL boss_pyroguard_emberseerAI : public LibDevFSAI
{
    boss_pyroguard_emberseerAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_FIRENOVA,6000,6000);
		AddEventOnTank(SPELL_FLAMEBUFFET,3000,14000);
		AddEvent(SPELL_PYROBLAST,14000,15000);
		Reset();
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
CreatureAI* GetAI_boss_pyroguard_emberseer(Creature* pCreature)
{
    return new boss_pyroguard_emberseerAI(pCreature);
}

void AddSC_boss_pyroguard_emberseer()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_pyroguard_emberseer";
    newscript->GetAI = &GetAI_boss_pyroguard_emberseer;
    newscript->RegisterSelf();
}
