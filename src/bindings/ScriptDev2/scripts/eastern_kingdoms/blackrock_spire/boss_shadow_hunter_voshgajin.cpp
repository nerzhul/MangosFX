#include "precompiled.h"

enum Spells
{
	SPELL_CURSEOFBLOOD     = 24673,
	SPELL_HEX              = 16708,
	SPELL_CLEAVE           = 20691
};

struct MANGOS_DLL_DECL boss_shadowvoshAI : public LibDevFSAI
{
    boss_shadowvoshAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_CURSEOFBLOOD,2000,45000);
		AddEvent(SPELL_HEX,8000,15000);
		AddEventOnTank(SPELL_CLEAVE,7000,6000);
    }

    void Reset()
    {
		ResetTimers();

        //DoCastMe(SPELL_ICEARMOR,true);
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
CreatureAI* GetAI_boss_shadowvosh(Creature* pCreature)
{
    return new boss_shadowvoshAI(pCreature);
}

void AddSC_boss_shadowvosh()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_shadow_hunter_voshgajin";
    newscript->GetAI = &GetAI_boss_shadowvosh;
    newscript->RegisterSelf();
}
