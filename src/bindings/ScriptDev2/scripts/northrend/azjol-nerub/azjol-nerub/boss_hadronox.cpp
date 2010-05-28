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

struct MANGOS_DLL_DECL boss_hadronoxAI : public LibDevFSAI
{
    boss_hadronoxAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(SPELL_REDUCEARMOR,8000,12000,4000);
		if(m_difficulty)
		{
			AddEvent(SPELL_ACID_H,6000,11000,2000);
			AddEvent(SPELL_POISON_H,12000,13000,7000);
			AddEvent(SPELL_SAISIE1_H,20000,40000,20000);
			//AddEvent(SPELL_SAISIE1_H,60000,45000,20000);
		}
		else
		{
			AddEvent(SPELL_ACID_N,6000,11000,2000);
			AddEvent(SPELL_POISON_N,12000,13000,7000);
			AddEvent(SPELL_SAISIE1_N,20000,40000,20000);
			//Tasks.AddEvent(SPELL_SAISIE1_H,60000,45000,20000);
		}
    }

    void Reset()
    {
		ResetTimers();
    }

	void JustDied(Unit* pWho)
	{
		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
	}

    void KilledUnit(Unit* pVictim)
    {
        AddPercentLife(me,10);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		UpdateEvent(diff);

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
