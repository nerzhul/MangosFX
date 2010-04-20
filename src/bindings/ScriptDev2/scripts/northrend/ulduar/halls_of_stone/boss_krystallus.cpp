#include "precompiled.h"

enum
{
	SPELL_BOULDER_N				=	50843,
	SPELL_BOULDER_H				=	59742,
	SPELL_GROUND_SPIKE_H		=	59750,
	SPELL_SHATTER_N				=	50810,
	SPELL_SHATTER_H				=	61546,
	SPELL_STOMP_N				=	50868,
	SPELL_STOMP_H				=	59744,
};

struct MANGOS_DLL_DECL boss_krystallusAI : public ScriptedAI
{
	uint32 Boulder_Timer;
	uint32 GroundSpike_Timer;
	uint32 Shatter_Timer;
	uint32 Stomp_Timer;
	uint32 break_Count;

	float percent;

	Unit* target;
    boss_krystallusAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	
	void Reset()
	{
		Boulder_Timer = 9000;
		GroundSpike_Timer = 21000;
		Shatter_Timer = 12000;
		Stomp_Timer = 15000;
		break_Count = 9;
		me->SetFloatValue(OBJECT_FIELD_SCALE_X,1.0);
	}

	void JustDied(Unit* pKiller)
	{
		me->SetFloatValue(OBJECT_FIELD_SCALE_X,1.0);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		percent = ((float)me->GetHealth() * 100/(float)me->GetMaxHealth());

		if(Boulder_Timer <= uiDiff)
		{
			DoCastRandom(m_bIsHeroic ? SPELL_SHATTER_H : SPELL_SHATTER_N);
			Boulder_Timer = urand(8000,11000);
		}
		else
			Boulder_Timer -= uiDiff;

		if(Shatter_Timer <= uiDiff)
		{
			DoCastVictim(m_bIsHeroic ? SPELL_SHATTER_H : SPELL_SHATTER_N);
			Shatter_Timer = urand(18000,21000);
		}
		else
			Shatter_Timer -= uiDiff;

		if(Stomp_Timer <= uiDiff)
		{
			DoCastVictim(m_bIsHeroic ? SPELL_STOMP_H : SPELL_STOMP_N);
			Stomp_Timer = urand(8000,14000);
		}
		else
			Stomp_Timer -= uiDiff;

		if(m_bIsHeroic && GroundSpike_Timer <= uiDiff)
		{
			DoCastVictim(SPELL_GROUND_SPIKE_H);
			GroundSpike_Timer = urand(15000,18000);
		}
		else
			GroundSpike_Timer -= uiDiff;

		if(percent < (float)(break_Count * 10))
		{
			me->SetFloatValue(OBJECT_FIELD_SCALE_X,break_Count * 1.09 /10.1);
			me->SummonCreature(27973,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetAngle(me),TEMPSUMMON_TIMED_DESPAWN,180000);
			break_Count--;
		}

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_krystallus(Creature* pCreature)
{
    return new boss_krystallusAI(pCreature);
}

void AddSC_boss_krystallus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_krystallus";
    newscript->GetAI = &GetAI_boss_krystallus;
    newscript->RegisterSelf();
}
