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
SDName: Boss_Brutallus
SD%Complete: 50
SDComment: Intro not made. Script for Madrigosa to be added here.
SDCategory: Sunwell Plateau
EndScriptData */

#include "precompiled.h"
#include "sunwell_plateau.h"

enum Brutallus
{
    YELL_INTRO                      = -1580017,
    YELL_INTRO_BREAK_ICE            = -1580018,
    YELL_INTRO_CHARGE               = -1580019,
    YELL_INTRO_KILL_MADRIGOSA       = -1580020,
    YELL_INTRO_TAUNT                = -1580021,

    YELL_MADR_ICE_BARRIER           = -1580031,
    YELL_MADR_INTRO                 = -1580032,
    YELL_MADR_ICE_BLOCK             = -1580033,
    YELL_MADR_TRAP                  = -1580034,
    YELL_MADR_DEATH                 = -1580035,

    YELL_AGGRO                      = -1580022,
    YELL_KILL1                      = -1580023,
    YELL_KILL2                      = -1580024,
    YELL_KILL3                      = -1580025,
    YELL_LOVE1                      = -1580026,
    YELL_LOVE2                      = -1580027,
    YELL_LOVE3                      = -1580028,
    YELL_BERSERK                    = -1580029,
    YELL_DEATH                      = -1580030,

    SPELL_METEOR_SLASH              = 45150,
    SPELL_BURN                      = 45141,
    SPELL_BURN_AURA_EFFECT          = 46394,
    SPELL_STOMP                     = 45185,
    SPELL_BERSERK                   = 26662
};

struct MANGOS_DLL_DECL boss_brutallusAI : public ScriptedAI
{
    boss_brutallusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;

    uint32 m_uiSlashTimer;
    uint32 m_uiBurnTimer;
    uint32 m_uiStompTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiLoveTimer;

    void Reset()
    {
        m_uiSlashTimer = 11000;
        m_uiStompTimer = 30000;
        m_uiBurnTimer = 60000;
        m_uiBerserkTimer = 360000;
        m_uiLoveTimer = urand(10000, 17000);

        //TODO: correct me when pre-event implemented
        if (pInstance)
            pInstance->SetData(TYPE_BRUTALLUS, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(YELL_AGGRO, me);

        if (pInstance)
            pInstance->SetData(TYPE_BRUTALLUS, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(YELL_KILL1, me); break;
            case 1: DoScriptText(YELL_KILL2, me); break;
            case 2: DoScriptText(YELL_KILL3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(YELL_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_BRUTALLUS, DONE);
    }

    void SpellHitTarget(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_BURN)
            pCaster->CastSpell(pCaster, SPELL_BURN_AURA_EFFECT, true, NULL, NULL, me->GetGUID());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (m_uiLoveTimer < diff)
        {
            switch(urand(0, 2))
            {
                case 0: DoScriptText(YELL_LOVE1, me); break;
                case 1: DoScriptText(YELL_LOVE2, me); break;
                case 2: DoScriptText(YELL_LOVE3, me); break;
            }
            m_uiLoveTimer = urand(15000, 23000);
        }
        else
            m_uiLoveTimer -= diff;

        if (m_uiSlashTimer < diff)
        {
            DoCastVictim(SPELL_METEOR_SLASH);
            m_uiSlashTimer = 11000;
        }
        else
            m_uiSlashTimer -= diff;

        if (m_uiStompTimer < diff)
        {
            if (Unit* pTarget = me->getVictim())
            {
                DoCast(pTarget,SPELL_STOMP);

                if (pTarget->HasAura(SPELL_BURN_AURA_EFFECT,0))
                    pTarget->RemoveAurasDueToSpell(SPELL_BURN_AURA_EFFECT);
            }

            m_uiStompTimer = 30000;
        }
        else
            m_uiStompTimer -= diff;

        if (m_uiBurnTimer < diff)
        {
            //returns any unit
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                //so we get owner, in case unit was pet/totem/etc
                if (Player* pPlayer = pTarget->GetCharmerOrOwnerPlayerOrPlayerItself())
                    DoCast(pPlayer, SPELL_BURN);
            }

            m_uiBurnTimer = 60000;
        }
        else
            m_uiBurnTimer -= diff;

        if (m_uiBerserkTimer < diff)
        {
            DoScriptText(YELL_BERSERK, me);
            DoCastMe(SPELL_BERSERK);
            m_uiBerserkTimer = 20000;
        }
        else
            m_uiBerserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_brutallus(Creature* pCreature)
{
    return new boss_brutallusAI(pCreature);
}

bool AreaTrigger_at_madrigosa(Player* pPlayer, AreaTriggerEntry* pAt)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        //this simply set encounter state, and trigger ice barrier become active
        //bosses can start pre-event based on this new state
        if (pInstance->GetData(TYPE_BRUTALLUS) == NOT_STARTED)
            pInstance->SetData(TYPE_BRUTALLUS, SPECIAL);
    }

    return false;
}

void AddSC_boss_brutallus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_brutallus";
    newscript->GetAI = &GetAI_boss_brutallus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_madrigosa";
    newscript->pAreaTrigger = &AreaTrigger_at_madrigosa;
    newscript->RegisterSelf();
}
