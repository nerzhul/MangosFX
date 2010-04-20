#include "precompiled.h"

enum
{
	SPELL_SHROUD			=	54524,
	SPELL_SHROUD_H			=	59745,
	SPELL_SUMMON			=	54369,
	SPELL_SHIFT				=	54361,
	SPELL_SHIFT_H			=	59743,
};
   
/*######
## boss_zuramat
######*/

struct MANGOS_DLL_DECL add_zuramatAI: public ScriptedAI
{
	uint32 move_Timer;
    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	float x,y,z;

	add_zuramatAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
	}

	void Reset()
	{
		move_Timer = 100;
	}

	void Aggro(Unit* pWho)
	{
		DoCastMe(m_bIsHeroic ? 59747 : 54342);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if(move_Timer <= uiDiff)
		{
			urand(0,1) ? x = me->GetPositionX() - urand(2,4) : x = me->GetPositionX() + urand(2,4);

			urand(0,1) ? y = me->GetPositionY() - urand(2,4) : y = me->GetPositionY() + urand(2,4);

			me->GetMotionMaster()->MovePoint(0,x,y,me->GetPositionZ());
			move_Timer = 1000;
		}
		else
			move_Timer -= uiDiff;
	}
};

struct MANGOS_DLL_DECL boss_zuramatAI : public ScriptedAI
{	
	uint32 summon_Timer;
	Unit* target;

	MobEventTasks Tasks;

    void Reset()
	{
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_SHIFT_H,10000,8000,5000,TARGET_RANDOM,0,-2000027);
			Tasks.AddEvent(SPELL_SHROUD_H,13000,20000,4000,TARGET_RANDOM,0,-2000028);
		}
		else
		{
			Tasks.AddEvent(SPELL_SHIFT,10000,8000,5000,TARGET_RANDOM,0,-2000027);
			Tasks.AddEvent(SPELL_SHROUD,13000,20000,4000,TARGET_RANDOM,0,-2000028);
		}
		summon_Timer = 7000;
	}
	 
	boss_zuramatAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	    
	void Aggro(Unit* pWho)
	{
		if (!CanDoSomething() || me->HasAura(66830,0))
            return;
		DoScriptText(-2000024,me);
	}

	void JustDied(Unit* pWho)
	{
		DoScriptText(-2000024,me);
	}

	void KilledUnit(Unit* pVictim)
    {
        uint8 randd = urand(0,2);
		switch(randd)
		{
			case 0:
				DoScriptText(-2000029,me);
				break;
			case 1:
				DoScriptText(-2000030,me);
				break;
			case 2:
				DoScriptText(-2000031,me);
				break;
		}
    }

    void UpdateAI(const uint32 uiDiff)
	{	
        if (!CanDoSomething())
            return;

		if(summon_Timer <= uiDiff)
		{
			me->CastStop();
			DoCastMe(SPELL_SUMMON);
			summon_Timer = urand(7000,11000);
		}
		else
			summon_Timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_zuramat(Creature* pCreature)
{
    return new boss_zuramatAI(pCreature);
} 

CreatureAI* GetAI_add_zuramat(Creature* pCreature)
{
    return new add_zuramatAI(pCreature);
} 

void AddSC_boss_zuramat()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_zuramat";
    newscript->GetAI = &GetAI_boss_zuramat;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_zuramat";
    newscript->GetAI = &GetAI_add_zuramat;
    newscript->RegisterSelf();
}

