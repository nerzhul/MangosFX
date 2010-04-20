/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Netherspite
SD%Complete: 65%
SDComment: Incomplete: void zone, red beam, netherbreath timer
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"
#include "karazhan.h"
#include "GameObject.h"

#define SPELL_NETHERBURN        30523
//#define SPELL_NETHERBURN_2    30522
#define SPELL_VOID_ZONE         37063
#define SPELL_NETHERBREATH      38523
#define SPELL_EMPOWERMENT       38549
#define SPELL_NETHER_INFUSION   38688
#define SPELL_NETHERSPITE_ROAR  38684

#define SPELL_PERSEVERENCE_NS       30466
#define SPELL_PERSEVERENCE_PLR      30421
#define SPELL_SERENITY_NS           30467
#define SPELL_SERENITY_PLR          30422
#define SPELL_DOMINANCE_NS          30468
#define SPELL_DOMINANCE_PLR         30423
#define SPELL_BEAM_DOM              30402
#define SPELL_BEAM_SER              30464
#define SPELL_BEAM_PER              30465
#define BLUE_PORTAL                 30491
#define GREEN_PORTAL                30490
#define RED_PORTAL                  30487
#define SPELL_EXHAUSTION_DOM        38639
#define SPELL_EXHAUSTION_SER        38638
#define SPELL_EXHAUSTION_PER        38637

#define SPELL_BANISH_VISUAL         39833
#define SPELL_ROOT                  42716

//emotes
#define EMOTE_PHASE_PORTAL          -1532089
#define EMOTE_PHASE_BANISH          -1532090


float Basic_Coordinates[3][3] =
{
    {-11094.493164, -1591.969238, 279.949188},
    {-11195.353516, -1613.237183, 278.237258},
    {-11137.846680, -1685.607422, 278.239258}
};

float Fixed_Coordinates[3][3] = 
{
    {-11094.493164, -1591.969238, 279.949188},
    {-11195.353516, -1613.237183, 278.237258},
    {-11137.846680, -1685.607422, 278.239258}
};

enum Phases
{
    NULL_PHASE = 0,
    PORTAL_PHASE = 1,
    BANISH_PHASE = 2,
};

enum Events
{
    NULL_EVENT = 0,
    EVENT_ENRAGE = 1,
    EVENT_EXHAUST_CHECK = 2,

    //portal phase events
    EVENT_PORTAL_END_TIMER = 3,
    EVENT_BEAM_INITIAL = 4,
    EVENT_BEAM_PERIODIC = 5,
    EVENT_NETHERBURN = 6,
    EVENT_VOID_ZONE = 7,

    //banish phase events
    EVENT_BANISH_END_TIMER = 3,
    EVENT_NETHERBREATH = 4,
};

static int Beam_Buff[3][2] = 
{
    {SPELL_DOMINANCE_NS,SPELL_DOMINANCE_PLR},
    {SPELL_SERENITY_NS,SPELL_SERENITY_PLR},
    {SPELL_PERSEVERENCE_NS,SPELL_PERSEVERENCE_PLR}
};

static int Beam_Debuff[3] = 
{
    SPELL_EXHAUSTION_DOM,
    SPELL_EXHAUSTION_SER,
    SPELL_EXHAUSTION_PER
};

static int Portals[3] =
{
    BLUE_PORTAL,
    GREEN_PORTAL,
    RED_PORTAL
};

static int Beam_Visuals[3] = 
{
    SPELL_BEAM_DOM,
    SPELL_BEAM_SER,
    SPELL_BEAM_PER
};

static int MaxTimeCounter[3] = {0,8,5};

struct MANGOS_DLL_DECL boss_netherspiteAI : public ScriptedAI
{
    boss_netherspiteAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    Phases Active_Phase;
    Events Active_Event;
    uint32 uiTimers[9], uiHp_bonus;
    float m_fDISTANCE, p_fDISTANCE, m_fANGLE, p_fANGLE, fRadian;
    Unit* pCurrent_Candidate[3];
    Unit* pLast_Candidate[3];
    Unit* pBeamer[3];
    Unit* pBeamer_Helper[3];
    Unit* pExhaust_Candidates[3][10];
    Unit* pHealth_Modified[10];
    Unit* pPlayer;
    bool bExists[3], bHpIncreased;
    int bmcolor[2];

    //0: blue
    //1: green
    //2: red

    void Reset()
    {
        if (m_pInstance)
        {
            //door opens
            m_pInstance->SetData(TYPE_NETHERSPITE, NOT_STARTED);
            if (GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_MASSIVE_DOOR)))
                pDoor->SetGoState(GO_STATE_ACTIVE);
        }

        DestroyPortals();
        Active_Phase = NULL_PHASE;
        Active_Event = NULL_EVENT;
        uiHp_bonus = 31000;
        me->RemoveAurasDueToSpell(SPELL_ROOT);
        me->RemoveAurasDueToSpell(SPELL_BANISH_VISUAL);
        fRadian = 0.3; //0.174;

        for(int i=0;i<3;i++)
        {
            pCurrent_Candidate[i] = me;
            pLast_Candidate[i] = me;
            pBeamer[i] = NULL;
            pBeamer_Helper[i] = NULL;
            pHealth_Modified[i] = NULL;
            for(int j=0;j<10;j++)
            {
                pExhaust_Candidates[i][j] = NULL;
            }
        }
    }

    void Shuffle_Coordinates()
    {
        //we shuffle the portal locations here
        int RANGE_MAX2 = 2;
        int RANGE_MIN3 = 0;
        int RANGE_MAX1a = 2;
        int RANGE_MIN2a = 1;
        int RANGE_MAX1b = 1;
        int RANGE_MIN2b = 0;
        int color1 = (((double) rand() / (double) RAND_MAX) * RANGE_MAX2 + RANGE_MIN3);
        int color2a = (((double) rand() / (double) RAND_MAX) * RANGE_MAX1a + RANGE_MIN2a);
        int color2b = (((double) rand() / (double) RAND_MAX) * RANGE_MAX1b + RANGE_MIN2b);

        bmcolor[0] = color1;

        if(bmcolor[0] == 0) {
            Fixed_Coordinates[0][0] = Basic_Coordinates[0][0];
            Fixed_Coordinates[0][1] = Basic_Coordinates[0][1];
            Fixed_Coordinates[0][2] = Basic_Coordinates[0][2];
            bmcolor[1] = color2a;

            if(bmcolor[1] == 1) {
                Fixed_Coordinates[1][0] = Basic_Coordinates[1][0];
                Fixed_Coordinates[1][1] = Basic_Coordinates[1][1];
                Fixed_Coordinates[1][2] = Basic_Coordinates[1][2];
                Fixed_Coordinates[2][0] = Basic_Coordinates[2][0];
                Fixed_Coordinates[2][1] = Basic_Coordinates[2][1];
                Fixed_Coordinates[2][2] = Basic_Coordinates[2][2];
            }
            else {
                Fixed_Coordinates[1][0] = Basic_Coordinates[2][0];
                Fixed_Coordinates[1][1] = Basic_Coordinates[2][1];
                Fixed_Coordinates[1][2] = Basic_Coordinates[2][2];
                Fixed_Coordinates[2][0] = Basic_Coordinates[1][0];
                Fixed_Coordinates[2][1] = Basic_Coordinates[1][1];
                Fixed_Coordinates[2][2] = Basic_Coordinates[1][2];
            }
        }

        if(bmcolor[0] == 2)
         {
            Fixed_Coordinates[0][0] = Basic_Coordinates[2][0];
            Fixed_Coordinates[0][1] = Basic_Coordinates[2][1];
            Fixed_Coordinates[0][2] = Basic_Coordinates[2][2];
            bmcolor[1] = color2b;

            if(bmcolor[1] == 0) {
                Fixed_Coordinates[1][0] = Basic_Coordinates[0][0];
                Fixed_Coordinates[1][1] = Basic_Coordinates[0][1];
                Fixed_Coordinates[1][2] = Basic_Coordinates[0][2];
                Fixed_Coordinates[2][0] = Basic_Coordinates[1][0];
                Fixed_Coordinates[2][1] = Basic_Coordinates[1][1];
                Fixed_Coordinates[2][2] = Basic_Coordinates[1][2];
            }
            else {
                Fixed_Coordinates[1][0] = Basic_Coordinates[1][0];
                Fixed_Coordinates[1][1] = Basic_Coordinates[1][1];
                Fixed_Coordinates[1][2] = Basic_Coordinates[1][2];
                Fixed_Coordinates[2][0] = Basic_Coordinates[0][0];
                Fixed_Coordinates[2][1] = Basic_Coordinates[0][1];
                Fixed_Coordinates[2][2] = Basic_Coordinates[0][2];
            }
        }

        if(bmcolor[0] == 1)
        {
            Fixed_Coordinates[0][0] = Basic_Coordinates[1][0];
            Fixed_Coordinates[0][1] = Basic_Coordinates[1][1];
            Fixed_Coordinates[0][2] = Basic_Coordinates[1][2];
            Fixed_Coordinates[1][0] = Basic_Coordinates[2][0];
            Fixed_Coordinates[1][1] = Basic_Coordinates[2][1];
            Fixed_Coordinates[1][2] = Basic_Coordinates[2][2];
            Fixed_Coordinates[2][0] = Basic_Coordinates[0][0];
            Fixed_Coordinates[2][1] = Basic_Coordinates[0][1];
            Fixed_Coordinates[2][2] = Basic_Coordinates[0][2];
        }
    }

    void Nether_Beam(int color)
    {
        pCurrent_Candidate[color] = me;

        ThreatList const& m_threatlist = me->getThreatManager().getThreatList();

        for (ThreatList::const_iterator itr = m_threatlist.begin();itr != m_threatlist.end(); ++itr)
        {
            //here we calculate possible player blockers, else beam goes to netherspite
            pPlayer = Unit::GetUnit((*me), (*itr)->getUnitGuid());
            m_fDISTANCE = me->GetDistance(pBeamer[color]);
            m_fANGLE = me->GetAngle(pBeamer[color]);
            p_fDISTANCE = pPlayer->GetDistance(pBeamer[color]);
            p_fANGLE = pPlayer->GetAngle(pBeamer[color]);

            if((m_fANGLE - fRadian) < p_fANGLE && p_fANGLE < (m_fANGLE + fRadian))
                if(p_fDISTANCE < m_fDISTANCE)
                    if(!pPlayer->HasAura(Beam_Debuff[color],0))
                        pCurrent_Candidate[color] = pPlayer;
        }

        if(pCurrent_Candidate[color] == me)
        {
            if(pCurrent_Candidate[color] != pLast_Candidate[color])
            {
                //we need to kill the beamer, or else the beam visual doesn't disappear
                pBeamer[color]->DealDamage(pBeamer_Helper[color], pBeamer_Helper[color]->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                pBeamer_Helper[color] = pBeamer[color]->SummonCreature(1557,Fixed_Coordinates[color][0],Fixed_Coordinates[color][1],Fixed_Coordinates[color][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
                pBeamer_Helper[color]->CastSpell(pBeamer_Helper[color], SPELL_ROOT, true);
                pBeamer_Helper[color]->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
                pBeamer_Helper[color]->setFaction(me->getFaction());
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
                pLast_Candidate[color] = pCurrent_Candidate[color];
           }
                pBeamer[color]->CastSpell(pCurrent_Candidate[color],Beam_Buff[color][0],true);
                pBeamer_Helper[color]->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, pCurrent_Candidate[color]->GetGUID());
                pBeamer_Helper[color]->SetUInt32Value(UNIT_CHANNEL_SPELL, Beam_Visuals[color]);
        }
        else
        {
            if(pCurrent_Candidate[color] != pLast_Candidate[color])
            {
                pBeamer[color]->DealDamage(pBeamer_Helper[color], pBeamer_Helper[color]->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                pBeamer_Helper[color] = pBeamer[color]->SummonCreature(1557,Fixed_Coordinates[color][0],Fixed_Coordinates[color][1],Fixed_Coordinates[color][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
                pBeamer_Helper[color]->CastSpell(pBeamer_Helper[color], SPELL_ROOT, true);
                pBeamer_Helper[color]->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
                pBeamer_Helper[color]->setFaction(me->getFaction());
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                pBeamer_Helper[color]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
                Exhaust_Handler(pCurrent_Candidate[color],color);
                if(color==2)
                {
                    hpIncrease_Handler(pCurrent_Candidate[color]);
                    //me->AddThreat(PERcandidate, 100000.0f);   //not tested if it works
                }
                pLast_Candidate[color] = pCurrent_Candidate[color];
            }
                pCurrent_Candidate[color]->CastSpell(pCurrent_Candidate[color],Beam_Buff[color][1],true);
                pBeamer_Helper[color]->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, pCurrent_Candidate[color]->GetGUID());
                pBeamer_Helper[color]->SetUInt32Value(UNIT_CHANNEL_SPELL, Beam_Visuals[color]);
        }
    }

    void Exhaust_Handler(Unit *pPlayer, int color)
    {
        //storing the possible players that will get the exhaustion debuff
        for(int i=0;i<3;i++)
            bExists[i] = false;

        for(int i=0;i<10;i++)
        {
            if(pExhaust_Candidates[color][i] == pPlayer)
            {
                bExists[color] = true;
                break;
            }
        }

        if(!bExists[color])
        {
            for(int i=0;i<10;i++)
            {
                if(pExhaust_Candidates[color][i] == NULL)
                {
                    pExhaust_Candidates[color][i] = pPlayer;
                    break;
                }
            }
        }
    }

    void hpIncrease_Handler(Unit *pPlayer)
    {
        //experimental function to increase max hit points of red beam blocker and store them for future reference
        bHpIncreased = false;
        for(int i=0;i<10;i++)
        {
            if(pHealth_Modified[i] == pPlayer)
            {
                bHpIncreased = true;
                break;
            }
        }

        if(!bHpIncreased)
        {
           for(int i=0;i<10;i++)
            {
                if(pHealth_Modified[i] == NULL)
                {
                    pHealth_Modified[i] = pPlayer;
                    //pHealth_Modified[i]->GetUInt32Value(UNIT_FIELD_MAXHEALTH) =  pHealth_Modified[i]->GetUInt32Value(UNIT_FIELD_MAXHEALTH) + uiHp_bonus; // this doesn't work, we need to add 31000 hp to red blockers.
                }
            }
        }
    }

    void Apply_Exhaustion(int color)
    {
        for(int i=0;i<10;i++)
        {
            if(pExhaust_Candidates[color][i] != NULL)
            {
                if(!pExhaust_Candidates[color][i]->HasAura(Beam_Buff[color][1],0))
                {
                    if(color == 2)
                    {
                        for(int j=0;j<10;j++)
                        {
                            if(pHealth_Modified[j] == pExhaust_Candidates[color][i])
                            {
                                pHealth_Modified[j]->SetMaxHealth(pHealth_Modified[j]->GetMaxHealth());
                                pHealth_Modified[j] = NULL;
                                break;
                            }
                        }
                    }
                pExhaust_Candidates[color][i]->CastSpell(pExhaust_Candidates[color][i],Beam_Debuff[color],true);
                pExhaust_Candidates[color][i] = NULL;
                }
            }
        }
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NETHERSPITE, IN_PROGRESS);

        if (m_pInstance)
        {
            //door closes
            if (GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_MASSIVE_DOOR)))
                pDoor->SetGoState(GO_STATE_READY);
        }
        SwitchPhase();
        DoMeleeAttackIfReady();
    }

    void DestroyPortals()
    {
        for(int i=0;i<3;i++)
        {
            if (pBeamer[i] && pBeamer[i]->isAlive())
                pBeamer[i]->DealDamage(pBeamer[i], pBeamer[i]->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        for(int i=0;i<3;i++)
        {
            if (pBeamer_Helper[i] && pBeamer_Helper[i]->isAlive())
                pBeamer_Helper[i]->DealDamage(pBeamer_Helper[i], pBeamer_Helper[i]->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void SwitchPhase()
    {
        switch(Active_Phase)
        {
        case NULL_PHASE: //initialize
            Active_Phase = PORTAL_PHASE;
            SpawnPortals();
            AttackStart(me->getVictim());
            DoScriptText(EMOTE_PHASE_PORTAL,me);
            DoCast(me,SPELL_EMPOWERMENT);
            uiTimers[EVENT_ENRAGE] = 540000;
            uiTimers[EVENT_EXHAUST_CHECK] = 2000;
            uiTimers[EVENT_PORTAL_END_TIMER] = 60000;
            uiTimers[EVENT_BEAM_INITIAL] = 10000;
            uiTimers[EVENT_BEAM_PERIODIC] = 0;
            uiTimers[EVENT_NETHERBURN] = 5000;
            uiTimers[EVENT_VOID_ZONE] = 15000;
            break;
        case PORTAL_PHASE:  //enter banish phase
            Active_Phase = BANISH_PHASE;
            me->RemoveAllAuras();
            DoCast(me, SPELL_BANISH_VISUAL, true);
            DoCast(me, SPELL_ROOT, true);
            DoResetThreat();
            DestroyPortals();
            AttackStart(me->getVictim());
            Shuffle_Coordinates();
            DoScriptText(EMOTE_PHASE_BANISH,me);
            uiTimers[EVENT_BANISH_END_TIMER] = 30000;
            uiTimers[EVENT_NETHERBREATH] = 5000;
            break;
        case BANISH_PHASE:  //enter portal phase
            Active_Phase = PORTAL_PHASE;
            me->RemoveAurasDueToSpell(SPELL_ROOT);
            me->RemoveAurasDueToSpell(SPELL_BANISH_VISUAL);
            DoResetThreat();
            SpawnPortals();
            AttackStart(me->getVictim());
            DoScriptText(EMOTE_PHASE_PORTAL,me);
            DoCast(me,SPELL_EMPOWERMENT);
            uiTimers[EVENT_PORTAL_END_TIMER] = 60000;
            uiTimers[EVENT_BEAM_INITIAL] = 10000;
            uiTimers[EVENT_BEAM_PERIODIC] = 0;
            uiTimers[EVENT_NETHERBURN] = 5000;
            uiTimers[EVENT_VOID_ZONE] = 15000;
            break;
        default:
            break;
        }
        Active_Event = NULL_EVENT;
    }

    void SpawnPortals()
    {
        for(int i=0;i<3;i++)
        {
            pBeamer[i] = me->SummonCreature(1557,Fixed_Coordinates[i][0],Fixed_Coordinates[i][1],Fixed_Coordinates[i][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);
            pBeamer_Helper[i] = pBeamer[i]->SummonCreature(1557,Fixed_Coordinates[i][0],Fixed_Coordinates[i][1],Fixed_Coordinates[i][2],0,TEMPSUMMON_CORPSE_DESPAWN,0);

            pBeamer[i]->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
            pBeamer_Helper[i]->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686);
            pBeamer[i]->setFaction(me->getFaction());
            pBeamer_Helper[i]->setFaction(me->getFaction());
            pBeamer[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pBeamer[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pBeamer[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            pBeamer[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
            pBeamer_Helper[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pBeamer_Helper[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pBeamer_Helper[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            pBeamer_Helper[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
            pBeamer[i]->CastSpell(pBeamer[i], Portals[i], true);
            pBeamer[i]->CastSpell(pBeamer[i], SPELL_ROOT, true);
            pBeamer_Helper[i]->CastSpell(pBeamer_Helper[i], SPELL_ROOT, true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        Active_Event = NULL_EVENT;

        for (uint32 i=1; i<MaxTimeCounter[Active_Phase]; i++)
        {
            if (uiTimers[i])
                if (uiTimers[i] <= diff)
                {
                    if (!Active_Event)
                        Active_Event = (Events)i;
                }
                else uiTimers[i] -= diff;
        }

        switch(Active_Phase)
        {
        case PORTAL_PHASE:
            if(Active_Event == EVENT_PORTAL_END_TIMER)
                SwitchPhase();
            break;
        case BANISH_PHASE:
            if(Active_Event == EVENT_BANISH_END_TIMER)
                SwitchPhase();
            break;
        default:
            break;
        }

        if((Active_Phase == PORTAL_PHASE && me->isAlive()) || (Active_Phase == BANISH_PHASE && me->isAlive()))
        {
            switch(Active_Event)
            {
            case EVENT_ENRAGE:
                DoCast(me, SPELL_NETHERSPITE_ROAR);
                DoCast(me, SPELL_NETHER_INFUSION);
                uiTimers[EVENT_ENRAGE] = 0;
                break;
            case EVENT_EXHAUST_CHECK:
                Apply_Exhaustion(0);
                Apply_Exhaustion(1);
                Apply_Exhaustion(2);
                uiTimers[EVENT_EXHAUST_CHECK] = 2000;
                break;
            default:
                break;
            }
        }

        if(Active_Phase == PORTAL_PHASE && me->isAlive())
        {
            switch(Active_Event)
            {
            case EVENT_BEAM_INITIAL:
                Nether_Beam(0);
                Nether_Beam(1);
                //Nether_Beam(2);   // <--uncomment to activate red beam, not complete
                uiTimers[EVENT_BEAM_INITIAL] = 0;
                uiTimers[EVENT_BEAM_PERIODIC] = 1000;
                break;
            case EVENT_BEAM_PERIODIC:
                Nether_Beam(0);
                Nether_Beam(1);
                //Nether_Beam(2);   // <--uncomment to activate red beam, not complete
                uiTimers[EVENT_BEAM_PERIODIC] = 1000;
                break;
            case EVENT_NETHERBURN:
                DoCast(me,SPELL_NETHERBURN);
                uiTimers[EVENT_NETHERBURN] = 5000;
                break;
            case EVENT_VOID_ZONE:
                DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_VOID_ZONE,true);
                uiTimers[EVENT_VOID_ZONE] = 15000;
                break;
            default:
                break;
            }
            if(pCurrent_Candidate[2] != me)
            {
                AttackStart(pCurrent_Candidate[2]);
            }
            else DoMeleeAttackIfReady();
        }

        if(Active_Phase == BANISH_PHASE && me->isAlive())
        {
            switch(Active_Event)
            {
            case EVENT_NETHERBREATH:
                DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_NETHERBREATH);
                uiTimers[EVENT_NETHERBREATH] = 5000;
                break;
            default:
                break;
            }
            DoMeleeAttackIfReady();
        }
    }

    void KilledUnit(Unit* Victim)
    {
        for(int i=0;i<10;i++)
        {
            if(pHealth_Modified[i] == Victim)
            {
                //pHealth_Modified[i]->GetUInt32Value(UNIT_FIELD_MAXHEALTH) =  pHealth_Modified[i]->GetUInt32Value(UNIT_FIELD_MAXHEALTH) - uiHp_bonus;
                pHealth_Modified[i] = NULL;
                break;
            }
        }
    }

    void JustDied(Unit* Killer)
    {
        for(int i=0;i<10;i++)
        {
            if(pHealth_Modified[i])
            {
                //pHealth_Modified[i]->GetUInt32Value(UNIT_FIELD_MAXHEALTH) =  pHealth_Modified[i]->GetUInt32Value(UNIT_FIELD_MAXHEALTH) - uiHp_bonus;
                pHealth_Modified[i] = NULL;
            }
        }

        m_pInstance->SetData(TYPE_NETHERSPITE, DONE);

        if (m_pInstance)
        {
            //door opens
            if (GameObject* pDoor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(DATA_GO_MASSIVE_DOOR)))
                pDoor->SetGoState(GO_STATE_ACTIVE);
        }
        else
            return;
        DestroyPortals();
    }
};

CreatureAI* GetAI_boss_netherspite(Creature* pCreature)
{
    return new boss_netherspiteAI(pCreature);
}

void AddSC_boss_netherspite()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_netherspite";
    newscript->GetAI = &GetAI_boss_netherspite;
    newscript->RegisterSelf();
}