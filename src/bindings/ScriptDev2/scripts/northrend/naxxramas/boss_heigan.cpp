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
SDName: Boss_Heigan
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_AGGRO1          -1533109
#define SAY_AGGRO2          -1533110
#define SAY_AGGRO3          -1533111
#define SAY_SLAY            -1533112
#define SAY_TAUNT1          -1533113
#define SAY_TAUNT2          -1533114
#define SAY_TAUNT3          -1533115
#define SAY_TAUNT4          -1533116
#define SAY_TAUNT5          -1533117
#define SAY_DEATH           -1533118

//Spell used by floor peices to cause damage to players
#define SPELL_ERUPTION      29371

//Spells by boss
#define SPELL_DISRUPTION    29310
#define SPELL_FEAVER        29998
#define H_SPELL_FEAVER      55011
#define SPELL_PLAGUED_CLOUD 30122

//Spell by eye stalks
#define SPELL_MIND_FLAY     26143

#define POS_X 2793.86
#define POS_Y -3707.38
#define POS_Z 276.627
#define POS_O 0.593

#define TRIGGER_X 2769
#define TRIGGER_Y -3671
#define TRIGGER_Z 273.667
#define TRIGGER_O 0

struct MANGOS_DLL_DECL boss_heiganAI : public ScriptedAI
{
    boss_heiganAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
	MobEventTasks Tasks;

    uint32 Disruption_Timer;
    uint32 Feaver_Timer;
    uint32 Erupt_Timer;
    uint32 Phase_Timer;
    uint32 Cloud_Timer;

    uint32 eruptSection;
    bool eruptDirection;

    uint8 phase;

    void Reset()
    {
		Tasks.SetObjects(this,me);
        if(me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        phase = 0;

        if(pInstance)
            SetInstanceData(TYPE_HEIGAN, NOT_STARTED);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if(phase != 1)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            DoStartMovement(pWho);
        }

    }
    void SetPhase(uint8 tPhase)
    {
        if(tPhase == 0)
        {
            if(phase == 1)
            {
                phase++;
            }
            else if(phase == 2)
            {
                phase--;
            }
            else phase = 1;
        }else phase = tPhase;

        eruptSection = rand()%4;

        if(phase == 1)
        {
            me->InterruptNonMeleeSpells(false);
            Feaver_Timer = 20000;
            Phase_Timer = 85000;
            Erupt_Timer = 10000;
            Disruption_Timer = 5000+rand()%10000;
            if(me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoStartMovement(me->getVictim());
        }else if(phase == 2)
        {
            me->InterruptNonMeleeSpells(true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->StopMoving();
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveIdle();
            me->GetMap()->CreatureRelocation(me, POS_X, POS_Y, POS_Z, POS_O);
            me->SendMonsterMove(POS_X, POS_Y, POS_Z, SPLINETYPE_NORMAL, SPLINEFLAG_NONE, 0);

            Erupt_Timer = 5000;
            Phase_Timer = 45000;
            Cloud_Timer = 1000;
        }
    }
    void Aggro(Unit *who)
    {
        me->SummonCreature(15384, TRIGGER_X, TRIGGER_Y, TRIGGER_Z, TRIGGER_O, TEMPSUMMON_DEAD_DESPAWN, 0);
        SetPhase(1);
        switch (urand(0,2))
        {
            case 0: DoScriptText(SAY_AGGRO1, me); break;
            case 1: DoScriptText(SAY_AGGRO2, me); break;
            case 2: DoScriptText(SAY_AGGRO3, me); break;
        }

        if(pInstance)
            SetInstanceData(TYPE_HEIGAN, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_SLAY, me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, me);

        if(pInstance)
            SetInstanceData(TYPE_HEIGAN, DONE);
		GiveEmblemsToGroup((!m_bIsRegularMode) ? VAILLANCE : HEROISME);
    }

    void UpdateAI(const uint32 diff)
    {
        if(phase == 0 || CanDoSomething())
            return;

        if (Phase_Timer < diff)
        {
            SetPhase(0);
        }else Phase_Timer -= diff;

		if (Erupt_Timer < diff)
        {
            SetInstanceData(DATA_HEIGAN_ERUPT, eruptSection);

            if (eruptSection == 0)
                eruptDirection = true;
            else if (eruptSection == 3)
                eruptDirection = false;

            eruptDirection ? ++eruptSection : --eruptSection;
            if(phase == 1)
                Erupt_Timer = 10000;
			else 
				Erupt_Timer = 3000;

        }
		else 
			Erupt_Timer -= diff;

        if (phase != 1)
		{
            if (Cloud_Timer < diff)
            {
                DoCast(me, SPELL_PLAGUED_CLOUD);
                Cloud_Timer = 1000;
            }
			else 
				Phase_Timer -= diff;

			if (Disruption_Timer < diff)
			{
				DoCastVictim(SPELL_DISRUPTION);
				Disruption_Timer = 5000+rand()%10000;
			}
			else 
				Disruption_Timer -= diff;

			if (Feaver_Timer < diff)
			{
				DoCastVictim(m_bIsRegularMode ? SPELL_FEAVER : H_SPELL_FEAVER);
				Feaver_Timer = 30000+rand()%10000;
			}
			else 
				Feaver_Timer -= diff;

		}
		else
			DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_heigan_eruptionAI : public ScriptedAI
{
    npc_heigan_eruptionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (pCreature->GetInstanceData());
        Reset();
    }
 
    uint32 phase;
    uint32 safeSpot;
    uint32 fastTimer;
    uint32 phaseTimer;
    uint32 slowTimer;
    bool forward;
    std::list<GameObject*> GetGameObjectsByEntry(uint32 entry)
    {
        CellPair pair(MaNGOS::ComputeCellPair(me->GetPositionX(), me->GetPositionY()));
        Cell cell(pair);
        cell.data.Part.reserved = ALL_DISTRICT;
        cell.SetNoCreate();
 
        std::list<GameObject*> gameobjectList;
 
        AllGameObjectsWithEntryInRange check(me, entry, 100);
        MaNGOS::GameObjectListSearcher<AllGameObjectsWithEntryInRange> searcher(me, gameobjectList, check);
        TypeContainerVisitor<MaNGOS::GameObjectListSearcher<AllGameObjectsWithEntryInRange>, GridTypeMapContainer> visitor(searcher);
 
        cell.Visit(pair, visitor, *(me->GetMap()));
 
        return gameobjectList;
    }
    //Let's Dance!
    void DoErupt(uint32 safePlace)
    {
        uint64 heiganGUID = pInstance->GetData64(NPC_HEIGAN);
        Map::PlayerList const &PlList = pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return;
 
        if(safePlace != 1)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181678);
            //Visual part of eruption
            for (int32 i = 181510; i <= 181526; i++)
            {
                if (i == 181513 || i == 181512 || i == 181511 || i == 181525 || i == 181514 || i == 181515 || i == 181516)
                    continue;
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                        //Required GO Custom Animation Patch for this
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            //Damage part of eruption
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;
 
                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 8.0f)
                                //We use originalCaster for deal damage by Plague Fissure
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        }
        //Change direction of dance
        else forward = true;
        if(safePlace != 2)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181676);
            for (int32 i = 181511; i <= 181531; i++)
            {
                if ((i > 181516 && i < 181525) || (i == 181526))
                    continue;
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;
 
                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 8.0f)
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        }
        if(safePlace != 3)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181677);
            for (int32 i = 181532; i <= 181545; i++)
            {
                if (i >= 181537 && i <= 181539)
                    continue;
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;
 
                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 8.0f)
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        }
        if(safePlace != 4)
        {
            std::list<GameObject*> eruptGOs = GetGameObjectsByEntry(181695);
            for (int32 i = 181537; i <= 181552; i++)
            {
                if (i > 181539 && i < 181545)
                    continue;
                std::list<GameObject*> visualGO = GetGameObjectsByEntry(i);
                for (std::list<GameObject*>::iterator itr = visualGO.begin(); itr != visualGO.end(); ++itr)
                {
                    if((*itr))
                    {
                        WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM,8+4);
                        data << (*itr)->GetGUID();
                        data << 0;
                        (*itr)->SendMessageToSet(&data,true);
                    }
                }
            }
            for (std::list<GameObject*>::iterator itr = eruptGOs.begin(); itr != eruptGOs.end(); ++itr)
            {
                if(!(*itr))
                    continue;
                for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* pPlayer = i->getSource())
                    {
                        if (pPlayer->isGameMaster())
                            continue;
 
                        if (pPlayer->isAlive())
                        {
                            if(pPlayer->GetDistance((*itr)) <= 8.0f)
                                DoCast(pPlayer, SPELL_ERUPTION, true);
                        }
                    }
                }
            }
        //Let's dance back!
        }else forward=false;
    }
 
    void Reset()
    {
        phase = 1;
        safeSpot = 1;
        fastTimer = 3500;
        slowTimer = 10500;
        phaseTimer = 90000;
    }
    void Aggro(Unit* who)
    {
        //This is just for dance. It doesn't attack anybody.
        DoStopAttack();
        SetCombatMovement(false);
    }
    void JustDied(Unit* who)
    {
        //If dance mob was somehow killed - respawn him.
        me->Respawn();
    }
    void UpdateAI(const uint32 diff)
    {
        if(me->GetMapId() != 533)
            return;

        if(pInstance->GetData(TYPE_HEIGAN) != IN_PROGRESS)
        {
            me->ForcedDespawn();
        }
 
        if (phase == 1)
        {
            if(phaseTimer < diff)
            {
                // Let's fast dance
                phase = 2;
                phaseTimer = 45000;
                safeSpot = 1;
            }else phaseTimer-=diff;
            if(slowTimer < diff)
            {
                DoErupt(safeSpot);
                if(forward)
                    safeSpot++;
                else
                    safeSpot--;
                slowTimer = 10500;
            }else slowTimer-=diff;
        }
        else if(phase == 2)
        {
            if(phaseTimer < diff)
            {
                // Slow dance again
                phase = 1;
                AttackStart(me->getVictim());
                phaseTimer = 90000;
                safeSpot = 1;
            }else phaseTimer-=diff;
            if(fastTimer < diff)
            {
                DoErupt(safeSpot);
                if(forward)
                    safeSpot++;
                else
                    safeSpot--;
                fastTimer = 3500;
            }else fastTimer-=diff;
        }
    }
};

CreatureAI* GetAI_boss_heigan(Creature* pCreature)
{
    return new boss_heiganAI(pCreature);
}

CreatureAI* GetAI_npc_heigan_eruptionAI(Creature* pCreature)
{
    return new npc_heigan_eruptionAI(pCreature);
}

void AddSC_boss_heigan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_heigan";
    newscript->GetAI = &GetAI_boss_heigan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_heigan_eruption";
    newscript->GetAI = &GetAI_npc_heigan_eruptionAI;
    newscript->RegisterSelf();
}
