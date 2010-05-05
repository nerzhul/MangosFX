#include "precompiled.h"

enum Spells
{
	SPELL_CRYSTALIZE               = 16104,
	SPELL_MOTHERSMILK              = 16468,
	SPELL_SUMMON_SPIRE_SPIDERLING  = 16103
};

struct MANGOS_DLL_DECL boss_mothersmolderwebAI : public LibDevFSAI
{
    boss_mothersmolderwebAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnMe(SPELL_CRYSTALIZE,20000,15000);
		AddEventOnMe(SPELL_MOTHERSMILK,10000,5000,7500);
	}

    void Reset()
    {
        ResetTimers();
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (me->GetHealth() <= damage)
            DoCastMe(SPELL_SUMMON_SPIRE_SPIDERLING,true);
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
CreatureAI* GetAI_boss_mothersmolderweb(Creature* pCreature)
{
    return new boss_mothersmolderwebAI(pCreature);
}

void AddSC_boss_mothersmolderweb()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_mother_smolderweb";
    newscript->GetAI = &GetAI_boss_mothersmolderweb;
    newscript->RegisterSelf();
}
