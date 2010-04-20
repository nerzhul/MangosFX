
#include "precompiled.h"

enum
{
	SPELL_CHARGE		= 60067,
	SPELL_WARCRY		= 31403,
	SPELL_FEAR			= 19134,
	SPELL_TOURBILLON	= 38618,
	SPELL_TOURBILLON_D	= 38619,
};


struct MANGOS_DLL_DECL boss_allyhordeAI : public ScriptedAI
{
    boss_allyhordeAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		Tasks.AddEvent(SPELL_CHARGE,5000,14000,1000);
		Tasks.AddEvent(SPELL_TOURBILLON,15000,18000,2000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_FEAR,10000,23000,2000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_WARCRY,120000,120000,0,TARGET_ME);
    }

    void Aggro(Unit* pWho)
    {
        //DoScriptText(SAY_AGGRO, me);
		DoCastMe(SPELL_WARCRY);
    }

    void JustDied(Unit* pKiller)
    {
        //DoScriptText(SAY_DEATH, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        /*if (urand(0, 1))
            DoScriptText(SAY_KILL, me);*/
		Tasks.GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!Tasks.CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_allyhordeAI(Creature* pCreature)
{
    return new boss_allyhordeAI(pCreature);
}


void AddSC_boss_allyhorde()
{
    Script *newscript;

	newscript = new Script;
    newscript->Name = "boss_commander_kolurg";
    newscript->GetAI = &GetAI_boss_allyhordeAI;
    newscript->RegisterSelf();
}