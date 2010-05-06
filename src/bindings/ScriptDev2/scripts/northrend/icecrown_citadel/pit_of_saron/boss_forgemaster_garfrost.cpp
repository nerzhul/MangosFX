/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{

};

struct MANGOS_DLL_DECL boss_garfrostAI : public ScriptedAI
{
    boss_garfrostAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
    }


    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;
	
      
		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
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
