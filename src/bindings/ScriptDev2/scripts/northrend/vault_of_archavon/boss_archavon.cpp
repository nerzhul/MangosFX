/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Archavon_The_Stone_Watcher
SD%Complete: 0%
SDComment:
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

enum
{
    SPELL_ROCK_SHARDS_LEFT_N                = 58695,
    SPELL_ROCK_SHARDS_LEFT_H                = 60883,
    SPELL_ROCK_SHARDS_RIGHT_N               = 58696,
    SPELL_ROCK_SHARDS_RIGHT_H               = 60884,
    SPELL_CRUSHING_LEAP_N                   = 58963,
    SPELL_CRUSHING_LEAP_H                   = 60895,
    SPELL_STOMP_N                           = 58663,
    SPELL_STOMP_H                           = 60880,
    SPELL_IMPALE_DMG_N                      = 58666,
    SPELL_IMPALE_DMG_H                      = 60882,
    SPELL_IMPALE_STUN                       = 50839,
    SPELL_BERSERK                           = 47008
};

struct MANGOS_DLL_DECL boss_archavonAI : public ScriptedAI
{
    boss_archavonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
        m_fDefaultMoveSpeed = pCreature->GetSpeedRate(MOVE_RUN);
        Reset();
    }

    bool m_bIsRegularMode;
    float m_fDefaultMoveSpeed;
    uint32 m_uiEvadeCheckCooldown;

    uint32 m_uiBerserkTimer;
    uint32 m_uiRockShardsTimer;
    bool m_bRockShardsInProgress;
    uint32 m_uiRockShardsProgressTimer;
    uint32 m_uiRockShardTimer;
    bool m_bRLRockShard;
    Unit* m_pRockShardsTarget;
    uint32 m_uiCrushingLeapTimer;
    Unit* m_pCrushingLeapTarget;
    bool m_bCrushingLeapInProgress;
    uint32 m_uiCrushingLeapSecureTimer;
    uint32 m_uiStompTimer;
    uint32 m_uiImpaleAfterStompTimer;
    bool m_bImpaleInProgress;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_BERSERK,300000,30000,0,TARGET_ME);
        m_uiEvadeCheckCooldown = 2000;
        me->SetSpeedRate(MOVE_RUN, m_fDefaultMoveSpeed);
        m_uiBerserkTimer = 300000;
        m_uiRockShardsTimer = 15000;
        m_bRockShardsInProgress = false;
        m_uiRockShardsProgressTimer = 3000;
        m_uiRockShardTimer = 0;
        m_bRLRockShard = true;
        m_pRockShardsTarget = NULL;
        m_uiCrushingLeapTimer = 30000;
        m_pCrushingLeapTarget = NULL;
        m_bCrushingLeapInProgress = false;
        m_uiCrushingLeapSecureTimer = 2000;
        m_uiStompTimer = 45000;
        m_uiImpaleAfterStompTimer = 1000;
        m_bImpaleInProgress = false;

        if(pInstance)
            pInstance->SetData(TYPE_ARCHAVON, NOT_STARTED);
    }

    void Aggro(Unit *pWho)
    {
        if(pInstance)
            pInstance->SetData(TYPE_ARCHAVON, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if(pInstance)
            pInstance->SetData(TYPE_ARCHAVON, DONE);

		GiveEmblemsToGroup((m_bIsRegularMode) ? VAILLANCE : HEROISME ,3);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

        if (m_uiEvadeCheckCooldown < diff)
        {
            if (me->GetDistance2d(138.287f, -101.09f) > 150.0f)
                EnterEvadeMode();
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= diff;

        if (m_bImpaleInProgress)
        {
            if (m_uiImpaleAfterStompTimer < diff)
            {
                if (Unit* pTarget = me->getVictim())
                {
                    DoCastVictim(m_bIsRegularMode ? SPELL_IMPALE_DMG_N : SPELL_IMPALE_DMG_H);
                    pTarget->CastSpell(pTarget, SPELL_IMPALE_STUN, true);
                }
                m_bImpaleInProgress = false;
            }
            else
            {
                m_uiImpaleAfterStompTimer -= diff;
                return;
            }
        }

        if (m_bCrushingLeapInProgress)
        {
            if (m_pCrushingLeapTarget)
            {
                if (m_pCrushingLeapTarget->isDead() || !m_pCrushingLeapTarget->IsInWorld() && !m_pCrushingLeapTarget->IsInMap(me))
                {
                    m_bCrushingLeapInProgress = false;
                    return;
                }
            }
            else
            {
                m_bCrushingLeapInProgress = false;
                return;
            }
            if ((m_uiCrushingLeapSecureTimer < diff) || (m_pCrushingLeapTarget && me->IsWithinDist(m_pCrushingLeapTarget, 5.0f)))
            {
                //me->AddThreat(m_pCrushingLeapTarget, -100000000.0f);
                me->SetSpeedRate(MOVE_RUN, m_fDefaultMoveSpeed);
                DoCast(m_pCrushingLeapTarget, m_bIsRegularMode ? SPELL_CRUSHING_LEAP_N : SPELL_CRUSHING_LEAP_H, true);
                m_bCrushingLeapInProgress = false;
            }
            else
                m_uiCrushingLeapSecureTimer -= diff;

            return;
        }

        if (m_bRockShardsInProgress)
        {
            if (m_uiRockShardsProgressTimer < diff)
            {
                m_bRockShardsInProgress = false;
                /*if (m_pRockShardsTarget)
                    me->AddThreat(m_pRockShardsTarget, -100000000.0f);*/
                return;
            }
            else
                m_uiRockShardsProgressTimer -= diff;

            if (m_uiRockShardTimer < diff)
            {
                if (m_pRockShardsTarget && m_pRockShardsTarget->isAlive())
                {
                    DoCast(m_pRockShardsTarget, m_bIsRegularMode ? (m_bRLRockShard ? SPELL_ROCK_SHARDS_LEFT_N : SPELL_ROCK_SHARDS_RIGHT_N) : (m_bRLRockShard ? SPELL_ROCK_SHARDS_LEFT_H : SPELL_ROCK_SHARDS_RIGHT_H));
                    m_bRLRockShard = !m_bRLRockShard;
                }
                m_uiRockShardTimer = 100;
            }
            else
                m_uiRockShardsTimer -= diff;

            return;
        }

        if (m_uiRockShardsTimer < diff)
        {
            m_bRockShardsInProgress = true;
            m_uiRockShardsProgressTimer = 3000;
            m_bRLRockShard = true;
            m_pRockShardsTarget = NULL;
            if (m_pRockShardsTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                me->AddThreat(m_pRockShardsTarget, 1000000.0f);
            m_uiRockShardsTimer = urand(15000,30000);
            return;
        }
        else
            m_uiRockShardsTimer -= diff;

        if (m_uiCrushingLeapTimer < diff)
        {
            ThreatList const& tList = me->getThreatManager().getThreatList();
            std::list<Unit*> lTargets;
            for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
            {
                Unit *pTemp = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                if (pTemp && pTemp->GetTypeId() == TYPEID_PLAYER && !me->IsWithinDist(pTemp, 10.0f) && me->IsWithinDist(pTemp, 150.0f))
                    lTargets.push_back(pTemp);
            }
            m_pCrushingLeapTarget = NULL;
            if (!lTargets.empty())
            {
                std::list<Unit*>::iterator pTarget = lTargets.begin();
                advance(pTarget, (rand() % lTargets.size()));
                m_pCrushingLeapTarget = *pTarget;
                if (m_pCrushingLeapTarget)
                {
                    me->MonsterSay(m_pCrushingLeapTarget->GetName(), LANG_UNIVERSAL, NULL);
                    me->AddThreat(m_pCrushingLeapTarget, 1000000.0f);
                    me->SetSpeedRate(MOVE_RUN, m_fDefaultMoveSpeed*10.0f);
                    m_bCrushingLeapInProgress = true;
                    m_uiCrushingLeapSecureTimer = 2000;
                }
            }
            m_uiCrushingLeapTimer = urand(30000,45000);
            return;
        }
        else
            m_uiCrushingLeapTimer -= diff;

        if (m_uiStompTimer < diff)
        {
            DoCastMe(m_bIsRegularMode ? SPELL_STOMP_N : SPELL_STOMP_H);
            m_uiImpaleAfterStompTimer = 1000;
            m_bImpaleInProgress = true;
            m_uiStompTimer = urand(45000,60000);
        }
        else
            m_uiStompTimer -= diff;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_archavon(Creature *pCreature)
{
    return new boss_archavonAI (pCreature);
};

void AddSC_boss_archavon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_archavon";
    newscript->GetAI = &GetAI_boss_archavon;
    newscript->RegisterSelf();
};
