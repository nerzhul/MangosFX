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
SDName: Tanaris
SD%Complete: 80
SDComment: Quest support: 648, 1560, 2954, 4005, 10277, 10279(Special flight path). Noggenfogger vendor
SDCategory: Tanaris
EndScriptData */

/* ContentData
mob_aquementas
npc_custodian_of_time
npc_marin_noggenfogger
npc_oox17tn
npc_steward_of_time
npc_stone_watcher_of_norgannon
npc_tooga
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "follower_ai.h"

/*######
## mob_aquementas
######*/

#define AGGRO_YELL_AQUE     -1000168

#define SPELL_AQUA_JET      13586
#define SPELL_FROST_SHOCK   15089

struct MANGOS_DLL_DECL mob_aquementasAI : public ScriptedAI
{
    mob_aquementasAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 SendItem_Timer;
    uint32 SwitchFaction_Timer;
    bool isFriendly;

    uint32 FrostShock_Timer;
    uint32 AquaJet_Timer;

    void Reset()
    {
        SendItem_Timer = 0;
        SwitchFaction_Timer = 10000;
        me->setFaction(35);
        isFriendly = true;

        AquaJet_Timer = 5000;
        FrostShock_Timer = 1000;
    }

    void SendItem(Unit* receiver)
    {
        if (((Player*)receiver)->HasItemCount(11169,1,false) &&
            ((Player*)receiver)->HasItemCount(11172,11,false) &&
            ((Player*)receiver)->HasItemCount(11173,1,false) &&
            !((Player*)receiver)->HasItemCount(11522,1,true))
        {
            ItemPosCountVec dest;
            uint8 msg = ((Player*)receiver)->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 11522, 1, false);
            if (msg == EQUIP_ERR_OK)
                ((Player*)receiver)->StoreNewItem(dest, 11522, 1, true);
        }
    }

    void Aggro(Unit* who)
    {
        DoScriptText(AGGRO_YELL_AQUE, me, who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (isFriendly)
        {
            if (SwitchFaction_Timer < diff)
            {
                me->setFaction(91);
                isFriendly = false;
            }else SwitchFaction_Timer -= diff;
        }

        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        if (!isFriendly)
        {
            if (SendItem_Timer < diff)
            {
                if (me->getVictim()->GetTypeId() == TYPEID_PLAYER)
                    SendItem(me->getVictim());
                SendItem_Timer = 5000;
            }else SendItem_Timer -= diff;
        }

        if (FrostShock_Timer < diff)
        {
            DoCastVictim(SPELL_FROST_SHOCK);
            FrostShock_Timer = 15000;
        }else FrostShock_Timer -= diff;

        if (AquaJet_Timer < diff)
        {
            DoCastMe(SPELL_AQUA_JET);
            AquaJet_Timer = 15000;
        }else AquaJet_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_aquementas(Creature* pCreature)
{
    return new mob_aquementasAI(pCreature);
}

/*######
## npc_custodian_of_time
######*/

#define WHISPER_CUSTODIAN_1     -1000217
#define WHISPER_CUSTODIAN_2     -1000218
#define WHISPER_CUSTODIAN_3     -1000219
#define WHISPER_CUSTODIAN_4     -1000220
#define WHISPER_CUSTODIAN_5     -1000221
#define WHISPER_CUSTODIAN_6     -1000222
#define WHISPER_CUSTODIAN_7     -1000223
#define WHISPER_CUSTODIAN_8     -1000224
#define WHISPER_CUSTODIAN_9     -1000225
#define WHISPER_CUSTODIAN_10    -1000226
#define WHISPER_CUSTODIAN_11    -1000227
#define WHISPER_CUSTODIAN_12    -1000228
#define WHISPER_CUSTODIAN_13    -1000229
#define WHISPER_CUSTODIAN_14    -1000230

struct MANGOS_DLL_DECL npc_custodian_of_timeAI : public npc_escortAI
{
    npc_custodian_of_timeAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(i)
        {
            case 0: DoScriptText(WHISPER_CUSTODIAN_1, me, pPlayer); break;
            case 1: DoScriptText(WHISPER_CUSTODIAN_2, me, pPlayer); break;
            case 2: DoScriptText(WHISPER_CUSTODIAN_3, me, pPlayer); break;
            case 3: DoScriptText(WHISPER_CUSTODIAN_4, me, pPlayer); break;
            case 5: DoScriptText(WHISPER_CUSTODIAN_5, me, pPlayer); break;
            case 6: DoScriptText(WHISPER_CUSTODIAN_6, me, pPlayer); break;
            case 7: DoScriptText(WHISPER_CUSTODIAN_7, me, pPlayer); break;
            case 8: DoScriptText(WHISPER_CUSTODIAN_8, me, pPlayer); break;
            case 9: DoScriptText(WHISPER_CUSTODIAN_9, me, pPlayer); break;
            case 10: DoScriptText(WHISPER_CUSTODIAN_4, me, pPlayer); break;
            case 13: DoScriptText(WHISPER_CUSTODIAN_10, me, pPlayer); break;
            case 14: DoScriptText(WHISPER_CUSTODIAN_4, me, pPlayer); break;
            case 16: DoScriptText(WHISPER_CUSTODIAN_11, me, pPlayer); break;
            case 17: DoScriptText(WHISPER_CUSTODIAN_12, me, pPlayer); break;
            case 18: DoScriptText(WHISPER_CUSTODIAN_4, me, pPlayer); break;
            case 22: DoScriptText(WHISPER_CUSTODIAN_13, me, pPlayer); break;
            case 23: DoScriptText(WHISPER_CUSTODIAN_4, me, pPlayer); break;
            case 24:
                DoScriptText(WHISPER_CUSTODIAN_14, me, pPlayer);
                DoCast(pPlayer, 34883);
                //below here is temporary workaround, to be removed when spell works properly
                pPlayer->AreaExploredOrEventHappens(10277);
                break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (((Player*)who)->HasAura(34877,1) && ((Player*)who)->GetQuestStatus(10277) == QUEST_STATUS_INCOMPLETE)
            {
                float Radius = 10.0;

                if (me->IsWithinDistInMap(who, Radius))
                    Start(false, false, who->GetGUID());
            }
        }
    }

    void Reset() { }
};

CreatureAI* GetAI_npc_custodian_of_time(Creature* pCreature)
{
    return new npc_custodian_of_timeAI(pCreature);
}

/*######
## npc_marin_noggenfogger
######*/

bool GossipHello_npc_marin_noggenfogger(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor() && pPlayer->GetQuestRewardStatus(2662))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_marin_noggenfogger(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
## npc_oox17tn
######*/

enum
{
    SAY_OOX_START           = -1000287,
    SAY_OOX_AGGRO1          = -1000288,
    SAY_OOX_AGGRO2          = -1000289,
    SAY_OOX_AMBUSH          = -1000290,
    SAY_OOX17_AMBUSH_REPLY  = -1000291,
    SAY_OOX_END             = -1000292,

    QUEST_RESCUE_OOX_17TN   = 648,

    NPC_SCORPION            = 7803,
    NPC_SCOFFLAW            = 7805,
    NPC_SHADOW_MAGE         = 5617
};

struct MANGOS_DLL_DECL npc_oox17tnAI : public npc_escortAI
{
    npc_oox17tnAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            //1. Ambush: 3 scorpions
            case 22:
                DoScriptText(SAY_OOX_AMBUSH, me);
                me->SummonCreature(NPC_SCORPION, -8340.70, -4448.17, 9.17, 3.10, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                me->SummonCreature(NPC_SCORPION, -8343.18, -4444.35, 9.44, 2.35, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                me->SummonCreature(NPC_SCORPION, -8348.70, -4457.80, 9.58, 2.02, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            //2. Ambush: 2 Rogues & 1 Shadow Mage
            case 28:
                DoScriptText(SAY_OOX_AMBUSH, me);

                me->SummonCreature(NPC_SCOFFLAW, -7488.02, -4786.56 ,10.67, 3.74, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                me->SummonCreature(NPC_SHADOW_MAGE, -7486.41, -4791.55 ,10.54, 3.26, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);

                if (Creature* pCreature = me->SummonCreature(NPC_SCOFFLAW, -7488.47, -4800.77, 9.77, 2.50,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
                    DoScriptText(SAY_OOX17_AMBUSH_REPLY,pCreature);

                break;
            case 34:
                DoScriptText(SAY_OOX_END, me);
                // Award quest credit
                pPlayer->GroupEventHappens(QUEST_RESCUE_OOX_17TN, me);
                break;
        }
    }

    void Reset() { }

    void Aggro(Unit* who)
    {
        //For an small probability he say something when it aggros
        switch(urand(0, 9))
        {
           case 0: DoScriptText(SAY_OOX_AGGRO1, me); break;
           case 1: DoScriptText(SAY_OOX_AGGRO2, me); break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }
};

CreatureAI* GetAI_npc_oox17tn(Creature* pCreature)
{
    return new npc_oox17tnAI(pCreature);
}

bool QuestAccept_npc_oox17tn(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_RESCUE_OOX_17TN)
    {
        DoScriptText(SAY_OOX_START, pCreature);

        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        if (pPlayer->GetTeam() == ALLIANCE)
            pCreature->setFaction(FACTION_ESCORT_A_PASSIVE);

        if (pPlayer->GetTeam() == HORDE)
            pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);

        if (npc_oox17tnAI* pEscortAI = dynamic_cast<npc_oox17tnAI*>(pCreature->AI()))
            pEscortAI->Start(true, false, pPlayer->GetGUID(), pQuest);
    }
    return true;
}

/*######
## npc_steward_of_time
######*/

#define GOSSIP_ITEM_FLIGHT  "Please take me to the master's lair."

bool GossipHello_npc_steward_of_time(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(10279) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestRewardStatus(10279))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(9978, pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(9977, pCreature->GetGUID());

    return true;
}

bool QuestAccept_npc_steward_of_time(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == 10279)                      //Quest: To The Master's Lair
        pPlayer->CastSpell(pPlayer,34891,true);             //(Flight through Caverns)

    return false;
}

bool GossipSelect_npc_steward_of_time(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->CastSpell(pPlayer,34891,true);             //(Flight through Caverns)

    return true;
}

/*######
## npc_stone_watcher_of_norgannon
######*/

#define GOSSIP_ITEM_NORGANNON_1     "What function do you serve?"
#define GOSSIP_ITEM_NORGANNON_2     "What are the Plates of Uldum?"
#define GOSSIP_ITEM_NORGANNON_3     "Where are the Plates of Uldum?"
#define GOSSIP_ITEM_NORGANNON_4     "Excuse me? We've been \"reschedueled for visitations\"? What does that mean?!"
#define GOSSIP_ITEM_NORGANNON_5     "So, what's inside Uldum?"
#define GOSSIP_ITEM_NORGANNON_6     "I will return when i have the Plates of Uldum."

bool GossipHello_npc_stone_watcher_of_norgannon(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(2954) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(1674, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_stone_watcher_of_norgannon(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(1675, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(1676, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(1677, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->SEND_GOSSIP_MENU(1678, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(1679, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(2954);
            break;
    }
    return true;
}

/*####
# npc_tooga
####*/

enum
{
    SAY_TOOG_THIRST             = -1000391,
    SAY_TOOG_WORRIED            = -1000392,
    SAY_TOOG_POST_1             = -1000393,
    SAY_TORT_POST_2             = -1000394,
    SAY_TOOG_POST_3             = -1000395,
    SAY_TORT_POST_4             = -1000396,
    SAY_TOOG_POST_5             = -1000397,
    SAY_TORT_POST_6             = -1000398,

    QUEST_TOOGA                 = 1560,
    NPC_TORTA                   = 6015,

    POINT_ID_TO_WATER           = 1
};

const float m_afToWaterLoc[] = {-7032.664551, -4906.199219, -1.606446};

struct MANGOS_DLL_DECL npc_toogaAI : public FollowerAI
{
    npc_toogaAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }

    uint32 m_uiCheckSpeechTimer;
    uint32 m_uiPostEventTimer;
    uint32 m_uiPhasePostEvent;

    Unit* pTorta;

    void Reset()
    {
        m_uiCheckSpeechTimer = 2500;
        m_uiPostEventTimer = 1000;
        m_uiPhasePostEvent = 0;

        pTorta = NULL;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!me->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE | STATE_FOLLOW_POSTEVENT) && pWho->GetEntry() == NPC_TORTA)
        {
            if (me->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    if (pPlayer->GetQuestStatus(QUEST_TOOGA) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->GroupEventHappens(QUEST_TOOGA, me);
                }

                pTorta = pWho;
                SetFollowComplete(true);
            }
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        FollowerAI::MovementInform(uiMotionType, uiPointId);

        if (uiMotionType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_TO_WATER)
            SetFollowComplete();
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
        {
            //we are doing the post-event, or...
            if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            {
                if (m_uiPostEventTimer < uiDiff)
                {
                    m_uiPostEventTimer = 5000;

                    if (!pTorta || !pTorta->isAlive())
                    {
                        //something happened, so just complete
                        SetFollowComplete();
                        return;
                    }

                    switch(m_uiPhasePostEvent)
                    {
                        case 1:
                            DoScriptText(SAY_TOOG_POST_1, me);
                            break;
                        case 2:
                            DoScriptText(SAY_TORT_POST_2, pTorta);
                            break;
                        case 3:
                            DoScriptText(SAY_TOOG_POST_3, me);
                            break;
                        case 4:
                            DoScriptText(SAY_TORT_POST_4, pTorta);
                            break;
                        case 5:
                            DoScriptText(SAY_TOOG_POST_5, me);
                            break;
                        case 6:
                            DoScriptText(SAY_TORT_POST_6, pTorta);
                            me->GetMotionMaster()->MovePoint(POINT_ID_TO_WATER, m_afToWaterLoc[0], m_afToWaterLoc[1], m_afToWaterLoc[2]);
                            break;
                    }

                    ++m_uiPhasePostEvent;
                }
                else
                    m_uiPostEventTimer -= uiDiff;
            }
            //...we are doing regular speech check
            else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
            {
                if (m_uiCheckSpeechTimer < uiDiff)
                {
                    m_uiCheckSpeechTimer = 5000;

                    switch(urand(0, 50))
                    {
                        case 10: DoScriptText(SAY_TOOG_THIRST, me); break;
                        case 25: DoScriptText(SAY_TOOG_WORRIED, me); break;
                    }
                }
                else
                    m_uiCheckSpeechTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tooga(Creature* pCreature)
{
    return new npc_toogaAI(pCreature);
}

bool QuestAccept_npc_tooga(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOOGA)
    {
        if (npc_toogaAI* pToogaAI = dynamic_cast<npc_toogaAI*>(pCreature->AI()))
            pToogaAI->StartFollow(pPlayer, FACTION_ESCORT_N_FRIEND_PASSIVE, pQuest);
    }

    return true;
}

void AddSC_tanaris()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_aquementas";
    newscript->GetAI = &GetAI_mob_aquementas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_custodian_of_time";
    newscript->GetAI = &GetAI_npc_custodian_of_time;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_marin_noggenfogger";
    newscript->pGossipHello =  &GossipHello_npc_marin_noggenfogger;
    newscript->pGossipSelect = &GossipSelect_npc_marin_noggenfogger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_oox17tn";
    newscript->GetAI = &GetAI_npc_oox17tn;
    newscript->pQuestAccept = &QuestAccept_npc_oox17tn;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_steward_of_time";
    newscript->pGossipHello =  &GossipHello_npc_steward_of_time;
    newscript->pGossipSelect = &GossipSelect_npc_steward_of_time;
    newscript->pQuestAccept =  &QuestAccept_npc_steward_of_time;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_stone_watcher_of_norgannon";
    newscript->pGossipHello =  &GossipHello_npc_stone_watcher_of_norgannon;
    newscript->pGossipSelect = &GossipSelect_npc_stone_watcher_of_norgannon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tooga";
    newscript->GetAI = &GetAI_npc_tooga;
    newscript->pQuestAccept = &QuestAccept_npc_tooga;
    newscript->RegisterSelf();
}
