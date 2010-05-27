/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{
	// falric
	SPELL_HOPELESSNESS_1	=	72395,
	SPELL_HOPELESSNESS_2	=	72396,
	SPELL_HOPELESSNESS_3	=	72397,
	SPELL_HOPELESSNESS_1_H	=	72390,
	SPELL_HOPELESSNESS_2_H	=	72391,
	SPELL_HOPELESSNESS_3_H	=	72393,
	SPELL_DESPAIR			=	72426,
	SPELL_HORROR			=	72435,
	SPELL_QUIVERING_STRIKE	=	72422,
	
	// marwin
	SPELL_OBLITERATE		=	72360,
	SPELL_SUFFERING			=	72368,
	SPELL_WELL_OF_CORRUPT	=	72362,
	SPELL_FLESH				=	72363,
};

struct MANGOS_DLL_DECL boss_falricAI : public LibDevFSAI
{
    boss_falricAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(SPELL_HORROR,15000,30000);
        AddEventOnTank(SPELL_QUIVERING_STRIKE,6000,6000);
        AddEvent(SPELL_DESPAIR,10000,15000,3000);
    }

	uint8 phase;
    void Reset()
    {
		ResetTimers();
		phase = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		switch(phase)
		{
			case 0:
				if(CheckPercentLife(75))
				{
					DoCastVictim((m_difficulty) ? SPELL_HOPELESSNESS_1_H : SPELL_HOPELESSNESS_1);
					phase++;
				}
				break;
			case 1:
				if(CheckPercentLife(50))
				{
					DoCastVictim((m_difficulty) ? SPELL_HOPELESSNESS_2_H : SPELL_HOPELESSNESS_2);
					phase++;
				}
				break;
			case 2:
				if(CheckPercentLife(25))
				{
					DoCastVictim((m_difficulty) ? SPELL_HOPELESSNESS_3_H : SPELL_HOPELESSNESS_3);
					phase++;
				}
				break;
		}
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_falric(Creature* pCreature)
{
    return new boss_falricAI (pCreature);
}

struct MANGOS_DLL_DECL boss_marwinAI : public LibDevFSAI
{
    boss_marwinAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(SPELL_OBLITERATE,6000,6000,2000);
        AddEventOnTank(SPELL_FLESH,20000,30000);
        AddEvent(SPELL_SUFFERING,4000,4000,3000);
        AddEvent(SPELL_WELL_OF_CORRUPT,5000,10000);
    }

    void Reset()
    {
		ResetTimers();
    }


    void UpdateAI(const uint32 diff)
    {
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

CreatureAI* GetAI_boss_marwin(Creature* pCreature)
{
    return new boss_marwinAI (pCreature);
}

void AddSC_boss_falric_marwyn()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_falric";
    newscript->GetAI = &GetAI_boss_falric;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_marwin";
    newscript->GetAI = &GetAI_boss_marwin;
    newscript->RegisterSelf();

}
