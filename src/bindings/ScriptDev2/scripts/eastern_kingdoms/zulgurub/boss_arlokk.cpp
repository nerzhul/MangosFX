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
SDName: Boss_Arlokk
SD%Complete: 95
SDComment: Wrong cleave and red aura is missing.
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

bool GOHello_go_gong_of_bethekk(Player* pPlayer, GameObject* pGo)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_ARLOKK) == DONE || pInstance->GetData(TYPE_ARLOKK) == IN_PROGRESS)
            return true;

        pInstance->SetData(TYPE_ARLOKK, IN_PROGRESS);
    }

    return false;
}

enum
{
    SAY_AGGRO                   = -1309011,
    SAY_FEAST_PANTHER           = -1309012,
    SAY_DEATH                   = -1309013,

    SPELL_SHADOWWORDPAIN        = 23952,
    SPELL_GOUGE                 = 24698,
    SPELL_MARK                  = 24210,
    SPELL_CLEAVE                = 26350,                    //Perhaps not right. Not a red aura...
    SPELL_PANTHER_TRANSFORM     = 24190,

    MODEL_ID_NORMAL             = 15218,
    MODEL_ID_PANTHER            = 15215,
    MODEL_ID_BLANK              = 11686,

    NPC_ZULIAN_PROWLER          = 15101
};

struct MANGOS_DLL_DECL boss_arlokkAI : public ScriptedAI
{
    boss_arlokkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 m_uiShadowWordPain_Timer;
    uint32 m_uiGouge_Timer;
    uint32 m_uiMark_Timer;
    uint32 m_uiCleave_Timer;
    uint32 m_uiVanish_Timer;
    uint32 m_uiVisible_Timer;

    uint32 m_uiSummon_Timer;
    uint32 m_uiSummonCount;

    Unit* m_pMarkedTarget;

    bool m_bIsPhaseTwo;
    bool m_bIsVanished;

    void Reset()
    {
        m_uiShadowWordPain_Timer = 8000;
        m_uiGouge_Timer = 14000;
        m_uiMark_Timer = 35000;
        m_uiCleave_Timer = 4000;
        m_uiVanish_Timer = 60000;
        m_uiVisible_Timer = 6000;

        m_uiSummon_Timer = 5000;
        m_uiSummonCount = 0;

        m_bIsPhaseTwo = false;
        m_bIsVanished = false;

        m_pMarkedTarget = NULL;

        me->SetDisplayId(MODEL_ID_NORMAL);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_ARLOKK, NOT_STARTED);

        //we should be summoned, so despawn
        me->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);

        me->SetDisplayId(MODEL_ID_NORMAL);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (pInstance)
            pInstance->SetData(TYPE_ARLOKK, DONE);
    }

    void DoSummonPhanters()
    {
        if (m_pMarkedTarget)
            DoScriptText(SAY_FEAST_PANTHER, me, m_pMarkedTarget);

        me->SummonCreature(NPC_ZULIAN_PROWLER, -11532.7998, -1649.6734, 41.4800, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
        me->SummonCreature(NPC_ZULIAN_PROWLER, -11532.9970, -1606.4840, 41.2979, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (m_pMarkedTarget)
            pSummoned->AI()->AttackStart(m_pMarkedTarget);

        ++m_uiSummonCount;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (!m_bIsPhaseTwo)
        {
            if (m_uiShadowWordPain_Timer < diff)
            {
                DoCastVictim(SPELL_SHADOWWORDPAIN);
                m_uiShadowWordPain_Timer = 15000;
            }
            else
                m_uiShadowWordPain_Timer -= diff;

            if (m_uiMark_Timer < diff)
            {
                m_pMarkedTarget = SelectUnit(SELECT_TARGET_RANDOM,0);

                if (m_pMarkedTarget)
                    DoCast(m_pMarkedTarget, SPELL_MARK);
                else
                    error_log("SD2: boss_arlokk could not accuire m_pMarkedTarget.");

                m_uiMark_Timer = 15000;
            }
            else
                m_uiMark_Timer -= diff;
        }
        else
        {
            //Cleave_Timer
            if (m_uiCleave_Timer < diff)
            {
                DoCastVictim( SPELL_CLEAVE);
                m_uiCleave_Timer = 16000;
            }
            else
                m_uiCleave_Timer -= diff;

            //Gouge_Timer
            if (m_uiGouge_Timer < diff)
            {
                DoCastVictim( SPELL_GOUGE);

                if (me->getThreatManager().getThreat(me->getVictim()))
                    me->getThreatManager().modifyThreatPercent(me->getVictim(),-80);

                m_uiGouge_Timer = urand(17000, 27000);
            }
            else
                m_uiGouge_Timer -= diff;
        }

        if (m_uiSummonCount <= 30)
        {
            if (m_uiSummon_Timer < diff)
            {
                DoSummonPhanters();
                m_uiSummon_Timer = 5000;
            }
            else
                m_uiSummon_Timer -= diff;
        }

        if (m_uiVanish_Timer < diff)
        {
            //Invisble Model
            me->SetDisplayId(MODEL_ID_BLANK);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            me->AttackStop();
            DoResetThreat();

            m_bIsVanished = true;

            m_uiVanish_Timer = 45000;
            m_uiVisible_Timer = 6000;
        }
        else
            m_uiVanish_Timer -= diff;

        if (m_bIsVanished)
        {
            if (m_uiVisible_Timer < diff)
            {
                //The Panther Model
                me->SetDisplayId(MODEL_ID_PANTHER);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                const CreatureInfo *cinfo = me->GetCreatureInfo();
                me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 35)));
                me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 35)));
                me->UpdateDamagePhysical(BASE_ATTACK);

                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    AttackStart(pTarget);

                m_bIsPhaseTwo = true;
                m_bIsVanished = false;
            }
            else
                m_uiVisible_Timer -= diff;
        }
        else
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_arlokk(Creature* pCreature)
{
    return new boss_arlokkAI(pCreature);
}

void AddSC_boss_arlokk()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_gong_of_bethekk";
    newscript->pGOHello = &GOHello_go_gong_of_bethekk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_arlokk";
    newscript->GetAI = &GetAI_boss_arlokk;
    newscript->RegisterSelf();
}
