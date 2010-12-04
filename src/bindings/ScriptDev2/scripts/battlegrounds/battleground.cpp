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
SDName: Battleground
SD%Complete: 100
SDComment: Spirit guides in battlegrounds will revive all players every 30 sec
SDCategory: Battlegrounds
EndScriptData */

#include "precompiled.h"

// **** Script Info ****
// Spiritguides in battlegrounds resurrecting many players at once
// every 30 seconds - through a channeled spell, which gets autocasted
// the whole time
// if spiritguide despawns all players around him will get teleported
// to the next spiritguide
// here i'm not sure, if a dummyspell exist for it

// **** Quick Info ****
// battleground spiritguides - this script handles gossipHello
// and JustDied also it let autocast the channel-spell

enum
{
    SPELL_SPIRIT_HEAL_CHANNEL       = 22011,                // Spirit Heal Channel

    SPELL_SPIRIT_HEAL               = 22012,                // Spirit Heal
    SPELL_SPIRIT_HEAL_MANA          = 44535,                // in battlegrounds player get this no-mana-cost-buff

    SPELL_WAITING_TO_RESURRECT      = 2584                  // players who cancel this aura don't want a resurrection
};

struct MANGOS_DLL_DECL npc_spirit_guideAI : public ScriptedAI
{
    npc_spirit_guideAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
		me->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, me->GetGUID());
		me->SetUInt32Value(UNIT_CHANNEL_SPELL, 22011);
		me->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        // auto cast the whole time this spell
        if (!me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            me->CastSpell(me, SPELL_SPIRIT_HEAL_CHANNEL, false);
    }

    void CorpseRemoved(uint32 &)
    {
        // TODO: would be better to cast a dummy spell
        Map* pMap = me->GetMap();

        if (!pMap || !pMap->IsBattleGround())
            return;

        Map::PlayerList const &PlayerList = pMap->GetPlayers();

        for(Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (!pPlayer || !pPlayer->IsWithinDistInMap(me, 20.0f) || !pPlayer->HasAura(SPELL_WAITING_TO_RESURRECT))
                continue;

            // repop player again - now this node won't be counted and another node is searched
            pPlayer->RepopAtGraveyard();
        }
    }

    void SpellHitTarget (Unit* pUnit, const SpellEntry* pSpellEntry)
    {
        if (pSpellEntry->Id == SPELL_SPIRIT_HEAL && pUnit->GetTypeId() == TYPEID_PLAYER
            && pUnit->HasAura(SPELL_WAITING_TO_RESURRECT))
		{
            pUnit->CastSpell(pUnit, SPELL_SPIRIT_HEAL_MANA, true);
			// LittleHack for rez bug in BG
			if(pUnit->GetTypeId() == TYPEID_PLAYER)
				((Player*)pUnit)->ResurrectPlayer(1.0f);
		}
    }
};

bool GossipHello_npc_spirit_guide(Player* pPlayer, Creature* pCreature)
{
    pPlayer->CastSpell(pPlayer, SPELL_WAITING_TO_RESURRECT, true);
	pPlayer->AddPlayerToWintergraspQueue(pCreature->GetGUID());
    return true;
}

CreatureAI* GetAI_npc_spirit_guide(Creature* pCreature)
{
    return new npc_spirit_guideAI(pCreature);
}

enum VehicleType
{
	CATAPULT	=	0,	// 56663
	DEMOLISHER	=	1,	// 56575
	SIEGE		=	2	// all : 56661 horde : 61408
};

bool GoHello_wg_engineer( Player *pPlayer, Creature* pCreature)
{
	if(pPlayer->CanCreateWGVehicle())
	{
		if(pPlayer->HasAura(33280) || pPlayer->HasAura(55629) || pPlayer->isGameMaster())
		{
			pPlayer->ADD_GOSSIP_ITEM(0, "Construire une catapulte", GOSSIP_SENDER_MAIN, CATAPULT);
			pPlayer->ADD_GOSSIP_ITEM(0, "Construire un demolisseur", GOSSIP_SENDER_MAIN, DEMOLISHER);
		}
		if(pPlayer->HasAura(55629) || pPlayer->isGameMaster())
			pPlayer->ADD_GOSSIP_ITEM(0, "Construire un engin de siege", GOSSIP_SENDER_MAIN, SIEGE);
	}
	pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
    return true;
}

bool GOSelect_wg_engineer( Player *pPlayer, Creature* pCreature, uint32 sender, uint32 action )
{
    if(sender != GOSSIP_SENDER_MAIN) 
		return true;
    if(!pPlayer->getAttackers().empty()) 
		return true;

    switch(action)
    {
		case CATAPULT:
			if(pPlayer->CanCreateWGVehicle())
				pPlayer->CastSpell(pPlayer,56663,false);
			break;
		case DEMOLISHER:
			if(pPlayer->CanCreateWGVehicle())
				pPlayer->CastSpell(pPlayer,56575,false);
			break;
		case SIEGE:
			if(pPlayer->CanCreateWGVehicle())
				pPlayer->CastSpell(pPlayer,pPlayer->GetTeam() == ALLIANCE ? 56661 : 61408, false);
			break;
    }
	pPlayer->CLOSE_GOSSIP_MENU();
	return true;
}

void AddSC_battleground()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_spirit_guide";
    newscript->GetAI = &GetAI_npc_spirit_guide;
    newscript->pGossipHello = &GossipHello_npc_spirit_guide;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "wg_engineer";
    newscript->pGossipHello = &GoHello_wg_engineer;
	newscript->pGossipSelect = &GOSelect_wg_engineer;
    newscript->RegisterSelf();
}
