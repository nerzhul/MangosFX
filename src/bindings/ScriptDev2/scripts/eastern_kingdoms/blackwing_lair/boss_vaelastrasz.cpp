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
SDName: Boss_Vaelastrasz
SD%Complete: 75
SDComment: Burning Adrenaline not correctly implemented in core
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"

#define SAY_LINE1           -1469026
#define SAY_LINE2           -1469027
#define SAY_LINE3           -1469028
#define SAY_HALFLIFE        -1469029
#define SAY_KILLTARGET      -1469030

#define GOSSIP_ITEM         "Start Event <Needs Gossip Text>"

enum Spells
{
	SPELL_ESSENCEOFTHERED      = 23513,
	SPELL_FLAMEBREATH          = 23461,
	SPELL_FIRENOVA             = 23462,
	SPELL_TAILSWIPE            = 15847,
	SPELL_BURNINGADRENALINE    = 23620,
	SPELL_CLEAVE               = 20684                   //Chain cleave is most likely named something different and contains a dummy effect
};
struct MANGOS_DLL_DECL boss_vaelAI : public LibDevFSAI
{
    boss_vaelAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        pCreature->SetUInt32Value(UNIT_NPC_FLAGS,1);
        pCreature->setFaction(35);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        InitIA();
        AddEventOnTank(SPELL_CLEAVE,8000,15000);
        AddEventOnTank(SPELL_FLAMEBREATH,11000,4000,4000);
        AddEvent(SPELL_BURNINGADRENALINE,15000,15000,0,TARGET_RANDOM,0,0,true,3);
        AddEventOnTank(SPELL_FIRENOVA,5000,5000);
    }

    uint64 PlayerGUID;
    uint32 SpeachTimer;
    uint32 SpeachNum;
    uint32 BurningAdrenalineTank_Timer;
    uint32 TailSwipe_Timer;
    bool HasYelled;
    bool DoingSpeach;

    void Reset()
    {
        PlayerGUID = 0;
        SpeachTimer = 0;
        SpeachNum = 0;
        BurningAdrenalineTank_Timer = 45000;
        TailSwipe_Timer = 20000;
        HasYelled = false;
        DoingSpeach = false;
    }

    void BeginSpeach(Unit* target)
    {
        //Stand up and begin speach
        PlayerGUID = target->GetGUID();

        //10 seconds
        DoScriptText(SAY_LINE1, me);

        SpeachTimer = 10000;
        SpeachNum = 0;
        DoingSpeach = true;
    }

    void KilledUnit(Unit *victim)
    {
        if (urand(0, 4))
            return;

        DoScriptText(SAY_KILLTARGET, me, victim);
    }

    void Aggro(Unit* pWho)
    {
        DoCastMe(SPELL_ESSENCEOFTHERED);
        me->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        //Speach
        if (DoingSpeach)
        {
            if (SpeachTimer < diff)
            {
                switch (SpeachNum)
                {
                    case 0:
                        //16 seconds till next line
                        DoScriptText(SAY_LINE2, me);
                        SpeachTimer = 16000;
                        ++SpeachNum;
                        break;
                    case 1:
                        //This one is actually 16 seconds but we only go to 10 seconds because he starts attacking after he says "I must fight this!"
                        DoScriptText(SAY_LINE3, me);
                        SpeachTimer = 10000;
                        ++SpeachNum;
                        break;
                    case 2:
                        me->setFaction(103);
                        me->SetHealth(int(me->GetMaxHealth()*.3));
                        if (PlayerGUID && Unit::GetUnit((*me),PlayerGUID))
                        {
                            AttackStart(Unit::GetUnit((*me),PlayerGUID));
                            DoCastMe(SPELL_ESSENCEOFTHERED);
                        }
                        SpeachTimer = 0;
                        DoingSpeach = false;
                        break;
                }
            }else SpeachTimer -= diff;
        }

        //Return since we have no target
        if (!CanDoSomething())
            return;

        // Yell if hp lower than 15%
        if (CheckPercentLife(15) && !HasYelled)
        {
            DoScriptText(SAY_HALFLIFE, me);
            HasYelled = true;
        }

        //BurningAdrenalineTank_Timer
        if (BurningAdrenalineTank_Timer < diff)
        {
            // have the victim cast the spell on himself otherwise the third effect aura will be applied
            // to Vael instead of the player
            me->getVictim()->CastSpell(me->getVictim(),SPELL_BURNINGADRENALINE,1);

            BurningAdrenalineTank_Timer = 45000;
        }else BurningAdrenalineTank_Timer -= diff;

        //TailSwipe_Timer
        if (TailSwipe_Timer < diff)
        {
            //Only cast if we are behind
            if (!me->HasInArc(M_PI, me->getVictim()))
            {
				DoCastVictim(SPELL_TAILSWIPE);
            }

            TailSwipe_Timer = 20000;
        }else TailSwipe_Timer -= diff;
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

void SendDefaultMenu_boss_vael(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)               //Fight time
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        ((boss_vaelAI*)pCreature->AI())->BeginSpeach((Unit*)pPlayer);
    }
}

bool GossipSelect_boss_vael(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN)
        SendDefaultMenu_boss_vael(pPlayer, pCreature, uiAction);

    return true;
}

bool GossipHello_boss_vael(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM        , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(907, pCreature->GetGUID());

    return true;
}

CreatureAI* GetAI_boss_vael(Creature* pCreature)
{
    return new boss_vaelAI(pCreature);
}

void AddSC_boss_vael()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_vaelastrasz";
    newscript->GetAI = &GetAI_boss_vael;
    newscript->pGossipHello = &GossipHello_boss_vael;
    newscript->pGossipSelect = &GossipSelect_boss_vael;
    newscript->RegisterSelf();
}
