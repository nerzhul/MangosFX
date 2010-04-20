/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{

};

struct MANGOS_DLL_DECL boss_falricAI : public ScriptedAI
{
    boss_falricAI(Creature *pCreature) : ScriptedAI(pCreature)
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


    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!me->SelectHostileTarget() && !me->getVictim())
            return;
	
      
		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
};

CreatureAI* GetAI_boss_falric(Creature* pCreature)
{
    return new boss_falricAI (pCreature);
}

void AddSC_boss_falric_marwyn()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_falric";
    newscript->GetAI = &GetAI_boss_falric;
    newscript->RegisterSelf();

}
