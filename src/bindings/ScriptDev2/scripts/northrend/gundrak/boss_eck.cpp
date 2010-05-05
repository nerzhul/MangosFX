/* ScriptData
SDName: Boss_eck
SD%Complete: 90%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"

enum
{
	spell_saut					= 55815,
	spell_morsure				= 55813,
	spell_crachat				= 55814,
	spell_enrage				= 55816,
    
};

/*######
## boss_galdarah
######*/

struct MANGOS_DLL_DECL boss_eck : public ScriptedAI
{
	uint32 spell_saut_timer;
	uint32 spell_morsure_timer;
	uint32 spell_crachat_timer;
	uint32 spell_enrage_timer;
	uint32 i;

    boss_eck(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {		
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(spell_morsure,7000,10000,0,TARGET_MAIN);
		Tasks.AddEvent(spell_enrage,150000,60000,0,TARGET_ME);
		Tasks.AddEvent(spell_saut_timer,15000,5000);
		Tasks.AddEvent(spell_crachat_timer,21000,5000,0,TARGET_MAIN);
		spell_saut_timer = 10000;
		spell_morsure_timer = 7000;
		spell_crachat_timer = 25000;
		spell_enrage_timer = 150000;
		i = 0;
    }

	void JustDied(Unit* pWho)
	{
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
	}

    void UpdateAI(const uint32 diff)
    { 
		if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eck(Creature* pCreature)
{
    return new boss_eck(pCreature);
}

void AddSC_boss_eck()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_eck";
    newscript->GetAI = &GetAI_boss_eck;
    newscript->RegisterSelf();
}