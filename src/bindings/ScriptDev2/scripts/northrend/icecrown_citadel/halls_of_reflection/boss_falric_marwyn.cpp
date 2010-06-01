/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "halls_of_reflection.h"

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

	void Aggro(Unit* who)
	{
		Say(16710,"Hommes, femmes, enfants. Nul n'a echape au courroux du maitre. Vos morts ne seront pas differentes.");
	}

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Yell(16711,"Vermisseau pleurnichard !");
		else
			Yell(16712,"Les enfants de Stratholme se battaient mieux que ca.");
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
		{
			DoCastMe(66830);
			DoResetThreat();
            return;
		}
		
		switch(phase)
		{
			if(urand(0,1))
				Say(16715,"Le desespoir... si delicieux...");
			else
				Say(16716,"La peur... si exaltante...");

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
	   pInstance->SetData(TYPE_FALRIC,DONE);
	   Say(16713,"Marwyn... Charge toi d'eux...");
    }
};

CreatureAI* GetAI_boss_falric(Creature* pCreature)
{
    return new boss_falricAI (pCreature);
}

struct MANGOS_DLL_DECL boss_marwynAI : public LibDevFSAI
{
    boss_marwynAI(Creature *pCreature) : LibDevFSAI(pCreature)
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

	void Aggro(Unit* pwho)
	{
		Yell(16734,"Et la mort est tout ce que vous trouverez ici !");
	}

	void KilledUnit(Unit* pwho)
	{
		if(urand(0,1))
			Yell(16735,"J'ai vu le meme regard passer dans ses yeux a sa mort... Terenas... Il n'arrivait pas a y croire. Hahahaha !");
		else
			Yell(16736,"Que la souffrance vous etouffe !");

	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
		{
			DoCastMe(66830);
			DoResetThreat();
            return;
		}
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
	   pInstance->SetData(TYPE_MARWYN,DONE);
	   Say(16737,"Oui... courrez... courrez vers votre destin. Son etreinte froide et amere... vous attend.");
    }
};

CreatureAI* GetAI_boss_marwyn(Creature* pCreature)
{
    return new boss_marwynAI (pCreature);
}

void AddSC_boss_falric_marwyn()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_falric";
    newscript->GetAI = &GetAI_boss_falric;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_marwyn";
    newscript->GetAI = &GetAI_boss_marwyn;
    newscript->RegisterSelf();

}
