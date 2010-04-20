/* Script Data Start
SDName: Boss drakos
SDAuthor: LordVanMartin
SD%Complete:
SDComment:
SDCategory:
Script Data End */

/*** SQL START ***
update creature_template set scriptname = '' where entry = '';
*** SQL END ***/
#include "precompiled.h"
#include "def_oculus.h"

enum Spells
{
    SPELL_MAGIC_PULL                           = 51336,
    SPELL_THUNDERING_STOMP                     = 50774,
    SPELL_THUNDERING_STOMP_2                   = 59370
};

//not in db
enum Yells
{
    SAY_AGGRO                               = -1578000,
    SAY_KILL_1                              = -1578001,
    SAY_KILL_2                              = -1578002,
    SAY_KILL_3                              = -1578003,
    SAY_DEATH                               = -1578004,
    SAY_PULL_1                              = -1578005,
    SAY_PULL_2                              = -1578006,
    SAY_PULL_3                              = -1578007,
    SAY_PULL_4                              = -1578008,
    SAY_STOMP_1                             = -1578009,
    SAY_STOMP_2                             = -1578010,
    SAY_STOMP_3                             = -1578011
};

struct MANGOS_DLL_DECL boss_drakosAI : public ScriptedAI
{
    boss_drakosAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->IsRegularDifficulty();
    }
    
    ScriptedInstance* pInstance;
	bool m_bIsHeroic;
    void Reset()
    {
        if (pInstance)
            pInstance->SetData(DATA_DRAKOS_EVENT, NOT_STARTED);
    }
    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);
        
        if (pInstance)
            pInstance->SetData(DATA_DRAKOS_EVENT, IN_PROGRESS);
    }
    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, me);
        
        if (pInstance)
            pInstance->SetData(DATA_DRAKOS_EVENT, DONE);
//		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
    void KilledUnit(Unit *victim)
    {
		switch(urand(0,2))
		{
			case 0:
				DoScriptText(SAY_KILL_1, me);
				break;
			case 1: 
				DoScriptText(SAY_KILL_2, me);
				break;
			case 2:
				DoScriptText(SAY_KILL_3, me);
				break;
		}
    }
};

CreatureAI* GetAI_boss_drakos(Creature* pCreature)
{
    return new boss_drakosAI (pCreature);
}

void AddSC_boss_drakos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_drakos";
    newscript->GetAI = &GetAI_boss_drakos;
    newscript->RegisterSelf();
}
