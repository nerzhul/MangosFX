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
SDName: Boss_Morogrim_Tidewalker
SD%Complete: 90
SDComment:
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "precompiled.h"
#include "serpent_shrine.h"

enum
{
    SAY_AGGRO                       = -1548030,
    SAY_SUMMON1                     = -1548031,
    SAY_SUMMON2                     = -1548032,
    SAY_SUMMON_BUBL1                = -1548033,
    SAY_SUMMON_BUBL2                = -1548034,
    SAY_SLAY1                       = -1548035,
    SAY_SLAY2                       = -1548036,
    SAY_SLAY3                       = -1548037,
    SAY_DEATH                       = -1548038,
    EMOTE_WATERY_GRAVE              = -1548039,
    EMOTE_EARTHQUAKE                = -1548040,
    EMOTE_WATERY_GLOBULES           = -1548041,

    SPELL_TIDAL_WAVE                = 37730,
    SPELL_EARTHQUAKE                = 37764,

    SPELL_WATERY_GRAVE_1            = 37850,
    SPELL_WATERY_GRAVE_2            = 38023,
    SPELL_WATERY_GRAVE_3            = 38024,
    SPELL_WATERY_GRAVE_4            = 38025,

    SPELL_SUMMON_MURLOC_A6          = 39813,
    SPELL_SUMMON_MURLOC_A7          = 39814,
    SPELL_SUMMON_MURLOC_A8          = 39815,
    SPELL_SUMMON_MURLOC_A9          = 39816,
    SPELL_SUMMON_MURLOC_A10         = 39817,

    SPELL_SUMMON_MURLOC_B6          = 39818,
    SPELL_SUMMON_MURLOC_B7          = 39819,
    SPELL_SUMMON_MURLOC_B8          = 39820,
    SPELL_SUMMON_MURLOC_B9          = 39821,
    SPELL_SUMMON_MURLOC_B10         = 39822,

    SPELL_SUMMON_GLOBULE_1          = 37854,
    SPELL_SUMMON_GLOBULE_2          = 37858,
    SPELL_SUMMON_GLOBULE_3          = 37860,
    SPELL_SUMMON_GLOBULE_4          = 37861,

    NPC_WATER_GLOBULE               = 21913,
    NPC_TIDEWALKER_LURKER           = 21920
};

//Morogrim Tidewalker AI
struct MANGOS_DLL_DECL boss_morogrim_tidewalkerAI : public ScriptedAI
{
    boss_morogrim_tidewalkerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        Reset();
    }

    // timers
    uint32 m_uiTidalWave_Timer;
    uint32 m_uiWateryGrave_Timer;
    uint32 m_uiEarthquake_Timer;
    uint32 m_uiWateryGlobules_Timer;

    bool m_bEarthquake;
    bool m_bPhase2;

    void Reset()
    {
        m_uiTidalWave_Timer      = 10000;
        m_uiWateryGrave_Timer    = 30000;
        m_uiEarthquake_Timer     = 40000;
        m_uiWateryGlobules_Timer = 0;

        m_bEarthquake = false;
        m_bPhase2     = false;

        if (pInstance)
            SetInstanceData(TYPE_MOROGRIM_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);

        if (pInstance)
            SetInstanceData(TYPE_MOROGRIM_EVENT, IN_PROGRESS);
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

    void JustDied(Unit* pVictim)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            SetInstanceData(TYPE_MOROGRIM_EVENT, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_TIDEWALKER_LURKER)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }

        if (pSummoned->GetEntry() == NPC_WATER_GLOBULE)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                pSummoned->GetMotionMaster()->MoveFollow(pTarget, 0.0f, 0.0f);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //m_uiEarthquake_Timer
        if (m_uiEarthquake_Timer < diff)
        {
            if (!m_bEarthquake)
            {
                DoCastVictim( SPELL_EARTHQUAKE);
                m_bEarthquake = true;
                m_uiEarthquake_Timer = 5000;
            }
            else
            {
                DoScriptText(urand(0,1) ? SAY_SUMMON1 : SAY_SUMMON2, me);

                //north
                me->CastSpell(me,SPELL_SUMMON_MURLOC_A6,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_A7,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_A8,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_A9,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_A10,true);

                //south
                me->CastSpell(me,SPELL_SUMMON_MURLOC_B6,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_B7,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_B8,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_B9,true);
                me->CastSpell(me,SPELL_SUMMON_MURLOC_B10,true);

                DoScriptText(EMOTE_EARTHQUAKE, me);

                m_bEarthquake = false;
                m_uiEarthquake_Timer = urand(40000, 45000);
            }
        }else m_uiEarthquake_Timer -= diff;

        //m_uiTidalWave_Timer
        if (m_uiTidalWave_Timer < diff)
        {
            DoCastVictim( SPELL_TIDAL_WAVE);
            m_uiTidalWave_Timer = 20000;
        }else m_uiTidalWave_Timer -= diff;

        if (!m_bPhase2)
        {
            //m_uiWateryGrave_Timer
            if (m_uiWateryGrave_Timer < diff)
            {
                //Teleport 4 players under the waterfalls
                for(uint8 i = 0; i < 4; ++i)
                {
                    Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);

                    if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER && !pTarget->HasAuraType(SPELL_AURA_MOD_STUN) && pTarget->IsWithinDistInMap(me, 45.0f))
                    {
                        switch(i)
                        {
                            case 0: me->CastSpell(pTarget,SPELL_WATERY_GRAVE_1,false); break;
                            case 1: me->CastSpell(pTarget,SPELL_WATERY_GRAVE_2,false); break;
                            case 2: me->CastSpell(pTarget,SPELL_WATERY_GRAVE_3,false); break;
                            case 3: me->CastSpell(pTarget,SPELL_WATERY_GRAVE_4,false); break;
                        }
                    }
                }

                DoScriptText(urand(0,1) ? SAY_SUMMON_BUBL1 : SAY_SUMMON_BUBL2, me);
                DoScriptText(EMOTE_WATERY_GRAVE, me);

                m_uiWateryGrave_Timer = 30000;
            }else m_uiWateryGrave_Timer -= diff;

            //Start Phase2
            if ((me->GetHealth()*100 / me->GetMaxHealth()) < 25)
                m_bPhase2 = true;
        }
        else
        {
            //m_uiWateryGlobules_Timer
            if (m_uiWateryGlobules_Timer < diff)
            {
                DoScriptText(EMOTE_WATERY_GLOBULES, me);

                me->CastSpell(me,SPELL_SUMMON_GLOBULE_1,true);
                me->CastSpell(me,SPELL_SUMMON_GLOBULE_2,true);
                me->CastSpell(me,SPELL_SUMMON_GLOBULE_3,true);
                me->CastSpell(me,SPELL_SUMMON_GLOBULE_4,false);

                m_uiWateryGlobules_Timer = 25000;
            }else m_uiWateryGlobules_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

//Water Globule AI
struct MANGOS_DLL_DECL mob_water_globuleAI : public ScriptedAI
{
    mob_water_globuleAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    // timers
    uint32 m_uiCheck_Timer;

    void Reset()
    {
        m_uiCheck_Timer = 1000;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || me->getVictim())
            return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(me) && me->IsHostileTo(pWho))
        {
            //no attack radius check - it attacks the first target that moves in his los
            pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            AttackStart(pWho);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (m_uiCheck_Timer < diff)
        {
            if (me->IsWithinDistInMap(me->getVictim(), ATTACK_DISTANCE))
            {
                me->DealDamage(me->getVictim(), 4000+rand()%2000, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FROST, NULL, false);

                //despawn
                me->ForcedDespawn();
                return;
            }
            m_uiCheck_Timer = 500;
        }else m_uiCheck_Timer -= diff;

        //do NOT deal any melee damage to the target.
    }
};

CreatureAI* GetAI_boss_morogrim_tidewalker(Creature* pCreature)
{
    return new boss_morogrim_tidewalkerAI (pCreature);
}
CreatureAI* GetAI_mob_water_globule(Creature* pCreature)
{
    return new mob_water_globuleAI (pCreature);
}

void AddSC_boss_morogrim_tidewalker()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_morogrim_tidewalker";
    newscript->GetAI = &GetAI_boss_morogrim_tidewalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_water_globule";
    newscript->GetAI = &GetAI_mob_water_globule;
    newscript->RegisterSelf();
}
