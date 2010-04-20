/* ScriptDev Frost Sapphire Studios */

#include "precompiled.h"
#include "utgarde_keep.h"

enum
{
    SAY_AGGRO_FIRST         = -1574005,
    SAY_AGGRO_SECOND        = -1574006,
    SAY_DEATH_FIRST         = -1574007,
    SAY_DEATH_SECOND        = -1574008,
    SAY_KILL_FIRST          = -1574009,
    SAY_KILL_SECOND         = -1574010,
    EMOTE_ROAR              = -1574022,

	SAY_ANNHYLDE			= -1616001,

	SPELL_SMASH				= 67038,
	SPELL_CLEAVE			= 42724,
	SPELL_STAGGERING_ROAR_N	= 42708,
	SPELL_STAGGERING_ROAR_H	= 59708,
	SPELL_DREADFULL_ROAR_N	= 42729,
	SPELL_DREADFULL_ROAR_H	= 59734,
	SPELL_DARK_SMASH		= 42723,
	SPELL_ENRAGE_NORMAL		= 42705,
	SPELL_ENRAGE_HEROIC		= 59707,
	SPELL_WOE_NORMAL		= 42730,
	SPELL_WOE_HEROIC		= 59735,
	SPELL_INGVAR_MORPH		= 42796,
	SPELL_FEINT_DEATH		= 42795,
	SPELL_AURA_DEATH		= 42862,
	SPELL_CAST_DEATH		= 42857,
	SPELL_EFFECT_REZ		= 42704,

    SPELL_SHADOW_AXE            = 42748,
    SPELL_SHADOW_AXE_PROC       = 42751,
    SPELL_SHADOW_AXE_PROC_H     = 59720,

	NPC_THROW_TARGET            = 23996,                    //the target, casting spell and target of moving dummy
    NPC_THROW_DUMMY             = 23997,                    //the axe, moving to target

};

Unit * Dummy_Target;

/*######
## boss_ingvar
######*/

struct MANGOS_DLL_DECL boss_ingvarAI : public ScriptedAI
{

	uint32 Smash_Timer;
	uint32 Rez_Event_Timer;
	uint32 Annhylde_Wait_Timer;
	uint32 rez_phase;
	bool axe_here;
	uint8 phase;
	Creature* Annhylde;

    boss_ingvarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    bool m_bIsResurrected;

    void Reset()
    {
		phase = 0;

		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_CLEAVE,7000,15000);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_STAGGERING_ROAR_H,22000,20000,0,TARGET_MAIN,1,EMOTE_ROAR,true);
			Tasks.AddEvent(SPELL_DREADFULL_ROAR_H,22000,20000,0,TARGET_MAIN,2,EMOTE_ROAR,true);
			Tasks.AddEvent(SPELL_WOE_HEROIC,12000,25000,0,TARGET_MAIN,2);
		}
		else
		{
			Tasks.AddEvent(SPELL_STAGGERING_ROAR_N,22000,20000,0,TARGET_MAIN,1,EMOTE_ROAR,true);
			Tasks.AddEvent(SPELL_DREADFULL_ROAR_H,22000,20000,0,TARGET_MAIN,2,EMOTE_ROAR,true);
			Tasks.AddEvent(SPELL_WOE_NORMAL,12000,25000,0,TARGET_MAIN,2);
		}
		Smash_Timer = 6000;
		Annhylde_Wait_Timer = 1000;
		rez_phase = 1;
        m_bIsResurrected = false;
		axe_here = false;
    }

    void Aggro(Unit* pWho)
    {
		if (!CanDoSomething())
            return;
		phase = 1;
        DoScriptText(m_bIsResurrected ? SAY_AGGRO_SECOND : SAY_AGGRO_FIRST, me);
		if(m_bIsResurrected == true)
		{
			if(m_bIsHeroic == false)
				for(short i=0;i<3;i++)
					DoCastMe(SPELL_ENRAGE_NORMAL);
			else
				for(short i=0;i<3;i++)
					DoCastMe(SPELL_ENRAGE_HEROIC);
		}
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(m_bIsResurrected ? SAY_DEATH_SECOND : SAY_DEATH_FIRST, me);
		if(m_bIsResurrected)
			GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(m_bIsResurrected ? SAY_KILL_SECOND : SAY_KILL_FIRST, me);
    }

	void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (damage > me->GetHealth() && m_bIsResurrected == false)                // Don't let ourselves be slain before we do our death speech
        {
            damage = 0;
			phase = 2;
            me->SetHealth(me->GetMaxHealth()/100);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!CanDoSomething())
            return;

		uint32 PercentLife = Tasks.GetPercentLife();
		if((PercentLife < 79 && PercentLife > 72) || (PercentLife < 52 && PercentLife > 47) || (PercentLife < 27 && PercentLife > 22))
		{
			if(m_bIsHeroic == true)
			{
				if(Smash_Timer <= uiDiff)
				{
					DoCastRandom(m_bIsResurrected ? SPELL_DARK_SMASH : SPELL_SMASH);
					Smash_Timer = 6000;
				}
				else
					Smash_Timer -= uiDiff;
			}
		}

		if(m_bIsResurrected == true)
		{
			if(PercentLife < 54 && PercentLife > 45 && !axe_here)
			{
				axe_here = true;
				DoCastMe(SPELL_SHADOW_AXE);
				Tasks.CallCreature(NPC_THROW_TARGET);
				Tasks.CallCreature(NPC_THROW_DUMMY,TEN_MINS,ON_ME,NOTHING);
			}
		}

		if(PercentLife < 1 && m_bIsResurrected == false || rez_phase == 6)
		{
			me->AttackStop();
			me->StopMoving();
			if(Annhylde_Wait_Timer <= uiDiff)
			{
				if(rez_phase == 1)
				{
					me->RemoveAllAuras();
					me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					me->SetStandState(UNIT_STAND_STATE_DEAD);
					Annhylde = me->SummonCreature(24068,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+7.0f,8.0f,TEMPSUMMON_TIMED_DESPAWN,20000);
					DoScriptText(SAY_ANNHYLDE,Annhylde);
					Annhylde->AddMonsterMoveFlag(MONSTER_MOVE_FLY);
					Annhylde->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Annhylde->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+5.0f);
					rez_phase = 2;
					Annhylde_Wait_Timer = 2500;
				}
				else if(rez_phase == 2)
				{
					me->CastSpell(me,66830,false);
					rez_phase = 3;
					Annhylde_Wait_Timer = 1000;
				}
				else if(rez_phase == 3)
				{
					DoCast(Annhylde,SPELL_AURA_DEATH,true);
					Annhylde_Wait_Timer = 1000;
					rez_phase = 4;
				}
				else if(rez_phase == 4)
				{
					Annhylde->CastSpell(me,SPELL_CAST_DEATH,true);
					Annhylde_Wait_Timer = 10000;
					rez_phase = 5;
				}
				else if(rez_phase == 5)
				{
					me->RemoveAllAuras();
					DoCastMe(SPELL_EFFECT_REZ);
					me->SetStandState(UNIT_STAND_STATE_STAND);
					me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
					Annhylde->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+8.0f);
					Annhylde_Wait_Timer = 3000;
					rez_phase = 6;
				}
				else if(rez_phase == 6)
				{
					DoCastMe(SPELL_INGVAR_MORPH);
					m_bIsResurrected = true;
					rez_phase = 7;
				}

			}
			else
				Annhylde_Wait_Timer -= uiDiff;
		}
		
		Tasks.UpdateEvent(uiDiff);
		Tasks.UpdateEvent(uiDiff,phase);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL axe_ingvarAI : public ScriptedAI
{
	axe_ingvarAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		if(m_bIsHeroic)
			Tasks.AddEvent(SPELL_SHADOW_AXE_PROC_H,1000,2000);
		else
			Tasks.AddEvent(SPELL_SHADOW_AXE_PROC,1000,2000);
	}

	void UpdateAI(const uint32 uiDiff)
    {
		Tasks.UpdateEvent(uiDiff);
	}

};

CreatureAI* GetAI_boss_ingvar(Creature* pCreature)
{
    return new boss_ingvarAI(pCreature);
}

CreatureAI* GetAI_axe_ingvar(Creature* pCreature)
{
    return new axe_ingvarAI(pCreature);
}


void AddSC_boss_ingvar()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ingvar";
    newscript->GetAI = &GetAI_boss_ingvar;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "axe_ingvar";
    newscript->GetAI = &GetAI_axe_ingvar;
    newscript->RegisterSelf();
}
