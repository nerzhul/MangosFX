/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "nexus.h"

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

struct MANGOS_DLL_DECL boss_anomalusAI : public LibDevFSAI
{
    boss_anomalusAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_ATTRAC,7000,9000);
		AddEvent(m_difficulty ? SPELL_SPARK_H : SPELL_SPARK,10000,12000);
    }

	bool invoc;
	uint32 phase_invoc_Timer;
	uint32 invoc_Timer;
	uint64 target;
	uint8 phase;
	bool Achievement;

    void Reset()
    {
		CleanMyAdds();
		ResetTimers();			
		phase = 1;
		invoc = false;
		Achievement = true;
		SetInstanceData(TYPE_ANOMALUS,NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
		SetInstanceData(TYPE_ANOMALUS,IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
		CleanMyAdds();
		SetInstanceData(TYPE_ANOMALUS,DONE);
		if(Achievement && m_difficulty)
			CompleteAchievementForGroup(2037);
    }

	void FailAchievement()
	{
		Achievement = false;
	}

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, me);
    }

	void call_portal()
	{
		CallCreature(NPC_FAILLE,TEN_MINS,NEAR_7M);
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
	
	void Do_Spec_Phase(const uint32 diff)
	{
		if(invoc == false)
			portail_phase();
		else
		{
			if(invoc_Timer <= diff)
				call_portal();
			else
			{
				// todo : arriver a cast le sort
				DoCastMe(SPELL_CHARGE_RIFT);
				invoc_Timer -= diff;
			}

			phase_invoc_Timer -= diff;
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		if (CheckPercentLife(52) &&  phase == 1)
			Do_Spec_Phase(diff);

		if(phase_invoc_Timer <= diff && invoc)
		{
			invoc = false;
			phase++;
		}

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL faille_anomalusAI : public LibDevFSAI
{
	faille_anomalusAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddSummonEvent(NPC_PORTAL_ADD,2000,2000);
    }

	uint32 storm_Timer;

    void Reset()
    {
		ResetTimers();
		storm_Timer = 1000;
    }

	void JustDied(Unit* pWho)
	{
		if(Creature* Anomalus = GetInstanceCreature(TYPE_ANOMALUS))
			((boss_anomalusAI*)Anomalus->AI())->FailAchievement();
	}

	void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(storm_Timer <= diff)
		{
			me->SetSpeedRate(MOVE_WALK,0.01f,true);
			if(me->HasAura(SPELL_CHARGE_RIFT))
				DoCastRandom(SPELL_STORM_C);
			else
				DoCastRandom(SPELL_STORM_UC);
			storm_Timer = 1200;
		}
		else
			storm_Timer -= diff;

		UpdateEvent(diff);
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
