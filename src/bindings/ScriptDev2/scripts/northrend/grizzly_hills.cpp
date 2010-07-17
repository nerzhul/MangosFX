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
SDName: Grizzly_Hills
SD%Complete: 80
SDComment: Quest support: 12247
SDCategory: Grizzly Hills
EndScriptData */

/* ContentData
npc_orsonn_and_kodian
EndContentData */

#include "precompiled.h"

#define GOSSIP_ITEM1 "You're free to go Orsonn, but first tell me what's wrong with the furbolg."
#define GOSSIP_ITEM2 "What happened then?"
#define GOSSIP_ITEM3 "Thank you, Son of Ursoc. I'll see what can be done."
#define GOSSIP_ITEM4 "Who was this stranger?"
#define GOSSIP_ITEM5 "Thank you, Kodian. I'll do what I can."

enum
{
    GOSSIP_TEXTID_ORSONN1       = 12793,
    GOSSIP_TEXTID_ORSONN2       = 12794,
    GOSSIP_TEXTID_ORSONN3       = 12796,

    GOSSIP_TEXTID_KODIAN1       = 12797,
    GOSSIP_TEXTID_KODIAN2       = 12798,

    NPC_ORSONN                  = 27274,
    NPC_KODIAN                  = 27275,

    //trigger creatures
    NPC_ORSONN_CREDIT           = 27322,
    NPC_KODIAN_CREDIT           = 27321,

    QUEST_CHILDREN_OF_URSOC     = 12247
};

bool GossipHello_npc_orsonn_and_kodian(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_CHILDREN_OF_URSOC) == QUEST_STATUS_INCOMPLETE)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_ORSONN:
                if (!pPlayer->GetReqKillOrCastCurrentCount(QUEST_CHILDREN_OF_URSOC, NPC_ORSONN_CREDIT))
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN1, pCreature->GetGUID());
                    return true;
                }
                break;
            case NPC_KODIAN:
                if (!pPlayer->GetReqKillOrCastCurrentCount(QUEST_CHILDREN_OF_URSOC, NPC_KODIAN_CREDIT))
                {
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_KODIAN1, pCreature->GetGUID());
                    return true;
                }
                break;
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_orsonn_and_kodian(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN2, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN3, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TalkedToCreature(NPC_ORSONN_CREDIT, pCreature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_KODIAN2, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->TalkedToCreature(NPC_KODIAN_CREDIT, pCreature->GetGUID());
            break;
    }

    return true;
}

// Tallhorn Stage

enum etallhornstage
{
    OBJECT_HAUNCH                   = 188665
};

struct npc_tallhorn_stagAI : public ScriptedAI
{
    npc_tallhorn_stagAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	void Reset(){};
    void UpdateAI(const uint32 diff)
    {
        if (GameObject* haunch = me->GetClosestGameObjectWithEntry(OBJECT_HAUNCH, 2.0f))
        {
            me->SetStandState(UNIT_STAND_STATE_DEAD);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        }
    }
};

CreatureAI* GetAI_npc_tallhorn_stag(Creature* pCreature)
{
    return new npc_tallhorn_stagAI (pCreature);
}

// Amberpine Woodsman

enum eamberpinewoodsman
{
    TALLHORN_STAG                   = 26363
};

struct npc_amberpine_woodsmanAI : public ScriptedAI
{
    npc_amberpine_woodsmanAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    uint8 m_uiPhase;
    uint32 m_uiTimer;

    void Reset()
    {
        m_uiTimer = 0;
        m_uiPhase = 1;
    }  
       
        void UpdateAI(const uint32 diff)
    {  
                if (Creature* stag = GetClosestCreatureWithEntry(me,TALLHORN_STAG, 0.2f))
                {
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USESTANDING);
        }
                else
            if (m_uiPhase)
            {
                if (m_uiTimer <= diff)
                {
                    switch(m_uiPhase)
                    {
                        case 1:
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_LOOT);
                            m_uiTimer = 3000;
                            m_uiPhase = 2;
                            break;                                              
                        case 2:
                            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ATTACK1H);
                            m_uiTimer = 4000;
                            m_uiPhase = 1;
                            break;
                    }
                }
                else
                m_uiTimer -= diff;
            }
            ScriptedAI::UpdateAI(diff);              
       
        if (!CanDoSomething())
            return;
    }
};


CreatureAI* GetAI_npc_amberpine_woodsman(Creature* pCreature)
{
    return new npc_amberpine_woodsmanAI (pCreature);
}


void AddSC_grizzly_hills()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_orsonn_and_kodian";
    newscript->pGossipHello = &GossipHello_npc_orsonn_and_kodian;
    newscript->pGossipSelect = &GossipSelect_npc_orsonn_and_kodian;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_tallhorn_stag";
    newscript->GetAI = &GetAI_npc_tallhorn_stag;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_amberpine_woodsman";
    newscript->GetAI = &GetAI_npc_amberpine_woodsman;
    newscript->RegisterSelf();

}
