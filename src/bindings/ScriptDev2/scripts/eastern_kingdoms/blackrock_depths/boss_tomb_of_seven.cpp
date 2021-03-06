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
SDName: Boss_Tomb_Of_Seven
SD%Complete: 90
SDComment: Learning Smelt Dark Iron if tribute quest rewarded. Basic event implemented. Correct order and timing of event is unknown.
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "blackrock_depths.h"

enum
{
    FACTION_NEUTRAL             = 734,
    FACTION_HOSTILE             = 754,

    SPELL_SMELT_DARK_IRON       = 14891,
    SPELL_LEARN_SMELT           = 14894,
    QUEST_SPECTRAL_CHALICE      = 4083,
    SKILLPOINT_MIN              = 230
};

#define GOSSIP_ITEM_TEACH_1 "Teach me the art of smelting dark iron"
#define GOSSIP_ITEM_TEACH_2 "Continue..."
#define GOSSIP_ITEM_TRIBUTE "I want to pay tribute"

bool GossipHello_boss_gloomrel(Player* pPlayer, Creature* pCreature)
{
    if (InstanceData* pInstance = pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_TOMB_OF_SEVEN) == NOT_STARTED)
        {
            if (pPlayer->GetQuestRewardStatus(QUEST_SPECTRAL_CHALICE) &&
                pPlayer->GetSkillValue(SKILL_MINING) >= SKILLPOINT_MIN &&
                !pPlayer->HasSpell(SPELL_SMELT_DARK_IRON))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (!pPlayer->GetQuestRewardStatus(QUEST_SPECTRAL_CHALICE) &&
                pPlayer->GetSkillValue(SKILL_MINING) >= SKILLPOINT_MIN)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRIBUTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
    }
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_boss_gloomrel(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TEACH_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            pPlayer->SEND_GOSSIP_MENU(2606, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+11:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, SPELL_LEARN_SMELT, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[PH] Continue...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
            pPlayer->SEND_GOSSIP_MENU(2604, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+22:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (InstanceData* pInstance = pCreature->GetInstanceData())
            {
                //are 5 minutes expected? go template may have data to despawn when used at quest
                pInstance->DoRespawnGameObject(pInstance->GetData64(DATA_GO_CHALICE),MINUTE*5);
            }
            break;
    }
    return true;
}

enum
{
    SPELL_SHADOWBOLTVOLLEY              = 15245,
    SPELL_IMMOLATE                      = 12742,
    SPELL_CURSEOFWEAKNESS               = 12493,
    SPELL_DEMONARMOR                    = 13787,
    SPELL_SUMMON_VOIDWALKERS            = 15092,

    SAY_DOOMREL_START_EVENT             = -1230003,

    MAX_DWARF                           = 7
};

#define GOSSIP_ITEM_CHALLENGE   "Your bondage is at an end, Doom'rel. I challenge you!"

struct MANGOS_DLL_DECL boss_doomrelAI : public ScriptedAI
{
    boss_doomrelAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        Reset();
    }

    uint32 m_uiCallToFight_Timer;
    uint8 m_uiDwarfRound;
    bool m_bHasSummoned;
    MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_SHADOWBOLTVOLLEY,10000,12000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_IMMOLATE,18000,25000);
		Tasks.AddEvent(SPELL_CURSEOFWEAKNESS,5000,45000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_DEMONARMOR,16000,300000,0,TARGET_ME);
        m_uiCallToFight_Timer = 0;
        m_uiDwarfRound = 0;
        m_bHasSummoned = false;
    }

    void JustReachedHome()
    {
        if (pInstance)
            SetInstanceData(TYPE_TOMB_OF_SEVEN, FAIL);
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            SetInstanceData(TYPE_TOMB_OF_SEVEN, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    Creature* GetDwarfForPhase(uint8 uiPhase)
    {
        switch(uiPhase)
        {
            case 0:
                return pInstance->instance->GetCreature(pInstance->GetData64(DATA_ANGERREL));
            case 1:
                return pInstance->instance->GetCreature(pInstance->GetData64(DATA_SEETHREL));
            case 2:
                return pInstance->instance->GetCreature(pInstance->GetData64(DATA_DOPEREL));
            case 3:
                return pInstance->instance->GetCreature(pInstance->GetData64(DATA_GLOOMREL));
            case 4:
                return pInstance->instance->GetCreature(pInstance->GetData64(DATA_VILEREL));
            case 5:
                return pInstance->instance->GetCreature(pInstance->GetData64(DATA_HATEREL));
            case 6:
                return me;
        }
        return NULL;
    }

    void CallToFight(bool bStartFight)
    {
        if (Creature* pDwarf = GetDwarfForPhase(m_uiDwarfRound))
        {
            if (bStartFight && pDwarf->isAlive())
            {
                pDwarf->setFaction(FACTION_HOSTILE);
                pDwarf->SetInCombatWithZone();              // attackstart
            }
            else
            {
                if (!pDwarf->isAlive() || pDwarf->isDead())
                    pDwarf->Respawn();

                pDwarf->setFaction(FACTION_NEUTRAL);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance)
        {
            if (pInstance->GetData(TYPE_TOMB_OF_SEVEN) == IN_PROGRESS)
            {
                if (m_uiDwarfRound < MAX_DWARF)
                {
                    if (m_uiCallToFight_Timer < diff)
                    {
                        CallToFight(true);
                        ++m_uiDwarfRound;
                        m_uiCallToFight_Timer = 30000;
                    }
                    else
                        m_uiCallToFight_Timer -= diff;
                }
            }
            else if (pInstance->GetData(TYPE_TOMB_OF_SEVEN) == FAIL)
            {
                for (m_uiDwarfRound = 0; m_uiDwarfRound < MAX_DWARF; ++m_uiDwarfRound)
                    CallToFight(false);

                m_uiDwarfRound = 0;
                m_uiCallToFight_Timer = 0;

                if (pInstance)
                    SetInstanceData(TYPE_TOMB_OF_SEVEN, NOT_STARTED);
            }
        }

        if (!CanDoSomething())
            return;

        //Summon Voidwalkers
        if (!m_bHasSummoned && CheckPercentLife(50))
        {
            me->CastSpell(me,SPELL_SUMMON_VOIDWALKERS, true);
            m_bHasSummoned = true;
        }
		Tasks.UpdateEvent(diff);
		
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_doomrel(Creature* pCreature)
{
    return new boss_doomrelAI(pCreature);
}

bool GossipHello_boss_doomrel(Player* pPlayer, Creature* pCreature)
{
    if (InstanceData* pInstance = pCreature->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_TOMB_OF_SEVEN) == NOT_STARTED)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(2601, pCreature->GetGUID());
    return true;
}

bool GossipSelect_boss_doomrel(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            DoScriptText(SAY_DOOMREL_START_EVENT, pCreature, pPlayer);
            // start event
            if (InstanceData* pInstance = pCreature->GetInstanceData())
                pInstance->SetData(TYPE_TOMB_OF_SEVEN, IN_PROGRESS);

            break;
    }
    return true;
}

void AddSC_boss_tomb_of_seven()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_gloomrel";
    newscript->pGossipHello = &GossipHello_boss_gloomrel;
    newscript->pGossipSelect = &GossipSelect_boss_gloomrel;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_doomrel";
    newscript->GetAI = &GetAI_boss_doomrel;
    newscript->pGossipHello = &GossipHello_boss_doomrel;
    newscript->pGossipSelect = &GossipSelect_boss_doomrel;
    newscript->RegisterSelf();
}
