
#include "precompiled.h"

enum Spells
{
	SPELL_FIRENOVA               =   23462,
	SPELL_CLEAVE                 =   20691,
	SPELL_CONFLIGURATION         =   16805,
	SPELL_THUNDERCLAP            =   15548               //Not sure if right ID. 23931 would be a harder possibility.
};

struct MANGOS_DLL_DECL boss_drakkisathAI : public LibDevFSAI
{
    boss_drakkisathAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitAI();
		AddEventOnTank(SPELL_FIRENOVA,6000,10000);
		AddEventOnTank(SPELL_CLEAVE,8000,8000);
		AddEventOnTank(SPELL_CONFLIGURATION,15000,18000);
		AddEventOnTank(SPELL_THUNDERCLAP,17000,20000);
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
CreatureAI* GetAI_boss_drakkisath(Creature* pCreature)
{
    return new boss_drakkisathAI(pCreature);
}

void AddSC_boss_drakkisath()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_drakkisath";
    newscript->GetAI = &GetAI_boss_drakkisath;
    newscript->RegisterSelf();
}
