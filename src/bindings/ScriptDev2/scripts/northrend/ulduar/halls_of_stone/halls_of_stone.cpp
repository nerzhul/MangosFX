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
SDName: Halls_of_Stone
SD%Complete: 20%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"
#include "halls_of_stone.h"

enum
{
    SAY_KILL_1                          = -1599012,
    SAY_KILL_2                          = -1599013,
    SAY_KILL_3                          = -1599014,
    SAY_LOW_HEALTH                      = -1599015,
    SAY_DEATH                           = -1599016,
    SAY_PLAYER_DEATH_1                  = -1599017,
    SAY_PLAYER_DEATH_2                  = -1599018,
    SAY_PLAYER_DEATH_3                  = -1599019,
    SAY_ESCORT_START                    = -1599020,

    SAY_SPAWN_DWARF                     = -1599021,
    SAY_SPAWN_TROGG                     = -1599022,
    SAY_SPAWN_OOZE                      = -1599023,
    SAY_SPAWN_EARTHEN                   = -1599024,

    SAY_EVENT_INTRO_1                   = -1599025,
    SAY_EVENT_INTRO_2                   = -1599026,
    SAY_EVENT_INTRO_3_ABED              = -1599027,

    SAY_EVENT_A_1                       = -1599028,
    SAY_EVENT_A_2_KADD                  = -1599029,
    SAY_EVENT_A_3                       = -1599030,

    SAY_EVENT_B_1                       = -1599031,
    SAY_EVENT_B_2_MARN                  = -1599032,
    SAY_EVENT_B_3                       = -1599033,

    SAY_EVENT_C_1                       = -1599034,
    SAY_EVENT_C_2_ABED                  = -1599035,
    SAY_EVENT_C_3                       = -1599036,

    SAY_EVENT_D_1                       = -1599037,
    SAY_EVENT_D_2_ABED                  = -1599038,
    SAY_EVENT_D_3                       = -1599039,
    SAY_EVENT_D_4_ABED                  = -1599040,

    SAY_EVENT_END_01                    = -1599041,
    SAY_EVENT_END_02                    = -1599042,
    SAY_EVENT_END_03_ABED               = -1599043,
    SAY_EVENT_END_04                    = -1599044,
    SAY_EVENT_END_05_ABED               = -1599045,
    SAY_EVENT_END_06                    = -1599046,
    SAY_EVENT_END_07_ABED               = -1599047,
    SAY_EVENT_END_08                    = -1599048,
    SAY_EVENT_END_09_KADD               = -1599049,
    SAY_EVENT_END_10                    = -1599050,
    SAY_EVENT_END_11_KADD               = -1599051,
    SAY_EVENT_END_12                    = -1599052,
    SAY_EVENT_END_13_KADD               = -1599053,
    SAY_EVENT_END_14                    = -1599054,
    SAY_EVENT_END_15_MARN               = -1599055,
    SAY_EVENT_END_16                    = -1599056,
    SAY_EVENT_END_17_MARN               = -1599057,
    SAY_EVENT_END_18                    = -1599058,
    SAY_EVENT_END_19_MARN               = -1599059,
    SAY_EVENT_END_20                    = -1599060,
    SAY_EVENT_END_21_ABED               = -1599061,

    SAY_VICTORY_SJONNIR_1               = -1599062,
    SAY_VICTORY_SJONNIR_2               = -1599063,

    SAY_ENTRANCE_MEET                   = -1599064,

    TEXT_ID_START                       = 13100,
    TEXT_ID_PROGRESS                    = 13101,

    NPC_TRIBUNAL_OF_THE_AGES           = 28234,
    NPC_BRANN_BRONZEBEARD              = 28070,
    SPELL_STEALTH                      = 58506,

    // KADDRAK
    SPELL_GLARE_OF_THE_TRIBUNAL        = 50988,
    SPELL_GLARE_OF_THE_TRIBUNAL_H      = 59870,

    // MARNAK
    SPELL_DARK_MATTER                  = 51012,
    SPELL_DARK_MATTER_H                = 59868,
    NPC_DARK_MATTER_TARGET             = 28237,

    // ABEDNEUM
    SPELL_SEARING_GAZE                 = 51136,
    SPELL_SEARING_GAZE_H               = 59867,
    NPC_SEARING_GAZE_TARGET            = 28265,

    NPC_DARK_RUNE_PROTECTOR            = 27983,
    NPC_DARK_RUNE_STORMCALLER          = 27984,
    NPC_IRON_GOLEM_CUSTODIAN           = 27985,

    QUEST_HALLS_OF_STONE               = 13207,
};

#define GOSSIP_ITEM_START               "Brann, it would be our honor!"
#define GOSSIP_ITEM_PROGRESS            "Let's move Brann, enough of the history lessons!"

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations SpawnLoc[]=
{
    {946.992, 397.016, 208.374},
    {960.748, 382.944, 208.374},
};


/*######
## mob_tribuna_controller
######*/

struct MANGOS_DLL_DECL mob_tribuna_controllerAI : public ScriptedAI
{
    mob_tribuna_controllerAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        Reset();
    }

    bool m_bIsRegularMode;

    std::list<Creature*> m_lKaddrakGUIDList;
    //std::list<Creature*> m_lMarnakGUIDList;
    //std::list<Creature*> m_lAbedneumGUIDList;

    bool m_bIsActivateKaddrak;
    bool m_bIsActivateMarnak;
    bool m_bIsActivateAbedneum;

    uint32 m_uiKaddrak_Encounter_timer;
    uint32 m_uiMarnak_Encounter_timer;
    uint32 m_uiAbedneum_Encounter_timer;

    void Reset()
    {
        m_bIsActivateKaddrak = false;
        m_bIsActivateMarnak = false;
        m_bIsActivateAbedneum = false;

        m_uiKaddrak_Encounter_timer = 1500;
        m_uiMarnak_Encounter_timer = 10000;
        m_uiAbedneum_Encounter_timer = 10000;

        m_lKaddrakGUIDList.clear();
        //m_lMarnakGUIDList.clear();
        //m_lAbedneumGUIDList.clear();
    }

    void UpdateFacesList()
    {
        GetCreatureListWithEntryInGrid(m_lKaddrakGUIDList, me, NPC_KADDRAK, 50.0f);
        if (!m_lKaddrakGUIDList.empty())
        {
            uint32 uiPositionCounter = 0;
            for(std::list<Creature*>::iterator itr = m_lKaddrakGUIDList.begin(); itr != m_lKaddrakGUIDList.end(); ++itr)
            {
                if ((*itr)->isAlive())
                {
                    if (uiPositionCounter == 0)
                    {
                        (*itr)->GetMap()->CreatureRelocation((*itr), 927.265, 333.200, 218.780, (*itr)->GetOrientation());
                        (*itr)->SendMonsterMove(927.265, 333.200, 218.780, SPLINETYPE_NORMAL, (*itr)->GetSplineFlags(), 1);
                    }
                    else
                    {
                        (*itr)->GetMap()->CreatureRelocation((*itr), 921.745, 328.076, 218.780, (*itr)->GetOrientation());
                        (*itr)->SendMonsterMove(921.745, 328.076, 218.780, SPLINETYPE_NORMAL, (*itr)->GetSplineFlags(), 1);
                    }
                }
                ++uiPositionCounter;
            }
        }
        //GetCreatureListWithEntryInGrid(m_lMarnakGUIDList, me, NPC_MARNAK, 50.0f);
        //GetCreatureListWithEntryInGrid(m_lAbedneumGUIDList, me, NPC_ABEDNEUM, 50.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_bIsActivateKaddrak)
        {
            if (m_uiKaddrak_Encounter_timer < diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    if (!m_lKaddrakGUIDList.empty())
                        for(std::list<Creature*>::iterator itr = m_lKaddrakGUIDList.begin(); itr != m_lKaddrakGUIDList.end(); ++itr)
                            if ((*itr)->isAlive())
                                (*itr)->CastSpell(pTarget, m_bIsRegularMode ? SPELL_GLARE_OF_THE_TRIBUNAL_H : SPELL_GLARE_OF_THE_TRIBUNAL, true);

                m_uiKaddrak_Encounter_timer = 1500;
            }
            else
                m_uiKaddrak_Encounter_timer -= diff;
        }
        if (m_bIsActivateMarnak)
        {
            if (m_uiMarnak_Encounter_timer < diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    if (Creature* pTemp = me->SummonCreature(NPC_DARK_MATTER_TARGET, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                    {
                        pTemp->SetDisplayId(11686);
                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pTemp->CastSpell(pTarget, m_bIsRegularMode ? SPELL_DARK_MATTER_H : SPELL_DARK_MATTER, true);
                    }

                m_uiMarnak_Encounter_timer = 30000 + rand()%1000;
            }
            else
                m_uiMarnak_Encounter_timer -= diff;
        }
        if (m_bIsActivateAbedneum)
        {
            if (m_uiAbedneum_Encounter_timer < diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    if (Creature* pTemp = me->SummonCreature(NPC_SEARING_GAZE_TARGET, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10000))
                    {
                        pTemp->SetDisplayId(11686);
                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pTemp->CastSpell(pTemp, m_bIsRegularMode ? SPELL_SEARING_GAZE_H : SPELL_SEARING_GAZE, true);
                    }

                m_uiAbedneum_Encounter_timer = 30000 + rand()%1000;
            }
            else
                m_uiAbedneum_Encounter_timer -= diff;
        }
    }
};

/*######
## npc_brann_hos
######*/

struct MANGOS_DLL_DECL npc_brann_hosAI : public npc_escortAI
{
    npc_brann_hosAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    bool m_bIsBattle;
    bool m_bIsLowHP;

    uint32 m_uiStep;
    uint32 m_uiPhase_timer;

    uint64 m_uiControllerGUID;
    std::list<uint64> m_lDwarfGUIDList;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_bIsLowHP = false;
            m_bIsBattle = false;

            m_uiStep = 0;
            m_uiPhase_timer = 0;

            m_uiControllerGUID = 0;

            DespawnDwarf();

            if(pInstance)
                SetInstanceData(TYPE_BRANN, NOT_STARTED);
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!m_bIsBattle)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 7:
                if (Creature* pCreature = GetClosestCreatureWithEntry(me, NPC_TRIBUNAL_OF_THE_AGES, 100.0f))
                {
                    if (!pCreature->isAlive())
                        pCreature->Respawn();
                    ((mob_tribuna_controllerAI*)pCreature->AI())->UpdateFacesList();
                    m_uiControllerGUID = pCreature->GetGUID();
                }
                break;
            case 13:
                DoScriptText(SAY_EVENT_INTRO_1, me);
                SetEscortPaused(true);
                SetRun(true);
                JumpToNextStep(20000);
                break;
            case 17:
                DoScriptText(SAY_EVENT_INTRO_2, me);
                if (pInstance)
                    pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_TRIBUNAL_CONSOLE));
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                SetEscortPaused(true);
                JumpToNextStep(8500);
                break;
            case 18:
                SetEscortPaused(true);
                break;
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_KILL_1, me); break;
            case 1: DoScriptText(SAY_KILL_2, me); break;
            case 2: DoScriptText(SAY_KILL_3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void DespawnDwarf()
    {
        if (m_lDwarfGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = m_lDwarfGUIDList.begin(); itr != m_lDwarfGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, *itr))
            {
                if (pTemp->isAlive())
                    pTemp->ForcedDespawn(1000);
            }
        }

        m_lDwarfGUIDList.clear();
    }

    void SpawnDwarf(uint32 uiType)
    {
        switch(uiType)
        {
            case 1:
            {
                uint32 uiSpawnNumber = (m_bIsRegularMode ? 3 : 2);
                for (uint8 i = 0; i < uiSpawnNumber; ++i)
                    me->SummonCreature(NPC_DARK_RUNE_PROTECTOR, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                me->SummonCreature(NPC_DARK_RUNE_STORMCALLER, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            }
            case 2:
                for (uint8 i = 0; i < 2; ++i)
                    me->SummonCreature(NPC_DARK_RUNE_STORMCALLER, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
            case 3:
                me->SummonCreature(NPC_IRON_GOLEM_CUSTODIAN, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        m_lDwarfGUIDList.push_back(pSummoned->GetGUID());
        pSummoned->AddThreat(me, 0.0f);
        pSummoned->AI()->AttackStart(me);
    }

    void JumpToNextStep(uint32 uiTimer)
    {
        m_uiPhase_timer = uiTimer;
        m_uiStep++;
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (m_uiPhase_timer < diff)
        {
            switch(m_uiStep)
            {
                case 0: // unused
                    break;
                case 1:
                    if (pInstance)
                    {
                        if (pInstance->GetData(TYPE_BRANN) != NOT_STARTED)
                            return;

                        SetInstanceData(TYPE_BRANN, IN_PROGRESS);
                    }
                    m_bIsBattle = false;
                    DoScriptText(SAY_ESCORT_START, me);
                    JumpToNextStep(0);
                    break;
                case 3:
                    SetEscortPaused(false);
                    JumpToNextStep(0);
                    break;
                case 5:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_INTRO_3_ABED, pTemp);
                    JumpToNextStep(8500);
                    break;
                case 6:
                    DoScriptText(SAY_EVENT_A_1, me);
                    JumpToNextStep(6500);
                    break;
                case 7:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_KADDRAK))))
                            DoScriptText(SAY_EVENT_A_2_KADD, pTemp);
                    JumpToNextStep(12500);
                    break;
                case 8:
                    DoScriptText(SAY_EVENT_A_3, me);
                    if (pInstance)
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_KADDRAK));
                    if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_uiControllerGUID)))
                        ((mob_tribuna_controllerAI*)pTemp->AI())->m_bIsActivateKaddrak = true;
                    JumpToNextStep(5000);
                    break;
                case 9:
                    SpawnDwarf(1);
                    JumpToNextStep(20000);
                    break;
                case 10:
                    DoScriptText(SAY_EVENT_B_1, me);
                    JumpToNextStep(6000);
                    break;
                case 11:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_MARNAK))))
                            DoScriptText(SAY_EVENT_B_2_MARN, pTemp);
                    SpawnDwarf(1);
                    JumpToNextStep(20000);
                    break;
                case 12:
                    DoScriptText(SAY_EVENT_B_3, me);
                    if (pInstance)
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_MARNAK));
                    if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_uiControllerGUID)))
                        ((mob_tribuna_controllerAI*)pTemp->AI())->m_bIsActivateMarnak = true;
                    JumpToNextStep(10000);
                    break;
                case 13:
                    SpawnDwarf(1);
                    JumpToNextStep(10000);
                    break;
                case 14:
                    SpawnDwarf(2);
                    JumpToNextStep(20000);
                    break;
                case 15:
                    DoScriptText(SAY_EVENT_C_1, me);
                    SpawnDwarf(1);
                    JumpToNextStep(10000);
                    break;
                case 16:
                    SpawnDwarf(2);
                    JumpToNextStep(20000);
                    break;
                case 17:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_C_2_ABED, pTemp);
                    SpawnDwarf(1);
                    JumpToNextStep(20000);
                    break;
                case 18:
                    DoScriptText(SAY_EVENT_C_3, me);
                    if (pInstance)
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_ABEDNEUM));
                    if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_uiControllerGUID)))
                        ((mob_tribuna_controllerAI*)pTemp->AI())->m_bIsActivateAbedneum = true;
                    JumpToNextStep(5000);
                    break;
                case 19:
                    SpawnDwarf(2);
                    JumpToNextStep(10000);
                    break;
                case 20:
                    SpawnDwarf(1);
                    JumpToNextStep(15000);
                    break;
                case 21:
                    DoScriptText(SAY_EVENT_D_1, me);
                    SpawnDwarf(3);
                    JumpToNextStep(20000);
                    break;
                case 22:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_D_2_ABED, pTemp);
                    SpawnDwarf(1);
                    JumpToNextStep(5000);
                    break;
                case 23:
                    SpawnDwarf(2);
                    JumpToNextStep(15000);
                    break;
                case 24:
                    DoScriptText(SAY_EVENT_D_3, me);
                    SpawnDwarf(3);
                    JumpToNextStep(5000);
                    break;
                case 25:
                    SpawnDwarf(1);
                    JumpToNextStep(5000);
                    break;
                case 26:
                    SpawnDwarf(2);
                    JumpToNextStep(10000);
                    break;
                case 27:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_D_4_ABED, pTemp);
                    SpawnDwarf(1);
                    JumpToNextStep(10000);
                    break;
                case 28:
                    DoScriptText(SAY_EVENT_END_01, me);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    if (pInstance)
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_SKY_FLOOR));
                    if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_uiControllerGUID)))
                        pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    m_bIsBattle = true;
                    SetEscortPaused(false);
                    JumpToNextStep(6500);
                    break;
                case 29:
                    DoScriptText(SAY_EVENT_END_02, me);
                    JumpToNextStep(5500);
                    break;
                case 30:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_END_03_ABED, pTemp);
                    JumpToNextStep(8500);
                    break;
                case 31:
                    DoScriptText(SAY_EVENT_END_04, me);
                    JumpToNextStep(11500);
                    break;
                case 32:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_END_05_ABED, pTemp);
                    JumpToNextStep(11500);
                    break;
                case 33:
                    DoScriptText(SAY_EVENT_END_06, me);
                    JumpToNextStep(4500);
                    break;
                case 34:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_END_07_ABED, pTemp);
                    JumpToNextStep(22500);
                    break;
                case 35:
                    DoScriptText(SAY_EVENT_END_08, me);
                    JumpToNextStep(7500);
                    break;
                case 36:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_KADDRAK))))
                            DoScriptText(SAY_EVENT_END_09_KADD, pTemp);
                    JumpToNextStep(18500);
                    break;
                case 37:
                    DoScriptText(SAY_EVENT_END_10, me);
                    JumpToNextStep(5500);
                    break;
                case 38:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_KADDRAK))))
                            DoScriptText(SAY_EVENT_END_11_KADD, pTemp);
                    JumpToNextStep(20500);
                    break;
                case 39:
                    DoScriptText(SAY_EVENT_END_12, me);
                    JumpToNextStep(2500);
                    break;
                case 40:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_KADDRAK))))
                            DoScriptText(SAY_EVENT_END_13_KADD, pTemp);
                    JumpToNextStep(19500);
                    break;
                case 41:
                    DoScriptText(SAY_EVENT_END_14, me);
                    JumpToNextStep(10500);
                    break;
                case 42:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_MARNAK))))
                            DoScriptText(SAY_EVENT_END_15_MARN, pTemp);
                    JumpToNextStep(6500);
                    break;
                case 43:
                    DoScriptText(SAY_EVENT_END_16, me);
                    JumpToNextStep(6500);
                    break;
                case 44:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_MARNAK))))
                            DoScriptText(SAY_EVENT_END_17_MARN, pTemp);
                    JumpToNextStep(25500);
                    break;
                case 45:
                    DoScriptText(SAY_EVENT_END_18, me);
                    JumpToNextStep(23500);
                    break;
                case 46:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_MARNAK))))
                            DoScriptText(SAY_EVENT_END_19_MARN, pTemp);
                    JumpToNextStep(3500);
                    break;
                case 47:
                    DoScriptText(SAY_EVENT_END_20, me);
                    JumpToNextStep(8500);
                    break;
                case 48:
                    if (pInstance)
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ABEDNEUM))))
                            DoScriptText(SAY_EVENT_END_21_ABED, pTemp);
                    JumpToNextStep(5500);
                    break;
                case 49:
                {
                    if (pInstance)
                    {
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_KADDRAK));
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_MARNAK));
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_ABEDNEUM));
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(DATA_GO_SKY_FLOOR));
                        SetInstanceData(TYPE_BRANN, DONE);
                    }

                    Player* pPlayer = GetPlayerForEscort();
                    if (pPlayer)
                        pPlayer->GroupEventHappens(QUEST_HALLS_OF_STONE, me);

                    me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    me->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    JumpToNextStep(180000);
                    break;
                }
                case 50:
                    SetEscortPaused(false);
                    break;
            }
        }
        else m_uiPhase_timer -= diff;
 
        if (!m_bIsLowHP && (me->GetHealth()*100 / me->GetMaxHealth()) <= 30)
        {
            DoScriptText(SAY_LOW_HEALTH, me);
            m_bIsLowHP = true;
        }
        else if (m_bIsLowHP && (me->GetHealth()*100 / me->GetMaxHealth()) > 30)
            m_bIsLowHP = false;
    }
};

bool GossipHello_npc_brann_hos(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

	if(((npc_brann_hosAI*)pCreature->AI())->m_uiStep < 49)
	{
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
		pPlayer->SEND_GOSSIP_MENU(TEXT_ID_START, pCreature->GetGUID());
	}
	else
	{
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PROGRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
		pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetGUID());
	}
    return true;
}

bool GossipSelect_npc_brann_hos(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1 || uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        ((npc_brann_hosAI*)pCreature->AI())->m_uiStep = 1;
        ((npc_brann_hosAI*)pCreature->AI())->Start(true, false, pPlayer->GetGUID());
    }

    return true;
}

CreatureAI* GetAI_npc_brann_hos(Creature* pCreature)
{
    return new npc_brann_hosAI(pCreature);
}

CreatureAI* GetAI_mob_tribuna_controller(Creature* pCreature)
{
    return new mob_tribuna_controllerAI (pCreature);
}

void AddSC_halls_of_stone()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_brann_hos";
    newscript->GetAI = &GetAI_npc_brann_hos;
    newscript->pGossipHello = &GossipHello_npc_brann_hos;
    newscript->pGossipSelect = &GossipSelect_npc_brann_hos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_tribuna_controller";
    newscript->GetAI = &GetAI_mob_tribuna_controller;
    newscript->RegisterSelf();
}
