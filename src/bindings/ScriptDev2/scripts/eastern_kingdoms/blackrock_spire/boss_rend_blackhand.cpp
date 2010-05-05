#include "precompiled.h"

enum Spells
{
	SPELL_WHIRLWIND                = 26038,
	SPELL_CLEAVE                   = 20691,
	SPELL_THUNDERCLAP              = 23931               //Not sure if he cast this spell
};

struct MANGOS_DLL_DECL boss_rend_blackhandAI : public LibDevFSAI
{
    boss_rend_blackhandAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_WHIRLWIND,20000,18000);
		AddEventOnTank(SPELL_CLEAVE,5000,10000);
		AddEventOnTank(SPELL_THUNDERCLAP,9000,16000);
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
CreatureAI* GetAI_boss_rend_blackhand(Creature* pCreature)
{
    return new boss_rend_blackhandAI(pCreature);
}

void AddSC_boss_rend_blackhand()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_rend_blackhand";
    newscript->GetAI = &GetAI_boss_rend_blackhand;
    newscript->RegisterSelf();
}
