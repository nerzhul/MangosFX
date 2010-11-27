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
SDName: Trial Of the Champion
SD%Complete: 80%
SDComment: event script
SDCategory: trial_of_the_champion
EndScriptData */

/* ContentData
npc_toc5_announcer
EndContentData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

#define GOSSIP_START_EVENT     "Im ready for the next challenge."

/*######
## npc_toc5_announcer
######*/

struct MANGOS_DLL_DECL npc_toc5_announcerAI : public ScriptedAI
{
    npc_toc5_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        Reset();
    }

    void Reset()
    {
    }

	void StartEvent(Player* pPlayer)
    {
		if (!pInstance)
			return;
		if (pInstance->GetData(TYPE_GRAND_CHAMPIONS) == NOT_STARTED)
		{
			pInstance->SetData(DATA_TOC5_ANNOUNCER, me->GetGUID());
			if (pInstance->GetData(DATA_TOC5_ANNOUNCER) == pInstance->GetData(DATA_ARELAS))
			{
				pInstance->SetData(DATA_BLACK_KNIGHT_MINION, 35564);
				switch(urand(0, 4))
				{
					case 0:
						pInstance->SetData(DATA_CHAMPIONID_1, 35572);
					break;
					case 1:
						pInstance->SetData(DATA_CHAMPIONID_1, 35569);
					break;
					case 2:
						pInstance->SetData(DATA_CHAMPIONID_1, 35571);
					break;
					case 3:
						pInstance->SetData(DATA_CHAMPIONID_1, 35570);
					break;
					case 4:
						pInstance->SetData(DATA_CHAMPIONID_1, 35617);
					break;
				}
				do{
					switch(urand(0, 4))
					{
						case 0:
							pInstance->SetData(DATA_CHAMPIONID_2, 35572);
						break;
						case 1:
							pInstance->SetData(DATA_CHAMPIONID_2, 35569);
						break;
						case 2:
							pInstance->SetData(DATA_CHAMPIONID_2, 35571);
						break;
						case 3:
							pInstance->SetData(DATA_CHAMPIONID_2, 35570);
						break;
						case 4:
							pInstance->SetData(DATA_CHAMPIONID_2, 35617);
						break;
					}
				} while(pInstance->GetData(DATA_CHAMPIONID_1) == pInstance->GetData(DATA_CHAMPIONID_2));
				do{
					switch(urand(0, 4))
					{
						case 0:
							pInstance->SetData(DATA_CHAMPIONID_3, 35572);
						break;
						case 1:
							pInstance->SetData(DATA_CHAMPIONID_3, 35569);
						break;
						case 2:
							pInstance->SetData(DATA_CHAMPIONID_3, 35571);
						break;
						case 3:
							pInstance->SetData(DATA_CHAMPIONID_3, 35570);
						break;
						case 4:
							pInstance->SetData(DATA_CHAMPIONID_3, 35617);
						break;
					}
				} while(pInstance->GetData(DATA_CHAMPIONID_1) == pInstance->GetData(DATA_CHAMPIONID_3) || pInstance->GetData(DATA_CHAMPIONID_2) == pInstance->GetData(DATA_CHAMPIONID_3));
			}
			if (pInstance->GetData(DATA_TOC5_ANNOUNCER) == pInstance->GetData(DATA_JAEREN))
			{
				pInstance->SetData(DATA_BLACK_KNIGHT_MINION, 123);
				switch(urand(0, 4))
				{
					case 0:
						pInstance->SetData(DATA_CHAMPIONID_1, 34705);
					break;
					case 1:
						pInstance->SetData(DATA_CHAMPIONID_1, 34702);
					break;
					case 2:
						pInstance->SetData(DATA_CHAMPIONID_1, 34701);
					break;
					case 3:
						pInstance->SetData(DATA_CHAMPIONID_1, 34657);
					break;
					case 4:
						pInstance->SetData(DATA_CHAMPIONID_1, 34703);
					break;
				}
				do{
					switch(urand(0, 4))
					{
						case 0:
							pInstance->SetData(DATA_CHAMPIONID_2, 34705);
						break;
						case 1:
							pInstance->SetData(DATA_CHAMPIONID_2, 34702);
						break;
						case 2:
							pInstance->SetData(DATA_CHAMPIONID_2, 34701);
						break;
						case 3:
							pInstance->SetData(DATA_CHAMPIONID_2, 34657);
						break;
						case 4:
							pInstance->SetData(DATA_CHAMPIONID_2, 34703);
						break;
					}
				} while(pInstance->GetData(DATA_CHAMPIONID_1) == pInstance->GetData(DATA_CHAMPIONID_2));
				do{
					switch(urand(0, 4))
					{
						case 0:
							pInstance->SetData(DATA_CHAMPIONID_3, 34705);
						break;
						case 1:
							pInstance->SetData(DATA_CHAMPIONID_3, 34702);
						break;
						case 2:
							pInstance->SetData(DATA_CHAMPIONID_3, 34701);
						break;
						case 3:
							pInstance->SetData(DATA_CHAMPIONID_3, 34657);
						break;
						case 4:
							pInstance->SetData(DATA_CHAMPIONID_3, 34703);
						break;
					}
				} while(pInstance->GetData(DATA_CHAMPIONID_1) == pInstance->GetData(DATA_CHAMPIONID_3) || pInstance->GetData(DATA_CHAMPIONID_2) == pInstance->GetData(DATA_CHAMPIONID_3));
			}
			pInstance->SetData(TYPE_GRAND_CHAMPIONS, FAIL);
		}
		if (pInstance->GetData(TYPE_GRAND_CHAMPIONS) == FAIL)
		{
			me->SummonCreature(pInstance->GetData(DATA_CHAMPIONID_1), 738.665771, 661.031433, 412.394623, 4.698702, TEMPSUMMON_MANUAL_DESPAWN, 0);
			me->SummonCreature(pInstance->GetData(DATA_CHAMPIONID_2), 746.864441, 660.918762, 411.695465, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
			me->SummonCreature(pInstance->GetData(DATA_CHAMPIONID_3), 754.360779, 660.816162, 412.395996, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
			pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
		}
		if (pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
		{
			if (pInstance->GetData(TYPE_ARGENT_CHALLENGE) == NOT_STARTED)
			{
				switch(urand(0, 1))
				{
					case 0:
						pInstance->SetData(DATA_ARGENT_CHALLENGER, 35119);
					break;
					case 1:
						pInstance->SetData(DATA_ARGENT_CHALLENGER, 34928);
					break;
				}
				pInstance->SetData(TYPE_ARGENT_CHALLENGE, FAIL);
			}
			if (pInstance->GetData(TYPE_ARGENT_CHALLENGE) == FAIL)
			{
				me->SummonCreature(pInstance->GetData(DATA_ARGENT_CHALLENGER), 746.864441, 660.918762, 411.695465, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
				pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
			}
			if (pInstance->GetData(TYPE_ARGENT_CHALLENGE) == DONE)
			{
				if (pInstance->GetData(TYPE_BLACK_KNIGHT) == DONE)
					me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				else
				{
					me->SummonCreature(35451, 746.864441, 660.918762, 411.695465, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
					me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				}
			}
		}
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething()) return;

		if (!pInstance)
            return;
    }
};

CreatureAI* GetAI_npc_toc5_announcer(Creature* pCreature)
{
    return new npc_toc5_announcerAI(pCreature);
}

bool GossipHello_npc_toc5_announcer(Player* pPlayer, Creature* pCreature)
{
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_toc5_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
		((npc_toc5_announcerAI*)pCreature->AI())->StartEvent(pPlayer);
    }

    return true;
}

void AddSC_trial_of_the_champion()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc5_announcer";
	NewScript->GetAI = &GetAI_npc_toc5_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc5_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc5_announcer;
    NewScript->RegisterSelf();
}