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
SDName: Boss_Mograine_And_Whitemane
SD%Complete: 90
SDComment:
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"
#include "scarlet_monastery.h"

enum
{
    //Mograine says
    SAY_MO_AGGRO                 = -1189005,
    SAY_MO_KILL                  = -1189006,
    SAY_MO_RESSURECTED           = -1189007,

    //Whitemane says
    SAY_WH_INTRO                 = -1189008,
    SAY_WH_KILL                  = -1189009,
    SAY_WH_RESSURECT             = -1189010,

    //Mograine Spells
    SPELL_CRUSADERSTRIKE         = 14518,
    SPELL_HAMMEROFJUSTICE        = 5589,
    SPELL_LAYONHANDS             = 9257,
    SPELL_RETRIBUTIONAURA        = 8990,

    //Whitemanes Spells
    SPELL_DEEPSLEEP              = 9256,
    SPELL_SCARLETRESURRECTION    = 9232,
    SPELL_DOMINATEMIND           = 14515,
    SPELL_HOLYSMITE              = 9481,
    SPELL_HEAL                   = 12039,
    SPELL_POWERWORDSHIELD        = 22187
};

struct MANGOS_DLL_DECL boss_scarlet_commander_mograineAI : public ScriptedAI
{
    boss_scarlet_commander_mograineAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiCrusaderStrike_Timer;
    uint32 m_uiHammerOfJustice_Timer;

    bool m_bHasDied;
    bool m_bHeal;
    bool m_bFakeDeath;

    void Reset()
    {
        m_uiCrusaderStrike_Timer = 10000;
        m_uiHammerOfJustice_Timer = 10000;

        //Incase wipe during phase that mograine fake death
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetStandState(UNIT_STAND_STATE_STAND);

        m_bHasDied = false;
        m_bHeal = false;
        m_bFakeDeath = false;

		if (!m_pInstance)
            return;

        if (Creature* pWhitemane = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_WHITEMANE)))
        {
            if (me->isAlive() && !pWhitemane->isAlive())
                pWhitemane->Respawn();
        }

    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_MO_AGGRO, me);
        DoCastMe(SPELL_RETRIBUTIONAURA);

        me->CallForHelp(VISIBLE_RANGE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_MO_KILL, me);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage < me->GetHealth() || m_bHasDied)
            return;

        if (!m_pInstance)
            return;

        //On first death, fake death and open door, as well as initiate whitemane if exist
        if (Creature* pWhitemane = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_WHITEMANE)))
        {
            m_pInstance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, IN_PROGRESS);

            pWhitemane->GetMotionMaster()->MovePoint(1,1163.113370,1398.856812,32.527786);

            me->GetMotionMaster()->MovementExpired();
            me->GetMotionMaster()->MoveIdle();

            me->SetHealth(0);

            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            me->ClearComboPointHolders();
            me->RemoveAllAuras();
            me->ClearAllReactives();

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetStandState(UNIT_STAND_STATE_DEAD);

            m_bHasDied = true;
            m_bFakeDeath = true;

            uiDamage = 0;
        }
    }

    void SpellHit(Unit* pWho, const SpellEntry* pSpell)
    {
        //When hit with ressurection say text
        if (pSpell->Id == SPELL_SCARLETRESURRECTION)
        {
            DoScriptText(SAY_MO_RESSURECTED, me);
            m_bFakeDeath = false;

            if (m_pInstance)
                m_pInstance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, SPECIAL);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        if (m_bHasDied && !m_bHeal && m_pInstance && m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == SPECIAL)
        {
            //On ressurection, stop fake death and heal whitemane and resume fight
            if (Creature* pWhitemane = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_WHITEMANE)))
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(pWhitemane, SPELL_LAYONHANDS);

                m_uiCrusaderStrike_Timer = 10000;
                m_uiHammerOfJustice_Timer = 10000;

                if (me->getVictim())
                    me->GetMotionMaster()->MoveChase(me->getVictim());

                m_bHeal = true;
            }
        }

        //This if-check to make sure mograine does not attack while fake death
        if (m_bFakeDeath)
            return;

        //m_uiCrusaderStrike_Timer
        if (m_uiCrusaderStrike_Timer < uiDiff)
        {
            DoCastVictim(SPELL_CRUSADERSTRIKE);
            m_uiCrusaderStrike_Timer = 10000;
        }else m_uiCrusaderStrike_Timer -= uiDiff;

        //m_uiHammerOfJustice_Timer
        if (m_uiHammerOfJustice_Timer < uiDiff)
        {
            DoCastVictim(SPELL_HAMMEROFJUSTICE);
            m_uiHammerOfJustice_Timer = 60000;
        }else m_uiHammerOfJustice_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_high_inquisitor_whitemaneAI : public ScriptedAI
{
    boss_high_inquisitor_whitemaneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiHeal_Timer;
    uint32 m_uiPowerWordShield_Timer;
    uint32 m_uiHolySmite_Timer;
    uint32 m_uiWait_Timer;

    bool m_bCanResurrectCheck;
    bool m_bCanResurrect;

    void Reset()
    {
        m_uiWait_Timer = 7000;
        m_uiHeal_Timer = 10000;
        m_uiPowerWordShield_Timer = 15000;
        m_uiHolySmite_Timer = 6000;

        m_bCanResurrectCheck = false;
        m_bCanResurrect = false;

        if (!m_pInstance)
            return;

        if (Creature* pMograine = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_MOGRAINE)))
        {
            if (me->isAlive() && !pMograine->isAlive())
                pMograine->Respawn();
        }

    }


    void JustReachedHome()
    {
        
        if (m_pInstance)
        {
            if (!(m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == NOT_STARTED) || !(m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == FAIL))
                m_pInstance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, FAIL);
        }
    }

    void MoveInLineOfSight()
    {
        //This needs to be empty because Whitemane should NOT aggro while fighting Mograine. Mograine will give us a target.
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage < me->GetHealth())
            return;

        if(!m_bCanResurrectCheck || m_bCanResurrect)
        {
            // prevent killing blow before rezzing commander
            me->SetHealth(uiDamage+1);
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (m_pInstance && (m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == NOT_STARTED || m_pInstance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == FAIL))
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_WH_INTRO, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_WH_KILL, me);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        if (m_bCanResurrect)
        {
            //When casting resuruction make sure to delay so on rez when reinstate battle deepsleep runs out
            if (m_pInstance && m_uiWait_Timer < uiDiff)
            {
                if (Creature* pMograine = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_MOGRAINE)))
                {
                    DoCast(pMograine, SPELL_SCARLETRESURRECTION);
                    DoScriptText(SAY_WH_RESSURECT, me);
                    m_bCanResurrect = false;
                }
            }
            else m_uiWait_Timer -= uiDiff;
        }

        //Cast Deep sleep when health is less than 50%
        if (!m_bCanResurrectCheck && me->GetHealth()*100 / me->GetMaxHealth() <= 50)
        {
            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            DoCastVictim( SPELL_DEEPSLEEP);
            m_bCanResurrectCheck = true;
            m_bCanResurrect = true;
            return;
        }

        //while in "resurrect-mode", don't do anything
        if (m_bCanResurrect)
            return;

        //If we are <75% hp cast healing spells at self or Mograine
        if (m_uiHeal_Timer < uiDiff)
        {
            Creature* pTarget = NULL;

            if (me->GetHealth() <= me->GetMaxHealth()*0.75f)
                pTarget = me;

            if (m_pInstance)
            {
                if (Creature* pMograine = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_MOGRAINE)))
                {
                    if (pMograine->isAlive() && pMograine->GetHealth() <= pMograine->GetMaxHealth()*0.75f)
                        pTarget = pMograine;
                }
            }

            if (pTarget)
                DoCast(pTarget, SPELL_HEAL);

            m_uiHeal_Timer = 13000;
        }else m_uiHeal_Timer -= uiDiff;

        //m_uiPowerWordShield_Timer
        if (m_uiPowerWordShield_Timer < uiDiff)
        {
            DoCastMe(SPELL_POWERWORDSHIELD);
            m_uiPowerWordShield_Timer = 15000;
        }else m_uiPowerWordShield_Timer -= uiDiff;

        //m_uiHolySmite_Timer
        if (m_uiHolySmite_Timer < uiDiff)
        {
            DoCastVictim(SPELL_HOLYSMITE);
            m_uiHolySmite_Timer = 6000;
        }else m_uiHolySmite_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_scarlet_commander_mograine(Creature* pCreature)
{
    return new boss_scarlet_commander_mograineAI(pCreature);
}

CreatureAI* GetAI_boss_high_inquisitor_whitemane(Creature* pCreature)
{
    return new boss_high_inquisitor_whitemaneAI(pCreature);
}

void AddSC_boss_mograine_and_whitemane()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_scarlet_commander_mograine";
    newscript->GetAI = &GetAI_boss_scarlet_commander_mograine;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_high_inquisitor_whitemane";
    newscript->GetAI = &GetAI_boss_high_inquisitor_whitemane;
    newscript->RegisterSelf();
}
