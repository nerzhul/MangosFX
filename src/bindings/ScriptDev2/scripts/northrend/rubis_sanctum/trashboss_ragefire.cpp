#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{

};

struct MANGOS_DLL_DECL trashboss_ragefireAI : public LibDevFSAI
{	
	trashboss_ragefireAI(Creature* pCreature) : LibDevFSAI(pCreature)
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

CreatureAI* GetAI_trashboss_ragefire(Creature* pCreature)
{
    return new trashboss_ragefireAI(pCreature);
} 

void AddSC_trashboss_ragefire()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "trashboss_ragefire";
    newscript->GetAI = &GetAI_trashboss_ragefire;
    newscript->RegisterSelf();
}

