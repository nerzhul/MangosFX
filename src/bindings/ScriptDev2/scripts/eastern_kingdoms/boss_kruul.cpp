#include "precompiled.h"

enum Spells
{
	SPELL_SHADOWVOLLEY          21341,
	SPELL_CLEAVE                20677,
	SPELL_THUNDERCLAP           23931,
	SPELL_TWISTEDREFLECTION     21063,
	SPELL_VOIDBOLT              21066,
	SPELL_RAGE                  21340,
	SPELL_CAPTURESOUL           21054
};

struct MANGOS_DLL_DECL boss_kruulAI : public LibDevFSAI
{
    boss_kruulAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitBoss();
		AddEventOnTank(SPELL_SHADOWVOLLEY,10000,5000);
		AddEventOnTank(SPELL_CLEAVE,14000,10000);
		AddEventOnTank(SPELL_THUNDERCLAP,20000,12000);
		AddEventOnTank(SPELL_TWISTEDREFLECTION,25000,30000);
		AddEventOnTank(SPELL_VOIDBOLT,30000,18000);
		AddEventOnTank(SPELL_RAGE,60000,70000);
		AddSummonEvent(19207,8000,45000,0,0,3,300000,NEAR_7M);
	}

    void Reset()
    {
		ResetTimers();
    }

    void KilledUnit()
    {
        DoCastMe(SPELL_CAPTURESOUL);
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
CreatureAI* GetAI_boss_kruul(Creature* pCreature)
{
    return new boss_kruulAI(pCreature);
}

void AddSC_boss_kruul()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kruul";
    newscript->GetAI = &GetAI_boss_kruul;
    newscript->RegisterSelf();
}
