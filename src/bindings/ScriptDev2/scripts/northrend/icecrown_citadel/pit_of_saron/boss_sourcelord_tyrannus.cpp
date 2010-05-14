/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{

};

struct MANGOS_DLL_DECL boss_tyrannusAI : public LibDevFSAI
{
    boss_tyrannusAI(Creature *pCreature) : LibDevFSAI(pCreature)
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

CreatureAI* GetAI_boss_tyrannus(Creature* pCreature)
{
    return new boss_tyrannusAI (pCreature);
}

void AddSC_boss_Tyrannus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_tyrannus";
    newscript->GetAI = &GetAI_boss_tyrannus;
    newscript->RegisterSelf();

}
