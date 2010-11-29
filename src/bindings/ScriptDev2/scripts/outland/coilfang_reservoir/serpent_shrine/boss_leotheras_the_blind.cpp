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
SDName: Boss_Leotheras_The_Blind
SD%Complete: 50
SDComment: Missing Inner Demons
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "precompiled.h"
#include "serpent_shrine.h"

enum
{
    SAY_AGGRO               = -1548009,
    SAY_SWITCH_TO_DEMON     = -1548010,
    SAY_INNER_DEMONS        = -1548011,
    SAY_DEMON_SLAY1         = -1548012,
    SAY_DEMON_SLAY2         = -1548013,
    SAY_DEMON_SLAY3         = -1548014,
    SAY_NIGHTELF_SLAY1      = -1548015,
    SAY_NIGHTELF_SLAY2      = -1548016,
    SAY_NIGHTELF_SLAY3      = -1548017,
    SAY_FINAL_FORM          = -1548018,
    SAY_FREE                = -1548019,
    SAY_DEATH               = -1548020,

    SPELL_ENRAGE            = 26662,

    SPELL_WHIRLWIND         = 37640,
    SPELL_CHAOS_BLAST       = 37674,
    SPELL_INSIDIOUS_WHISPER = 37676,                        //not implemented yet. After cast (spellHit), do the inner demon
    SPELL_CONS_MADNESS      = 37749,

    SPELL_DEMON_ALIGNMENT   = 37713,                        //inner demon have this aura
    SPELL_SHADOW_BOLT       = 39309,                        //inner demon spell spam

    FACTION_DEMON_1         = 1829,
    FACTION_DEMON_2         = 1830,
    FACTION_DEMON_3         = 1831,
    FACTION_DEMON_4         = 1832,
    FACTION_DEMON_5         = 1833,

    MODEL_NIGHTELF          = 20514,
    MODEL_DEMON             = 20125,

    NPC_INNER_DEMON         = 21857,
    NPC_SHADOW_LEO          = 21875
};

//Original Leotheras the Blind AI
struct MANGOS_DLL_DECL boss_leotheras_the_blindAI : public ScriptedAI
{
    boss_leotheras_the_blindAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        m_uiShadowLeo = 0;
        Reset();
    }

    // timers
    uint32 m_uiWhirlwind_Timer;
    uint32 m_uiInnerDemon_Timer;
    uint32 m_uiSwitch_Timer;
    uint32 m_uiEnrage_Timer;

    bool m_bDemonForm;
    bool m_bIsFinalForm;

    uint64 m_uiShadowLeo;

    void Reset()
    {
        m_uiWhirlwind_Timer  = 18500;
        m_uiInnerDemon_Timer = 15000;
        m_uiSwitch_Timer     = 45000;
        m_uiEnrage_Timer     = MINUTE*10*IN_MILLISECONDS;

        m_bDemonForm   = false;
        m_bIsFinalForm = false;

        if (me->GetDisplayId() != MODEL_NIGHTELF)
            me->SetDisplayId(MODEL_NIGHTELF);

        if (pInstance)
            SetInstanceData(TYPE_LEOTHERAS_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);

        if (pInstance)
            SetInstanceData(TYPE_LEOTHERAS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch(urand(0, 2))
        {
            case 0: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY1 : SAY_NIGHTELF_SLAY1, me); break;
            case 1: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY2 : SAY_NIGHTELF_SLAY2, me); break;
            case 2: DoScriptText(m_bDemonForm ? SAY_DEMON_SLAY3 : SAY_NIGHTELF_SLAY3, me); break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (me->getVictim() && pSummoned->GetEntry() == NPC_SHADOW_LEO)
        {
            m_uiShadowLeo = pSummoned->GetGUID();
            pSummoned->AI()->AttackStart(me->getVictim());
        }
    }

    void JustDied(Unit* pVictim)
    {
        DoScriptText(SAY_DEATH, me);

        //despawn copy
        if (m_uiShadowLeo)
        {
            if (Creature* pShadowLeo = (Creature*)Unit::GetUnit((*me), m_uiShadowLeo))
                pShadowLeo->ForcedDespawn();
        }

        if (pInstance)
            SetInstanceData(TYPE_LEOTHERAS_EVENT, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (!m_bDemonForm)
        {
            //Whirlwind_Timer
            if (m_uiWhirlwind_Timer < diff)
            {
                DoCastMe( SPELL_WHIRLWIND);
                m_uiWhirlwind_Timer = 30000;
            }else m_uiWhirlwind_Timer -= diff;

            //Switch_Timer
            if (!m_bIsFinalForm)
            {
                if (m_uiSwitch_Timer < diff)
                {
                    DoScriptText(SAY_SWITCH_TO_DEMON, me);

                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                    {
                        //set false, so MoveChase is not triggered in AttackStart
                        SetCombatMovement(false);

                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MoveIdle();
                        me->StopMoving();
                    }

                    //switch to demon form
                    me->SetDisplayId(MODEL_DEMON);
                    DoResetThreat();
                    m_bDemonForm = true;

                    m_uiInnerDemon_Timer = 15000;
                    m_uiSwitch_Timer = 60000;
                }else m_uiSwitch_Timer -= diff;
            }
        }
        else
        {
            //inner demon
            if (m_uiInnerDemon_Timer < diff)
            {
                DoScriptText(SAY_INNER_DEMONS, me);

                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

                DoCastMe( SPELL_INSIDIOUS_WHISPER);

                m_uiInnerDemon_Timer = 60000;
            }else m_uiInnerDemon_Timer -= diff;

            //chaos blast spam
            if (!me->IsNonMeleeSpellCasted(false))
                me->CastSpell(me->getVictim(), SPELL_CHAOS_BLAST, false);

            //Switch_Timer
            if (m_uiSwitch_Timer < diff)
            {
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

                //switch to nightelf form
                me->SetDisplayId(MODEL_NIGHTELF);

                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    me->GetMotionMaster()->MoveChase(me->getVictim());

                //set true
                SetCombatMovement(true);

                DoResetThreat();
                m_bDemonForm = false;

                m_uiWhirlwind_Timer = 18500;
                m_uiSwitch_Timer = 45000;
            }else m_uiSwitch_Timer -= diff;
        }

        if (!m_bIsFinalForm && (me->GetHealth()*100 / me->GetMaxHealth()) < 15)
        {
            DoScriptText(SAY_FINAL_FORM, me);

            //at this point he divides himself in two parts
            me->SummonCreature(NPC_SHADOW_LEO, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);

            if (m_bDemonForm)
            {
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

                //switch to nightelf form
                me->SetDisplayId(MODEL_NIGHTELF);

                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    me->GetMotionMaster()->MoveChase(me->getVictim());

                //set true
                SetCombatMovement(true);

                DoResetThreat();
                m_bDemonForm = false;
            }

            m_bIsFinalForm = true;
        }

        //m_uiEnrage_Timer
        if (m_uiEnrage_Timer < diff)
        {
            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            DoCastMe( SPELL_ENRAGE);
            m_uiEnrage_Timer = MINUTE*5*IN_MILLISECONDS;
        }else m_uiEnrage_Timer -= diff;

        if (!m_bDemonForm)
            DoMeleeAttackIfReady();
    }
};

//Leotheras the Blind Demon Form AI
struct MANGOS_DLL_DECL boss_leotheras_the_blind_demonformAI : public ScriptedAI
{
    boss_leotheras_the_blind_demonformAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset() { }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_FREE, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_DEMON_SLAY1, me); break;
            case 1: DoScriptText(SAY_DEMON_SLAY2, me); break;
            case 2: DoScriptText(SAY_DEMON_SLAY3, me); break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (!me->IsNonMeleeSpellCasted(false))
            me->CastSpell(me->getVictim(), SPELL_CHAOS_BLAST, false);

        //Do NOT deal any melee damage to the target.
    }
};

CreatureAI* GetAI_boss_leotheras_the_blind(Creature* pCreature)
{
    return new boss_leotheras_the_blindAI(pCreature);
}

CreatureAI* GetAI_boss_leotheras_the_blind_demonform(Creature* pCreature)
{
    return new boss_leotheras_the_blind_demonformAI(pCreature);
}

void AddSC_boss_leotheras_the_blind()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_leotheras_the_blind";
    newscript->GetAI = &GetAI_boss_leotheras_the_blind;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_leotheras_the_blind_demonform";
    newscript->GetAI = &GetAI_boss_leotheras_the_blind_demonform;
    newscript->RegisterSelf();
}
