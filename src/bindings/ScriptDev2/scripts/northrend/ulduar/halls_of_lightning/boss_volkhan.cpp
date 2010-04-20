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
SDName: Boss Volkhan
SD%Complete: 60%
SDComment: Not considered complete. Some events may fail and need further development
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "halls_of_lightning.h"

enum
{
    SAY_AGGRO                               = -1602032,
    SAY_SLAY_1                              = -1602033,
    SAY_SLAY_2                              = -1602034,
    SAY_SLAY_3                              = -1602035,
    SAY_DEATH                               = -1602036,
    SAY_STOMP_1                             = -1602037,
    SAY_STOMP_2                             = -1602038,
    SAY_FORGE_1                             = -1602039,
    SAY_FORGE_2                             = -1602040,
    EMOTE_TO_ANVIL                          = -1602041,
    EMOTE_SHATTER                           = -1602042,

    SPELL_HEAT_N                            = 52387,
    SPELL_HEAT_H                            = 59528,
    SPELL_SHATTERING_STOMP_N                = 52237,
    SPELL_SHATTERING_STOMP_H                = 59529,

    //unclear how "directions" of spells must be. Last, summoning GO, what is it for? Script depend on:
    SPELL_TEMPER                            = 52238,        //TARGET_SCRIPT boss->anvil
    SPELL_TEMPER_DUMMY                      = 52654,        //TARGET_SCRIPT anvil->boss

    //SPELL_TEMPER_VISUAL                     = 52661,        //summons GO

    SPELL_SUMMON_MOLTEN_GOLEM               = 52405,

    //Molten Golem
    SPELL_BLAST_WAVE                        = 23113,
    SPELL_IMMOLATION_STRIKE_N               = 52433,
    SPELL_IMMOLATION_STRIKE_H               = 59530,
    SPELL_SHATTER_N                         = 52429,
    SPELL_SHATTER_H                         = 59527,

    NPC_VOLKHAN_ANVIL                       = 28823,
    NPC_MOLTEN_GOLEM                        = 28695,
    NPC_BRITTLE_GOLEM                       = 28681,

    POINT_ID_ANVIL                          = 0,
    MAX_GOLEM                               = 2
};

/*######
## Boss Volkhan
######*/

struct MANGOS_DLL_DECL boss_volkhanAI : public ScriptedAI
{
    boss_volkhanAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    std::list<uint64> m_lGolemGUIDList;

    bool m_bIsHeroic;
    bool m_bHasTemper;
    bool m_bIsStriking;
    bool m_bCanShatterGolem;

    uint32 m_uiPause_Timer;
    uint32 m_uiShatteringStomp_Timer;
    uint32 m_uiShatter_Timer;

    uint32 m_uiHealthAmountModifier;

    void Reset()
    {
        m_bIsStriking = false;
        m_bHasTemper = false;
        m_bCanShatterGolem = false;

        m_uiPause_Timer = 3500;
        m_uiShatteringStomp_Timer = 0;
        m_uiShatter_Timer = 5000;

        m_uiHealthAmountModifier = 1;

        DespawnGolem();
        m_lGolemGUIDList.clear();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho)
    {
        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);

            if (!m_bHasTemper)
                me->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
        DespawnGolem();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLKHAN, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SLAY_3, me); break;
        }
    }

    void DespawnGolem()
    {
        if (m_lGolemGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, *itr))
            {
                if (pTemp->isAlive())
                    pTemp->ForcedDespawn();
            }
        }

        m_lGolemGUIDList.clear();
    }

    void ShatterGolem()
    {
        if (m_lGolemGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, *itr))
            {
                 // only shatter brittle golems
                if (pTemp->isAlive() && pTemp->GetEntry() == NPC_BRITTLE_GOLEM)
                    pTemp->CastSpell(pTemp, m_bIsHeroic ? SPELL_SHATTER_H : SPELL_SHATTER_N, false);
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_TEMPER_DUMMY)
            m_bIsStriking = true;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_MOLTEN_GOLEM)
        {
            m_lGolemGUIDList.push_back(pSummoned->GetGUID());

            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);

            //why healing when just summoned?
            pSummoned->CastSpell(pSummoned, m_bIsHeroic ? SPELL_HEAT_H : SPELL_HEAT_N, false, NULL, NULL, me->GetGUID());
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (CanDoSomething())
            return;

        if (m_bIsStriking)
        {
            if (m_uiPause_Timer < uiDiff)
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                {
                    if (me->getVictim())
                        me->GetMotionMaster()->MoveChase(me->getVictim());
                }

                m_bHasTemper = false;
                m_bIsStriking = false;
                m_uiPause_Timer = 3500;
            }
            else
                m_uiPause_Timer -= uiDiff;

            return;
        }

        // When to start shatter? After 60, 40 or 20% hp?
        if (!m_bHasTemper && m_uiHealthAmountModifier >= 3)
        {
            if (m_uiShatteringStomp_Timer < uiDiff)
            {
                //should he stomp even if he has no brittle golem to shatter?

                DoScriptText(urand(0, 1) ? SAY_STOMP_1 : SAY_STOMP_2, me);

                DoCastMe( m_bIsHeroic ? SPELL_SHATTERING_STOMP_H : SPELL_SHATTERING_STOMP_N);

                DoScriptText(EMOTE_SHATTER, me);

                m_uiShatteringStomp_Timer = 30000;
                m_bCanShatterGolem = true;
            }
            else
                m_uiShatteringStomp_Timer -= uiDiff;
        }

        // Shatter Golems 3 seconds after Shattering Stomp
        if (m_bCanShatterGolem)
        {
            if (m_uiShatter_Timer < uiDiff)
            {
                ShatterGolem();
				m_uiShatter_Timer = 3000;
                m_bCanShatterGolem = false;
            }
            else
                m_uiShatter_Timer -= uiDiff;
        }

        // Health check
        if (!m_bCanShatterGolem && (me->GetHealth()*100 / me->GetMaxHealth()) < (100-(20*m_uiHealthAmountModifier)))
        {
            ++m_uiHealthAmountModifier;

            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            DoScriptText(urand(0, 1) ? SAY_FORGE_1 : SAY_FORGE_2, me);

            m_bHasTemper = true;

            me->CastSpell(me, SPELL_TEMPER, false);
			for(uint8 i = 0; i < MAX_GOLEM; ++i)
				me->CastSpell(me, SPELL_SUMMON_MOLTEN_GOLEM, true);
			m_bIsStriking = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_volkhan(Creature* pCreature)
{
    return new boss_volkhanAI(pCreature);
}

/*######
## npc_volkhan_anvil
######*/

bool EffectDummyCreature_npc_volkhan_anvil(Unit* pCaster, uint32 uiSpellId, uint32 uiEffIndex, Creature* pCreatureTarget)
{
    //always check spellid and effectindex
    if (uiSpellId == SPELL_TEMPER && uiEffIndex == 0)
    {
        if (pCaster->GetEntry() != NPC_VOLKHAN || pCreatureTarget->GetEntry() != NPC_VOLKHAN_ANVIL)
            return true;

        DoScriptText(EMOTE_TO_ANVIL, pCaster);

        float fX, fY, fZ;
        pCreatureTarget->GetContactPoint(pCaster, fX, fY, fZ, INTERACTION_DISTANCE);

        pCaster->AttackStop();

        if (pCaster->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
            pCaster->GetMotionMaster()->MovementExpired();

        ((Creature*)pCaster)->GetMap()->CreatureRelocation((Creature*)pCaster, fX, fY, fZ, pCreatureTarget->GetOrientation());
        ((Creature*)pCaster)->SendMonsterMove(fX, fY, fZ, 0, ((Creature*)pCaster)->GetMonsterMoveFlags(), 1);

        pCreatureTarget->CastSpell(pCaster, SPELL_TEMPER_DUMMY, false);

        //always return true when we are handling this spell and effect
        return true;
    }

    return false;
}

/*######
## mob_molten_golem
######*/

struct MANGOS_DLL_DECL mob_molten_golemAI : public ScriptedAI
{
    mob_molten_golemAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsHeroic;
    bool m_bIsFrozen;

    uint32 m_uiBlast_Timer;
    uint32 m_uiDeathDelay_Timer;
    uint32 m_uiImmolation_Timer;

    void Reset()
    {
        m_bIsFrozen = false;

        m_uiBlast_Timer = 20000;
        m_uiDeathDelay_Timer = 0;
        m_uiImmolation_Timer = 5000;
    }

    void AttackStart(Unit* pWho)
    {
        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);

            if (!m_bIsFrozen)
                me->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bIsFrozen)
        {
            //workaround for now, brittled should be immune to any kind of attacks
            uiDamage = 0;
            return;
        }

        if (uiDamage > me->GetHealth())
        {
            m_bIsFrozen = true;

            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            me->RemoveAllAuras();
            me->AttackStop();

            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                me->GetMotionMaster()->MovementExpired();

            uiDamage = me->GetHealth()-1;

            me->UpdateEntry(NPC_BRITTLE_GOLEM);
            me->SetHealth(1);
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        //this is the dummy effect of the spells
        if (pSpell->Id == SPELL_SHATTER_N || pSpell->Id == SPELL_SHATTER_H)
        {
            if (me->GetEntry() == NPC_BRITTLE_GOLEM)
                me->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target or if we are frozen
        if (CanDoSomething() || m_bIsFrozen)
            return;

        if (m_uiBlast_Timer < uiDiff)
        {
            DoCastMe( SPELL_BLAST_WAVE);
            m_uiBlast_Timer = 20000;
        }
        else
            m_uiBlast_Timer -= uiDiff;

        if (m_uiImmolation_Timer < uiDiff)
        {
            DoCastVictim( m_bIsHeroic ? SPELL_IMMOLATION_STRIKE_H : SPELL_IMMOLATION_STRIKE_N);
            m_uiImmolation_Timer = 5000;
        }
        else
            m_uiImmolation_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_molten_golem(Creature* pCreature)
{
    return new mob_molten_golemAI(pCreature);
}

void AddSC_boss_volkhan()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_volkhan";
    newscript->GetAI = &GetAI_boss_volkhan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_volkhan_anvil";
    newscript->pEffectDummyCreature = &EffectDummyCreature_npc_volkhan_anvil;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_molten_golem";
    newscript->GetAI = &GetAI_mob_molten_golem;
    newscript->RegisterSelf();
}
