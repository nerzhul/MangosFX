#include "precompiled.h"

enum Spells
{
	SPELL_FLAMEBREAK           = 16785,
	SPELL_IMMOLATE             = 20294,
	SPELL_TERRIFYINGROAR       = 14100
};

struct MANGOS_DLL_DECL boss_thebeastAI : public LibDevFSAI
{
    boss_thebeastAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_FLAMEBREAK,12000,10000);
		AddEvent(SPELL_IMMOLATE,3000,8000);
		AddEventOnTank(SPELL_TERRIFYINGROAR,23000,20000);
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
CreatureAI* GetAI_boss_thebeast(Creature* pCreature)
{
    return new boss_thebeastAI(pCreature);
}

void AddSC_boss_thebeast()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_the_beast";
    newscript->GetAI = &GetAI_boss_thebeast;
    newscript->RegisterSelf();
}
