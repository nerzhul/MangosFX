/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{
	SPELL_MIRRORED_SOUL			=	69051,
	SPELL_PHANTOM_BLAST			=	68982,
	SPELL_PHANTOM_BLAST_H		=	70322,
	SPELL_UNLEASHED_SOULS		=	68939,
	SPELL_WAILING_SOULS			=	68912,
	SPELL_WELL_OF_SOULS			=	68820
};

struct MANGOS_DLL_DECL boss_devourer_of_soulsAI : public ScriptedAI
{
    boss_devourer_of_soulsAI(Creature *pCreature) : ScriptedAI(pCreature)
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
        if (CanDoSomething())
            return;
	
      
		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
};

CreatureAI* GetAI_boss_devourer_of_souls(Creature* pCreature)
{
    return new boss_devourer_of_soulsAI (pCreature);
}

void AddSC_boss_devourer_of_souls()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_devourer_of_souls";
    newscript->GetAI = &GetAI_boss_devourer_of_souls;
    newscript->RegisterSelf();

}
