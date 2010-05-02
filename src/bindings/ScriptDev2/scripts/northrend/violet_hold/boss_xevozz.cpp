#include "precompiled.h"

enum
{
		SPELL_BARRAGE			=	54202,
		SPELL_BARRAGE_H			=	59483,
		SPELL_BUFFER			=	54226,
		SPELL_BUFFER_H			=	59485,
		SPELL_ADD1				=	54102,
		SPELL_ADD2				=	54137,
		SPELL_ADD3				=	54138,
		SPELL_ADD1_H			=	61337,
		SPELL_ADD2_H			=	61338,
		SPELL_ADD3_H			=	61339,
};
   
/*######
## boss_xevozz
######*/

struct MANGOS_DLL_DECL boss_xevozzAI : public ScriptedAI
{	
	float life;
	uint8 add;
	Unit* target;

    void Reset()
	{
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_BARRAGE_H,5000,5000,3000);
			Tasks.AddEvent(SPELL_BUFFER_H,9000,14000,4000);
		}
		else
		{
			Tasks.AddEvent(SPELL_BARRAGE,5000,5000,3000);
			Tasks.AddEvent(SPELL_BUFFER,9000,14000,4000);
		}
		add = 0;
	}
	 
	boss_xevozzAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;
	    
	void Aggro(Unit* pWho)
	{
		if (CanDoSomething() || me->HasAura(66830,0))
            return;
		DoScriptText(-2000038,me);
	}

	void JustDied(Unit* pWho)
	{
		CleanAdds();
		DoScriptText(-2000043,me);
	}

	void KilledUnit(Unit* pVictim)
    {
        uint8 randd = urand(0,2);
		switch(randd)
		{
			case 0:
				DoScriptText(-2000040,me);
				break;
			case 1:
				DoScriptText(-2000041,me);
				break;
			case 2:
				DoScriptText(-2000042,me);
				break;
		}
    }
	void CleanAdds()
	{
        m_pInstance->SetData(2,1);
	}

    void UpdateAI(const uint32 uiDiff)
	{	
        if (!CanDoSomething())
            return;

		life = me->GetHealth()*100 / me->GetMaxHealth();
		if(CheckPercentLife(75) && add == 0)
		{
			DoScriptText(-2000044,me);
			me->CastStop();
			add++;
			DoCastMe(m_bIsHeroic ? SPELL_ADD1_H : SPELL_ADD1);
		}
		else if(CheckPercentLife(50) && add == 1)
		{
			DoScriptText(-2000045,me);
			me->CastStop();
			add++;
			DoCastMe(m_bIsHeroic ? SPELL_ADD2_H : SPELL_ADD2);
		}
		else if(CheckPercentLife(25) && add == 2)
		{
			DoScriptText(-2000046,me);
			me->CastStop();
			add++;
			DoCastMe(m_bIsHeroic ? SPELL_ADD3_H : SPELL_ADD3);
		}

		Tasks.UpdateEvent(uiDiff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_xevozz(Creature* pCreature)
{
    return new boss_xevozzAI(pCreature);
} 

struct MANGOS_DLL_DECL add_xevozzAI: public ScriptedAI
{
	uint32 move_Timer;
	uint32 explode_Timer;
	Creature* pXevozz;
    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	float x,y,z;

	add_xevozzAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
	}

	void Reset()
	{
		move_Timer = 100;
		explode_Timer = 1000;
	}

	void Aggro(Unit* pWho)
	{
		if (!CanDoSomething())
            return;

		DoCastMe(54164);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if(explode_Timer <= uiDiff)
		{
			me->StopMoving();
			DoCastMe(m_bIsHeroic ? 59474 : 54160);

			explode_Timer = 1000;
		}
		else
			explode_Timer -= uiDiff;

		if(move_Timer <= uiDiff)
		{
			
			urand(0,1) ? x = me->GetPositionX() - urand(2,4) : x = me->GetPositionX() + urand(2,4);

			urand(0,1) ? y = me->GetPositionY() - urand(2,4) : y = me->GetPositionY() + urand(2,4);

			me->GetMotionMaster()->MovePoint(0,x,y,me->GetPositionZ());
			move_Timer = 3000;
		}
		else
			move_Timer -= uiDiff;
	}
};

CreatureAI* GetAI_add_xevozz(Creature* pCreature)
{
    return new add_xevozzAI(pCreature);
} 

void AddSC_boss_xevozz()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_xevozz";
    newscript->GetAI = &GetAI_boss_xevozz;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_xevozz";
    newscript->GetAI = &GetAI_add_xevozz;
    newscript->RegisterSelf();
}

