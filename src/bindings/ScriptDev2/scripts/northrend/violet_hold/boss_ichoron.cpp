#include "precompiled.h"

enum
{
	SPELL_WATER_BOLT		=	54241,
	SPELL_WATER_BOLT_H		=	59521,
	SPELL_WATER_BLAST		=	54237,
	SPELL_WATER_BLAST_H		=	59520,
	SPELL_ENRAGE			=	54312,
	SPELL_ENRAGE_H			=	59522,
	SPELL_DRAINED			=	59820,
	SPELL_PROTECTIVE		=	54306,

	KNOCKBACK_H				=	64626,

	SOUND_AGGRO				=	-2000008,
	SOUND_ENRAGE			=	-2000006,
	SOUND_BUBBLE			=	-2000007,
	SOUND_EXPLODE			=	-2000009,
	SOUND_DEAD				=	-2000010,
};
                                 
   
/*######
## boss_ichoron
######*/

struct MANGOS_DLL_DECL boss_ichoronAI : public ScriptedAI
{	
    uint32 waterblast_Timer;
	uint32 waterbolt_Timer;
	uint32 drain_Timer;
	bool protect;

	Unit* target;

	MobEventTasks Tasks;

    void Reset()
	{
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_WATER_BLAST_H,12000,18000,6000);
			Tasks.AddEvent(SPELL_WATER_BOLT_H,17000,12000,4000);
		}
		else
		{
			Tasks.AddEvent(SPELL_WATER_BLAST,12000,18000,6000);
			Tasks.AddEvent(SPELL_WATER_BOLT,17000,12000,4000);
		}
		drain_Timer			= 100000000;
		waterbolt_Timer		= 17000;
		waterblast_Timer	= 12000;
	}
	 
	boss_ichoronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	   
	void JustDied(Unit* pWho)
	{
		m_pInstance->SetData(3,1);
		DoScriptText(SOUND_DEAD,me);
		Tasks.CleanMyAdds();
	}

	void KilledUnit(Unit* pWho)
	{
		uint8 randd = urand(0,2);
		switch(randd)
		{
			case 0:
				DoScriptText(-2000011,me);
				break;
			case 1:
				DoScriptText(-2000012,me);
				break;
			case 2:
				DoScriptText(-2000013,me);
				break;
		}
	}

	void Aggro(Unit* pWho)
	{
		if (!CanDoSomething() || me->HasAura(66830,0))
            return;
		protect = true;
		DoCastMe(SPELL_PROTECTIVE);
		DoScriptText(SOUND_AGGRO,me);
	}

	void DoPopAdds()
	{
		for(short i=0; i<5; i++)
			Tasks.CallCreature(29321,TEN_MINS,NEAR_30M,GO_TO_CREATOR);
	}

    void UpdateAI(const uint32 uiDiff)
	{	
        if (!CanDoSomething())
            return;

		if(Tasks.CheckPercentLife(25))
		{
			if(!me->HasAura(SPELL_PROTECTIVE,0) && protect == true)
			{
				me->RemoveAllAuras();
				DoScriptText(SOUND_EXPLODE,me);
				protect = false;
				if(m_bIsHeroic == true)
					DoCastMe(KNOCKBACK_H);

				DoCastMe(SPELL_DRAINED);
				DoPopAdds();
				me->SetStandState(UNIT_STAND_STATE_DEAD);
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				me->DealDamage(me,me->GetMaxHealth() / 4,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				drain_Timer = 15000;
			}

			if(drain_Timer <= uiDiff)
			{
				protect = true;
				DoScriptText(SOUND_BUBBLE,me);
				DoCastMe(SPELL_PROTECTIVE);
				me->SetStandState(UNIT_STAND_STATE_STAND);
				me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				drain_Timer = 100000000;
			}
			else
				drain_Timer -= uiDiff;
		}
		else
		{
			if(!m_bIsHeroic && !me->HasAura(SPELL_ENRAGE,0))
			{
				me->SetStandState(UNIT_STAND_STATE_STAND);
				me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				DoScriptText(SOUND_ENRAGE,me);
				DoCastMe(SPELL_ENRAGE);
			}
			else if (m_bIsHeroic && !me->HasAura(SPELL_ENRAGE_H,0))
			{
				me->SetStandState(UNIT_STAND_STATE_STAND);
				me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				DoScriptText(SOUND_ENRAGE,me);
				DoCastMe(SPELL_ENRAGE_H);
			}
		}

		Tasks.UpdateEvent(uiDiff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_ichoron(Creature* pCreature)
{
    return new boss_ichoronAI(pCreature);
} 

void AddSC_boss_ichoron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ichoron";
    newscript->GetAI = &GetAI_boss_ichoron;
    newscript->RegisterSelf();
}

