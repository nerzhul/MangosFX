/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "azjol-nerub.h"

enum
{
	SPELL_ACID_N		=	53400,
	SPELL_ACID_H		=	59419,
	SPELL_REDUCEARMOR	=	53418,
	SPELL_POISON_N		=	53030,
	SPELL_POISON_H		=	59417,
	SPELL_PORTEAVANT	=	53185,
	SPELL_PORTELATERALE	=	53177,
	SPELL_SAISIE1_N		=	53406,
	SPELL_SAISIE2_N		=	57731,
	SPELL_SAISIE1_H		=	59420,
	SPELL_SAISIE2_H		=	59421,
};

/*######
## boss_hadronox
######*/

struct MANGOS_DLL_DECL boss_hadronoxAI : public ScriptedAI
{
    boss_hadronoxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

	MobEventTasks Tasks;
    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_REDUCEARMOR,8000,12000,4000,TARGET_MAIN);

		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_ACID_H,6000,11000,2000);
			Tasks.AddEvent(SPELL_POISON_H,12000,13000,7000);
			Tasks.AddEvent(SPELL_SAISIE1_H,20000,40000,20000);
			//Tasks.AddEvent(SPELL_SAISIE1_H,60000,45000,20000);
		}
		else
		{
			Tasks.AddEvent(SPELL_ACID_N,6000,11000,2000);
			Tasks.AddEvent(SPELL_POISON_N,12000,13000,7000);
			Tasks.AddEvent(SPELL_SAISIE1_N,20000,40000,20000);
			//Tasks.AddEvent(SPELL_SAISIE1_H,60000,45000,20000);
		}
    }

	void JustDied(Unit* pWho)
	{
		Tasks.GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
	}

    void KilledUnit(Unit* pVictim)
    {
        me->SetHealth(me->GetHealth() + (me->GetMaxHealth() * 0.1));
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_hadronox(Creature* pCreature)
{
    return new boss_hadronoxAI(pCreature);
}

void AddSC_boss_hadronox()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_hadronox";
    newscript->GetAI = &GetAI_boss_hadronox;
    newscript->RegisterSelf();
}
