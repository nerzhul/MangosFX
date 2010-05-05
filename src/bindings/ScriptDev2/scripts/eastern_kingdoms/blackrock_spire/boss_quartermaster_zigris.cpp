#include "precompiled.h"

enum Spells
{
	SPELL_SHOOT            = 16496,
	SPELL_STUNBOMB         = 16497,
	SPELL_HEALING_POTION   = 15504,
	SPELL_HOOKEDNET        = 15609
};

struct MANGOS_DLL_DECL boss_quatermasterzigrisAI : public LibDevFSAI
{
    boss_quatermasterzigrisAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_SHOOT,1000,500);
		AddEventOnTank(SPELL_STUNBOMB,16000,14000);
		AddEventMaxPrioOnMe(SPELL_HEALING_POTION,25000,30000);
		AddEventMaxPrioOnTank(SPELL_HOOKEDNET,12000,15000);
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
CreatureAI* GetAI_boss_quatermasterzigris(Creature* pCreature)
{
    return new boss_quatermasterzigrisAI(pCreature);
}

void AddSC_boss_quatermasterzigris()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "quartermaster_zigris";
    newscript->GetAI = &GetAI_boss_quatermasterzigris;
    newscript->RegisterSelf();
}
