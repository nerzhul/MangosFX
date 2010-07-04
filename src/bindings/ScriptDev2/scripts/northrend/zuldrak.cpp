/*
 * Copyright (C) 2009 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "precompiled.h"

/*####
## npc_drakuru_shackles
####*/

enum eDrakuruShackles
{
    SPELL_LEFT_CHAIN           = 59951,
    SPELL_RIGHT_CHAIN          = 59952,
    SPELL_UNLOCK_SHACKLE       = 55083,
    SPELL_FREE_RAGECLAW        = 55223,

    NPC_RAGECLAW               = 29686
};

struct MANGOS_DLL_DECL npc_drakuru_shacklesAI : public ScriptedAI
{
    npc_drakuru_shacklesAI(Creature *c) : ScriptedAI(c) {}

    Unit* Rageclaw;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        float x, y, z;
        Rageclaw = NULL;
        me->GetClosePoint(x, y, z, me->GetObjectBoundingRadius()/3,0.1);
        if (Unit* summon = me->SummonCreature(NPC_RAGECLAW,x,y,z,0,TEMPSUMMON_DEAD_DESPAWN,1000))
            DoActionOnRageclaw(true,summon);
    }

    void DoActionOnRageclaw(bool locking, Unit *who)
    {
        if (!who)
            return;

        if (locking)
        {
            if (who)
            {
                Rageclaw = who;

                me->SetInFront(Rageclaw);
                Rageclaw->SetInFront(me);

                DoCast(Rageclaw, SPELL_LEFT_CHAIN, true);
                DoCast(Rageclaw, SPELL_RIGHT_CHAIN, true);
            }
        }
        else
        {
            DoCast(Rageclaw, SPELL_FREE_RAGECLAW, true);
            who->CastSpell(Rageclaw, SPELL_UNLOCK_SHACKLE, true);
            me->setDeathState(DEAD);
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_UNLOCK_SHACKLE)
        {
            if (Rageclaw)
                DoActionOnRageclaw(false,caster);
            else
                me->setDeathState(JUST_DIED);

        }
    }
};

CreatureAI* GetAI_npc_drakuru_shackles(Creature* pCreature)
{
    return new npc_drakuru_shacklesAI (pCreature);
}

/*####
## npc_captured_rageclaw
####*/

enum eRageclaw
{
    SPELL_UNSHACKLED           = 55085,
    SPELL_KNEEL                = 39656
};

const char * SAY_RAGECLAW_1 =      "I poop on you, trollses!";
const char * SAY_RAGECLAW_2 =      "ARRRROOOOGGGGAAAA!";
const char * SAY_RAGECLAW_3 =      "No more mister nice wolvar!";

struct MANGOS_DLL_DECL npc_captured_rageclawAI : public ScriptedAI
{
    npc_captured_rageclawAI(Creature *c) : ScriptedAI(c) {}

    uint32 DespawnTimer;
    bool Despawn;

    void Reset()
    {
        Despawn = false;
        DespawnTimer = 0;
        me->setFaction(35);
        DoCastMe( SPELL_KNEEL, true); // Little Hack for kneel - Thanks Illy :P
    }

    void MoveInLineOfSight(Unit *who){}

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_FREE_RAGECLAW)
        {
            me->RemoveAurasDueToSpell(SPELL_LEFT_CHAIN);

            me->RemoveAurasDueToSpell(SPELL_RIGHT_CHAIN);

            me->RemoveAurasDueToSpell(SPELL_KNEEL);

            me->setFaction(me->GetCreatureInfo()->faction_H);

            DoCastMe( SPELL_UNSHACKLED, true);
			const char* tmpTxt;
			switch(urand(0,2))
			{
				case 0:
					tmpTxt = SAY_RAGECLAW_1;
					break;
				case 1:
					tmpTxt = SAY_RAGECLAW_2;
					break;
				case 2:
					tmpTxt = SAY_RAGECLAW_3;
					break;
			}
            me->MonsterSay(tmpTxt, LANG_UNIVERSAL, NULL);
            me->GetMotionMaster()->MoveConfused();

            DespawnTimer = 10000;
            Despawn = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (me->getVictim())
        {
            DoMeleeAttackIfReady();
            return;
        }

        if (!Despawn)
            return;

        if (DespawnTimer <= diff)
            me->setDeathState(JUST_DIED);
        else DespawnTimer-=diff;
   }
};

CreatureAI* GetAI_npc_captured_rageclaw(Creature* pCreature)
{
    return new npc_captured_rageclawAI (pCreature);
}

/*####
## npc_gymer
####*/

#define    GOSSIP_ITEM_G "I'm ready, Gymer. Let's go!"

enum eGymer
{
    QUEST_STORM_KING_VENGEANCE    = 12919,
    SPELL_GYMER                   = 55568
};

    bool GossipHello_npc_gymer(Player *pPlayer, Creature *pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

        if (pPlayer->GetQuestStatus(QUEST_STORM_KING_VENGEANCE) == QUEST_STATUS_INCOMPLETE)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_G, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(13640, pCreature->GetGUID());
        }

        return true;
    }

    bool GossipSelect_npc_gymer(Player *pPlayer, Creature *pCreature, uint32 uiSender, uint32 uiAction)
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_GYMER, true);
        }

        return true;
    }

void AddSC_zuldrak()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_drakuru_shackles";
    newscript->GetAI = &GetAI_npc_drakuru_shackles;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_captured_rageclaw";
    newscript->GetAI = &GetAI_npc_captured_rageclaw;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_gymer";
    newscript->pGossipHello = &GossipHello_npc_gymer;
    newscript->pGossipSelect = &GossipSelect_npc_gymer;
    newscript->RegisterSelf();
}
