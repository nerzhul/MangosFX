/*
* LibDevFS Crazy IA's
* For Black Diamond Events
*
*/

#include "precompiled.h"

struct MANGOS_DLL_DECL crazyIA_nerzhulAI : public LibDevFSAI
{	
	crazyIA_nerzhulAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitIA();
    }

	uint32 vote_Timer;
	uint32 joug_Timer;
    void Reset()
	{
		vote_Timer = 300000;
		joug_Timer = 900000;
	}
	    
    void UpdateAI(const uint32 diff)
	{	
		if(vote_Timer <= diff)
		{
			Yell(0,"N'oubliez pas de voter pour le serveur toutes les 2h !");
			vote_Timer = 5400000;
		}
		else
			vote_Timer -= diff;
			
		if(joug_Timer <= diff)
		{
			Yell(0,"Rappel : il est interdit de cliquer sur l'orbe du joug si tous les joueurs de la faction d'adverse n'ont pas été éliminés");
			joug_Timer = 5400000;
		}
		else
			joug_Timer -= diff;
			
        if (!CanDoSomething())
            return;
	}
};

CreatureAI* GetAI_crazyIA_nerzhulAI(Creature* pCreature)
{
    return new crazyIA_nerzhulAI(pCreature);
} 

void AddSC_crazy_ias()
{
	Script *newscript;
	newscript = new Script;
    newscript->Name = "crazyIA_nerzhul";
    newscript->GetAI = &GetAI_crazyIA_nerzhulAI;
    newscript->RegisterSelf();
}
