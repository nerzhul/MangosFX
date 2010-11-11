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
SDName: Boss_Gruul
SD%Complete: 60
SDComment: Ground Slam need further development (knock back effect and shatter effect must be added to mangos)
SDCategory: Gruul's Lair
EndScriptData */

#include "precompiled.h"
#include "gruuls_lair.h"

enum
{
    SAY_AGGRO                   = -1565010,
    SAY_SLAM1                   = -1565011,
    SAY_SLAM2                   = -1565012,
    SAY_SHATTER1                = -1565013,
    SAY_SHATTER2                = -1565014,
    SAY_SLAY1                   = -1565015,
    SAY_SLAY2                   = -1565016,
    SAY_SLAY3                   = -1565017,
    SAY_DEATH                   = -1565018,

    EMOTE_GROW                  = -1565019,

    SPELL_GROWTH                = 36300,
    SPELL_CAVE_IN               = 36240,
    SPELL_GROUND_SLAM           = 33525,                    //AoE Ground Slam applying Ground Slam to everyone with a script effect (most likely the knock back, we can code it to a set knockback)
    SPELL_REVERBERATION         = 36297,
    SPELL_SHATTER               = 33654,

    SPELL_SHATTER_EFFECT        = 33671,
    SPELL_HURTFUL_STRIKE        = 33813,
    SPELL_STONED                = 33652,                    //Spell is self cast by target

    SPELL_MAGNETIC_PULL         = 28337,
    SPELL_KNOCK_BACK            = 24199                     //Knockback spell until correct implementation is made
};

struct MANGOS_DLL_DECL boss_gruulAI : public ScriptedAI
{
    boss_gruulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 m_uiGrowth_Timer;
    uint32 m_uiCaveIn_Timer;
    uint32 m_uiCaveIn_StaticTimer;
    uint32 m_uiGroundSlamTimer;
    uint32 m_uiHurtfulStrike_Timer;
    uint32 m_uiReverberation_Timer;

    bool m_bPerformingGroundSlam;

    void Reset()
    {
        m_uiGrowth_Timer            = 30000;
        m_uiCaveIn_Timer            = 27000;
        m_uiCaveIn_StaticTimer      = 30000;
        m_uiGroundSlamTimer         = 35000;
        m_uiHurtfulStrike_Timer     = 8000;
        m_uiReverberation_Timer     = 60000+45000;
        m_bPerformingGroundSlam     = false;
    }

    void Aggro(Unit *pWho)
    {
        DoScriptText(SAY_AGGRO, me);

        if (!pInstance)
            return;

        pInstance->SetData(TYPE_GRUUL_EVENT, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_GRUUL_EVENT, NOT_STARTED);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, me); break;
            case 1: DoScriptText(SAY_SLAY2, me); break;
            case 2: DoScriptText(SAY_SLAY3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);

        if (!pInstance)
            return;

        pInstance->SetData(TYPE_GRUUL_EVENT, DONE);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        //This to emulate effect1 (77) of SPELL_GROUND_SLAM, knock back to any direction
        //It's initially wrong, since this will cause fall damage, which is by comments, not intended.
        if (pSpell->Id == SPELL_GROUND_SLAM)
        {
            if (pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                switch(urand(0, 1))
                {
                    case 0: pTarget->CastSpell(pTarget, SPELL_MAGNETIC_PULL, true, NULL, NULL, me->GetGUID()); break;
                    case 1: pTarget->CastSpell(pTarget, SPELL_KNOCK_BACK, true, NULL, NULL, me->GetGUID()); break;
                }
            }
        }

        //this part should be in mangos
        if (pSpell->Id == SPELL_SHATTER)
        {
            //this spell must have custom handling in mangos, dealing damage based on distance
            pTarget->CastSpell(pTarget, SPELL_SHATTER_EFFECT, true);

            if (pTarget->HasAura(SPELL_STONED))
                pTarget->RemoveAurasDueToSpell(SPELL_STONED);

            //clear this, if we are still performing
            if (m_bPerformingGroundSlam)
            {
                m_bPerformingGroundSlam = false;

                //and correct movement, if not already
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                {
                    if (me->getVictim())
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        // Growth
        // Gruul can cast this spell up to 30 times
        if (m_uiGrowth_Timer < diff)
        {
            DoScriptText(EMOTE_GROW, me);
            DoCastMe(SPELL_GROWTH);
            m_uiGrowth_Timer = 30000;
        }
        else
            m_uiGrowth_Timer -= diff;

        if (m_bPerformingGroundSlam)
        {
            if (m_uiGroundSlamTimer < diff)
            {
                m_uiGroundSlamTimer     = 120000;
                m_uiHurtfulStrike_Timer = 8000;

                //Give a little time to the players to undo the damage from shatter
                if (m_uiReverberation_Timer < 10000)
                    m_uiReverberation_Timer += 10000;

                DoCastMe( SPELL_SHATTER);
            }
            else
                m_uiGroundSlamTimer -= diff;
        }
        else
        {
            // Hurtful Strike
            if (m_uiHurtfulStrike_Timer < diff)
            {
                // Find 2nd-aggro target within melee range.
                Unit *pTarget = NULL;
                std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
                std::list<HostileReference *>::iterator itr = t_list.begin();
                std::advance(itr, 1);
                for(; itr!= t_list.end(); ++itr)
                {
                    pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                    // exclude pets, totems & player out of melee range
                    if (pTarget->GetTypeId() != TYPEID_PLAYER || !pTarget->IsWithinDist(me, ATTACK_DISTANCE, false))
                    {
                        pTarget = NULL;
                        continue;
                    }
                    //we've found someone
                    break;
                }

                if (pTarget)
                    DoCast(pTarget,SPELL_HURTFUL_STRIKE);
                else
                    DoCastVictim(SPELL_HURTFUL_STRIKE);

                m_uiHurtfulStrike_Timer = 8000;
            }
            else
                m_uiHurtfulStrike_Timer -= diff;

            // Reverberation
            if (m_uiReverberation_Timer < diff)
            {
                DoCastVictim( SPELL_REVERBERATION, true);
                m_uiReverberation_Timer = urand(15000, 25000);
            }
            else
                m_uiReverberation_Timer -= diff;

            // Cave In
            if (m_uiCaveIn_Timer < diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget,SPELL_CAVE_IN);

                if (m_uiCaveIn_StaticTimer >= 4000)
                    m_uiCaveIn_StaticTimer -= 2000;

                    m_uiCaveIn_Timer = m_uiCaveIn_StaticTimer;

            }
            else
                m_uiCaveIn_Timer -= diff;

            // Ground Slam, Gronn Lord's Grasp, Stoned, Shatter
            if (m_uiGroundSlamTimer < diff)
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();

                m_bPerformingGroundSlam = true;
                m_uiGroundSlamTimer     = 10000;

                DoCastMe( SPELL_GROUND_SLAM);
            }
            else
                m_uiGroundSlamTimer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_gruul(Creature* pCreature)
{
    return new boss_gruulAI(pCreature);
}

void AddSC_boss_gruul()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gruul";
    newscript->GetAI = &GetAI_boss_gruul;
    newscript->RegisterSelf();
}
