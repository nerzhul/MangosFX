/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{

};

struct MANGOS_DLL_DECL boss_garfrostAI : public LibDevFSAI
{
    boss_garfrostAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
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

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_garfrost(Creature* pCreature)
{
    return new boss_garfrostAI (pCreature);
}

void AddSC_boss_garfrost()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_garfrost";
    newscript->GetAI = &GetAI_boss_garfrost;
    newscript->RegisterSelf();

}
