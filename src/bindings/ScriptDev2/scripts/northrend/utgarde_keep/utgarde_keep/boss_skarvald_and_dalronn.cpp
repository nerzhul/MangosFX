/* ScriptDev Frost Sapphire Studios */

#include "precompiled.h"
#include "utgarde_keep.h"

enum
{
    SAY_SKA_AGGRO                       = -1574011,
    SAY_SKA_DEATH                       = -1574012,
    SAY_SKA_DEATH_REAL                  = -1574013,
    SAY_SKA_KILL                        = -1574014,
    SAY_SKA_DAL_DIES_REPLY              = -1574015,

    SAY_DAL_AGGRO_REPLY                 = -1574016,
    SAY_DAL_DEATH                       = -1574017,
    SAY_DAL_DEATH_REAL                  = -1574018,
    SAY_DAL_KILL                        = -1574019,
    SAY_DAL_SKA_DIES_REPLY              = -1574020,

    SPELL_SUMMON_DAL_GHOST              = 48612,
    SPELL_SUMMON_SKA_GHOST              = 48613,

	SPELL_SHADOWBOLT_NORMAL				= 43649,
	SPELL_SHADOWBOLT_HEROIC				= 59575,
	SPELL_CHARGE						= 43651,
	SPELL_STONEHIT						= 48583,
	SPELL_ENRAGE						= 48193,
	SPELL_SKELETON_INVOC				= 52611,
	SPELL_DEBILITER						= 43650,
	SPELL_DEBILITER_H                   = 59577,
	SPELL_IMMUNE						= 50494, //TODO : find the real spell
    NPC_DAL_GHOST                       = 27389,
    NPC_SKA_GHOST                       = 27390
};

struct Yell
{
    int32   m_iTextId;
    int32   m_iTextReplyId;
};

Yell m_aYell[] =
{
    {SAY_SKA_AGGRO, SAY_DAL_AGGRO_REPLY},
    {SAY_SKA_DEATH, SAY_DAL_SKA_DIES_REPLY},
    {SAY_DAL_DEATH, SAY_SKA_DAL_DIES_REPLY}
};

struct MANGOS_DLL_DECL boss_s_and_d_dummyAI : public ScriptedAI
{
    boss_s_and_d_dummyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        m_uiGhostGUID = 0;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
    uint64 m_uiGhostGUID;
	Creature* m_BossGhost;

    Creature* GetBuddy()
    {
        if (!m_pInstance)
            return NULL;

        return m_pInstance->instance->GetCreature(m_pInstance->GetData64(me->GetEntry() == NPC_DALRONN ? NPC_SKARVALD : NPC_DALRONN));
    }

    void Reset() { }

    void EnterCombat(Unit* pWho)
    {
        if (!pWho)
            return;

        if (Creature* pBuddy = GetBuddy())
        {
            if (!pBuddy->getVictim())
                pBuddy->AI()->AttackStart(pWho);
        }

        Aggro(pWho);
    }

    void JustSummoned(Creature* pSummoned)
    {
        // EventAI can probably handle ghosts
        if (pSummoned->GetEntry() == NPC_DAL_GHOST || pSummoned->GetEntry() == NPC_SKA_GHOST)
        {
            m_BossGhost = pSummoned;

            Unit* pTarget = SelectUnit(SELECT_TARGET_TOPAGGRO,1);
			
            if (me->getVictim())
			{
				pSummoned->CastSpell(pSummoned,SPELL_IMMUNE,false);
                pSummoned->AI()->AttackStart(pTarget ? pTarget : me->getVictim());
			}
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (Creature* pBuddy = GetBuddy())
        {
            if (pBuddy->isAlive())
            {
                DoScriptText(me->GetEntry() == NPC_SKARVALD ? m_aYell[1].m_iTextId : m_aYell[2].m_iTextId, me);
                DoScriptText(me->GetEntry() == NPC_SKARVALD ? m_aYell[1].m_iTextReplyId : m_aYell[2].m_iTextReplyId, pBuddy);

                pBuddy->CastSpell(me, me->GetEntry() == NPC_SKARVALD ? SPELL_SUMMON_SKA_GHOST : SPELL_SUMMON_DAL_GHOST, true);

                me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            }
            else
            {
				if(m_BossGhost)
					m_BossGhost->ForcedDespawn();
            }
        }
    }
};

/*######
## boss_skarvald
######*/

struct MANGOS_DLL_DECL boss_skarvaldAI : public boss_s_and_d_dummyAI
{
    boss_skarvaldAI(Creature* pCreature) : boss_s_and_d_dummyAI(pCreature) { 
		Reset(); }

    uint32 m_uiYellDelayTimer;
	MobEventTasks Tasks;

	void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_CHARGE,500,12000);
		Tasks.AddEvent(SPELL_STONEHIT,5000,10000,0,TARGET_MAIN);
		if(m_bIsHeroic)
			Tasks.AddEvent(SPELL_ENRAGE,10000,30000,0,TARGET_ME);

        m_uiYellDelayTimer = 0;
    }

    void Aggro(Unit* pWho)
    {
		if(me->GetEntry() != NPC_SKA_GHOST)
		{
			DoScriptText(m_aYell[0].m_iTextId, me);
			m_uiYellDelayTimer = 5000;
		}
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SKA_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (m_uiYellDelayTimer && m_uiYellDelayTimer < diff)
        {
            if (Creature* pBuddy = GetBuddy())
                DoScriptText(m_aYell[0].m_iTextReplyId, pBuddy);

            m_uiYellDelayTimer = 0;
        }
        else m_uiYellDelayTimer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_skarvald(Creature* pCreature)
{
    return new boss_skarvaldAI(pCreature);
}

/*######
## boss_dalronn
######*/

struct MANGOS_DLL_DECL boss_dalronnAI : public boss_s_and_d_dummyAI
{
    boss_dalronnAI(Creature* pCreature) : boss_s_and_d_dummyAI(pCreature) { Reset(); }

	uint32 Aggro_Text_Timer;
	bool AggroSaid;
	MobEventTasks Tasks;

    void Reset() 
	{
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_SHADOWBOLT_HEROIC,2000,2000);
			Tasks.AddEvent(SPELL_SKELETON_INVOC,10000,11000,0,TARGET_ME);
			Tasks.AddEvent(SPELL_DEBILITER_H,12000,20000,0,TARGET_RANDOM,0,0,true);
		}
		else
		{
			Tasks.AddEvent(SPELL_SHADOWBOLT_HEROIC,2000,2000);
			Tasks.AddEvent(SPELL_DEBILITER,12000,20000,0,TARGET_RANDOM,0,0,true);
		}
		Aggro_Text_Timer = 5000;
		AggroSaid = false;
	}

    void KilledUnit(Unit* pVictim)
    {
			DoScriptText(SAY_DAL_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(AggroSaid == false)
		{
			if(Aggro_Text_Timer <= diff)
			{
				DoScriptText(m_aYell[1].m_iTextId, me);
				AggroSaid = true;
			}
			else
				Aggro_Text_Timer -= diff;
		}

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dalronn(Creature* pCreature)
{
    return new boss_dalronnAI(pCreature);
}

void AddSC_boss_skarvald_and_dalronn()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_skarvald";
    newscript->GetAI = &GetAI_boss_skarvald;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_dalronn";
    newscript->GetAI = &GetAI_boss_dalronn;
    newscript->RegisterSelf();
}
