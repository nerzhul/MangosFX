#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{

};

struct MANGOS_DLL_DECL boss_halionAI : public LibDevFSAI
{	
	boss_halionAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
    }

    void Reset()
	{
		ResetTimers();
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
	
		DoMeleeAttackIfReady();

	}
};

CreatureAI* GetAI_boss_halion(Creature* pCreature)
{
    return new boss_halionAI(pCreature);
} 

void AddSC_boss_halion()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_halion";
    newscript->GetAI = &GetAI_boss_halion;
    newscript->RegisterSelf();
}

