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
SDName: Boss_Saurfang
SD%Complete: 0
SDComment: Written by K
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    BLOOD_LINK          = 72178,
    FALLEN_CHAMPION     = 72293,
    FRENZY              = 72737,
    BOILING_BLOOD       = 72385,
    BLOOD_NOVA          = 72380,
    RUNE_OF_BLOOD       = 72408,
    BLOOD_BEAST         = 72173,
};

struct MANGOS_DLL_DECL boss_saurfangAI : public ScriptedAI
{
    boss_saurfangAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 BloodBeast_Timer;
    uint32 RuneOfBlood_Timer;
    uint32 BoilingBlood_Timer;
    uint32 BloodNova_Timer;

    void Reset()
    {
        BloodBeast_Timer = 40000;
        RuneOfBlood_Timer = 30000;
        BoilingBlood_Timer = 60000;
        BloodNova_Timer = 15000;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAURFANG, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAURFANG, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAURFANG, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

        if (!me->HasAura(BLOOD_LINK))
            DoCastSpellIfCan(me, BLOOD_LINK);

        if ((me->GetHealth()*100 / me->GetMaxHealth() < 30) && (!me->HasAura(FRENZY)))
            DoCastSpellIfCan(me, FRENZY);

        /*
        if (me->GetPower(me->getPowerType())*100 / me->GetMaxPower(me->getPowerType()) == 100)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCastSpellIfCan(target, FALLEN_CHAMPION);
        }
        */

        if (BloodBeast_Timer < uiDiff)
        {
            DoCastSpellIfCan(me, BLOOD_BEAST);
            BloodBeast_Timer = 40000;
        }
        else BloodBeast_Timer -= uiDiff;

        if (RuneOfBlood_Timer < uiDiff)
        {
            DoCastSpellIfCan(me->getVictim(), RUNE_OF_BLOOD);
            RuneOfBlood_Timer = 30000;
        }
        else RuneOfBlood_Timer -= uiDiff;

        if (BoilingBlood_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCastSpellIfCan(target, BOILING_BLOOD);
            BoilingBlood_Timer = 60000;
        }
        else BoilingBlood_Timer -= uiDiff;

        if (BloodNova_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCastSpellIfCan(target, BLOOD_NOVA);
            BloodNova_Timer = 15000;
        }
        else BloodNova_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_saurfang(Creature* pCreature)
{
    return new boss_saurfangAI(pCreature);
}

void AddSC_boss_saurfang()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_saurfang";
    NewScript->GetAI = &GetAI_boss_saurfang;
    NewScript->RegisterSelf();
}