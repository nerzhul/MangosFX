/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1576006,
    SAY_RIFT                = -1576007,
    SAY_SHIELD              = -1576008,
    SAY_KILL                = -1576009,
    SAY_DEATH               = -1576010,

    SPELL_CREATE_RIFT       = 47743,
    SPELL_CHARGE_RIFT       = 47747,
    SPELL_RIFT_SHIELD       = 47748,

    SPELL_SPARK             = 47751,
    SPELL_SPARK_H           = 57062,
	SPELL_ATTRAC			= 57063,

	SPELL_STORM_UC			= 47688,
	SPELL_STORM_C			= 47737,

	NPC_FAILLE				= 26918,
	NPC_PORTAL_ADD			= 26746,
};

/*######
## boss_anomalus
######*/

struct MANGOS_DLL_DECL boss_anomalusAI : public ScriptedAI
{
	bool invoc;
	uint32 phase_invoc_Timer;
	uint32 invoc_Timer;
	Unit* target;
	uint8 phase;

    boss_anomalusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_ATTRAC,7000,9000);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SPARK_H : SPELL_SPARK,10000,12000);
			
		phase = 1;
		invoc = false;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
		Tasks.CleanMyAdds();
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, me);
    }

	void call_portal()
	{
		Tasks.CallCreature(NPC_FAILLE,TEN_MINS,NEAR_7M);
		DoScriptText(SAY_RIFT, me);
		DoCastMe(SPELL_CREATE_RIFT);
		invoc_Timer = 15000;
	}
	void portail_phase()
	{
		invoc = true;
		DoScriptText(SAY_SHIELD, me);
		DoCastMe(SPELL_RIFT_SHIELD);
		call_portal();
		DoCastMe(SPELL_CHARGE_RIFT);
		phase_invoc_Timer = 45000;
	}
	
	void Do_Spec_Phase(const uint32 uiDiff)
	{
		if(invoc == false)
			portail_phase();
		else
		{
			if(invoc_Timer <= uiDiff)
				call_portal();
			else
			{
				// todo : arriver a cast le sort
				DoCastMe(SPELL_CHARGE_RIFT);
				invoc_Timer -= uiDiff;
			}

			phase_invoc_Timer -= uiDiff;
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!CanDoSomething())
            return;
		
		if (CheckPercentLife(52) &&  phase == 1)
			Do_Spec_Phase(uiDiff);

		if(phase_invoc_Timer <= uiDiff && invoc)
		{
			invoc = false;
			phase++;
		}

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL faille_anomalusAI : public ScriptedAI
{
	uint32 storm_Timer;
	Unit* target;

	faille_anomalusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddSummonEvent(NPC_PORTAL_ADD,2000,2000);
		storm_Timer = 1000;
    }

	void UpdateAI(const uint32 uiDiff)
    {
		if (!CanDoSomething())
            return;

		if(storm_Timer <= uiDiff)
		{
			me->SetSpeedRate(MOVE_WALK,0.01f,true);
			target = SelectUnit(SELECT_TARGET_RANDOM,0);
			if(me->HasAura(SPELL_CHARGE_RIFT))
				DoCast(target,SPELL_STORM_C);
			else
				DoCast(target,SPELL_STORM_UC);
			storm_Timer = 1200;
		}
		else
			storm_Timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);
	}
};

CreatureAI* GetAI_boss_anomalus(Creature* pCreature)
{
    return new boss_anomalusAI(pCreature);
}

CreatureAI* GetAI_portal_anomalus(Creature* pCreature)
{
    return new faille_anomalusAI(pCreature);
}


void AddSC_boss_anomalus()
{
    Script *newscript;

	newscript = new Script;
    newscript->Name = "portal_anomalus";
    newscript->GetAI = &GetAI_portal_anomalus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_anomalus";
    newscript->GetAI = &GetAI_boss_anomalus;
    newscript->RegisterSelf();
}
