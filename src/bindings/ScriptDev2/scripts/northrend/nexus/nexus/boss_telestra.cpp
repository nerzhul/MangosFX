/* LibDevFS by Frost Sapphire Studios
 */

#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1576000,
    SAY_SPLIT_1             = -1576001,
    SAY_SPLIT_2             = -1576002,
    SAY_MERGE               = -1576003,
    SAY_KILL                = -1576004,
    SAY_DEATH               = -1576005,

    SPELL_FIREBOMB          = 47773,
    SPELL_FIREBOMB_H        = 56934,

    SPELL_ICE_NOVA          = 47772,
    SPELL_ICE_NOVA_H        = 56935,

    SPELL_GRAVITY_WELL      = 47756,

    SPELL_SUMMON_CLONES     = 47710,

    SPELL_ARCANE_VISUAL     = 47704,
    SPELL_FIRE_VISUAL       = 47705,
    SPELL_FROST_VISUAL      = 47706,

    SPELL_SUMMON_FIRE       = 47707,
    SPELL_SUMMON_ARCANE     = 47708,
    SPELL_SUMMON_FROST      = 47709,

    SPELL_FIRE_DIES         = 47711,
    SPELL_ARCANE_DIES       = 47713,
    SPELL_FROST_DIES        = 47712,

    SPELL_SPAWN_BACK_IN     = 47714,

    NPC_TELEST_FIRE         = 26928,
    NPC_TELEST_ARCANE       = 26929,
    NPC_TELEST_FROST        = 26930,

	SPELL_FROST_BLIZZ		= 47727,
	SPELL_FROST_BLIZZ_H		= 56936,
	SPELL_FROST_ICE			= 47729,
	SPELL_FROST_ICE_H		= 56937,

	SPELL_ARCA_SHEEP		= 47731,
	SPELL_ARCA_TIME			= 47736,

	SPELL_FIRE_BLAST		= 47721,
	SPELL_FIRE_BLAST_H		= 56939,
	SPELL_FIRE_SCORCH		= 47723,
	SPELL_FIRE_SCORCH_H		= 56938,
};

/*######
## boss_telestra
######*/

struct MANGOS_DLL_DECL boss_telestraAI : public LibDevFSAI
{
	boss_telestraAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		if(m_difficulty)
		{
			AddEvent(SPELL_FIREBOMB_H,1000,3000,1000);
			AddEvent(SPELL_ICE_NOVA_H,5000,10000,2000);
		}
		else
		{
			AddEvent(SPELL_FIREBOMB,1000,3000,1000);
			AddEvent(SPELL_ICE_NOVA,5000,10000,2000);
		}
    }

	uint8 phase;
	uint8 subphase;
	uint32 phase_Timer;

	uint64 add[3];
	uint8 addDown;
	int32 ach_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		phase_Timer = 0;
		subphase = 0;
		ach_Timer = 5000;
		addDown = 0;
		me->SetVisibility(VISIBILITY_ON);
		for(uint8 i=0;i<3;i++)
			add[i] = 0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

	void DeclareAddDown()
	{
		addDown++;
	}

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		me->SetVisibility(VISIBILITY_ON);
		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
		if(ach_Timer > 0 && addDown == 3)
			pInstance->CompleteAchievementForGroup(2150);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(subphase == 0)
			UpdateEvent(diff);

		if(addDown > 0 && phase >= 2)
		{
			ach_Timer -= diff;
		}

		if((CheckPercentLife(56) && phase == 1) || (CheckPercentLife(25) && phase == 2))
		{
			if(phase_Timer <= diff)
			{
				switch(subphase)
				{
					case 0:
						me->CastStop();
						me->AttackStop();
						DoCastMe(SPELL_GRAVITY_WELL);
						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						phase_Timer = 3000;
						break;
					case 1:		
						me->CastStop();
						me->Relocate(504.905f,88.960f,-16.3f,0.025f);
						DoCastMe(SPELL_SPAWN_BACK_IN);
						phase_Timer = 500;
						break;
					case 2:
						DoCastMe(SPELL_SUMMON_CLONES);
						phase_Timer = 2000;
						break;
					case 3:
						me->SetVisibility(VISIBILITY_OFF);
						me->CastStop();
						DoCastMe(66830);
						if(Creature* cr = CallCreature(NPC_TELEST_FIRE,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,499.783f,97.371f,-16.01f))
							add[0] = cr->GetGUID();
						phase_Timer = 400;
						break;
					case 4:
						if(Creature* cr = CallCreature(NPC_TELEST_FROST,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,492.028f,78.537f,-16.01f,0.793f))
							add[1] = cr->GetGUID();
						phase_Timer = 400;
						break;
					case 5:
						if(Creature* cr = CallCreature(NPC_TELEST_ARCANE,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,514.721f,89.250f,-16.01f))
							add[2] = cr->GetGUID();
						phase_Timer = 0;
						break;
				}
				subphase++;

				if(subphase == 6)
				{
					subphase = 0;
					phase++;
				}
			}
			else
				phase_Timer -= diff;

		}

		if(Creature* cr0 = GetGuidCreature(add[0]))
			if(Creature* cr1 = GetGuidCreature(add[1]))
				if(Creature* cr2 = GetGuidCreature(add[2]))
				{
					if(!cr0->isAlive() && !cr1->isAlive() && !cr2->isAlive())
					{
						me->SetVisibility(VISIBILITY_ON);
						cr0->ForcedDespawn();
						cr1->ForcedDespawn();
						cr2->ForcedDespawn();
						me->RemoveAllAuras();
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					}
				}

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL fire_telestraAI : public ScriptedAI
{
	 
	fire_telestraAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_FIRE_BLAST_H,200,7000,3000);
			Tasks.AddEvent(SPELL_FIRE_SCORCH_H,1000,1500,1000);
		}
		else
		{
			Tasks.AddEvent(SPELL_FIRE_BLAST,200,7000,3000);
			Tasks.AddEvent(SPELL_FIRE_SCORCH,1000,1500,1000);
		}
	}

	void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);
	}
};

struct MANGOS_DLL_DECL frost_telestraAI : public ScriptedAI
{
	frost_telestraAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_FROST_BLIZZ_H,200,12000);
			Tasks.AddEvent(SPELL_FROST_ICE_H,6200,5000);
		}
		else
		{
			Tasks.AddEvent(SPELL_FROST_BLIZZ,200,12000);
			Tasks.AddEvent(SPELL_FROST_ICE,6200,5000);
		}
	}

	void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;
	
		Tasks.UpdateEvent(diff);
	}
};

struct MANGOS_DLL_DECL arca_telestraAI : public ScriptedAI
{
	arca_telestraAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_ARCA_SHEEP,200,1500,1000);
		Tasks.AddEvent(SPELL_ARCA_TIME,2000,8000,2000);
	}

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);
	}
};
CreatureAI* GetAI_boss_telestra(Creature* pCreature)
{
    return new boss_telestraAI(pCreature);
}

CreatureAI* GetAI_fire_telestra(Creature* pCreature)
{
    return new fire_telestraAI(pCreature);
}

CreatureAI* GetAI_frost_telestra(Creature* pCreature)
{
    return new frost_telestraAI(pCreature);
}

CreatureAI* GetAI_arca_telestra(Creature* pCreature)
{
    return new arca_telestraAI(pCreature);
}


void AddSC_boss_telestra()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_telestra";
    newscript->GetAI = &GetAI_boss_telestra;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "fire_telestra";
    newscript->GetAI = &GetAI_fire_telestra;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "arca_telestra";
    newscript->GetAI = &GetAI_arca_telestra;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "frost_telestra";
    newscript->GetAI = &GetAI_frost_telestra;
    newscript->RegisterSelf();
}
