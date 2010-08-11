#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{

};

struct MANGOS_DLL_DECL trashboss_baltarusAI : public LibDevFSAI
{	
	trashboss_baltarusAI(Creature* pCreature) : LibDevFSAI(pCreature)
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

CreatureAI* GetAI_trashboss_baltarus(Creature* pCreature)
{
    return new trashboss_baltarusAI(pCreature);
} 

void AddSC_trashboss_baltarus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "trashboss_baltarus";
    newscript->GetAI = &GetAI_trashboss_baltarus;
    newscript->RegisterSelf();
}

