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
SDName: Boss_Nazan_And_Vazruden
SD%Complete: 30
SDComment: Encounter is not complete. TODO: re-check script when MovementInform call from core work as expected.
SDCategory: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "precompiled.h"
#include "hellfire_ramparts.h"

enum
{
    SAY_INTRO               = -1543017,
    SAY_AGGRO1              = -1543018,
    SAY_AGGRO2              = -1543019,
    SAY_AGGRO3              = -1543020,
    SAY_TAUNT               = -1543021,
    SAY_KILL1               = -1543022,
    SAY_KILL2               = -1543023,
    SAY_DEATH               = -1543024,
    EMOTE_DESCEND           = -1543025,

    //vazruden
    SPELL_REVENGE           = 40392,

    //nazan
    SPELL_FIREBALL          = 30691,
    SPELL_H_FIREBALL        = 36920,

    SPELL_CONE_OF_FIRE      = 30926,
    SPELL_H_CONE_OF_FIRE    = 36921,

    SPELL_H_BELLOW_ROAR     = 39427,

    //misc
    POINT_ID_CENTER         = 100,
    POINT_ID_WAITING        = 101,
    POINT_ID_COMBAT         = 102,

    NPC_VAZRUDEN_HERALD     = 17307,
    NPC_NAZAN               = 17536,
    NPC_VAZRUDEN            = 17537
};

const float afCenterPos[3] = {-1399.401, 1736.365, 86.008}; //moves here to drop off nazan
const float afCombatPos[3] = {-1413.848, 1754.019, 83.146}; //moves here when decending

struct MANGOS_DLL_DECL boss_vazrudenAI : public ScriptedAI
{
    boss_vazrudenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsHeroic;

    bool m_bHealthBelow;

    void Reset()
    {
        m_bHealthBelow = false;
    }

    void Aggro(Unit* pWho)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, me); break;
            case 1: DoScriptText(SAY_AGGRO2, me); break;
            case 2: DoScriptText(SAY_AGGRO3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_VAZRUDEN, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, me);
    }

    void PrepareAndDescendMount()
    {
        if (Creature* pHerald = pInstance->instance->GetCreature(pInstance->GetData64(DATA_HERALD)))
        {
            if (pHerald->HasSplineFlag(SPLINEFLAG_WALKMODE))
                pHerald->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

            pHerald->GetMotionMaster()->MovePoint(POINT_ID_COMBAT, afCombatPos[0], afCombatPos[1], afCombatPos[2]);

            DoScriptText(EMOTE_DESCEND, pHerald);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (!m_bHealthBelow && (me->GetHealth()*100 / me->GetMaxHealth()) <= 30)
        {
            if (pInstance)
                PrepareAndDescendMount();

            m_bHealthBelow = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vazruden(Creature* pCreature)
{
    return new boss_vazrudenAI(pCreature);
}

// Creature fly around platform by default.
// After "dropping off" Vazruden, transforms to mount (Nazan) and are then ready to fight when
// Vazruden reach 30% HP
struct MANGOS_DLL_DECL boss_vazruden_heraldAI : public ScriptedAI
{
    boss_vazruden_heraldAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pCreature->SetActiveObjectState(true);
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsHeroic;

    uint32 m_uiMovementTimer;

    void Reset()
    {
        if (me->GetEntry() != NPC_VAZRUDEN_HERALD)
            me->UpdateEntry(NPC_VAZRUDEN_HERALD);

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        m_uiMovementTimer = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pInstance && pInstance->GetData(TYPE_NAZAN) != IN_PROGRESS)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (!pInstance)
            return;

        if (uiType == WAYPOINT_MOTION_TYPE)
        {
            if (m_uiMovementTimer)
                return;

            if (pInstance->GetData(TYPE_NAZAN) == SPECIAL)
            {
                me->SetCombatStartPosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                m_uiMovementTimer = 1000;
            }
        }

        if (uiType == POINT_MOTION_TYPE)
        {
            if (uiPointId == POINT_ID_CENTER)
                DoSplit();
            else if (uiPointId == POINT_ID_COMBAT)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pInstance->SetData(TYPE_NAZAN, IN_PROGRESS);
            }
        }
    }

    void DoMoveToCenter()
    {
        DoScriptText(SAY_INTRO, me);

        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
        {
            me->GetMotionMaster()->MovementExpired();
            me->GetMotionMaster()->MoveIdle();
        }

        me->GetMotionMaster()->MovePoint(POINT_ID_CENTER, afCenterPos[0], afCenterPos[1], afCenterPos[2]);
    }

    void DoMoveToHold()
    {
        float fX, fY, fZ;
        me->GetCombatStartPosition(fX, fY, fZ);

        me->GetMotionMaster()->MovePoint(POINT_ID_WAITING, fX, fY, fZ);
    }

    void DoSplit()
    {
        me->UpdateEntry(NPC_NAZAN);

        me->SummonCreature(NPC_VAZRUDEN, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);

        m_uiMovementTimer = 3000;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pInstance)
            pInstance->SetData(TYPE_VAZRUDEN, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_NAZAN, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->getVictim() && m_uiMovementTimer)
        {
            if (m_uiMovementTimer <= diff)
            {
                if (pInstance)
                {
                    if (pInstance->GetData(TYPE_VAZRUDEN) == IN_PROGRESS)
                        DoMoveToHold();
                    else
                        DoMoveToCenter();
                }
                m_uiMovementTimer = 0;
            } else m_uiMovementTimer -= diff;
        }

        if (!CanDoSomething())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vazruden_herald(Creature* pCreature)
{
    return new boss_vazruden_heraldAI(pCreature);
}

void AddSC_boss_nazan_and_vazruden()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_vazruden";
    newscript->GetAI = &GetAI_boss_vazruden;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_vazruden_herald";
    newscript->GetAI = &GetAI_boss_vazruden_herald;
    newscript->RegisterSelf();
}
