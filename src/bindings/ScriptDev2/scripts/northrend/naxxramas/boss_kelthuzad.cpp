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
SDName: Boss_KelThuzud
SD%Complete: 0
SDComment: VERIFY SCRIPT
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//when shappiron dies. dialog between kel and lich king (in this order)
#define SAY_SAPP_DIALOG1            -1533084
#define SAY_SAPP_DIALOG2_LICH       -1533085
#define SAY_SAPP_DIALOG3            -1533086
#define SAY_SAPP_DIALOG4_LICH       -1533087
#define SAY_SAPP_DIALOG5            -1533088

//when cat dies
#define SAY_CAT_DIED                -1533089

//when each of the 4 wing bosses dies
#define SAY_TAUNT1                  -1533090
#define SAY_TAUNT2                  -1533091
#define SAY_TAUNT3                  -1533092
#define SAY_TAUNT4                  -1533093

#define SAY_SUMMON_MINIONS          -1533105                //start of phase 1

#define SAY_AGGRO1                  -1533094                //start of phase 2
#define SAY_AGGRO2                  -1533095
#define SAY_AGGRO3                  -1533096

#define SAY_SLAY1                   -1533097
#define SAY_SLAY2                   -1533098

#define SAY_DEATH                   -1533099

#define SAY_CHAIN1                  -1533100
#define SAY_CHAIN2                  -1533101
#define SAY_FROST_BLAST             -1533102

#define SAY_REQUEST_AID             -1533103                //start of phase 3
#define SAY_ANSWER_REQUEST          -1533104                //lich king answer

#define SAY_SPECIAL1_MANA_DET       -1533106
#define SAY_SPECIAL3_MANA_DET       -1533107
#define SAY_SPECIAL2_DISPELL        -1533108

//***THIS SCRIPTS IS UNDER DEVELOPMENT***
/*
DATA.
This script has been made with info taken from wowwikki... so there are things wrong...
like spell timers and Says. Also there's another major problem as there is no aggroed list
I cannot make Kel'thuzad to target specific party members, that is needed for spells like
Mana Detonation... so what I'm doing untill now is just to cast everything on my main aggroed
target. Sorry for him.
Another bug is that there are spells that are actually NOT working... I have to implement
them first.
Need DISPELL efect
I also don't know the emotes
*/

//Positional defines
#define ADDX_LEFT_FAR               3769.272705
#define ADDY_LEFT_FAR               -5071.697266
#define ADDZ_LEFT_FAR               143.711203
#define ADDO_LEFT_FAR               3.617599

#define ADDX_LEFT_MIDDLE            3729.291260
#define ADDY_LEFT_MIDDLE            -5044.239258
#define ADDZ_LEFT_MIDDLE            143.956909
#define ADDO_LEFT_MIDDLE            4.461900

#define ADDX_LEFT_NEAR              3683.868652
#define ADDY_LEFT_NEAR              -5057.281250
#define ADDZ_LEFT_NEAR              143.183884
#define ADDO_LEFT_NEAR              5.237086

#define ADDX_RIGHT_FAR              3749.355225
#define ADDY_RIGHT_FAR              -5158.128418
#define ADDZ_RIGHT_FAR              143.802383
#define ADDO_RIGHT_FAR              2.170104

#define ADDX_RIGHT_MIDDLE           3703.724365
#define ADDY_RIGHT_MIDDLE           -5169.123047
#define ADDZ_RIGHT_MIDDLE           143.928024
#define ADDO_RIGHT_MIDDLE           1.309310

#define ADDX_RIGHT_NEAR             3665.121094
#define ADDY_RIGHT_NEAR             -5138.679199
#define ADDZ_RIGHT_NEAR             143.183212
#define ADDO_RIGHT_NEAR             0.604023

#define WALKX_LEFT_FAR              3754.431396
#define WALKY_LEFT_FAR              -5080.727734
#define WALKZ_LEFT_FAR              142.036316
#define WALKO_LEFT_FAR              3.736189

#define WALKX_LEFT_MIDDLE           3724.396484
#define WALKY_LEFT_MIDDLE           -5061.330566
#define WALKZ_LEFT_MIDDLE           142.032700
#define WALKO_LEFT_MIDDLE           4.564785

#define WALKX_LEFT_NEAR             3687.158424
#define WALKY_LEFT_NEAR             -5076.834473
#define WALKZ_LEFT_NEAR             142.017319
#define WALKO_LEFT_NEAR             5.237086

#define WALKX_RIGHT_FAR             3687.571777
#define WALKY_RIGHT_FAR             -5126.831055
#define WALKZ_RIGHT_FAR             142.017807
#define WALKO_RIGHT_FAR             0.604023

#define WALKX_RIGHT_MIDDLE          3707.990733
#define WALKY_RIGHT_MIDDLE          -5151.450195
#define WALKZ_RIGHT_MIDDLE          142.032562
#define WALKO_RIGHT_MIDDLE          1.376855

#define WALKX_RIGHT_NEAR            3739.500000
#define WALKY_RIGHT_NEAR            -5141.883989
#define WALKZ_RIGHT_NEAR            142.0141130
#define WALKO_RIGHT_NEAR            2.121412

#define WALKX_MIDDLE                3716.384
#define WALKY_MIDDLE                -5106.453
#define WALKZ_MIDDLE                142

//spells to be casted
#define SPELL_FROST_BOLT            28478
#define H_SPELL_FROST_BOLT          55802
#define SPELL_FROST_BOLT_NOVA       28479
#define H_SPELL_FROST_BOLT_NOVA     55807

#define SPELL_CHAINS_OF_KELTHUZAD   28410                   //casted spell should be 28408. Also as of 303, heroic only
#define SPELL_MANA_DETONATION       27819
#define SPELL_SHADOW_FISURE         27810
#define SPELL_FROST_BLAST           27808

#define NPC_SOLDIERS_FROZEN_WASTES  16427
#define NPC_UNSTOPPABLE_ABOMINATIONS 16428
#define NPC_SOUL_WEAVERS            16429

#define HOME_X                      3748
#define HOME_Y                      -5113

struct MANGOS_DLL_DECL boss_kelthuzadAI : public ScriptedAI
{
    boss_kelthuzadAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
        GuardiansOfIcecrown[0] = 0;
        GuardiansOfIcecrown[1] = 0;
        GuardiansOfIcecrown[2] = 0;
        GuardiansOfIcecrown[3] = 0;
        GuardiansOfIcecrown[4] = 0;
        GuardiansOfIcecrown_Count = 0;
        Reset();
    }
    ScriptedInstance *pInstance;
    bool m_bIsHeroic;

    std::list<uint64> m_lSummonsGUIDList;
    std::list<uint64>::iterator m_uiSendSummon;

    uint64 GuardiansOfIcecrown[5];
    uint32 GuardiansOfIcecrown_Count;
    uint32 GuardiansOfIcecrown_Timer;
    uint32 ChainsOfKelthuzad_Timer;
    uint32 ChainsOfKelthuzad_Targets;
    uint32 Phase1_Timer;
    uint32 Phase1Encounter_Timer;
    bool SendSummon;
    bool Phase1;
    bool Phase2;
    bool Phase3;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_MANA_DETONATION,20000,20000,0,TARGET_RANDOM,0,-1533106);
		Tasks.AddEvent(SPELL_SHADOW_FISURE,25000,20000,0,TARGET_RANDOM,0,SAY_SPECIAL3_MANA_DET);
		Tasks.AddEvent(SPELL_FROST_BLAST,35000,30000,60000,TARGET_RANDOM,0,SAY_FROST_BLAST);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(H_SPELL_FROST_BOLT,20000,10000,35000,TARGET_RANDOM);
			Tasks.AddEvent(H_SPELL_FROST_BOLT_NOVA,15000,15000,0,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_FROST_BOLT,20000,10000,35000,TARGET_RANDOM);
			Tasks.AddEvent(SPELL_FROST_BOLT_NOVA,15000,15000,0,TARGET_MAIN);
		}
        ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;      //Cast no sooner than once every 30 seconds
        GuardiansOfIcecrown_Timer = 5000;                   //5 seconds for summoning each Guardian of Icecrown in phase 3

        for(int i=0; i<5; ++i)
        {
            if (GuardiansOfIcecrown[i])
            {
                //delete creature
                if (Creature* pGuardian = (Creature*)Unit::GetUnit(*me, GuardiansOfIcecrown[i]))
                {
                    if (pGuardian->isAlive())
                        //pGuardian->ForcedDespawn();
                        pGuardian->DealDamage(pGuardian, pGuardian->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }

                GuardiansOfIcecrown[i] = 0;
            }
        }
        DespawnSummons();

        Phase1_Timer = 280000;                              //Phase 1 lasts 5 minutes and 10 seconds
        Phase1Encounter_Timer = 3000;
        SendSummon = false;
        Phase1 = false;
        Phase2 = false;
        Phase3 = false;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (pInstance)
            pInstance->SetData(TYPE_KELTHUZAD, NOT_STARTED);
    }

    void KilledUnit()
    {
        if (rand()%2)
            DoScriptText(SAY_SLAY1, me);
        else
            DoScriptText(SAY_SLAY2, me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, me);

        for(int i=0; i<5; ++i)
        {
            if (GuardiansOfIcecrown[i])
            {
                Creature* pGuardian = (Creature*)Unit::GetUnit(*me, GuardiansOfIcecrown[i]);

                if (!pGuardian || !pGuardian->isAlive())
                    continue;

                pGuardian->ForcedDespawn();

                float Walk_Pos_X = 0.0f;
                float Walk_Pos_Y = 0.0f;
                float Walk_Pos_Z = 0.0f;

                switch(rand()%6)
                {
                    case 0:
                        Walk_Pos_X = ADDX_LEFT_FAR;
                        Walk_Pos_Y = ADDY_LEFT_FAR;
                        Walk_Pos_Z = ADDZ_LEFT_FAR;
                        break;
                    case 1:
                        Walk_Pos_X = ADDX_LEFT_MIDDLE;
                        Walk_Pos_Y = ADDY_LEFT_MIDDLE;
                        Walk_Pos_Z = ADDZ_LEFT_MIDDLE;
                        break;
                    case 2:
                        Walk_Pos_X = ADDX_LEFT_NEAR;
                        Walk_Pos_Y = ADDY_LEFT_NEAR;
                        Walk_Pos_Z = ADDZ_LEFT_NEAR;
                        break;
                    case 3:
                        Walk_Pos_X = ADDX_RIGHT_FAR;
                        Walk_Pos_Y = ADDY_RIGHT_FAR;
                        Walk_Pos_Z = ADDZ_RIGHT_FAR;
                        break;
                    case 4:
                        Walk_Pos_X = ADDX_RIGHT_MIDDLE;
                        Walk_Pos_Y = ADDY_RIGHT_MIDDLE;
                        Walk_Pos_Z = ADDZ_RIGHT_MIDDLE;
                        break;
                    case 5:
                        Walk_Pos_X = ADDX_RIGHT_NEAR;
                        Walk_Pos_Y = ADDY_RIGHT_NEAR;
                        Walk_Pos_Z = ADDZ_RIGHT_NEAR;
                        break;
                }

                //pGuardian->SendMonsterMoveWithSpeed(Walk_Pos_X, Walk_Pos_Y, Walk_Pos_Z);
                pGuardian->GetMotionMaster()->MovePoint(0, Walk_Pos_X, Walk_Pos_Y, Walk_Pos_Z);
            }
        }
        if (pInstance)
            pInstance->SetData(TYPE_KELTHUZAD, DONE);

		GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME, 3);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!who)
            return;

        if (Phase1 || Phase2 || Phase3)
            return;

        if (who->isTargetableForAttack() && who->GetTypeId() == TYPEID_PLAYER && me->GetDistance2d(who) <= 50)
        {
            me->AddThreat(who, 0.0f);
            me->SetInCombatWith(who);

            for(uint8 i = 0; i <= 80; ++i)
            {
                if (i == 5 || i == 15 || i == 25 || i == 35 || i == 45 || i == 55 || i == 65 || i == 75)
                    DoSpawnAdds(NPC_SOUL_WEAVERS);
                else if (i == 10 || i == 20 || i == 30 || i == 40 || i == 50 || i == 60 || i == 70 || i == 80)
                    DoSpawnAdds(NPC_UNSTOPPABLE_ABOMINATIONS);
                else
                    DoSpawnAdds(NPC_SOLDIERS_FROZEN_WASTES);
            }
            if (!m_lSummonsGUIDList.empty())
            {
                m_uiSendSummon = m_lSummonsGUIDList.begin();
                SendSummon = true;
            }
            Phase1 = true;
        }
    }

    void Aggro(Unit* who)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, me); break;
            case 1: DoScriptText(SAY_AGGRO2, me); break;
            case 2: DoScriptText(SAY_AGGRO3, me); break;
        }

        if (pInstance)
            pInstance->SetData(TYPE_KELTHUZAD, IN_PROGRESS);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!Phase2)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWithZone();
            //me->SetInCombatWith(pWho);
            //pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void DoSpawnAdds(uint32 uiEntry)
    {
        float Walk_Pos_X = 0.0f;
        float Walk_Pos_Y = 0.0f;
        float Walk_Pos_Z = 0.0f;

        switch(rand()%6)
        {
            case 0:
                Walk_Pos_X = ADDX_LEFT_FAR;
                Walk_Pos_Y = ADDY_LEFT_FAR;
                Walk_Pos_Z = ADDZ_LEFT_FAR;
                break;
            case 1:
                Walk_Pos_X = ADDX_LEFT_MIDDLE;
                Walk_Pos_Y = ADDY_LEFT_MIDDLE;
                Walk_Pos_Z = ADDZ_LEFT_MIDDLE;
                break;
            case 2:
                Walk_Pos_X = ADDX_LEFT_NEAR;
                Walk_Pos_Y = ADDY_LEFT_NEAR;
                Walk_Pos_Z = ADDZ_LEFT_NEAR;
                break;
            case 3:
                Walk_Pos_X = ADDX_RIGHT_FAR;
                Walk_Pos_Y = ADDY_RIGHT_FAR;
                Walk_Pos_Z = ADDZ_RIGHT_FAR;
                break;
            case 4:
                Walk_Pos_X = ADDX_RIGHT_MIDDLE;
                Walk_Pos_Y = ADDY_RIGHT_MIDDLE;
                Walk_Pos_Z = ADDZ_RIGHT_MIDDLE;
                break;
            case 5:
                Walk_Pos_X = ADDX_RIGHT_NEAR;
                Walk_Pos_Y = ADDY_RIGHT_NEAR;
                Walk_Pos_Z = ADDZ_RIGHT_NEAR;
                break;
        }
        if (Creature* pTemp = me->SummonCreature(uiEntry, Walk_Pos_X-5 + rand()%10, Walk_Pos_Y-5 + rand()%10, Walk_Pos_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
            m_lSummonsGUIDList.push_back(pTemp->GetGUID());
    }

    void DespawnSummons()
    {
        if (m_lSummonsGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lSummonsGUIDList.begin(); itr != m_lSummonsGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, *itr))
            {
                if (pTemp->isAlive())
                    pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        m_lSummonsGUIDList.clear();
    }

    void UpdateAI(const uint32 diff)
    {
		if(Phase2 || Phase3)
			Tasks.UpdateEvent(diff);

        if (SendSummon)
        {
            if (Phase1Encounter_Timer < diff)
            {
                if (m_lSummonsGUIDList.empty())
                    return;

                if (m_uiSendSummon != m_lSummonsGUIDList.end())
                {
                    if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, *m_uiSendSummon))
                        if (pTemp->isAlive() && !pTemp->getVictim())
                            pTemp->GetMotionMaster()->MovePoint(0, WALKX_MIDDLE, WALKY_MIDDLE, WALKZ_MIDDLE);
                    ++m_uiSendSummon;
                    Phase1Encounter_Timer = 3000;
                }
                else
                    SendSummon = false;

            }else Phase1Encounter_Timer -= diff;
        }

        if (Phase1 && !Phase2)
        {
            if (Phase1_Timer < diff)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AttackStart(me->getVictim());
                Phase1 = false;
                Phase2 = true;
                Phase1_Timer = 0;
            }else Phase1_Timer -= diff;
            return;
        }

        if (!CanDoSomething())
            return;

        //Check for Chains Of Kelthuzad
        if (ChainsOfKelthuzad_Timer < diff)
        {
            DoCastVictim(SPELL_CHAINS_OF_KELTHUZAD);

            //if (rand()%2)
                //DoScriptText(SAY_CHAIN1, me);
            //else
                //DoScriptText(SAY_CHAIN2, me);

            ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;
        }else ChainsOfKelthuzad_Timer -= diff;

        //start phase 3 when we are 40% health
        if (!Phase3 && (me->GetHealth()*100 / me->GetMaxHealth()) < 40)
        {
            Phase3 = true;
            DoScriptText(SAY_REQUEST_AID, me);
            //here Lich King should respond to KelThuzad but I don't know which creature to make talk
            //so for now just make Kelthuzad says it.
            DoScriptText(SAY_ANSWER_REQUEST, me);
        }

        if (Phase3 && (GuardiansOfIcecrown_Count < ((m_bIsHeroic == true) ? 4 : 2)))
        {
            if (GuardiansOfIcecrown_Timer < diff)
            {
                //Summon a Guardian of Icecrown in a random alcove (Creature # 16441)
                //uint32 TimeToWalk;
                Creature* pGuardian = NULL;

                float Walk_Pos_X;
                float Walk_Pos_Y;
                float Walk_Pos_Z;

                switch(rand()%6)
                {
                    case 0:
                        pGuardian = me->SummonCreature(16441,ADDX_LEFT_FAR,ADDY_LEFT_FAR,ADDZ_LEFT_FAR,ADDO_LEFT_FAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,300000);
                        //Setting walk position
                        Walk_Pos_X = WALKX_LEFT_FAR;
                        Walk_Pos_Y = WALKY_LEFT_FAR;
                        Walk_Pos_Z = WALKZ_LEFT_FAR;
                        break;
                    case 1:
                        pGuardian = me->SummonCreature(16441,ADDX_LEFT_MIDDLE,ADDY_LEFT_MIDDLE,ADDZ_LEFT_MIDDLE,ADDO_LEFT_MIDDLE,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,300000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_LEFT_MIDDLE;
                        Walk_Pos_Y = WALKY_LEFT_MIDDLE;
                        Walk_Pos_Z = WALKZ_LEFT_MIDDLE;
                        break;
                    case 2:
                        pGuardian = me->SummonCreature(16441,ADDX_LEFT_NEAR,ADDY_LEFT_NEAR,ADDZ_LEFT_NEAR,ADDO_LEFT_NEAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,300000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_LEFT_NEAR;
                        Walk_Pos_Y = WALKY_LEFT_NEAR;
                        Walk_Pos_Z = WALKZ_LEFT_NEAR;
                        break;
                    case 3:
                        pGuardian = me->SummonCreature(16441,ADDX_RIGHT_FAR,ADDY_RIGHT_FAR,ADDZ_RIGHT_FAR,ADDO_RIGHT_FAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,300000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_FAR;
                        Walk_Pos_Y = WALKY_RIGHT_FAR;
                        Walk_Pos_Z = WALKZ_RIGHT_FAR;
                        break;
                    case 4:
                        pGuardian = me->SummonCreature(16441,ADDX_RIGHT_MIDDLE,ADDY_RIGHT_MIDDLE,ADDZ_RIGHT_MIDDLE,ADDO_RIGHT_MIDDLE,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,300000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_MIDDLE;
                        Walk_Pos_Y = WALKY_RIGHT_MIDDLE;
                        Walk_Pos_Z = WALKZ_RIGHT_MIDDLE;
                        break;
                    case 5:
                        pGuardian = me->SummonCreature(16441,ADDX_RIGHT_NEAR,ADDY_RIGHT_NEAR,ADDZ_RIGHT_NEAR,ADDO_RIGHT_NEAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,300000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_NEAR;
                        Walk_Pos_Y = WALKY_RIGHT_NEAR;
                        Walk_Pos_Z = WALKZ_RIGHT_NEAR;
                        break;
                }

                if (pGuardian)
                {
                    //if we find no one to figth walk to the center
                    if (!pGuardian->getVictim())
                        //pGuardian->SendMonsterMoveWithSpeed(Walk_Pos_X,Walk_Pos_Y,Walk_Pos_Z);
                        pGuardian->GetMotionMaster()->MovePoint(0, Walk_Pos_X, Walk_Pos_Y, Walk_Pos_Z);

                    //Safe storing of creatures
                    GuardiansOfIcecrown[GuardiansOfIcecrown_Count] = pGuardian->GetGUID();

                    //Update guardian count
                    ++GuardiansOfIcecrown_Count;
                }

                //5 seconds until summoning next guardian
                GuardiansOfIcecrown_Timer = 5000;
            }else GuardiansOfIcecrown_Timer -= diff;
        }

        if (me->GetDistance2d(HOME_X, HOME_Y) > 80)
            EnterEvadeMode();

        DoMeleeAttackIfReady();
    }
};


/*######
## Mob Shadow Issure
######*/

struct MANGOS_DLL_DECL mob_shadow_issureAI : public Scripted_NoMovementAI
{
    mob_shadow_issureAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiShadowIssure_Timer;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
        m_uiShadowIssure_Timer = 4000;
		SetCombatMovement(false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiShadowIssure_Timer)
            if (m_uiShadowIssure_Timer < diff)
            {
                Map *map = me->GetMap();
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (PlayerList.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (i->getSource()->isAlive() && me->GetDistance2d(i->getSource()) < 2.0f)
                           Kill(i->getSource());
                    }
                }
                me->ForcedDespawn(1500);
                m_uiShadowIssure_Timer = 0;
            }
            else m_uiShadowIssure_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_kelthuzadAI(Creature* pCreature)
{
    return new boss_kelthuzadAI(pCreature);
}

CreatureAI* GetAI_mob_shadow_issureAI(Creature* pCreature)
{
    return new mob_shadow_issureAI(pCreature);
}

void AddSC_boss_kelthuzad()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kelthuzad";
    newscript->GetAI = &GetAI_boss_kelthuzadAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadow_issure";
    newscript->GetAI = &GetAI_mob_shadow_issureAI;
    newscript->RegisterSelf();
}
