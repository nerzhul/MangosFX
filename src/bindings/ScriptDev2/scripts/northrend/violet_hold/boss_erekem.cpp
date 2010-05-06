#include "precompiled.h"

enum
{
	SPELL_HEAL				=	54481,
	SPELL_HEAL_H			=	59473,
	SPELL_BLOODLUST			=	54516,
	SPELL_EARTHSHIELD		=	54479,
	SPELL_EARTHSHIELD_H		=	59471,
	SPELL_EARTH_SHOCK_H		=	54511,
	SPELL_LIGHTNING			=	53044,
	SPELL_STORMSTRIKE		=	51876,
	SPELL_BREAKBONDS_H		=	59463,
};
                                 
   
/*######
## boss_erekem
######*/

struct MANGOS_DLL_DECL boss_erekemAI : public ScriptedAI
{	
	float life;
	bool bloodlustcasted;

	MobEventTasks Tasks;

    void Reset()
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_LIGHTNING,10000,14000,2000);
		Tasks.AddEvent(SPELL_STORMSTRIKE,3000,3000,3000,TARGET_MAIN);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_HEAL_H,25000,15000,4000,TARGET_ME);
			Tasks.AddEvent(SPELL_EARTHSHIELD_H,1000,17000,5000,TARGET_ME);
			Tasks.AddEvent(SPELL_EARTH_SHOCK_H,7000,8000,4000);
			Tasks.AddEvent(SPELL_BREAKBONDS_H,15000,20000,5000,TARGET_ME);
		}
		else
		{
			Tasks.AddEvent(SPELL_HEAL,25000,15000,4000,TARGET_ME);
			Tasks.AddEvent(SPELL_EARTHSHIELD,1000,17000,5000,TARGET_ME);
		}

		bloodlustcasted = false;
	}
	 
	boss_erekemAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	    
	void Aggro(Unit* pWho)
	{
		if (!CanDoSomething())
            return;
		DoScriptText(-2000033,me);
	}

	void JustDied(Unit* pWho)
	{
		DoScriptText(-2000037,me);
	}

	void KilledUnit(Unit* pVictim)
    {
        uint8 randd = urand(0,2);
		switch(randd)
		{
			case 0:
				DoScriptText(-2000034,me);
				break;
			case 1:
				DoScriptText(-2000035,me);
				break;
			case 2:
				DoScriptText(-2000036,me);
				break;
		}
    }

    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething() || me->HasAura(66830,0))
            return;

		if(CheckPercentLife(45) && !bloodlustcasted && !me->HasAura(SPELL_BLOODLUST,0))
		{
			me->CastStop();
			DoCastMe(SPELL_BLOODLUST);
			bloodlustcasted = true;
		}

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_erekem(Creature* pCreature)
{
    return new boss_erekemAI(pCreature);
} 

void AddSC_boss_erekem()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_erekem";
    newscript->GetAI = &GetAI_boss_erekem;
    newscript->RegisterSelf();
}

