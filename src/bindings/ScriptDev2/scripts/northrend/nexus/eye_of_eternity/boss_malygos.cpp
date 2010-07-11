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
SDName: Boss_Malygos
SD%Complete: 45%
SDComment: 
SDAuthor: Tassadar
SDCategory: Nexus, Eye of Eternity
EndScriptData */

#include "precompiled.h"

enum
{
    // ******************************** SPELLS ******************************** //
    SPELL_BERSERK                  = 64238, // Maybe wrong but enought :)
    //////////////// PHASE 0 ////////////////
    SPELL_PORTAL_BEAM              = 56046,

    //////////////// PHASE 1 ////////////////
    SPELL_ARCANE_BREATH            = 56272,
    SPELL_ARCANE_BREATH_H          = 60072,
    SPELL_VORTEX_DUMMY             = 56105, // This is for addons, actualy does nothing
    SPELL_VORTEX                   = 56266, // Cast on trigger in middle of the room, this interrupt their cast and deal dmg
    SPELL_VORTEX_AOE_VISUAL        = 55873,    // visual effect around platform
    SPELL_POWER_SPARK              = 56152, // If spark reach malygos then buff him, if killed then to players
    SPELL_POWER_SPARK_PLAYERS      = 55852, // This one is propably for players
    SPELL_POWER_SPARK_VISUAL       = 55845,

    //////////////// PHASE 2 ////////////////
    SPELL_ARCANE_PULSE             = 57432, // Malygos takes a deep breath... cast on trigger
    SPELL_ARCANE_STORM             = 61693, // AOE
    SPELL_ARCANE_STORM_H           = 61694,
    SPELL_ARCANE_OVERLOAD          = 56432, // Cast this on arcane overload NPCs 
    SPELL_DESTROY_PLATFORM_PRE     = 58842, // lights all over the platform
    SPELL_DESTROY_PLATFROM_BOOM    = 59084, // Big Blue boom
    //NPCs spells
    SPELL_ARCANE_SHOCK             = 57058,
    SPELL_ARCANE_SHOCK_H           = 60073,
    SPELL_ARCANE_BARRAGE           = 63934,


    //////////////// PHASE 3 ////////////////
    SPELL_STATIC_FIELD             = 57428, // Summon trigger and cast this on them should be enought
    SPELL_SURGE_OF_POWER           = 56548, // Summon triggers and cast to random targets
    SPELL_SURGE_OF_POWER_H         = 57407,
    //Dragons spells
    SPELL_FLAME_SPIKE              = 56091,
    SPELL_ENGULF_IN_FLAMES         = 56092,
    SPELL_REVIVIFY                 = 57090,
    SPELL_LIFE_BURST               = 57143,
    SPELL_FLAME_SHIELD             = 57108,
    SPELL_BLAZING_SPEED            = 57092,

    // ******************************** NPCs & GObjects ******************************** //
    NPC_MALYGOS                    = 28859,
    ITEM_KEY_TO_FOCUSING_IRIS      = 44582,
    ITEM_KEY_TO_FOCUSING_IRIS_H    = 44581,
    //////////////// PHASE 1 ////////////////
    NPC_AOE_TRIGGER                = 22517,
    NPC_VORTEX                     = 30090,
    NPC_POWER_SPARK                = 30084,
    NPC_SPARK_PORTAL               = 30118, // For power sparks

    //////////////// PHASE 2 ////////////////
    NPC_HOVER_DISC                 = 30248, // Maybe wrong, two following NPC flying on them (vehicle)
    NPC_NEXUS_LORD                 = 30245, // two (?) of them are spawned on beginning of phase 2
    NPC_SCION_OF_ETERNITY          = 30249, // same, but unknow count
    NPC_ARCANE_OVERLOAD            = 30282, // Bubles
    GO_PLATFORM                    = 193070,

    //////////////// PHASE 3 ////////////////
    NPC_WYRMREST_SKYTALON          = 30161, // Dragon Vehicle in Third Phase
    NPC_SURGE_OF_POWER             = 30334, // What?
    NPC_STATIC_FIELD               = 30592, // Trigger for that spell. Hope its fly

    //////////////// PHASE 4 ////////////////
    NPC_ALEXSTRASZA                = 32295, // The Life-Binder
    GO_ALEXSTRASZAS_GIFT           = 193905, //Loot chest
    GO_EXIT_PORTAL                 = 193908,

    SAY_INTRO1                     = -1616000,
    SAY_INTRO2                     = -1616001,
    SAY_INTRO3                     = -1616002,
    SAY_INTRO4                     = -1616003,
    SAY_INTRO5                     = -1616004,
    SAY_AGGRO1                     = -1616005,
    SAY_AGGRO2                     = -1616013, 
    SAY_VORTEX                     = -1616006,
    SAY_POWER_SPARK                = -1616007,
    SAY_DEATH                      = -1616011,
    SAY_KILL1_1                    = -1616008,
    SAY_KILL1_2                    = -1616009,
    SAY_KILL1_3                    = -1616010,
    SAY_END_PHASE1                 = -1616012,
    SAY_ARCANE_PULSE               = -1616014,
    SAY_ARCANE_PULSE_WARN          = -1616015,

    SHELL_MIN_X                    = 722,
    SHELL_MAX_X                    = 768,
    SHELL_MIN_Y                    = 1290,
    SHELL_MAX_Y                    = 1339,

    NEXUS_LORD_COUNT               = 2,
    NEXUS_LORD_COUNT_H             = 4,
    SCION_OF_ETERNITY_COUNT        = 6,
    SCION_OF_ETERNITY_COUNT_H      = 8,

    PHASE_NOSTART                  = 0,
    SUBPHASE_FLY_UP            = 01,
    SUBPHASE_UP                = 03,
    SUBPHASE_FLY_DOWN1         = 04,
    SUBPHASE_FLY_DOWN2         = 05,
    PHASE_FLOOR                    = 1,
    SUBPHASE_VORTEX            = 11,
    PHASE_ADDS                     = 2,
    SUBPHASE_TALK              = 21,
    PHASE_DRAGONS                  = 3,
    SUBPHASE_DESTROY_PLATFORM  = 31,
    PHASE_OUTRO                    = 4,

};
struct Locations
{
    float x, y, z;
    uint32 id;
};
struct LocationsXY
{
    float x, y;
    uint32 id;
};
static LocationsXY SparkLoc[]=
{
    {652.417, 1200.52},
    {847.67, 1408.05},
    {647.675, 1403.8},
    {843.182, 1215.42},
};
static LocationsXY VortexLoc[]=
{
    {754, 1311},
    {734, 1334},
    {756, 1339},
    {781, 1329},
    {791, 1311},
    {790, 1283},
    {768, 1264},
    {739, 1261},
    {720, 1280},
    {714, 1299},
    {716, 1318},
    {734, 1334},
    {756, 1339},
    {781, 1329},
    {791, 1311},
    {790, 1283},
    {768, 1264},
    {739, 1261},
    {720, 1280},
    {714, 1299},
    {716, 1318},
};
#define MAX_VORTEX              21
#define VORTEX_Z                270

#define FLOOR_Z                 269.17
#define AIR_Z                   297.24   
/*######
## boss_malygos
######*/

struct MANGOS_DLL_DECL boss_malygosAI : public LibDevFSAI
{
    boss_malygosAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(TEN_MINS);
		AddTextEvent(14533,"Rien ne peut m'arrêter !",TEN_MINS,DAY*HOUR);
		AddEventOnMe(m_difficulty ? SPELL_ARCANE_BREATH_H : SPELL_ARCANE_BREATH,15000,18000,5000,PHASE_FLOOR);
		AddEventOnMe(m_difficulty ? SPELL_ARCANE_STORM_H : SPELL_ARCANE_STORM,10000,15000,3000,PHASE_FLOOR);
		AddEvent(60936,12000,30000,0,TARGET_RANDOM,PHASE_DRAGONS); 
		AddEvent(57430,15000,15000,5000,TARGET_RANDOM,PHASE_DRAGONS);
    }
    
    uint8 m_uiPhase; //Fight Phase
    uint8 m_uiSubPhase; //Subphase if needed
    uint8 m_uiSpeechCount;
    uint8 m_uiVortexPhase;
    std::list<uint64> m_lSparkGUIDList;

    uint32 m_uiSpeechTimer[5];
    uint32 m_uiTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiPowerSparkTimer;
    uint32 m_uiDeepBreathTimer;
    uint32 m_uiShellTimer;

	void Reset()
    {
		ResetTimers();
		CleanMyAdds();
        m_uiPhase = PHASE_FLOOR;
        m_uiSubPhase = 0;
        m_uiSpeechCount = 0;
        m_uiVortexPhase = 0;

        m_uiSpeechTimer[0] = 2000;
        m_uiSpeechTimer[1] = 10000;
        m_uiSpeechTimer[2] = 11000;
        m_uiSpeechTimer[3] = 13000;
        m_uiSpeechTimer[4] = 10000;
        m_uiSpeechTimer[5] = 7000;
        m_uiTimer = 7000;
        m_uiVortexTimer = 28000;
        m_uiPowerSparkTimer = 25000;
        m_uiDeepBreathTimer = 60000;
        m_uiShellTimer = 0;

		if(GameObject* go = GetClosestGameObjectWithEntry(me,193070,80.0f))
		{
			go->SetUInt32Value(GAMEOBJECT_DISPLAYID,go->GetGOInfo()->displayId);
			go->Rebuild();
		}
        
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }
    void AttackStart(Unit* pWho)
    {
        if(m_uiPhase != PHASE_FLOOR && m_uiPhase != PHASE_DRAGONS)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);

            me->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void Aggro(Unit* pWho)
    {
		Yell(14517,"Ma patience a des limites. Je vais me débarasser de vous !");
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        //DoScriptText(SAY_AGGRO1, me);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);

    }
    void KilledUnit(Unit* pVictim)
    {
        uint8 text = 0;
        switch(m_uiPhase)
        {
            case PHASE_FLOOR:
                text = urand(0, 2);
                break;
            case PHASE_ADDS:
                text = urand(3, 5);
                break;
            case PHASE_DRAGONS:
                text = urand(6, 8);
                break;
        }
        switch(text)
        {
            case 0: DoScriptText(SAY_KILL1_1, me); break;
            case 1: DoScriptText(SAY_KILL1_2, me); break;
            case 2: DoScriptText(SAY_KILL1_3, me); break;
        }
            
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if(pSpell->Id == SPELL_POWER_SPARK)
            DoScriptText(SAY_POWER_SPARK, me);
    }

    void DoMovement(float x, float y, float z, uint32 time, bool tofly = false)
    {        
        if(tofly){
            me->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        }else{
            me->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        }
        me->GetMap()->CreatureRelocation(me, x, y, z, me->GetOrientation());
        me->SendMonsterMove(x, y, z, SPLINETYPE_NORMAL, me->GetSplineFlags(), time);
    }

    void DoVortex(uint8 phase)
    {
        if(phase == 0)
        {
            me->GetMotionMaster()->Clear(false);
            SetCombatMovement(false);
            float x, y, z;
            me->GetPosition(x, y, z);
            z = z + 20;
            DoMovement(x, y, z, 0, true);
        }
        else if(phase == 1)
        {
            if(Creature *pTrigger = GetClosestCreatureWithEntry(me, NPC_AOE_TRIGGER, 60.0f))
                pTrigger->CastSpell(pTrigger, SPELL_VORTEX_AOE_VISUAL, false);

            Map* pMap = me->GetMap();
            if(!pMap)
                return;

            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                if(!itr->getSource()->isAlive())
                    continue;
                itr->getSource()->NearTeleportTo(VortexLoc[0].x, VortexLoc[0].y, VORTEX_Z, 0);
				ModifyAuraStack(SPELL_VORTEX,1,itr->getSource());
            }        
        }
        else if(phase > 1 && phase < 26)
		{
            Map* pMap = me->GetMap();
            if(!pMap)
                return;
            if(Creature *pVortex = CallCreature(NPC_VORTEX,10000,PREC_COORDS,NOTHING,VortexLoc[phase-1].x, VortexLoc[phase-1].y, VORTEX_Z))
            {
                pVortex->SetVisibility(VISIBILITY_OFF);
                Map::PlayerList const &lPlayers = pMap->GetPlayers();
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if(!itr->getSource()->isAlive())
                        continue;

                    float z = itr->getSource()->GetPositionZ() - VORTEX_Z;
                    itr->getSource()->KnockBackFrom(pVortex,-float(pVortex->GetDistance2d(itr->getSource())),7);
                }
            }
        }
		else if (phase == 30 || phase == 31)
        {    
            if(phase == 31)
            {
                if(me->getVictim())
                    me->GetMotionMaster()->MoveChase(me->getVictim());
                
                m_uiSubPhase = 0;
                return;
            }
            me->GetMotionMaster()->Clear(false);
            SetCombatMovement(true);
            float x, y, z;
            me->GetPosition(x, y, z);
            z = z - 20;
            DoMovement(x, y, z, 0);
        }
        
    }
    void PowerSpark(uint8 action)
    {
        if(action == 1) //Summon
        {
            uint8 random = urand(0, 3);
			BossEmote(0,"Une etincelle de puissance est sortie d'une faille");
            if(Creature *pSpark = CallCreature(NPC_POWER_SPARK,40000,PREC_COORDS,NOTHING,SparkLoc[random].x, SparkLoc[random].y, FLOOR_Z))
            {
				SetFlying(true,pSpark);
                pSpark->CastSpell(pSpark, SPELL_POWER_SPARK_VISUAL, false);
                pSpark->GetMotionMaster()->MoveFollow(me, 0, 0);
                me->AddSplineFlag(MONSTER_MOVE_SPLINE_FLY);
                m_lSparkGUIDList.push_back(pSpark->GetGUID());
            }
        }
		else if(action == 2 || action == 3) // Start/stop movement
        {
            if(action == 2)
                me->RemoveAurasDueToSpell(SPELL_POWER_SPARK);

            if (m_lSparkGUIDList.empty())
                return;

            for(std::list<uint64>::iterator itr = m_lSparkGUIDList.begin(); itr != m_lSparkGUIDList.end(); ++itr)
            {
                if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, *itr))
                {
                    if (pTemp->isAlive())
                    {
                        if(action == 2)
                            pTemp->GetMotionMaster()->MoveFollow(me, 0, 0);
                        else
                            pTemp->GetMotionMaster()->MovementExpired(false);
                    }
                }
            }
        }
    }
    void DoSpawnAdds()
    {
        //Nexus lords
        for(uint8 i=0; i < (m_difficulty ? NEXUS_LORD_COUNT_H : NEXUS_LORD_COUNT);i++)
        {
            if(Creature *pLord = CallCreature(NPC_NEXUS_LORD,TEN_MINS,NEAR_7M))
                pLord->AddThreat(me->getVictim());
        }
        //Scions of eternity
        for(uint8 i=0; i < (m_difficulty ? SCION_OF_ETERNITY_COUNT_H : SCION_OF_ETERNITY_COUNT);i++)
        {
            uint32 x = urand(SHELL_MIN_X, SHELL_MAX_X);
            uint32 y = urand(SHELL_MIN_Y, SHELL_MAX_Y);    
            CallCreature(NPC_SCION_OF_ETERNITY,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,x,y,FLOOR_Z+20);
        }       
    }
    bool IsThereAnyAdd()
    {
        //Search for Nexus lords
        if(GetClosestCreatureWithEntry(me, NPC_NEXUS_LORD, 80.0f))
            return true;

        //Search for Scions of eternity
        if(GetClosestCreatureWithEntry(me, NPC_SCION_OF_ETERNITY, 80.0f))
            return true;

        return false;
    }
    void DoSpawnShell()
    {
        uint32 x = urand(SHELL_MIN_X, SHELL_MAX_X);
        uint32 y = urand(SHELL_MIN_Y, SHELL_MAX_Y);
        if(Creature *pShell = me->SummonCreature(NPC_ARCANE_OVERLOAD, x, y, FLOOR_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 45000))
            pShell->CastSpell(pShell, SPELL_ARCANE_OVERLOAD, false);
    }
    void MountPlayers()
    {
        Map *pMap = me->GetMap();

        if(!pMap)
            return;

        Map::PlayerList const &lPlayers = pMap->GetPlayers();
        if (lPlayers.isEmpty())
            return;
        
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
            {
                if(pPlayer->GetVehicleGUID())
                    pPlayer->ExitVehicle();

                if(Creature *pTemp = CallCreature(NPC_WYRMREST_SKYTALON,TEN_MINS,PREC_COORDS,NOTHING,pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ()))
                    pPlayer->EnterVehicle(pTemp);
				if(GameObject* go = GetClosestGameObjectWithEntry(me,193070,80.0f))
				{
					go->DestroyForPlayer(pPlayer,true);
					go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
					go->SetUInt32Value(GAMEOBJECT_DISPLAYID,9060);
					go->SetUInt32Value(GAMEOBJECT_BYTES_1,8449);
				}
            }
        }
		
    }
    void UpdateAI(const uint32 diff)
    {
        if(m_uiPhase == PHASE_NOSTART)
        {
            if(m_uiSubPhase == SUBPHASE_FLY_UP){
                float x, y, z;
                me->GetPosition(x, y, z);
                z = z + 40;
                DoMovement(x, y, z, 0, true);
                m_uiSubPhase = SUBPHASE_UP;
            }
            else if(m_uiSubPhase == SUBPHASE_UP){
                if(m_uiSpeechTimer[m_uiSpeechCount] <= diff)
                {
                    DoScriptText(SAY_INTRO1-m_uiSpeechCount, me);
                    m_uiSpeechCount++;
                    if(m_uiSpeechCount == 5){
                        m_uiSubPhase = SUBPHASE_FLY_DOWN1;
                    }
                }
				else
					m_uiSpeechTimer[m_uiSpeechCount] -= diff;
            }
            else if(m_uiSubPhase == SUBPHASE_FLY_DOWN1){
                if(m_uiTimer <= diff)
                {
                    float x, y, z;
                    me->GetPosition(x, y, z);
                    z = z - 40;
                    DoMovement(x, y, z, 0, false);
                    m_uiSubPhase = SUBPHASE_FLY_DOWN2;
                    m_uiTimer = 1500;
                }
				else 
					m_uiTimer -= diff;
            }
            else if(m_uiSubPhase == SUBPHASE_FLY_DOWN2){
                if(m_uiTimer <= diff)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_uiSubPhase = 0;
                    m_uiPhase = PHASE_FLOOR;
                }
				else 
					m_uiTimer -= diff;
            }    
        }
        if (!CanDoSomething())
            return;

        if(m_uiPhase == PHASE_FLOOR)
        {
            if(m_uiSubPhase == SUBPHASE_VORTEX)
            {
                if(m_uiTimer <= diff)
                {
                    DoVortex(m_uiVortexPhase);
                    
                    if(m_uiVortexPhase == 1 || m_uiVortexPhase == 11){
                        if(m_uiVortexPhase == 1)
                            DoCast(me, SPELL_VORTEX_DUMMY);
                        m_uiTimer = 300;
                    }
					else
						m_uiTimer = 500;

                    if(m_uiVortexPhase >= MAX_VORTEX && m_uiVortexPhase < 29)
                    {
                        m_uiVortexPhase = 30;
                        DoVortex(m_uiVortexPhase);
                        PowerSpark(2);
                        m_uiTimer = 1000;
                        return;
                    }
                    m_uiVortexPhase++;
                }
				else 
					m_uiTimer -= diff;
                return;
            }

            //Vortex
            if(m_uiVortexTimer <= diff)
            {
                PowerSpark(3);
                DoVortex(0);
                m_uiVortexPhase = 1;
                m_uiSubPhase = SUBPHASE_VORTEX;
                m_uiVortexTimer = 50000;
                m_uiTimer = 1000;
                DoScriptText(SAY_VORTEX, me);
                return;
            }
			else 
				m_uiVortexTimer -= diff;

            //PowerSpark
            if(m_uiPowerSparkTimer<= diff)
            {
                PowerSpark(1);
                m_uiPowerSparkTimer = 25000;
            }
			else 
				m_uiPowerSparkTimer -= diff;

            //Health check
            if(m_uiTimer<= diff)
            {
                if(CheckPercentLife(50))
                {
                    me->InterruptNonMeleeSpells(true);
                    SetCombatMovement(false);
                    me->GetMotionMaster()->MovementExpired(false);
                    DoScriptText(SAY_END_PHASE1, me);
                    m_uiPhase = PHASE_ADDS;
                    m_uiSubPhase = SUBPHASE_TALK;
                    m_uiTimer = 20000;
                    return;
                }
                m_uiTimer = 1500;
            }
			else 
				m_uiTimer -= diff;  

            DoMeleeAttackIfReady();
        }
		else if(m_uiPhase == PHASE_ADDS)
        {
            if(m_uiSubPhase == SUBPHASE_TALK)
            {
                if(m_uiTimer <= diff)
                {
                    float x, y, z;
                    if(Creature *pTrigger = GetClosestCreatureWithEntry(me, NPC_AOE_TRIGGER, 60.0f))
                    {
                        pTrigger->GetPosition(x, y, z);
                        DoMovement(x, y, z+40, 0, true);
                    }
                    DoScriptText(SAY_AGGRO2, me);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    DoSpawnAdds();
                    DoSpawnShell();
                    m_uiShellTimer = 30000;
                    m_uiSubPhase = 0;
                }else m_uiTimer -= diff;
            }
			else
			{
				//Arcane overload (bubble)
				if(m_uiShellTimer <= diff)
				{
					DoSpawnShell();
					m_uiShellTimer = 30000;
				}else m_uiShellTimer -= diff;

				// Arcane Pulse
				if(m_uiDeepBreathTimer <= diff)
				{
					DoScriptText(SAY_ARCANE_PULSE, me);
					DoScriptText(SAY_ARCANE_PULSE_WARN, me);
					if(Creature *pTrigger = GetClosestCreatureWithEntry(me, NPC_AOE_TRIGGER, 60.0f))
						pTrigger->CastSpell(pTrigger, SPELL_ARCANE_PULSE, false);

					m_uiDeepBreathTimer = 60000;
				}else m_uiDeepBreathTimer -= diff;

				//Health check
				if(m_uiTimer<= diff)
				{
					if(!IsThereAnyAdd())
					{
						m_uiPhase = PHASE_DRAGONS;
						m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM;
						if(Creature *pTrigger = GetClosestCreatureWithEntry(me, NPC_AOE_TRIGGER, 60.0f))
							pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFORM_PRE, false);
						m_uiTimer = 3000;
						return;
					}
					m_uiTimer = 5000;
				}
				else 
					m_uiTimer -= diff;
			}
        }
        else if(m_uiPhase == PHASE_DRAGONS)
        {
            if(m_uiSubPhase == SUBPHASE_DESTROY_PLATFORM)
            {
                if(m_uiTimer<= diff)
                {
                    if(Creature *pTrigger = GetClosestCreatureWithEntry(me, NPC_AOE_TRIGGER, 60.0f))
                        pTrigger->CastSpell(pTrigger, SPELL_DESTROY_PLATFROM_BOOM, false);
                    MountPlayers();
                    m_uiSubPhase = 0;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    SetCombatMovement(true);
                    me->GetMotionMaster()->MoveChase(me->getVictim());
                }
				else 
					m_uiTimer -= diff;
                return;
            }

            DoMeleeAttackIfReady();
        }
		UpdateEvent(diff);
		UpdateEvent(diff,m_uiPhase);
    }
};
/*######
## mob_power_spark
######*/
struct MANGOS_DLL_DECL mob_power_sparkAI : public LibDevFSAI
{
    mob_power_sparkAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    bool isDead;
    uint32 m_uiCheckTimer;

    void Reset()
    {
        isDead = false;
        me->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        m_uiCheckTimer = 2500;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage >= me->GetHealth() && !isDead)
        {
            isDead = true;

            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            me->RemoveAllAuras();
            me->AttackStop();
            SetCombatMovement(false);

            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                me->GetMotionMaster()->MovementExpired();

            uiDamage = 0;
            me->SetHealth(1);
            DoCastMe(SPELL_POWER_SPARK_PLAYERS);
            me->ForcedDespawn(10000);
			if(Creature* pMalygos = GetClosestCreatureWithEntry(me, NPC_MALYGOS, 150.0f))
				if(pMalygos->isAlive())
					((boss_malygosAI*)pMalygos->AI())->m_lSparkGUIDList.clear();
        }
    }
    void UpdateAI(const uint32 diff)
    {
        if(m_uiCheckTimer <= diff)
        {
			if(Creature* pMalygos = GetClosestCreatureWithEntry(me, NPC_MALYGOS, 150.0f))
				if(pMalygos->isAlive())
				{
					if(me->GetDistance2d(pMalygos) <= 5.0f)
					{
	                    isDead = true;
						((boss_malygosAI*)pMalygos->AI())->m_lSparkGUIDList.clear();
						((boss_malygosAI*)pMalygos->AI())->SpellHit(me,GetSpellStore()->LookupEntry(SPELL_POWER_SPARK));
						ModifyAuraStack(SPELL_POWER_SPARK,1,pMalygos);
						me->ForcedDespawn(2000);
					}
				}
            m_uiCheckTimer = 2500;
        }
		else 
			m_uiCheckTimer -= diff;
    }
};
/*######
## go_focusing_iris
######*/
bool GOHello_go_focusing_iris(Player* pPlayer, GameObject* pGo)
{
    bool m_difficulty = pGo->GetMap()->IsRegularDifficulty();

    bool hasItem = false;
    if (m_difficulty)
    {
        if(pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS, 1) || pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS_H, 1)) 
            hasItem = true;
    }else{
        if(pPlayer->HasItemCount(ITEM_KEY_TO_FOCUSING_IRIS_H, 1))
            hasItem = true;        
    }
    if(!hasItem)        
        return false;

    if(Creature *pMalygos = GetClosestCreatureWithEntry(pGo, NPC_MALYGOS, 150.0f))
    {
        ((boss_malygosAI*)pMalygos->AI())->m_uiSubPhase = SUBPHASE_FLY_UP;
    }
    pGo->Delete();
    return false;
}
CreatureAI* GetAI_boss_malygos(Creature* pCreature)
{
    return new boss_malygosAI(pCreature);
}
CreatureAI* GetAI_mob_power_spark(Creature* pCreature)
{
    return new mob_power_sparkAI(pCreature);
}

void AddSC_boss_malygos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_malygos";
    newscript->GetAI = &GetAI_boss_malygos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_power_spark";
    newscript->GetAI = &GetAI_mob_power_spark;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_focusing_iris";
    newscript->pGOHello = &GOHello_go_focusing_iris;
    newscript->RegisterSelf();
}
