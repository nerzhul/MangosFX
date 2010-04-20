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
SDName: Boss_Marrowgar
SD%Complete: 0
SDComment: Written by K
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SPELL_SABER_LASH     = 69055,
    H_SPELL_SABER_LASH   = 70814,
    SPELL_COLDFLAME      = 69146,
    H_SPELL_COLDFLAME    = 70824,
    SPELL_BONE_SPIKE     = 69057,
    H_SPELL_BONE_SPIKE   = 72088,
    SPELL_BONE_STORM     = 69076,
    H_SPELL_BONE_STORM   = 70835
};

struct MANGOS_DLL_DECL boss_marrowgarAI : public ScriptedAI
{
    boss_marrowgarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 SaberLash_Timer;
    uint32 ColdFlame_Timer;
    uint32 BoneSpike_Timer;
    uint32 BoneStorm_Timer;

    void Reset()
    {
        SaberLash_Timer = 1000;
        ColdFlame_Timer = 15000;
        BoneSpike_Timer = 30000;
        BoneStorm_Timer = 45000;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MARROWGAR, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MARROWGAR, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MARROWGAR, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

        if (BoneStorm_Timer < uiDiff)
        {
            DoCastSpellIfCan(me->getVictim(), m_bIsRegularMode ? SPELL_BONE_STORM : H_SPELL_BONE_STORM);
            BoneStorm_Timer = 90000;
        }
        else BoneStorm_Timer -= uiDiff;

        if (BoneSpike_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCastSpellIfCan(target, m_bIsRegularMode ? SPELL_BONE_SPIKE : H_SPELL_BONE_SPIKE);
            SaberLash_Timer = 4000;
            BoneSpike_Timer = 30000;
        }
        else BoneSpike_Timer -= uiDiff;

        if (ColdFlame_Timer < uiDiff)
        {
            DoCastSpellIfCan(me->getVictim(), m_bIsRegularMode ? SPELL_COLDFLAME : H_SPELL_COLDFLAME);
            ColdFlame_Timer = 15000;
        }
        else ColdFlame_Timer -= uiDiff;

        if (SaberLash_Timer < uiDiff)
        {
            DoCastSpellIfCan(me->getVictim(), m_bIsRegularMode ? SPELL_SABER_LASH : H_SPELL_SABER_LASH);
            SaberLash_Timer = 1000;
        }
        else SaberLash_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_marrowgar(Creature* pCreature)
{
    return new boss_marrowgarAI(pCreature);
}

void AddSC_boss_marrowgar()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_marrowgar";
    NewScript->GetAI = &GetAI_boss_marrowgar;
    NewScript->RegisterSelf();
}