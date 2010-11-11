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
SDName: Boss_Terestian_Illhoof
SD%Complete: 95
SDComment: Complete! Needs adjustments to use spell though.
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"

enum
{
    SAY_SLAY1                   = -1532065,
    SAY_SLAY2                   = -1532066,
    SAY_DEATH                   = -1532067,
    SAY_AGGRO                   = -1532068,
    SAY_SACRIFICE1              = -1532069,
    SAY_SACRIFICE2              = -1532070,
    SAY_SUMMON1                 = -1532071,
    SAY_SUMMON2                 = -1532072,

    SPELL_SUMMON_DEMONCHAINS    = 30120,                    // Summons demonic chains that maintain the ritual of sacrifice.
    SPELL_DEMON_CHAINS          = 30206,                    // Instant - Visual Effect
    SPELL_ENRAGE                = 23537,                    // Increases the caster's attack speed by 50% and the Physical damage it deals by 219 to 281 for 10 min.
    SPELL_SHADOW_BOLT           = 30055,                    // Hurls a bolt of dark magic at an enemy, inflicting Shadow damage.
    SPELL_SACRIFICE             = 30115,                    // Teleports and adds the debuff
    SPELL_BERSERK               = 32965,                    // Increases attack speed by 75%. Periodically casts Shadow Bolt Volley.

    SPELL_SUMMON_IMP            = 30066,                    // Summons Kil'rek

    SPELL_FIENDISH_PORTAL       = 30171,                    // Opens portal and summons Fiendish Portal, 2 sec cast
    SPELL_FIENDISH_PORTAL_1     = 30179,                    // Opens portal and summons Fiendish Portal, instant cast

    SPELL_FIREBOLT              = 30050,                    // Blasts a target for 150 Fire damage.

    SPELL_BROKEN_PACT           = 30065,                    // All damage taken increased by 25%.
    SPELL_AMPLIFY_FLAMES        = 30053,                    // Increases the Fire damage taken by an enemy by 500 for 25 sec.

    NPC_DEMONCHAINS             = 17248,
    NPC_FIENDISHIMP             = 17267,
    NPC_PORTAL                  = 17265
};

const float PORTAL_Z = 179.434f;

float afPortalLocations[2][2]=
{
    {-11249.6933f, -1704.61023f},
    {-11242.1160f, -1713.33325f}
};

struct MANGOS_DLL_DECL mob_kilrekAI : public ScriptedAI
{
    mob_kilrekAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint64 m_uiTerestianGUID;
    uint32 m_uiAmplify_Timer;

    void Reset()
    {
        m_uiTerestianGUID = 0;
        m_uiAmplify_Timer = 2000;
    }

    void Aggro(Unit* pWho)
    {
        if (!pInstance)
        {
            ERROR_INST_DATA(me);
            return;
        }

        Creature* pTerestian = ((Creature*)Unit::GetUnit(*me, pInstance->GetData64(DATA_TERESTIAN)));
        if (pTerestian && (!pTerestian->SelectHostileTarget() && !pTerestian->getVictim()))
            pTerestian->AddThreat(pWho, 1.0f);
    }

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
        {
            uint64 m_uiTerestianGUID = pInstance->GetData64(DATA_TERESTIAN);
            if (m_uiTerestianGUID)
            {
                Unit* pTerestian = Unit::GetUnit((*me), m_uiTerestianGUID);
                if (pTerestian && pTerestian->isAlive())
                    DoCast(pTerestian, SPELL_BROKEN_PACT, true);
            }
        }
        else
            ERROR_INST_DATA(me);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (m_uiAmplify_Timer < diff)
        {
            me->InterruptNonMeleeSpells(false);
            DoCastVictim( SPELL_AMPLIFY_FLAMES);

            m_uiAmplify_Timer = urand(10000, 20000);
        }
        else
            m_uiAmplify_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_demon_chainAI : public ScriptedAI
{
    mob_demon_chainAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint64 m_uiSacrificeGUID;

    void Reset()
    {
        m_uiSacrificeGUID = 0;
    }

    void AttackStart(Unit* pWho) {}
    void MoveInLineOfSight(Unit* pWho) {}

    void JustDied(Unit* pKiller)
    {
        if (m_uiSacrificeGUID)
            if (Unit* pSacrifice = Unit::GetUnit((*me), m_uiSacrificeGUID))
                pSacrifice->RemoveAurasDueToSpell(SPELL_SACRIFICE);
    }
};

struct MANGOS_DLL_DECL boss_terestianAI : public ScriptedAI
{
    boss_terestianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        memset(&m_uiPortalGUID, 0, sizeof(m_uiPortalGUID));
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint64 m_uiKilrekGUID;
    uint64 m_uiPortalGUID[2];

    uint32 m_uiCheckKilrek_Timer;
    uint32 m_uiSacrifice_Timer;
    uint32 m_uiShadowbolt_Timer;
    uint32 m_uiSummon_Timer;
    uint32 m_uiBerserk_Timer;

    bool m_bReSummon;
    bool m_bSummonKilrek;
    bool m_bSummonedPortals;
    bool m_bBerserk;

    void Reset()
    {
        for(uint8 i = 0; i < 2; ++i)
        {
            if (m_uiPortalGUID[i])
            {
                if (Creature* pPortal = (Creature*)Unit::GetUnit(*me, m_uiPortalGUID[i]))
                    pPortal->ForcedDespawn();

                m_uiPortalGUID[i] = 0;
            }
        }

        m_uiCheckKilrek_Timer   = 5000;
        m_uiSacrifice_Timer     = 30000;
        m_uiShadowbolt_Timer    = 5000;
        m_uiSummon_Timer        = 10000;
        m_uiBerserk_Timer       = 600000;

        m_bSummonedPortals      = false;
        m_bBerserk              = false;
        m_bReSummon             = false;

        if (pInstance)
            pInstance->SetData(TYPE_TERESTIAN, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);

        if (pInstance)
        {
            Creature* pKilrek = ((Creature*)Unit::GetUnit(*me, pInstance->GetData64(DATA_KILREK)));

            // Respawn Kil'rek on aggro if Kil'rek is dead.
            if (pKilrek && !pKilrek->isAlive())
                pKilrek->Respawn();

            // Put Kil'rek in combat against our target so players don't skip him
            if (pKilrek && !pKilrek->getVictim())
                pKilrek->AddThreat(pWho, 0.0f);

            pInstance->SetData(TYPE_TERESTIAN, IN_PROGRESS);
        }
        else
            ERROR_INST_DATA(me);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, me);
    }

    void JustDied(Unit* pKiller)
    {
        for(uint8 i = 0; i < 2; ++i)
        {
            if (m_uiPortalGUID[i])
            {
                if (Creature* pPortal = (Creature*)Unit::GetUnit(*me, m_uiPortalGUID[i]))
                    pPortal->ForcedDespawn();

                m_uiPortalGUID[i] = 0;
            }
        }

        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_TERESTIAN, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (m_uiCheckKilrek_Timer < diff)
        {

            m_uiCheckKilrek_Timer = 5000;

            if (pInstance)
                uint64 m_uiKilrekGUID = pInstance->GetData64(DATA_KILREK);
            else
                ERROR_INST_DATA(me);

            Creature* pKilrek = ((Creature*)Unit::GetUnit(*me, pInstance->GetData64(DATA_KILREK)));
            if (m_bSummonKilrek && pKilrek)
            {
                pKilrek->Respawn();
                pKilrek->AI()->AttackStart(me->getVictim());
                me->RemoveAurasDueToSpell(SPELL_BROKEN_PACT);

                m_bSummonKilrek = false;
            }

            if (!pKilrek || !pKilrek->isAlive())
            {
                m_bSummonKilrek = true;
                m_uiCheckKilrek_Timer = 45000;
            }
        }
        else
            m_uiCheckKilrek_Timer -= diff;

        if (m_uiSacrifice_Timer < diff)
        {
            Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);
            if (pTarget && pTarget->isAlive() && pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                DoCast(pTarget, SPELL_SACRIFICE, true);

                Creature* pChains = me->SummonCreature(NPC_DEMONCHAINS, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 21000);
                if (pChains)
                {
                    ((mob_demon_chainAI*)pChains->AI())->m_uiSacrificeGUID = pTarget->GetGUID();
                    pChains->CastSpell(pChains, SPELL_DEMON_CHAINS, true);

                    DoScriptText(urand(0, 1) ? SAY_SACRIFICE1 : SAY_SACRIFICE2, me);

                    m_uiSacrifice_Timer = 30000;
                }
            }
        }
        else
            m_uiSacrifice_Timer -= diff;

        if (m_uiShadowbolt_Timer < diff)
        {
            DoCastVictim(SPELL_SHADOW_BOLT);

            m_uiShadowbolt_Timer = 10000;
        }
        else
            m_uiShadowbolt_Timer -= diff;

        if (m_uiSummon_Timer < diff)
        {
            if (!m_bSummonedPortals)
            {
                for(uint8 i = 0; i < 2; ++i)
                {
                    if (Creature* pPortal = me->SummonCreature(NPC_PORTAL, afPortalLocations[i][0], afPortalLocations[i][1], PORTAL_Z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                        m_uiPortalGUID[i] = pPortal->GetGUID();
                }
                m_bSummonedPortals = true;

                DoScriptText(urand(0, 1) ? SAY_SUMMON1 : SAY_SUMMON2, me);
            }
            uint32 uiRnd = urand(0, 1);
            Creature* pImp = me->SummonCreature(NPC_FIENDISHIMP, afPortalLocations[uiRnd][0], afPortalLocations[uiRnd][1], PORTAL_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 15000);
            if (pImp)
            {
                pImp->AddThreat(me->getVictim(), 1.0f);
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1))
                    pImp->AI()->AttackStart(pTarget);
            }
            m_uiSummon_Timer = 5000;
        }
        else
            m_uiSummon_Timer -= diff;

        if (!m_bBerserk)
        {
            if (m_uiBerserk_Timer < diff)
            {
                DoCastMe( SPELL_BERSERK);
                m_bBerserk = true;
            }
            else
                m_uiBerserk_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_karazhan_impAI : public ScriptedAI
{
    mob_karazhan_impAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiFirebolt_Timer;

    void Reset()
    {
        m_uiFirebolt_Timer = 2000;

        me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (m_uiFirebolt_Timer < diff)
        {
            DoCastVictim( SPELL_FIREBOLT);
            m_uiFirebolt_Timer = 2200;
        }
        else
            m_uiFirebolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_kilrek(Creature* pCreature)
{
    return new mob_kilrekAI(pCreature);
}

CreatureAI* GetAI_mob_karazhan_imp(Creature* pCreature)
{
    return new mob_karazhan_impAI(pCreature);
}

CreatureAI* GetAI_mob_demon_chain(Creature* pCreature)
{
    return new mob_demon_chainAI(pCreature);
}

CreatureAI* GetAI_boss_terestian_illhoof(Creature* pCreature)
{
    return new boss_terestianAI(pCreature);
}

void AddSC_boss_terestian_illhoof()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_terestian_illhoof";
    newscript->GetAI = &GetAI_boss_terestian_illhoof;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_karazhan_imp";
    newscript->GetAI = &GetAI_mob_karazhan_imp;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_kilrek";
    newscript->GetAI = &GetAI_mob_kilrek;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_demon_chain";
    newscript->GetAI = &GetAI_mob_demon_chain;
    newscript->RegisterSelf();
}
