/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss Ionar
SD%Complete: 80%
SDComment: Timer check
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "halls_of_lightning.h"

enum
{
    SAY_AGGRO                               = -1602011,
    SAY_SLAY_1                              = -1602012,
    SAY_SLAY_2                              = -1602013,
    SAY_SLAY_3                              = -1602014,
    SAY_DEATH                               = -1602015,
    SAY_SPLIT_1                             = -1602016,
    SAY_SPLIT_2                             = -1602017,

    SPELL_BALL_LIGHTNING_N                  = 52780,
    SPELL_BALL_LIGHTNING_H                  = 59800,
    SPELL_STATIC_OVERLOAD_N                 = 52658,
    SPELL_STATIC_OVERLOAD_H                 = 59795,

    SPELL_DISPERSE                          = 52770,
    SPELL_SUMMON_SPARK                      = 52746,
    SPELL_SPARK_DESPAWN                     = 52776,

    //Spark of Ionar
    SPELL_SPARK_VISUAL_TRIGGER_N            = 52667,
    SPELL_SPARK_VISUAL_TRIGGER_H            = 59833,

    NPC_SPARK_OF_IONAR                      = 28926,

    MAX_SPARKS                              = 5,
    POINT_CALLBACK                          = 0
};

/*######
## Boss Ionar
######*/

struct MANGOS_DLL_DECL boss_ionarAI : public ScriptedAI
{
    boss_ionarAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    std::list<uint64> m_lSparkGUIDList;

    bool m_bIsRegularMode;

    bool m_bIsSplitPhase;
    uint32 m_uiSplit_Timer;
    uint32 m_uiSparkAtHomeCount;

    uint32 m_uiHealthAmountModifier;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsRegularMode)
		{
			Tasks.AddEvent(SPELL_STATIC_OVERLOAD_N,urand(5000,6000),5000,1000);
			Tasks.AddEvent(SPELL_BALL_LIGHTNING_N,10000,10000,1000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_STATIC_OVERLOAD_H,urand(5000,6000),5000,1000);
			Tasks.AddEvent(SPELL_BALL_LIGHTNING_H,10000,10000,1000,TARGET_MAIN);
		}
        m_lSparkGUIDList.clear();

        m_bIsSplitPhase = true;
        m_uiSplit_Timer = 25000;
        m_uiSparkAtHomeCount = 0;

        m_uiHealthAmountModifier = 1;

        if (me->GetVisibility() == VISIBILITY_OFF)
            me->SetVisibility(VISIBILITY_ON);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (me->getVictim())
            return;

        if (me->GetVisibility() == VISIBILITY_OFF)
            return;

        AttackStart(pAttacker);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IONAR, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IONAR, NOT_STARTED);
    }

    void AttackStart(Unit* pWho)
    {
        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);

            if (me->GetVisibility() != VISIBILITY_OFF)
                me->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, me);
        DespawnSpark();
		me->SetVisibility(VISIBILITY_ON);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IONAR, DONE);
    }

    void KilledUnit(Unit *victim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SLAY_3, me); break;
        }
    }

    void DespawnSpark()
    {
        if (m_lSparkGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, *itr))
            {
                if (pTemp->isAlive())
                    pTemp->ForcedDespawn();
            }
        }

        m_lSparkGUIDList.clear();
    }

    //make sparks come back
    void CallBackSparks()
    {
        //should never be empty here, but check
        if (m_lSparkGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
        {
            if (Creature* pSpark = (Creature*)Unit::GetUnit(*me, *itr))
            {
                if (pSpark->isAlive())
                {
                    if (pSpark->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                        pSpark->GetMotionMaster()->MovementExpired();

                    pSpark->SetSpeedRate(MOVE_RUN, pSpark->GetCreatureInfo()->speed * 2);
                    pSpark->GetMotionMaster()->MovePoint(POINT_CALLBACK, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                }
            }
        }
    }

    void RegisterSparkAtHome()
    {
        ++m_uiSparkAtHomeCount;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_SPARK_OF_IONAR)
        {
            pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_SPARK_VISUAL_TRIGGER_N : SPELL_SPARK_VISUAL_TRIGGER_H, true);

            Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

            if (me->getVictim())
                pSummoned->AI()->AttackStart(pTarget ? pTarget : me->getVictim());

            m_lSparkGUIDList.push_back(pSummoned->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        // Splitted
        if (me->GetVisibility() == VISIBILITY_OFF)
        {
            if (!me->isInCombat())
            {
                Reset();
                return;
            }

            if (m_uiSplit_Timer < diff)
            {
                m_uiSplit_Timer = 2500;

                // Return sparks to where Ionar splitted
                if (m_bIsSplitPhase)
                {
                    CallBackSparks();
                    m_bIsSplitPhase = false;
                }
                // Lightning effect and restore Ionar
                else if (m_uiSparkAtHomeCount == MAX_SPARKS)
                {
                    me->SetVisibility(VISIBILITY_ON);
                    me->CastSpell(me, SPELL_SPARK_DESPAWN, false);

                    DespawnSpark();

                    m_uiSparkAtHomeCount = 0;
                    m_uiSplit_Timer = 25000;
                    m_bIsSplitPhase = true;

                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    {
                        if (me->getVictim())
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                    }
                }
            }
            else
                m_uiSplit_Timer -= diff;

            return;
        }

        if (!CanDoSomething())
            return;

		// Health check
        if ((me->GetHealth()*100 / me->GetMaxHealth()) < (100-(20*m_uiHealthAmountModifier)))
        {
            ++m_uiHealthAmountModifier;

            DoScriptText(urand(0, 1) ? SAY_SPLIT_1 : SAY_SPLIT_2, me);

            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            DoCast(me, SPELL_DISPERSE);
        }

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ionar(Creature* pCreature)
{
    return new boss_ionarAI(pCreature);
}

bool EffectDummyCreature_boss_ionar(Unit* pCaster, uint32 uiSpellId, uint32 uiEffIndex, Creature* pCreatureTarget)
{
    //always check spellid and effectindex
    if (uiSpellId == SPELL_DISPERSE && uiEffIndex == 0)
    {
        if (pCreatureTarget->GetEntry() != NPC_IONAR)
            return true;

        for(uint8 i = 0; i < MAX_SPARKS; ++i)
            pCreatureTarget->CastSpell(pCreatureTarget, SPELL_SUMMON_SPARK, true);

        pCreatureTarget->AttackStop();
        pCreatureTarget->SetVisibility(VISIBILITY_OFF);

        if (pCreatureTarget->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
            pCreatureTarget->GetMotionMaster()->MovementExpired();

        //always return true when we are handling this spell and effect
        return true;
    }
    return false;
}

/*######
## mob_spark_of_ionar
######*/

struct MANGOS_DLL_DECL mob_spark_of_ionarAI : public ScriptedAI
{
    mob_spark_of_ionarAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() { }

	void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
	{
		uiDamage = 0;
	}

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE || !m_pInstance)
            return;

        if (uiPointId == POINT_CALLBACK)
        {
            if (Creature* pIonar = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_IONAR)))
            {
                if (!pIonar->isAlive())
                {
                    me->ForcedDespawn();
                    return;
                }

                if (boss_ionarAI* pIonarAI = dynamic_cast<boss_ionarAI*>(pIonar->AI()))
                    pIonarAI->RegisterSparkAtHome();
            }
            else
                me->ForcedDespawn();
        }
    }
};

CreatureAI* GetAI_mob_spark_of_ionar(Creature* pCreature)
{
    return new mob_spark_of_ionarAI(pCreature);
}

void AddSC_boss_ionar()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ionar";
    newscript->GetAI = &GetAI_boss_ionar;
    newscript->pEffectDummyCreature = &EffectDummyCreature_boss_ionar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_spark_of_ionar";
    newscript->GetAI = &GetAI_mob_spark_of_ionar;
    newscript->RegisterSelf();
}
