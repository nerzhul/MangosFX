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
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests  80%     NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 5650
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_kingdom_of_dalaran_quests   Misc NPC's gossip option related to quests 12791, 12794 and 12796
npc_lunaclaw_spirit     100%    Appears at two different locations, quest 6001/6002
npc_mount_vendor        100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_rogue_trainer       80%     Scripted trainers, so they are able to offer item 17126 for class quest 6681
npc_sayge               100%    Darkmoon event fortune teller, buff player based on answers given
EndContentData */

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon creature at smaller range
    SPAWNTYPE_ALARMBOT,                                     // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 m_uiThisCreatureEntry;
    uint32 m_uiSpawnedCreatureEntry;
    SpawnType m_SpawnType;
};

enum
{
    SPELL_GUARDS_MARK               = 38067,
    AURA_DURATION_TIME_LEFT         = 5000
};

const float RANGE_TRIPWIRE          = 15.0f;
const float RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation m_aSpawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

struct MANGOS_DLL_DECL npc_air_force_botsAI : public ScriptedAI
{
    npc_air_force_botsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pSpawnAssoc = NULL;
        m_uiSpawnedGUID = 0;

        // find the correct spawnhandling
        static uint32 uiEntryCount = sizeof(m_aSpawnAssociations)/sizeof(SpawnAssociation);

        for (uint8 i=0; i<uiEntryCount; ++i)
        {
            if (m_aSpawnAssociations[i].m_uiThisCreatureEntry == pCreature->GetEntry())
            {
                m_pSpawnAssoc = &m_aSpawnAssociations[i];
                break;
            }
        }

        if (!m_pSpawnAssoc)
            error_db_log("SD2: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", pCreature->GetEntry());
        else
        {
            CreatureInfo const* spawnedTemplate = GetCreatureTemplateStore(m_pSpawnAssoc->m_uiSpawnedCreatureEntry);

            if (!spawnedTemplate)
            {
                m_pSpawnAssoc = NULL;
                error_db_log("SD2: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
                return;
            }
        }
    }

    SpawnAssociation* m_pSpawnAssoc;
    uint64 m_uiSpawnedGUID;

    void Reset() { }

    Creature* SummonGuard()
    {
        Creature* pSummoned = me->SummonCreature(m_pSpawnAssoc->m_uiSpawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

        if (pSummoned)
            m_uiSpawnedGUID = pSummoned->GetGUID();
        else
        {
            error_db_log("SD2: npc_air_force_bots: wasn't able to spawn creature %u", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
            m_pSpawnAssoc = NULL;
        }

        return pSummoned;
    }

    Creature* GetSummonedGuard()
    {
        Creature* pCreature = (Creature*)Unit::GetUnit(*me, m_uiSpawnedGUID);

        if (pCreature && pCreature->isAlive())
            return pCreature;

        return NULL;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_pSpawnAssoc)
            return;

        if (pWho->isTargetableForAttack() && me->IsHostileTo(pWho))
        {
            Player* pPlayerTarget = pWho->GetTypeId() == TYPEID_PLAYER ? (Player*)pWho : NULL;

            // airforce guards only spawn for players
            if (!pPlayerTarget)
                return;

            Creature* pLastSpawnedGuard = m_uiSpawnedGUID == 0 ? NULL : GetSummonedGuard();

            // prevent calling Unit::GetUnit at next MoveInLineOfSight call - speedup
            if (!pLastSpawnedGuard)
                m_uiSpawnedGUID = 0;

            switch(m_pSpawnAssoc->m_SpawnType)
            {
                case SPAWNTYPE_ALARMBOT:
                {
                    if (!pWho->IsWithinDistInMap(me, RANGE_GUARDS_MARK))
                        return;

                    Aura* pMarkAura = pWho->GetAura(SPELL_GUARDS_MARK, 0);
                    if (pMarkAura)
                    {
                        // the target wasn't able to move out of our range within 25 seconds
                        if (!pLastSpawnedGuard)
                        {
                            pLastSpawnedGuard = SummonGuard();

                            if (!pLastSpawnedGuard)
                                return;
                        }

                        if (pMarkAura->GetAuraDuration() < AURA_DURATION_TIME_LEFT)
                        {
                            if (!pLastSpawnedGuard->getVictim())
                                pLastSpawnedGuard->AI()->AttackStart(pWho);
                        }
                    }
                    else
                    {
                        if (!pLastSpawnedGuard)
                            pLastSpawnedGuard = SummonGuard();

                        if (!pLastSpawnedGuard)
                            return;

                        pLastSpawnedGuard->CastSpell(pWho, SPELL_GUARDS_MARK, true);
                    }
                    break;
                }
                case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                {
                    if (!pWho->IsWithinDistInMap(me, RANGE_TRIPWIRE))
                        return;

                    if (!pLastSpawnedGuard)
                        pLastSpawnedGuard = SummonGuard();

                    if (!pLastSpawnedGuard)
                        return;

                    // ROOFTOP only triggers if the player is on the ground
                    if (!pPlayerTarget->IsFlying())
                    {
                        if (!pLastSpawnedGuard->getVictim())
                            pLastSpawnedGuard->AI()->AttackStart(pWho);
                    }
                    break;
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_air_force_bots(Creature* pCreature)
{
    return new npc_air_force_botsAI(pCreature);
}

/*########
# npc_chicken_cluck
#########*/

enum
{
    EMOTE_A_HELLO           = -1000204,
    EMOTE_H_HELLO           = -1000205,
    EMOTE_CLUCK_TEXT2       = -1000206,

    QUEST_CLUCK             = 3861,
    FACTION_FRIENDLY        = 35,
    FACTION_CHICKEN         = 31
};

struct MANGOS_DLL_DECL npc_chicken_cluckAI : public ScriptedAI
{
    npc_chicken_cluckAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiResetFlagTimer;

    void Reset()
    {
        m_uiResetFlagTimer = 120000;

        me->setFaction(FACTION_CHICKEN);
        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        if (uiEmote == TEXTEMOTE_CHICKEN)
        {
            if (!urand(0, 29))
            {
                if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    me->setFaction(FACTION_FRIENDLY);

                    DoScriptText(EMOTE_A_HELLO, me);

                    /* are there any difference in texts, after 3.x ?
                    if (pPlayer->GetTeam() == HORDE)
                        DoScriptText(EMOTE_H_HELLO, me);
                    else
                        DoScriptText(EMOTE_A_HELLO, me);
                    */
                }
            }
        }

        if (uiEmote == TEXTEMOTE_CHEER)
        {
            if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
            {
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->setFaction(FACTION_FRIENDLY);
                DoScriptText(EMOTE_CLUCK_TEXT2, me);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        // Reset flags after a certain time has passed so that the next player has to start the 'event' again
        if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
        {
            if (m_uiResetFlagTimer < diff)
                EnterEvadeMode();
            else
                m_uiResetFlagTimer -= diff;
        }

        if (me->SelectHostileTarget() && me->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_chicken_cluck(Creature* pCreature)
{
    return new npc_chicken_cluckAI(pCreature);
}

bool QuestAccept_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

bool QuestComplete_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

/*######
## npc_dancing_flames
######*/

struct MANGOS_DLL_DECL npc_dancing_flamesAI : public ScriptedAI
{
    npc_dancing_flamesAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_DANCE)
            me->CastSpell(pPlayer,47057,false);
    }
};

CreatureAI* GetAI_npc_dancing_flames(Creature* pCreature)
{
    return new npc_dancing_flamesAI(pCreature);
}

/*######
## Triage quest
######*/

#define SAY_DOC1    -1000201
#define SAY_DOC2    -1000202
#define SAY_DOC3    -1000203

#define DOCTOR_ALLIANCE     12939
#define DOCTOR_HORDE        12920
#define ALLIANCE_COORDS     7
#define HORDE_COORDS        6

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[]=
{
    {-3757.38, -4533.05, 14.16, 3.62},                      // Top-far-right bunk as seen from entrance
    {-3754.36, -4539.13, 14.16, 5.13},                      // Top-far-left bunk
    {-3749.54, -4540.25, 14.28, 3.34},                      // Far-right bunk
    {-3742.10, -4536.85, 14.28, 3.64},                      // Right bunk near entrance
    {-3755.89, -4529.07, 14.05, 0.57},                      // Far-left bunk
    {-3749.51, -4527.08, 14.07, 5.26},                      // Mid-left bunk
    {-3746.37, -4525.35, 14.16, 5.22},                      // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96
#define A_RUNTOY -4531.52
#define A_RUNTOZ 11.91

static Location HordeCoords[]=
{
    {-1013.75, -3492.59, 62.62, 4.34},                      // Left, Behind
    {-1017.72, -3490.92, 62.62, 4.34},                      // Right, Behind
    {-1015.77, -3497.15, 62.82, 4.34},                      // Left, Mid
    {-1019.51, -3495.49, 62.82, 4.34},                      // Right, Mid
    {-1017.25, -3500.85, 62.98, 4.34},                      // Left, front
    {-1020.95, -3499.21, 62.98, 4.34}                       // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44
#define H_RUNTOY -3508.48
#define H_RUNTOZ 62.96

const uint32 AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

const uint32 HordeSoldierId[3] =
{
    12923,                                                  //12923 Injured Soldier
    12924,                                                  //12924 Badly injured Soldier
    12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/

struct MANGOS_DLL_DECL npc_doctorAI : public ScriptedAI
{
    npc_doctorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint64 Playerguid;

    uint32 SummonPatient_Timer;
    uint32 SummonPatientCount;
    uint32 PatientDiedCount;
    uint32 PatientSavedCount;

    bool Event;

    std::list<uint64> Patients;
    std::vector<Location*> Coordinates;

    void Reset()
    {
        Playerguid = 0;

        SummonPatient_Timer = 10000;
        SummonPatientCount = 0;
        PatientDiedCount = 0;
        PatientSavedCount = 0;

        Patients.clear();
        Coordinates.clear();

        Event = false;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void BeginEvent(Player* pPlayer);
    void PatientDied(Location* Point);
    void PatientSaved(Creature* soldier, Player* pPlayer, Location* Point);
    void UpdateAI(const uint32 diff);
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

struct MANGOS_DLL_DECL npc_injured_patientAI : public ScriptedAI
{
    npc_injured_patientAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint64 Doctorguid;
    Location* Coord;

    void Reset()
    {
        Doctorguid = 0;
        Coord = NULL;

        //no select
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //no regen health
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        //to make them lay with face down
        me->SetStandState(UNIT_STAND_STATE_DEAD);

        uint32 mobId = me->GetEntry();

        switch (mobId)
        {                                                   //lower max health
            case 12923:
            case 12938:                                     //Injured Soldier
                me->SetHealth(uint32(me->GetMaxHealth()*.75));
                break;
            case 12924:
            case 12936:                                     //Badly injured Soldier
                me->SetHealth(uint32(me->GetMaxHealth()*.50));
                break;
            case 12925:
            case 12937:                                     //Critically injured Soldier
                me->SetHealth(uint32(me->GetMaxHealth()*.25));
                break;
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && me->isAlive() && spell->Id == 20804)
        {
            if ((((Player*)caster)->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (((Player*)caster)->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
            {
                if (Doctorguid)
                {
                    if (Creature* Doctor = ((Creature*)Unit::GetUnit((*me), Doctorguid)))
                        ((npc_doctorAI*)Doctor->AI())->PatientSaved(me, ((Player*)caster), Coord);
                }
            }
            //make not selectable
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //regen health
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            //stand up
            me->SetStandState(UNIT_STAND_STATE_STAND);

            switch(urand(0, 2))
            {
                case 0: DoScriptText(SAY_DOC1,me); break;
                case 1: DoScriptText(SAY_DOC2,me); break;
                case 2: DoScriptText(SAY_DOC3,me); break;
            }

            me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

            uint32 mobId = me->GetEntry();

            switch (mobId)
            {
                case 12923:
                case 12924:
                case 12925:
                    me->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case 12936:
                case 12937:
                case 12938:
                    me->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //lower HP on every world tick makes it a useful counter, not officlone though
        if (me->isAlive() && me->GetHealth() > 6)
        {
            me->SetHealth(uint32(me->GetHealth()-5));
        }

        if (me->isAlive() && me->GetHealth() <= 6)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->setDeathState(JUST_DIED);
            me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

            if (Doctorguid)
            {
                if (Creature* Doctor = ((Creature*)Unit::GetUnit((*me), Doctorguid)))
                    ((npc_doctorAI*)Doctor->AI())->PatientDied(Coord);
            }
        }
    }
};

CreatureAI* GetAI_npc_injured_patient(Creature* pCreature)
{
    return new npc_injured_patientAI(pCreature);
}

/*
npc_doctor (continue)
*/

void npc_doctorAI::BeginEvent(Player* pPlayer)
{
    Playerguid = pPlayer->GetGUID();

    SummonPatient_Timer = 10000;
    SummonPatientCount = 0;
    PatientDiedCount = 0;
    PatientSavedCount = 0;

    switch(me->GetEntry())
    {
        case DOCTOR_ALLIANCE:
            for(uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                Coordinates.push_back(&AllianceCoords[i]);
            break;
        case DOCTOR_HORDE:
            for(uint8 i = 0; i < HORDE_COORDS; ++i)
                Coordinates.push_back(&HordeCoords[i]);
            break;
    }

    Event = true;
    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
}

void npc_doctorAI::PatientDied(Location* Point)
{
    Player* pPlayer = ((Player*)Unit::GetUnit((*me), Playerguid));

    if (pPlayer && ((pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
    {
        ++PatientDiedCount;

        if (PatientDiedCount > 5 && Event)
        {
            if (pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(6624);
            else if (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(6622);

            Reset();
            return;
        }

        Coordinates.push_back(Point);
    }
    else
        // If no player or player abandon quest in progress
        Reset();
}

void npc_doctorAI::PatientSaved(Creature* soldier, Player* pPlayer, Location* Point)
{
    if (pPlayer && Playerguid == pPlayer->GetGUID())
    {
        if ((pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
        {
            ++PatientSavedCount;

            if (PatientSavedCount == 15)
            {
                if (!Patients.empty())
                {
                    std::list<uint64>::iterator itr;
                    for(itr = Patients.begin(); itr != Patients.end(); ++itr)
                    {
                        if (Creature* Patient = ((Creature*)Unit::GetUnit((*me), *itr)))
                            Patient->setDeathState(JUST_DIED);
                    }
                }

                if (pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->AreaExploredOrEventHappens(6624);
                else if (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->AreaExploredOrEventHappens(6622);

                Reset();
                return;
            }

            Coordinates.push_back(Point);
        }
    }
}

void npc_doctorAI::UpdateAI(const uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatient_Timer < diff)
        {
            Creature* Patient = NULL;
            Location* Point = NULL;

            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin()+rand()%Coordinates.size();
            uint32 patientEntry = 0;

            switch(me->GetEntry())
            {
                case DOCTOR_ALLIANCE: patientEntry = AllianceSoldierId[urand(0, 2)]; break;
                case DOCTOR_HORDE:    patientEntry = HordeSoldierId[urand(0, 2)]; break;
                default:
                    error_log("SD2: Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            Point = *itr;

            Patient = me->SummonCreature(patientEntry, Point->x, Point->y, Point->z, Point->o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);

            if (Patient)
            {
                //303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

                Patients.push_back(Patient->GetGUID());
                ((npc_injured_patientAI*)Patient->AI())->Doctorguid = me->GetGUID();

                if (Point)
                    ((npc_injured_patientAI*)Patient->AI())->Coord = Point;

                Coordinates.erase(itr);
            }
            SummonPatient_Timer = 10000;
            ++SummonPatientCount;
        }else SummonPatient_Timer -= diff;
    }
}

bool QuestAccept_npc_doctor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if ((pQuest->GetQuestId() == 6624) || (pQuest->GetQuestId() == 6622))
        ((npc_doctorAI*)pCreature->AI())->BeginEvent(pPlayer);

    return true;
}

CreatureAI* GetAI_npc_doctor(Creature* pCreature)
{
    return new npc_doctorAI(pCreature);
}

/*######
## npc_garments_of_quests
######*/

//TODO: get text for each NPC

enum
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    SAY_COMMON_HEALED       = -1000231,
    SAY_DG_KEL_THANKS       = -1000232,
    SAY_DG_KEL_GOODBYE      = -1000233,
    SAY_ROBERTS_THANKS      = -1000256,
    SAY_ROBERTS_GOODBYE     = -1000257,
    SAY_KORJA_THANKS        = -1000258,
    SAY_KORJA_GOODBYE       = -1000259,
    SAY_DOLF_THANKS         = -1000260,
    SAY_DOLF_GOODBYE        = -1000261,
    SAY_SHAYA_THANKS        = -1000262,
    SAY_SHAYA_GOODBYE       = -1000263,
};

struct MANGOS_DLL_DECL npc_garments_of_questsAI : public npc_escortAI
{
    npc_garments_of_questsAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    uint64 caster;

    bool bIsHealed;
    bool bCanRun;

    uint32 RunAwayTimer;

    void Reset()
    {
        caster = 0;

        bIsHealed = false;
        bCanRun = false;

        RunAwayTimer = 5000;

        me->SetStandState(UNIT_STAND_STATE_KNEEL);
        //expect database to have RegenHealth=0
        me->SetHealth(int(me->GetMaxHealth()*0.7));
    }

    void SpellHit(Unit* pCaster, const SpellEntry *Spell)
    {
        if (Spell->Id == SPELL_LESSER_HEAL_R2 || Spell->Id == SPELL_FORTITUDE_R1)
        {
            //not while in combat
            if (me->isInCombat())
                return;

            //nothing to be done now
            if (bIsHealed && bCanRun)
                return;

            if (pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                switch(me->GetEntry())
                {
                    case ENTRY_SHAYA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_SHAYA_THANKS,me,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,me,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_ROBERTS:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_ROBERTS_THANKS,me,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,me,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DOLF:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DOLF_THANKS,me,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,me,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_KORJA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_KORJA_THANKS,me,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,me,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DG_KEL:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (bIsHealed && !bCanRun && Spell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DG_KEL_THANKS,me,pCaster);
                                bCanRun = true;
                            }
                            else if (!bIsHealed && Spell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                caster = pCaster->GetGUID();
                                me->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED,me,pCaster);
                                bIsHealed = true;
                            }
                        }
                        break;
                }

                //give quest credit, not expect any special quest objectives
                if (bCanRun)
                    ((Player*)pCaster)->TalkedToCreature(me->GetEntry(),me->GetGUID());
            }
        }
    }

    void WaypointReached(uint32 uiPoint)
    {
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (bCanRun && !me->isInCombat())
        {
            if (RunAwayTimer <= diff)
            {
                if (Unit *pUnit = Unit::GetUnit(*me,caster))
                {
                    switch(me->GetEntry())
                    {
                        case ENTRY_SHAYA: DoScriptText(SAY_SHAYA_GOODBYE,me,pUnit); break;
                        case ENTRY_ROBERTS: DoScriptText(SAY_ROBERTS_GOODBYE,me,pUnit); break;
                        case ENTRY_DOLF: DoScriptText(SAY_DOLF_GOODBYE,me,pUnit); break;
                        case ENTRY_KORJA: DoScriptText(SAY_KORJA_GOODBYE,me,pUnit); break;
                        case ENTRY_DG_KEL: DoScriptText(SAY_DG_KEL_GOODBYE,me,pUnit); break;
                    }

                    Start(false,true);
                }
                else
                    EnterEvadeMode();                       //something went wrong

                RunAwayTimer = 30000;
            }else RunAwayTimer -= diff;
        }

        if (!CanDoSomething())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_garments_of_quests(Creature* pCreature)
{
    return new npc_garments_of_questsAI(pCreature);
}

/*######
## npc_guardian
######*/

#define SPELL_DEATHTOUCH                5

struct MANGOS_DLL_DECL npc_guardianAI : public ScriptedAI
{
    npc_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (me->isAttackReady())
        {
            me->CastSpell(me->getVictim(),SPELL_DEATHTOUCH, true);
            me->resetAttackTimer();
        }
    }
};

CreatureAI* GetAI_npc_guardian(Creature* pCreature)
{
    return new npc_guardianAI(pCreature);
}

/*######
## npc_kingdom_of_dalaran_quests
######*/

enum
{
    SPELL_TELEPORT_DALARAN  = 53360,
    ITEM_KT_SIGNET          = 39740,
    QUEST_MAGICAL_KINGDOM_A = 12794,
    QUEST_MAGICAL_KINGDOM_H = 12791,
    QUEST_MAGICAL_KINGDOM_N = 12796
};

#define GOSSIP_ITEM_TELEPORT_TO "I am ready to be teleported to Dalaran."

bool GossipHello_npc_kingdom_of_dalaran_quests(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->HasItemCount(ITEM_KT_SIGNET,1) && (!pPlayer->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_A) ||
        !pPlayer->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_H) || !pPlayer->GetQuestRewardStatus(QUEST_MAGICAL_KINGDOM_N)))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TELEPORT_TO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_kingdom_of_dalaran_quests(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer,SPELL_TELEPORT_DALARAN,false);
    }
    return true;
}

#define GOSSIP_ITEM_BAD_COND "Conditions d'acces au Click To Play invalides. J'ai compris."
#define GOSSIP_ITEM_START "Je souhaite utiliser le Click to Play, je ne devrai pas quitter cette fenetre avant d'avoir fini"
#define GOSSIP_ITEM_PVE "J'ai mis les sacs et maintenant je souhaite obtenir mon stuff PvE (16 emplacements)"
#define GOSSIP_ITEM_PVP "Je souhaite obtenir mon stuff PvP (16 emplacements)"
#define GOSSIP_ITEM_SPEC_1 "Je souhaite obtenir un stuff DPS cac"
#define GOSSIP_ITEM_SPEC_2 "Je souhaite obtenir un stuff heal"
#define GOSSIP_ITEM_SPEC_3 "Je souhaite obtenir un stuff tank"
#define GOSSIP_ITEM_SPEC_4 "Je souhaite obtenir un stuff DPS distant"

bool GossipHello_bd_special_vendor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

	if((pPlayer->getLevel() == 1 || pPlayer->getClass() == CLASS_DEATH_KNIGHT && pPlayer->getLevel() == 55) && !pPlayer->isCTP())
	{
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
	}
	else
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BAD_COND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

	pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_bd_special_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	switch(uiAction)
	{
		case GOSSIP_ACTION_INFO_DEF+1:
		{
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
			pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
			break;
		}
		case GOSSIP_ACTION_INFO_DEF+2:
		{
			// Buys
			pPlayer->SetBankBagSlotCount(6);
			pPlayer->GiveLevel(80);
			pPlayer->UpdateSpecCount(2);
			pPlayer->learnSpell(63645, 0,false);
			pPlayer->learnSpell(63644, 0,false);
			// monte
			pPlayer->learnSpell(34093, 0,false);
			pPlayer->learnSpell(54197, 0,false);
			pPlayer->learnSpell(48954, 0,false);
			// secou
			pPlayer->learnSpell(45542,0,false);
			pPlayer->SetSkill(129,pPlayer->GetSkillStep(129),450,450);
			// cuisine
			pPlayer->learnSpell(51296,0,false);
			pPlayer->SetSkill(185,pPlayer->GetSkillStep(185),450,450);
			// peche
			pPlayer->learnSpell(51294,0,false);
			pPlayer->SetSkill(356,pPlayer->GetSkillStep(356),450,450);
			// 4 sacs
			pPlayer->AddItem(41599);
			pPlayer->AddItem(41599);
			pPlayer->AddItem(41599);
			pPlayer->AddItem(41599);

			pPlayer->SetCTP(true);

			pPlayer->SaveToDB();
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PVE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
			pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
			break;
		}
		case GOSSIP_ACTION_INFO_DEF+3:
		{
			if(pPlayer->getClass() == CLASS_MAGE || pPlayer->getClass() == CLASS_WARLOCK || pPlayer->getClass() == CLASS_PRIEST ||
				pPlayer->getClass() == CLASS_DRUID || pPlayer->getClass() == CLASS_DEATH_KNIGHT || pPlayer->getClass() == CLASS_SHAMAN ||
				pPlayer->getClass() == CLASS_HUNTER)
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPEC_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);

			if(pPlayer->getClass() == CLASS_ROGUE || pPlayer->getClass() == CLASS_DRUID || pPlayer->getClass() == CLASS_SHAMAN ||
				pPlayer->getClass() == CLASS_WARRIOR || pPlayer->getClass() == CLASS_DEATH_KNIGHT || pPlayer->getClass() == CLASS_PALADIN)
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPEC_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);

			if(pPlayer->getClass() == CLASS_WARRIOR || pPlayer->getClass() == CLASS_PALADIN || pPlayer->getClass() == CLASS_DEATH_KNIGHT ||
				pPlayer->getClass() == CLASS_DRUID)
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPEC_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);

			if(pPlayer->getClass() == CLASS_PRIEST || pPlayer->getClass() == CLASS_PALADIN || pPlayer->getClass() == CLASS_SHAMAN ||
				pPlayer->getClass() == CLASS_DRUID)
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPEC_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);

			pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
			break;
		}
		case GOSSIP_ACTION_INFO_DEF+4:
		{
			// STUFF DPS CASTER
			if(pPlayer->getClass() == CLASS_HUNTER)
			{
				pPlayer->AddItem(45224);
				pPlayer->AddItem(45524);
				pPlayer->AddItem(45562);
				pPlayer->AddItem(45157);
				pPlayer->AddItem(46048);
				pPlayer->AddItem(45836);
				pPlayer->AddItem(45164);
				pPlayer->AddItem(45143);
				pPlayer->AddItem(45820);
				pPlayer->AddItem(45543);
				pPlayer->AddItem(45263);
				pPlayer->AddItem(45931);
				pPlayer->AddItem(45553);
				pPlayer->AddItem(45454);
			}
			else if(pPlayer->getClass() == CLASS_PRIEST)
			{
				pPlayer->AddItem(45541);
				pPlayer->AddItem(45240);
				pPlayer->AddItem(45483);
				pPlayer->AddItem(45515);
				pPlayer->AddItem(45438);
				pPlayer->AddItem(46045);
				pPlayer->AddItem(45435);
				pPlayer->AddItem(45848);
				pPlayer->AddItem(51805);
				pPlayer->AddItem(46068);
				pPlayer->AddItem(46051);
				pPlayer->AddItem(45446);
				pPlayer->AddItem(45831);
				pPlayer->AddItem(45275);
			}
			else // warlock, mage, druid, shaman
			{
				pPlayer->AddItem(46042);
				pPlayer->AddItem(45515);
				pPlayer->AddItem(45438);
				pPlayer->AddItem(51805);
				pPlayer->AddItem(46051);
				pPlayer->AddItem(45446);
				if(pPlayer->getClass() == CLASS_SHAMAN)
				{
					pPlayer->AddItem(45531);
					pPlayer->AddItem(45513);
					pPlayer->AddItem(45837);
					pPlayer->AddItem(45118);
					pPlayer->AddItem(45544);
					pPlayer->AddItem(45440);
					pPlayer->AddItem(45554);
					pPlayer->AddItem(45187);
				}
				else if(pPlayer->getClass() == CLASS_DRUID)
				{
					pPlayer->AddItem(45237);
					pPlayer->AddItem(45565);
					pPlayer->AddItem(45839);
					pPlayer->AddItem(45439);
					pPlayer->AddItem(45847);
					pPlayer->AddItem(45492);
					pPlayer->AddItem(45185);
					pPlayer->AddItem(45149);
				}
				else // warlock, mage
				{
					pPlayer->AddItem(45272);
					pPlayer->AddItem(45567);
					pPlayer->AddItem(46045);
					pPlayer->AddItem(45150);
					pPlayer->AddItem(40398);
					pPlayer->AddItem(46068);
					pPlayer->AddItem(45831);
					pPlayer->AddItem(45275);
				}
			}
		}
		case GOSSIP_ACTION_INFO_DEF+5:
		{
			// STUFF DPS CAC
			if(uiAction == GOSSIP_ACTION_INFO_DEF+5)
			{
				pPlayer->AddItem(45263);
				if(pPlayer->getClass() == CLASS_DRUID || pPlayer->getClass() == CLASS_SHAMAN || pPlayer->getClass() == CLASS_ROGUE)
				{
					pPlayer->AddItem(45224);
					pPlayer->AddItem(45157);
					pPlayer->AddItem(46048);
					pPlayer->AddItem(51807);
					pPlayer->AddItem(46038);
					if(pPlayer->getClass() == CLASS_SHAMAN)
					{
						pPlayer->AddItem(46205);
						pPlayer->AddItem(45562);
						pPlayer->AddItem(45109);
						pPlayer->AddItem(45164);
						pPlayer->AddItem(45504);
						pPlayer->AddItem(45543);
						pPlayer->AddItem(45553);
						pPlayer->AddItem(45454);
					}
					else if(pPlayer->getClass() == CLASS_ROGUE || pPlayer->getClass() == CLASS_DRUID)
					{
						pPlayer->AddItem(45453);
						pPlayer->AddItem(45162);
						pPlayer->AddItem(45325);
						pPlayer->AddItem(45523);
						pPlayer->AddItem(44011);
						pPlayer->AddItem(45265);
						pPlayer->AddItem(45829);
						pPlayer->AddItem(45869);
					}
				}
				else // Warrior, pala, dk
				{
					pPlayer->AddItem(46320);
					pPlayer->AddItem(45225);
					pPlayer->AddItem(45559);
					pPlayer->AddItem(45250);
					pPlayer->AddItem(45106);
					pPlayer->AddItem(45833);
					pPlayer->AddItem(44006);
					pPlayer->AddItem(45841);
					pPlayer->AddItem(46040);
					pPlayer->AddItem(45320);
					pPlayer->AddItem(45931);
					pPlayer->AddItem(45550);
					pPlayer->AddItem(45888);
				}
			}
		}
		case GOSSIP_ACTION_INFO_DEF+6:
		{
			if(uiAction == GOSSIP_ACTION_INFO_DEF+6)
			{
				pPlayer->AddItem(45158);
				pPlayer->AddItem(45507);
				// STUFF TANK
				if(pPlayer->getClass() == CLASS_PALADIN || pPlayer->getClass() == CLASS_DEATH_KNIGHT ||
					pPlayer->getClass() == CLASS_WARRIOR)
				{
					pPlayer->AddItem(45139);
					pPlayer->AddItem(45334);
					pPlayer->AddItem(45560);
					pPlayer->AddItem(45326);
					pPlayer->AddItem(45871);
					pPlayer->AddItem(45228);
					pPlayer->AddItem(45502);
					pPlayer->AddItem(40589);
					pPlayer->AddItem(40387);
					pPlayer->AddItem(45251);
					pPlayer->AddItem(45111);
					pPlayer->AddItem(45825);
				}
				else // druid
				{
					pPlayer->AddItem(45224);
					pPlayer->AddItem(46159);
					pPlayer->AddItem(45232);
					pPlayer->AddItem(46048);
					pPlayer->AddItem(45456);
					pPlayer->AddItem(45325);
					pPlayer->AddItem(45993);
					pPlayer->AddItem(44011);
					pPlayer->AddItem(45945);
					pPlayer->AddItem(45265);
					pPlayer->AddItem(45829);
					pPlayer->AddItem(45869);
				}
			}
		}
		case GOSSIP_ACTION_INFO_DEF+7:
		{
			// STUFF HEAL
			if(uiAction == GOSSIP_ACTION_INFO_DEF+7)
			{
				pPlayer->AddItem(46321);
				pPlayer->AddItem(45113);
				pPlayer->AddItem(45438);
				pPlayer->AddItem(45822);
				pPlayer->AddItem(40532);
				pPlayer->AddItem(45446);
				if(pPlayer->getClass() == CLASS_PALADIN)
				{
					pPlayer->AddItem(45167);
					pPlayer->AddItem(45561);
					pPlayer->AddItem(45835);
					pPlayer->AddItem(45226);
					pPlayer->AddItem(45843);
					pPlayer->AddItem(40590);
					pPlayer->AddItem(45826);
					pPlayer->AddItem(45269);
				}
				else if(pPlayer->getClass() == CLASS_SHAMAN)
				{
					pPlayer->AddItem(45867);
					pPlayer->AddItem(45563);
					pPlayer->AddItem(45943);
					pPlayer->AddItem(45118);
					pPlayer->AddItem(45544);
					pPlayer->AddItem(45440);
					pPlayer->AddItem(45151);
					pPlayer->AddItem(45187);
				}
				else if(pPlayer->getClass() == CLASS_PRIEST)
				{
					pPlayer->AddItem(45272);
					pPlayer->AddItem(46050);
					pPlayer->AddItem(45273);
					pPlayer->AddItem(45532);
					pPlayer->AddItem(45848);
					pPlayer->AddItem(46068);
					pPlayer->AddItem(45831);
					pPlayer->AddItem(45275);
				}
				else // DRUID
				{
					pPlayer->AddItem(46186);
					pPlayer->AddItem(45565);
					pPlayer->AddItem(45293);
					pPlayer->AddItem(44007);
					pPlayer->AddItem(45482);
					pPlayer->AddItem(45492);
					pPlayer->AddItem(45556);
					pPlayer->AddItem(45149);
				}
			}
			// Suite
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PVP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);
			pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
			break;
		}
		case GOSSIP_ACTION_INFO_DEF+9:
		{
			if(pPlayer->getClass() == CLASS_MAGE || pPlayer->getClass() == CLASS_WARLOCK || pPlayer->getClass() == CLASS_PRIEST ||
				pPlayer->getClass() == CLASS_DRUID || pPlayer->getClass() == CLASS_SHAMAN ||
				pPlayer->getClass() == CLASS_HUNTER)
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPEC_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+10);

			if(pPlayer->getClass() == CLASS_ROGUE || pPlayer->getClass() == CLASS_DRUID || pPlayer->getClass() == CLASS_SHAMAN ||
				pPlayer->getClass() == CLASS_WARRIOR || pPlayer->getClass() == CLASS_DEATH_KNIGHT || pPlayer->getClass() == CLASS_PALADIN)
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPEC_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+11);

			if(pPlayer->getClass() == CLASS_PRIEST || pPlayer->getClass() == CLASS_PALADIN || pPlayer->getClass() == CLASS_SHAMAN ||
				pPlayer->getClass() == CLASS_DRUID)
				pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPEC_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+12);
			
			pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
			break;
		}
		case GOSSIP_ACTION_INFO_DEF+10:
		{
			pPlayer->AddItem(42116);
			pPlayer->AddItem(48999);
			// PVP dps caster
			if(pPlayer->getClass() == CLASS_PRIEST)
			{
				pPlayer->AddItem(42070);
				pPlayer->AddItem(41903);
				pPlayer->AddItem(41914);
				pPlayer->AddItem(41933);
				pPlayer->AddItem(41920);
				pPlayer->AddItem(41939);
				pPlayer->AddItem(41926);
				pPlayer->AddItem(42126);
				pPlayer->AddItem(42124);
				pPlayer->AddItem(42132);
				pPlayer->AddItem(42502);
				pPlayer->AddItem(42525);
				pPlayer->AddItem(42346);
				pPlayer->AddItem(41897);
				pPlayer->AddItem(42029);
				pPlayer->AddItem(41908);
			}
			else if(pPlayer->getClass() == CLASS_MAGE || pPlayer->getClass() == CLASS_DRUID ||
				pPlayer->getClass() == CLASS_WARLOCK || pPlayer->getClass() == CLASS_SHAMAN)
			{
				pPlayer->AddItem(42069);
				if(pPlayer->getClass() == CLASS_DRUID)
				{
					pPlayer->AddItem(41836);
					pPlayer->AddItem(41326);
					pPlayer->AddItem(41280);
					pPlayer->AddItem(41315);
					pPlayer->AddItem(41292);
					pPlayer->AddItem(41303);
					pPlayer->AddItem(42126);
					pPlayer->AddItem(42124);
					pPlayer->AddItem(42132);
					pPlayer->AddItem(44419);
					pPlayer->AddItem(42583);
					pPlayer->AddItem(41629);
					pPlayer->AddItem(42029);
					pPlayer->AddItem(41639);
				}
				else if(pPlayer->getClass() == CLASS_SHAMAN)
				{
					pPlayer->AddItem(41075);
					pPlayer->AddItem(41018);
					pPlayer->AddItem(41043);
					pPlayer->AddItem(40991);
					pPlayer->AddItem(41006);
					pPlayer->AddItem(41032);
					pPlayer->AddItem(42126);
					pPlayer->AddItem(42124);
					pPlayer->AddItem(42132);
					pPlayer->AddItem(42564);
					pPlayer->AddItem(42602);
					pPlayer->AddItem(41069);
					pPlayer->AddItem(42029);
					pPlayer->AddItem(41064);
				}
				else
				{
					pPlayer->AddItem(41903);
					if(pPlayer->getClass() == CLASS_MAGE)
					{
						pPlayer->AddItem(41945);
						pPlayer->AddItem(41964);
						pPlayer->AddItem(41951);
						pPlayer->AddItem(41970);
						pPlayer->AddItem(41958);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42132);
						pPlayer->AddItem(44419);
						pPlayer->AddItem(42502);
						pPlayer->AddItem(41897);
						pPlayer->AddItem(42029);
						pPlayer->AddItem(41908);
					}
					else
					{
						pPlayer->AddItem(41992);
						pPlayer->AddItem(42010);
						pPlayer->AddItem(41997);
						pPlayer->AddItem(42016);
						pPlayer->AddItem(42004);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42132);
						pPlayer->AddItem(44419);
						pPlayer->AddItem(42502);
						pPlayer->AddItem(41897);
						pPlayer->AddItem(42029);
						pPlayer->AddItem(41908);
					}
				}
			}
			else // hunt
			{
				pPlayer->AddItem(42074);
				pPlayer->AddItem(41230);
				pPlayer->AddItem(41156);
				pPlayer->AddItem(41216);
				pPlayer->AddItem(41086);
				pPlayer->AddItem(41142);
				pPlayer->AddItem(41204);
				pPlayer->AddItem(42126);
				pPlayer->AddItem(42124);
				pPlayer->AddItem(42131);
				pPlayer->AddItem(42490);
				pPlayer->AddItem(41234);
				pPlayer->AddItem(42317);
				pPlayer->AddItem(42027);
				pPlayer->AddItem(41224);
			}
		}
		case GOSSIP_ACTION_INFO_DEF+11:
		{
			// PVP dps cac
			if(uiAction == GOSSIP_ACTION_INFO_DEF+11)
			{
				pPlayer->AddItem(42117);
				pPlayer->AddItem(49000);
				if(pPlayer->getClass() == CLASS_ROGUE || pPlayer->getClass() == CLASS_DRUID ||
					pPlayer->getClass() == CLASS_SHAMAN)
				{
					pPlayer->AddItem(42074);
					if(pPlayer->getClass() == CLASS_ROGUE || pPlayer->getClass() == CLASS_DRUID)
					{
						pPlayer->AddItem(41635);
						if(pPlayer->getClass() == CLASS_DRUID)
						{
							pPlayer->AddItem(41677);
							pPlayer->AddItem(41714);
							pPlayer->AddItem(41660);
							pPlayer->AddItem(41772);
							pPlayer->AddItem(41666);
							pPlayer->AddItem(42126);
							pPlayer->AddItem(42124);
							pPlayer->AddItem(42131);
							pPlayer->AddItem(42390);
							pPlayer->AddItem(42588);
							pPlayer->AddItem(41831);
							pPlayer->AddItem(42027);
							pPlayer->AddItem(41839);
						}
						else
						{
							pPlayer->AddItem(41671);
							pPlayer->AddItem(41682);
							pPlayer->AddItem(41649);
							pPlayer->AddItem(41766);
							pPlayer->AddItem(41654);
							pPlayer->AddItem(42126);
							pPlayer->AddItem(42124);
							pPlayer->AddItem(42131);
							pPlayer->AddItem(42450);
							pPlayer->AddItem(42242);
							pPlayer->AddItem(42248);
							pPlayer->AddItem(41831);
							pPlayer->AddItem(42027);
							pPlayer->AddItem(41839);
						}
					}
					else
					{
						pPlayer->AddItem(41230);
						pPlayer->AddItem(41150);
						pPlayer->AddItem(41210);
						pPlayer->AddItem(41080);
						pPlayer->AddItem(41136);
						pPlayer->AddItem(41198);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42131);
						pPlayer->AddItem(42607);
						pPlayer->AddItem(41234);
						pPlayer->AddItem(42027);
						pPlayer->AddItem(42208);
						pPlayer->AddItem(42227);
						pPlayer->AddItem(41224);
					}
				}
				else // warrior, pala, dk
				{
					pPlayer->AddItem(42074);
					pPlayer->AddItem(40882);
					if(pPlayer->getClass() == CLASS_DEATH_KNIGHT)
					{
						pPlayer->AddItem(40824);
						pPlayer->AddItem(40863);
						pPlayer->AddItem(40784);
						pPlayer->AddItem(40806);
						pPlayer->AddItem(40845);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42131);
						pPlayer->AddItem(42317);
						pPlayer->AddItem(42620);
						pPlayer->AddItem(40879);
						pPlayer->AddItem(42027);
						pPlayer->AddItem(40888);
					}
					else if(pPlayer->getClass() == CLASS_PALADIN)
					{
						pPlayer->AddItem(40825);
						pPlayer->AddItem(40864);
						pPlayer->AddItem(40785);
						pPlayer->AddItem(40805);
						pPlayer->AddItem(40846);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42131);
						pPlayer->AddItem(42317);
						pPlayer->AddItem(42852);
						pPlayer->AddItem(40879);
						pPlayer->AddItem(42027);
						pPlayer->AddItem(40888);
					}
					else
					{
						pPlayer->AddItem(40823);
						pPlayer->AddItem(40862);
						pPlayer->AddItem(40786);
						pPlayer->AddItem(40804);
						pPlayer->AddItem(40844);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42131);
						pPlayer->AddItem(42317);
						pPlayer->AddItem(42559);
						pPlayer->AddItem(42450);
						pPlayer->AddItem(40879);
						pPlayer->AddItem(42027);
						pPlayer->AddItem(42208);
						pPlayer->AddItem(40888);
					}
				}
			}
		}
		case GOSSIP_ACTION_INFO_DEF+12:
		{
			// PVP heal
			if(uiAction == GOSSIP_ACTION_INFO_DEF+12)
			{
				pPlayer->AddItem(48999);
				pPlayer->AddItem(42116);
				if(pPlayer->getClass() == CLASS_PRIEST || pPlayer->getClass() == CLASS_DRUID)
				{
					pPlayer->AddItem(42073);
					if(pPlayer->getClass() == CLASS_DRUID)
					{
						pPlayer->AddItem(41621);
						pPlayer->AddItem(41320);
						pPlayer->AddItem(41274);
						pPlayer->AddItem(41309);
						pPlayer->AddItem(41286);
						pPlayer->AddItem(41297);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42132);
						pPlayer->AddItem(44419);
						pPlayer->AddItem(42578);
						pPlayer->AddItem(41616);
						pPlayer->AddItem(42033);
						pPlayer->AddItem(41624);
					}
					else
					{
						pPlayer->AddItem(41885);
						pPlayer->AddItem(41853);
						pPlayer->AddItem(41868);
						pPlayer->AddItem(41858);
						pPlayer->AddItem(41873);
						pPlayer->AddItem(41863);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42132);
						pPlayer->AddItem(42513);
						pPlayer->AddItem(42531);
						pPlayer->AddItem(42352);
						pPlayer->AddItem(41880);
						pPlayer->AddItem(42033);
						pPlayer->AddItem(41892);
					}
				}
				else // paladin, shaman
				{
					pPlayer->AddItem(42069);
					if(pPlayer->getClass() == CLASS_PALADIN)
					{
						pPlayer->AddItem(40977);
						pPlayer->AddItem(40932);
						pPlayer->AddItem(40962);
						pPlayer->AddItem(40905);
						pPlayer->AddItem(40926);
						pPlayer->AddItem(40938);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42132);
						pPlayer->AddItem(42570);
						pPlayer->AddItem(42614);
						pPlayer->AddItem(42352);
						pPlayer->AddItem(40974);
						pPlayer->AddItem(42033);
						pPlayer->AddItem(40982);
					}
					else
					{
						pPlayer->AddItem(41075);
						pPlayer->AddItem(41012);
						pPlayer->AddItem(41037);
						pPlayer->AddItem(40990);
						pPlayer->AddItem(41000);
						pPlayer->AddItem(41026);
						pPlayer->AddItem(42126);
						pPlayer->AddItem(42124);
						pPlayer->AddItem(42132);
						pPlayer->AddItem(42570);
						pPlayer->AddItem(42597);
						pPlayer->AddItem(41048);
						pPlayer->AddItem(42033);
						pPlayer->AddItem(41059);
					}
				}
			}
			//...
			// end
			switch(pPlayer->getClass())
			{
				case CLASS_DRUID:
					pPlayer->learnSpell(9634, 0,false);
					pPlayer->learnSpell(768, 0,false);
					pPlayer->learnSpell(6795, 0,false);
					pPlayer->learnSpell(1066, 0,false);
					pPlayer->learnSpell(6807, 0,false);
					pPlayer->learnSpell(26997, 0,false);
					pPlayer->learnSpell(33357, 0,false);
					pPlayer->learnSpell(27008, 0,false);
					pPlayer->learnSpell(62600, 0,false);
					pPlayer->learnSpell(27004, 0,false);
					pPlayer->learnSpell(5229, 0,false);
					pPlayer->learnSpell(22570, 0,false);
					pPlayer->learnSpell(1066, 0,false);
					pPlayer->learnSpell(768, 0,false);
					pPlayer->learnSpell(783, 0,false);
					pPlayer->learnSpell(9846, 0,false);
					pPlayer->learnSpell(20719, 0,false);
					pPlayer->learnSpell(27000, 0,false);
					pPlayer->learnSpell(27003, 0,false);
					pPlayer->learnSpell(33745, 0,false);
					pPlayer->learnSpell(27002, 0,false);
					pPlayer->learnSpell(16857, 0,false);
					pPlayer->learnSpell(24248, 0,false);
					pPlayer->learnSpell(5225, 0,false);
					pPlayer->learnSpell(27005, 0,false);
					pPlayer->learnSpell(22842, 0,false);
					pPlayer->learnSpell(9913, 0,false);
					pPlayer->learnSpell(26998, 0,false);
					pPlayer->learnSpell(5209, 0,false);
					pPlayer->learnSpell(8983, 0,false);
					pPlayer->learnSpell(27006, 0,false);
					pPlayer->learnSpell(26995, 0,false);
					pPlayer->learnSpell(26985, 0,false);
					pPlayer->learnSpell(26988, 0,false);
					pPlayer->learnSpell(22812, 0,false);
					pPlayer->learnSpell(27009, 0,false);
					pPlayer->learnSpell(26992, 0,false);
					pPlayer->learnSpell(26986, 0,false);
					pPlayer->learnSpell(18658, 0,false);
					pPlayer->learnSpell(29166, 0,false);
					pPlayer->learnSpell(770, 0,false);
					pPlayer->learnSpell(27012, 0,false);
					pPlayer->learnSpell(26989, 0,false);
					pPlayer->learnSpell(2893, 0,false);
					pPlayer->learnSpell(2782, 0,false);
					pPlayer->learnSpell(26991, 0,false);
					pPlayer->learnSpell(33763, 0,false);
					pPlayer->learnSpell(26990, 0,false);
					pPlayer->learnSpell(26982, 0,false);
					pPlayer->learnSpell(26994, 0,false);
					pPlayer->learnSpell(50764, 0,false);
					pPlayer->learnSpell(26980, 0,false);
					pPlayer->learnSpell(26979, 0,false);
					pPlayer->learnSpell(26983, 0,false);
					pPlayer->learnSpell(9634, 0,false);
					pPlayer->learnSpell(33786, 0,false);
					break;
				case CLASS_HUNTER:
					pPlayer->learnSpell(6991, 0,false);
					pPlayer->learnSpell(982, 0,false);
					pPlayer->learnSpell(1579, 0,false);
					pPlayer->learnSpell(883, 0,false);
					pPlayer->learnSpell(2641, 0,false);
					pPlayer->learnSpell(8737, 0,false);
					break;
				case CLASS_PALADIN:
					pPlayer->learnSpell(7328, 0,false);
					pPlayer->learnSpell(750, 0,false);
					break;
				case CLASS_SHAMAN:
					pPlayer->learnSpell(5394, 0,false);
					pPlayer->learnSpell(8071, 0,false);
					pPlayer->learnSpell(3599, 0,false);
					pPlayer->learnSpell(8737, 0,false);
					break;
				case CLASS_WARLOCK:
					pPlayer->learnSpell(688, 0,false);
					pPlayer->learnSpell(697, 0,false);
					pPlayer->learnSpell(712, 0,false);
					pPlayer->learnSpell(691, 0,false);
					break;
				case CLASS_WARRIOR:
					pPlayer->learnSpell(71, 0,false);
					pPlayer->learnSpell(2458, 0,false);
					pPlayer->learnSpell(20252, 0,false);
					pPlayer->learnSpell(355, 0,false);
					pPlayer->learnSpell(750, 0,false);
					break;
				case CLASS_MAGE:
					pPlayer->learnSpell(33946, 0,false);
					pPlayer->learnSpell(27125, 0,false);
					pPlayer->learnSpell(33944, 0,false);
					pPlayer->learnSpell(27131, 0,false);
					pPlayer->learnSpell(130, 0,false);
					pPlayer->learnSpell(2139, 0,false);
					pPlayer->learnSpell(30451, 0,false);
					pPlayer->learnSpell(475, 0,false);
					pPlayer->learnSpell(27082, 0,false);
					pPlayer->learnSpell(27127, 0,false);
					pPlayer->learnSpell(27126, 0,false);
					pPlayer->learnSpell(66, 0,false);
					pPlayer->learnSpell(27090, 0,false);
					pPlayer->learnSpell(27101, 0,false);
					pPlayer->learnSpell(12826, 0,false);
					pPlayer->learnSpell(38704, 0,false);
					pPlayer->learnSpell(43987, 0,false);
					pPlayer->learnSpell(1953, 0,false);
					pPlayer->learnSpell(30449, 0,false);
					pPlayer->learnSpell(30482, 0,false);
					pPlayer->learnSpell(38692, 0,false);
					pPlayer->learnSpell(27074, 0,false);
					pPlayer->learnSpell(27086, 0,false);
					pPlayer->learnSpell(27128, 0,false);
					pPlayer->learnSpell(27079, 0,false);
					pPlayer->learnSpell(7301, 0,false);
					pPlayer->learnSpell(27124, 0,false);
					pPlayer->learnSpell(27085, 0,false);
					pPlayer->learnSpell(45438, 0,false);
					pPlayer->learnSpell(27087, 0,false);
					pPlayer->learnSpell(38697, 0,false);
					pPlayer->learnSpell(32796, 0,false);
					pPlayer->learnSpell(30455, 0,false);
					pPlayer->learnSpell(27088, 0,false);
					break;
				case CLASS_ROGUE:
					pPlayer->learnSpell(8643, 0,false);
					pPlayer->learnSpell(1833, 0,false);
					pPlayer->learnSpell(6774, 0,false);
					pPlayer->learnSpell(51722, 0,false);
					pPlayer->learnSpell(48689, 0,false);
					pPlayer->learnSpell(32684, 0,false);
					pPlayer->learnSpell(26865, 0,false);
					pPlayer->learnSpell(8647, 0,false);
					pPlayer->learnSpell(26884, 0,false);
					pPlayer->learnSpell(48673, 0,false);
					pPlayer->learnSpell(26867, 0,false);
					pPlayer->learnSpell(26862, 0,false);
					pPlayer->learnSpell(26863, 0,false);
					pPlayer->learnSpell(1766, 0,false);
					pPlayer->learnSpell(26669, 0,false);
					pPlayer->learnSpell(27448, 0,false);
					pPlayer->learnSpell(5938, 0,false);
					pPlayer->learnSpell(11305, 0,false);
					pPlayer->learnSpell(1776, 0,false);
					pPlayer->learnSpell(11297, 0,false);
					pPlayer->learnSpell(1787, 0,false);
					pPlayer->learnSpell(31224, 0,false);
					pPlayer->learnSpell(2094, 0,false);
					pPlayer->learnSpell(1860, 0,false);
					pPlayer->learnSpell(1842, 0,false);
					pPlayer->learnSpell(2836, 0,false);
					pPlayer->learnSpell(26889, 0,false);
					pPlayer->learnSpell(1725, 0,false);
					pPlayer->learnSpell(921, 0,false);
					pPlayer->learnSpell(674, 0,false);
					break;
				case CLASS_PRIEST:
					break;
				case CLASS_DEATH_KNIGHT:
					pPlayer->learnSpell(50977, 0,false);
					pPlayer->learnSpell(53428, 0,false);
					break;
				
				default:
					break;
			}
			switch(pPlayer->getRace())
			{
				case RACE_HUMAN:
				case RACE_DWARF:
				case RACE_NIGHTELF:
				case RACE_GNOME:
				case RACE_DRAENEI:
					pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(932),42000);
					pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(72),24000);
					pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(946),42000);
					pPlayer->learnSpell(32242,0,false);
					break;
				case RACE_ORC:
				case RACE_UNDEAD_PLAYER:
				case RACE_TAUREN:
				case RACE_TROLL:
				case RACE_BLOODELF:
					pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(934),42000);
					pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(947),42000);
					pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(76),24000);
					pPlayer->learnSpell(32295,0,false);
					break;
				default:
					break;
			}
			pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(942),24000);
			pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(989),24000);
			pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(935),24000);
			pPlayer->GetReputationMgr().SetReputation(GetFactionStore()->LookupEntry(1011),24000);
			
			pPlayer->UpdateSkillsToMaxSkillsForLevel();
			pPlayer->SaveToDB();
			pPlayer->TeleportTo(571,5801.94f,839.66f,681.1f,0.0f);
			//pPlayer->CLOSE_GOSSIP_MENU();
			break;
		}
		case GOSSIP_ACTION_INFO_DEF+8:
		{
			// desactive le click to play
			break;
		}

	}
    return true;
}

/*######
## npc_lunaclaw_spirit
######*/

enum
{
    QUEST_BODY_HEART_A      = 6001,
    QUEST_BODY_HEART_H      = 6002,

    TEXT_ID_DEFAULT         = 4714,
    TEXT_ID_PROGRESS        = 4715
};

#define GOSSIP_ITEM_GRANT   "You have thought well, spirit. I ask you to grant me the strength of your body and the strength of your heart."

bool GossipHello_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_BODY_HEART_A) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_BODY_HEART_H) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GRANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_DEFAULT, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_lunaclaw_spirit(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->SEND_GOSSIP_MENU(TEXT_ID_PROGRESS, pCreature->GetGUID());
        pPlayer->AreaExploredOrEventHappens((pPlayer->getRace() == ALLIANCE) ? QUEST_BODY_HEART_A : QUEST_BODY_HEART_H);
    }
    return true;
}

/*######
## npc_mount_vendor
######*/

bool GossipHello_npc_mount_vendor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    bool canBuy;
    canBuy = false;
    uint32 vendor = pCreature->GetEntry();
    uint8 race = pPlayer->getRace();

    switch (vendor)
    {
        case 384:                                           //Katie Hunter
        case 1460:                                          //Unger Statforth
        case 2357:                                          //Merideth Carlson
        case 4885:                                          //Gregor MacVince
            if (pPlayer->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                pPlayer->SEND_GOSSIP_MENU(5855, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 1261:                                          //Veron Amberstill
            if (pPlayer->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                pPlayer->SEND_GOSSIP_MENU(5856, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 3362:                                          //Ogunaro Wolfrunner
            if (pPlayer->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                pPlayer->SEND_GOSSIP_MENU(5841, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 3685:                                          //Harb Clawhoof
            if (pPlayer->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                pPlayer->SEND_GOSSIP_MENU(5843, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 4730:                                          //Lelanai
            if (pPlayer->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                pPlayer->SEND_GOSSIP_MENU(5844, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 4731:                                          //Zachariah Post
            if (pPlayer->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD_PLAYER)
                pPlayer->SEND_GOSSIP_MENU(5840, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 7952:                                          //Zjolnir
            if (pPlayer->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                pPlayer->SEND_GOSSIP_MENU(5842, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 7955:                                          //Milli Featherwhistle
            if (pPlayer->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                pPlayer->SEND_GOSSIP_MENU(5857, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 16264:                                         //Winaestra
            if (pPlayer->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
                pPlayer->SEND_GOSSIP_MENU(10305, pCreature->GetGUID());
            else canBuy = true;
            break;
        case 17584:                                         //Torallius the Pack Handler
            if (pPlayer->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                pPlayer->SEND_GOSSIP_MENU(10239, pCreature->GetGUID());
            else canBuy = true;
            break;
    }

    if (canBuy)
    {
        if (pCreature->isVendor())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_mount_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
## npc_rogue_trainer
######*/

bool GossipHello_npc_rogue_trainer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isTrainer())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

    if (pCreature->isCanTrainingAndResetTalentsOf(pPlayer))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "I wish to unlearn my talents", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_UNLEARNTALENTS);

    if (pPlayer->getClass() == CLASS_ROGUE && pPlayer->getLevel() >= 24 && !pPlayer->HasItemCount(17126,1) && !pPlayer->GetQuestRewardStatus(6681))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<Take the letter>", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(5996, pCreature->GetGUID());
    } else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_rogue_trainer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer,21100,false);
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetGUID());
            break;
        case GOSSIP_OPTION_UNLEARNTALENTS:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SendTalentWipeConfirm(pCreature->GetGUID());
            break;
    }
    return true;
}

/*######
## npc_sayge
######*/

#define SPELL_DMG       23768                               //dmg
#define SPELL_RES       23769                               //res
#define SPELL_ARM       23767                               //arm
#define SPELL_SPI       23738                               //spi
#define SPELL_INT       23766                               //int
#define SPELL_STM       23737                               //stm
#define SPELL_STR       23735                               //str
#define SPELL_AGI       23736                               //agi
#define SPELL_FORTUNE   23765                               //faire fortune

bool GossipHello_npc_sayge(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->HasSpellCooldown(SPELL_INT) ||
        pPlayer->HasSpellCooldown(SPELL_ARM) ||
        pPlayer->HasSpellCooldown(SPELL_DMG) ||
        pPlayer->HasSpellCooldown(SPELL_RES) ||
        pPlayer->HasSpellCooldown(SPELL_STR) ||
        pPlayer->HasSpellCooldown(SPELL_AGI) ||
        pPlayer->HasSpellCooldown(SPELL_STM) ||
        pPlayer->HasSpellCooldown(SPELL_SPI))
        pPlayer->SEND_GOSSIP_MENU(7393, pCreature->GetGUID());
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(7339, pCreature->GetGUID());
    }

    return true;
}

void SendAction_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Slay the Man",                      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn him over to liege",            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confiscate the corn",               GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let him go and have the corn",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(7340, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painfully",     GOSSIP_SENDER_MAIN+1, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painlessly",    GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let your friend go",                GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7341, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confront the diplomat",             GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show not so quiet defiance",        GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remain quiet",                      GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7361, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Speak against your brother openly", GOSSIP_SENDER_MAIN+6, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Help your brother in",              GOSSIP_SENDER_MAIN+7, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Keep your brother out without letting him know", GOSSIP_SENDER_MAIN+8, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7362, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, keep gold",            GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, share the gold",       GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let the knight take credit",        GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7363, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks",                            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(7364, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pCreature->CastSpell(pPlayer, SPELL_FORTUNE, false);
            pPlayer->SEND_GOSSIP_MENU(7365, pCreature->GetGUID());
            break;
    }
}

bool GossipSelect_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiSender)
    {
        case GOSSIP_SENDER_MAIN:
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+1:
            pCreature->CastSpell(pPlayer, SPELL_DMG, false);
            pPlayer->AddSpellCooldown(SPELL_DMG,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+2:
            pCreature->CastSpell(pPlayer, SPELL_RES, false);
            pPlayer->AddSpellCooldown(SPELL_RES,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+3:
            pCreature->CastSpell(pPlayer, SPELL_ARM, false);
            pPlayer->AddSpellCooldown(SPELL_ARM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+4:
            pCreature->CastSpell(pPlayer, SPELL_SPI, false);
            pPlayer->AddSpellCooldown(SPELL_SPI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+5:
            pCreature->CastSpell(pPlayer, SPELL_INT, false);
            pPlayer->AddSpellCooldown(SPELL_INT,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+6:
            pCreature->CastSpell(pPlayer, SPELL_STM, false);
            pPlayer->AddSpellCooldown(SPELL_STM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+7:
            pCreature->CastSpell(pPlayer, SPELL_STR, false);
            pPlayer->AddSpellCooldown(SPELL_STR,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+8:
            pCreature->CastSpell(pPlayer, SPELL_AGI, false);
            pPlayer->AddSpellCooldown(SPELL_AGI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
    }
    return true;
}

struct MANGOS_DLL_DECL npc_mirror_image : public LibDevFSAI
{
    npc_mirror_image(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitIA();
		AddEventOnTank(38697,1000,3000,200);
		AddEventMaxPrioOnTank(27079,500,8000,1000);
	}


	uint32 Depop_Timer;
    void Reset()
    {
		SetCombatMovement(false);
		Depop_Timer = 30000;
    }

	void UpdateAI(const uint32 diff)
	{
		if(!CanDoSomething())
			return;

		if(Depop_Timer <= diff)
		{
			me->ForcedDespawn(1000);
			Depop_Timer = DAY;
		}
		else
			Depop_Timer -= diff;
		
		UpdateEvent(diff);
	}

};

CreatureAI* GetAI_npc_mirror_image(Creature* pCreature)
{
    return new npc_mirror_image(pCreature);
}

bool GossipHello_npc_nerzoule(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());
	
	uint32 lastvote = 7200 - pPlayer->GetSession()->getVoteTime();
	std::string msg = "";
	std::string suffix = "seconde(s)";
	if(lastvote >= 7200)
		msg = "Vous n'avez pas fait un vote depuis longtemps... Vous pouvez voter !";
	else
	{
		if(lastvote >= 3600)
		{
			lastvote = lastvote % 3600;
			suffix = "heures(s)";
		}
		else if(lastvote >= 60)
		{
			lastvote = lastvote % 60;
			suffix = "minutes(s)";
		}
		std::stringstream cvote;
		cvote << lastvote;
		msg = "Vous pourrez voter dans ";
		msg += cvote.str() + " " + suffix;

	}
	// TODO : choses
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, msg, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(pCreature->GetEntry(), pCreature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_nerzoule(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiSender)
    {
        case GOSSIP_SENDER_MAIN:
            break;
    }
    return true;
}

struct MANGOS_DLL_DECL ghoul_army_of_the_deadAI : public LibDevFSAI
{	
	ghoul_army_of_the_deadAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitIA();
		AddEventOnTank(70266,urand(1000,3000),1000,500);
		AddEventOnTank(47468,2500,2500,1000);
		me->setPowerType(POWER_ENERGY);
    }

    void Reset()
	{
		ResetTimers();
	}
	    
    void UpdateAI(const uint32 diff)
	{	
		UpdateEvent(diff);
	
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_ghoul_army_of_the_dead(Creature* pCreature)
{
    return new ghoul_army_of_the_deadAI(pCreature);
} 

struct MANGOS_DLL_DECL gargoyle_DK_AI : public LibDevFSAI
{	
	gargoyle_DK_AI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitIA();
		AddEventOnTank(31664,500,1600);
    }

	int32 power;
    void Reset()
	{
		if(Unit* owner = me->GetCharmerOrOwner())
			if(owner->GetTypeId() == TYPEID_PLAYER)
				power = int32(((Player*)owner)->GetUInt32Value(UNIT_FIELD_ATTACK_POWER) * 0.4);
	}
	    
    void UpdateAI(const uint32 diff)
	{	
		UpdateEvent(diff);
	}
};

CreatureAI* GetAI_gargoyle_DK(Creature* pCreature)
{
    return new gargoyle_DK_AI(pCreature);
}

struct MANGOS_DLL_DECL venomous_snake_hunt_AI : public LibDevFSAI
{	
	venomous_snake_hunt_AI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitIA();
		AddEventOnTank(30981,urand(1000,3000),15000,1000);
		me->ForcedDespawn(15000);
    }

    void Reset()
	{
		if(Unit* owner = me->GetCharmerOrOwner())
			if(owner->GetTypeId() == TYPEID_PLAYER)
				me->SetLevel(((Player*)owner)->getLevel());
	}
	    
    void UpdateAI(const uint32 diff)
	{	
		UpdateEvent(diff);
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_venomous_snake_hunt(Creature* pCreature)
{
    return new venomous_snake_hunt_AI(pCreature);
}

struct MANGOS_DLL_DECL viper_hunt_AI : public LibDevFSAI
{	
	viper_hunt_AI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitIA();
		AddEventOnTank(57970,urand(1000,4000),5000,2000);
		me->ForcedDespawn(15000);
    }

    void Reset()
	{
		if(Unit* owner = me->GetCharmerOrOwner())
			if(owner->GetTypeId() == TYPEID_PLAYER)
				me->SetLevel(((Player*)owner)->getLevel());
	}
	    
    void UpdateAI(const uint32 diff)
	{	
		UpdateEvent(diff);
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_viper_hunt(Creature* pCreature)
{
    return new viper_hunt_AI(pCreature);
}

/*######
## npc_training_dummy
######*/
#define OUT_OF_COMBAT_TIME 5000

struct MANGOS_DLL_DECL npc_training_dummyAI : public Scripted_NoMovementAI
{
    npc_training_dummyAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint32 combat_timer;

    void Reset()
    {
        //m_creature->addUnitState(UNIT_STAT_STUNNED);
        combat_timer = 0;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        combat_timer = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
        return;
      
        if (CheckPercentLife(10.0f)) // allow players using finishers
			me->ModifyHealth(me->GetMaxHealth());

        me->SetTargetGUID(0); // prevent from rotating
        combat_timer += diff;

        if (combat_timer > OUT_OF_COMBAT_TIME)
			EnterEvadeMode();
    }
};

CreatureAI* GetAI_npc_training_dummy(Creature* pCreature)
{
	return new npc_training_dummyAI(pCreature);
}

void AddSC_npcs_special()
{
    Script *newscript;

	newscript = new Script;
    newscript->Name = "npc_training_dummy";
    newscript->GetAI = &GetAI_npc_training_dummy;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_air_force_bots";
    newscript->GetAI = &GetAI_npc_air_force_bots;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_chicken_cluck";
    newscript->GetAI = &GetAI_npc_chicken_cluck;
    newscript->pQuestAccept =   &QuestAccept_npc_chicken_cluck;
    newscript->pQuestComplete = &QuestComplete_npc_chicken_cluck;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dancing_flames";
    newscript->GetAI = &GetAI_npc_dancing_flames;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_injured_patient";
    newscript->GetAI = &GetAI_npc_injured_patient;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_doctor";
    newscript->GetAI = &GetAI_npc_doctor;
    newscript->pQuestAccept = &QuestAccept_npc_doctor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_garments_of_quests";
    newscript->GetAI = &GetAI_npc_garments_of_quests;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_guardian";
    newscript->GetAI = &GetAI_npc_guardian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kingdom_of_dalaran_quests";
    newscript->pGossipHello =  &GossipHello_npc_kingdom_of_dalaran_quests;
    newscript->pGossipSelect = &GossipSelect_npc_kingdom_of_dalaran_quests;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lunaclaw_spirit";
    newscript->pGossipHello =  &GossipHello_npc_lunaclaw_spirit;
    newscript->pGossipSelect = &GossipSelect_npc_lunaclaw_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mount_vendor";
    newscript->pGossipHello =  &GossipHello_npc_mount_vendor;
    newscript->pGossipSelect = &GossipSelect_npc_mount_vendor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rogue_trainer";
    newscript->pGossipHello =  &GossipHello_npc_rogue_trainer;
    newscript->pGossipSelect = &GossipSelect_npc_rogue_trainer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sayge";
    newscript->pGossipHello = &GossipHello_npc_sayge;
    newscript->pGossipSelect = &GossipSelect_npc_sayge;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_nerzoule";
    newscript->pGossipHello = &GossipHello_npc_nerzoule;
    newscript->pGossipSelect = &GossipSelect_npc_nerzoule;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_bd_click_to_play";
    newscript->pGossipHello = &GossipHello_bd_special_vendor;
    newscript->pGossipSelect = &GossipSelect_bd_special_vendor;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_mirror_image";
    newscript->GetAI = &GetAI_npc_mirror_image;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_ghoul_army_of_the_dead";
    newscript->GetAI = &GetAI_ghoul_army_of_the_dead;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "dk_gargoyle";
    newscript->GetAI = &GetAI_gargoyle_DK;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "venomous_snake_hunt";
    newscript->GetAI = &GetAI_venomous_snake_hunt;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "viper_hunt";
    newscript->GetAI = &GetAI_viper_hunt;
    newscript->RegisterSelf();
}