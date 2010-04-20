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
SDName: Boss_Deathwhisper
SD%Complete: 0
SDComment: Written by K, with some ideas from salja
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

#define ADD_1X -619.006
#define ADD_1Y 2158.104
#define ADD_1Z 50.848

#define ADD_2X -598.697
#define ADD_2Y 2157.767
#define ADD_2Z 50.848

#define ADD_3X -577.992
#define ADD_3Y 2156.989
#define ADD_3Z 50.848

#define ADD_4X -618.748
#define ADD_4Y 2266.648
#define ADD_4Z 50.849

#define ADD_5X -598.573
#define ADD_5Y 2266.870
#define ADD_5Z 50.849

#define ADD_6X -578.360
#define ADD_6Y 2267.210
#define ADD_6Z 50.849

enum
{
    SPELL_MANA_BARRIER         = 70842,
    SPELL_SHADOW_BOLT          = 71254,
    H_SPELL_SHADOW_BOLT        = 72008,
    SPELL_DEATH_AND_DECAY      = 71001,
    H_SPELL_DEATH_AND_DECAY    = 72108,
    //SPELL_DARK_EMPOWERMENT     = 70901,
    SPELL_FROSTBOLT            = 71420,
    H_SPELL_FROSTBOLT          = 72007,
    SPELL_INSIGNIFICANCE       = 71204,
    SPELL_FROSTBOLT_VOLLEY     = 72905,
    H_SPELL_FROSTBOLT_VOLLEY   = 72906,
    SPELL_VENGEFUL_SHADE       = 71426,

    NPC_CULT_ADHERENT          = 37949,
    NPC_CULT_FANATIC           = 37890,
    NPC_VENGEFUL_SHADE         = 38222,
};

struct MANGOS_DLL_DECL boss_deathwhisperAI : public ScriptedAI
{
    boss_deathwhisperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 ShadowBolt_Timer;
    uint32 DeathAndDecay_Timer;
    uint32 Frostbolt_Timer;
    uint32 FrostboltVolley_Timer;
    uint32 Insignificance_Timer;
    uint32 Summon_Cult_Timer;
    uint32 Summon_Shade_Timer;
    uint8 Phase;
    uint8 Summon_Cult_Count;
    uint8 Summon_Shade_Count;
    bool SpawnLeft;

    void Reset()
    {
        ShadowBolt_Timer = 5000;
        DeathAndDecay_Timer = 30000;
        Summon_Cult_Timer = 20000;
        Frostbolt_Timer = 15000;
        FrostboltVolley_Timer = 40000;
        Insignificance_Timer = 5000+rand()%40000;
        Summon_Shade_Timer = 25000;
        Phase = 1;
        Summon_Cult_Count = 0;
        Summon_Shade_Count = 0;
        SpawnLeft = true;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHWHISPER, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHWHISPER, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DEATHWHISPER, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    //Mana Barrier is bugged so we override it for now
    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (me->HasAura(SPELL_MANA_BARRIER))
        {
            me->SetHealth(me->GetHealth()+damage);
            me->SetPower(POWER_MANA,me->GetPower(POWER_MANA)-damage);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

        if (Phase == 1)
        {
            if ((me->GetPower(POWER_MANA)*100 / me->GetMaxPower(POWER_MANA)) < 1)
            {
                Phase = 2;
                return;
            }

            if (!me->HasAura(SPELL_MANA_BARRIER))
                 DoCastSpellIfCan(me, SPELL_MANA_BARRIER);

            if (Summon_Cult_Timer < uiDiff)
            {
                if (SpawnLeft)
                {
                    me->SummonCreature(NPC_CULT_FANATIC,ADD_1X,ADD_1Y,ADD_1Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    me->SummonCreature(NPC_CULT_ADHERENT,ADD_2X,ADD_2Y,ADD_2Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    me->SummonCreature(NPC_CULT_FANATIC,ADD_3X,ADD_3Y,ADD_3Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    SpawnLeft = false;
                }
                else
                {
                    me->SummonCreature(NPC_CULT_ADHERENT,ADD_4X,ADD_4Y,ADD_4Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    me->SummonCreature(NPC_CULT_FANATIC,ADD_5X,ADD_5Y,ADD_5Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    me->SummonCreature(NPC_CULT_ADHERENT,ADD_6X,ADD_6Y,ADD_6Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    SpawnLeft = true;
                }

                Summon_Cult_Count++;
                Summon_Cult_Timer = 60000;
            }
            else Summon_Cult_Timer -= uiDiff;

            if (ShadowBolt_Timer < uiDiff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                     DoCastSpellIfCan(target, m_bIsRegularMode ? SPELL_SHADOW_BOLT : H_SPELL_SHADOW_BOLT);
                ShadowBolt_Timer = 5000;
            }
            else ShadowBolt_Timer -= uiDiff;

            DoStartNoMovement(me->getVictim());
        }

        if (Phase == 2)
        {
            if (me->HasAura(SPELL_MANA_BARRIER))
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);

            if (Insignificance_Timer < uiDiff)
            {
                 DoCastSpellIfCan(me->getVictim(), SPELL_INSIGNIFICANCE);
                Insignificance_Timer = 5000+rand()%40000;
            }
            else Insignificance_Timer -= uiDiff;

            if (FrostboltVolley_Timer < uiDiff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                     DoCastSpellIfCan(target, m_bIsRegularMode ? SPELL_FROSTBOLT_VOLLEY : H_SPELL_FROSTBOLT_VOLLEY);
                FrostboltVolley_Timer = 40000;
            }
            else FrostboltVolley_Timer -= uiDiff;

            if (Summon_Shade_Count < Summon_Cult_Count)
            {
                if (Summon_Shade_Timer < uiDiff)
                {
                     DoCastSpellIfCan(me, SPELL_VENGEFUL_SHADE);
                    Summon_Shade_Count++;
                    Summon_Shade_Timer = 25000;
                }
                else Summon_Shade_Timer -= uiDiff;
            }

            if (Frostbolt_Timer < uiDiff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                     DoCastSpellIfCan(target, m_bIsRegularMode ? SPELL_FROSTBOLT : H_SPELL_FROSTBOLT);
                Frostbolt_Timer = 15000;
            }
            else Frostbolt_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        if (DeathAndDecay_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                 DoCastSpellIfCan(target, m_bIsRegularMode ? SPELL_DEATH_AND_DECAY : H_SPELL_DEATH_AND_DECAY);
            DeathAndDecay_Timer = 30000;
        }
        else DeathAndDecay_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_deathwhisper(Creature* pCreature)
{
    return new boss_deathwhisperAI(pCreature);
}

void AddSC_boss_deathwhisper()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_deathwhisper";
    NewScript->GetAI = &GetAI_boss_deathwhisper;
    NewScript->RegisterSelf();
}