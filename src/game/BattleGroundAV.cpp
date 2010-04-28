/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
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

#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundAV.h"
#include "BattleGroundMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "Language.h"
#include "WorldPacket.h"
#include "ObjectAccessor.h"
#include "MapManager.h"
#include "ObjectMgr.h"
 
BattleGroundAV::BattleGroundAV()
{
	m_BgObjects.resize(BG_AV_OBJECT_MAX);

    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_AV_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_AV_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_AV_HAS_BEGUN;
}

BattleGroundAV::~BattleGroundAV()
{
}

void BattleGroundAV::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    BattleGround::HandleKillPlayer(player, killer);
    UpdateScore(BattleGroundTeamId(player->GetTeam()), -1);
	uint32 team = killer->GetTeam();
	if(player->GetGUID() != killer->GetGUID())
	{
		if(team == HORDE)
			RewardReputationToTeam(HORDE_REP,1,HORDE);
		else
			RewardReputationToTeam(ALLIANCE_REP,1,ALLIANCE);
	}

}

void BattleGroundAV::HandleKillUnit(Creature *creature, Player *killer)
{
    sLog.outDebug("BattleGroundAV: HandleKillUnit %i", creature->GetEntry());
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
    uint8 event1 = (sBattleGroundMgr.GetCreatureEventIndex(creature->GetDBTableGUIDLow())).event1;
    if (event1 == BG_EVENT_NONE)
        return;
    switch(event1)
    {
        /*case BG_AV_BOSS_A:
            CastSpellOnTeam(BG_AV_BOSS_KILL_QUEST_SPELL, HORDE);   // this is a spell which finishes a quest where a player has to kill the boss
            RewardReputationToTeam(BG_AV_FACTION_H, m_RepBoss, HORDE);
            RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_BOSS), HORDE);
            SendYellToAll(LANG_BG_AV_A_GENERAL_DEAD, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0));
            EndBattleGround(HORDE);
            break;
        case BG_AV_BOSS_H:
            CastSpellOnTeam(BG_AV_BOSS_KILL_QUEST_SPELL, ALLIANCE); // this is a spell which finishes a quest where a player has to kill the boss
            RewardReputationToTeam(BG_AV_FACTION_A, m_RepBoss, ALLIANCE);
            RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_BOSS), ALLIANCE);
            SendYellToAll(LANG_BG_AV_H_GENERAL_DEAD, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0));
            EndBattleGround(ALLIANCE);
            break;
        case BG_AV_CAPTAIN_A:
            if (IsActiveEvent(BG_AV_NodeEventCaptainDead_A, 0))
                return;
            RewardReputationToTeam(BG_AV_FACTION_H, m_RepCaptain, HORDE);
            RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_CAPTAIN), HORDE);
            UpdateScore(BG_TEAM_ALLIANCE, (-1) * BG_AV_RES_CAPTAIN);
            // spawn destroyed aura
            SpawnEvent(BG_AV_NodeEventCaptainDead_A, 0, true);
            break;
        case BG_AV_CAPTAIN_H:
            if (IsActiveEvent(BG_AV_NodeEventCaptainDead_H, 0))
                return;
            RewardReputationToTeam(BG_AV_FACTION_A, m_RepCaptain, ALLIANCE);
            RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_CAPTAIN), ALLIANCE);
            UpdateScore(BG_TEAM_HORDE, (-1) * BG_AV_RES_CAPTAIN);
            // spawn destroyed aura
            SpawnEvent(BG_AV_NodeEventCaptainDead_H, 0, true);
            break;
        case BG_AV_MINE_BOSSES_NORTH:
            ChangeMineOwner(BG_AV_NORTH_MINE, GetTeamIndexByTeamId(killer->GetTeam()));
            break;
        case BG_AV_MINE_BOSSES_SOUTH:
            ChangeMineOwner(BG_AV_SOUTH_MINE, GetTeamIndexByTeamId(killer->GetTeam()));
            break;*/
    }
}

void BattleGroundAV::HandleQuestComplete(uint32 questid, Player *player)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
    uint8 team = GetTeamIndexByTeamId(player->GetTeam());
    uint32 reputation = 0;                                  // reputation for the whole team (other reputation must be done in db)
    // TODO add events (including quest not available anymore, next quest availabe, go/npc de/spawning)
    sLog.outError("BattleGroundAV: Quest %i completed", questid);
    switch(questid)
    {
        case BG_AV_QUEST_A_SCRAPS1:
        case BG_AV_QUEST_A_SCRAPS2:
        case BG_AV_QUEST_H_SCRAPS1:
        case BG_AV_QUEST_H_SCRAPS2:
            m_Team_QuestStatus[team][0] += 20;
            reputation = 1;
            if( m_Team_QuestStatus[team][0] == 500 || m_Team_QuestStatus[team][0] == 1000 || m_Team_QuestStatus[team][0] == 1500 ) //25,50,75 turn ins
            {
                uint32 upgrade_mobs = 0;
                switch(m_Team_QuestStatus[team][0])
                {
                case 500:
                    upgrade_mobs = 1;
                    break;
                case 1000:
                    upgrade_mobs = 2;
                    break;
                case 1500:
                    upgrade_mobs = 3;
                    break;
                default:
                    break;
                }

                for (uint32 i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_FROSTWOLF_HUT; i++)
                {
                    if (m_Nodes[i].Owner == player->GetTeam() && m_Nodes[i].State == POINT_CONTROLLED)
                    {
                        BGCreatures::const_iterator itr = m_GraveCreatures[i][team][upgrade_mobs-1].begin();
                        for(; itr != m_GraveCreatures[i][team][upgrade_mobs-1].end(); ++itr)
                            SpawnBGCreature(*itr,RESPAWN_ONE_DAY);
                        itr = m_GraveCreatures[i][team][upgrade_mobs].begin();
                        for(; itr != m_GraveCreatures[i][team][upgrade_mobs].end(); ++itr)
                            SpawnBGCreature(*itr,RESPAWN_IMMEDIATELY);
                     }
                }
            }
            break;
        case BG_AV_QUEST_A_COMMANDER1:
        case BG_AV_QUEST_H_COMMANDER1:
            m_Team_QuestStatus[team][1]++;
            reputation = 1;
            if (m_Team_QuestStatus[team][1] == 120)
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            break;
        case BG_AV_QUEST_A_COMMANDER2:
        case BG_AV_QUEST_H_COMMANDER2:
            m_Team_QuestStatus[team][2]++;
            reputation = 2;
            if (m_Team_QuestStatus[team][2] == 60)
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            break;
        case BG_AV_QUEST_A_COMMANDER3:
        case BG_AV_QUEST_H_COMMANDER3:
            m_Team_QuestStatus[team][3]++;
            reputation = 5;
            RewardReputationToTeam(team, 1, player->GetTeam());
            if (m_Team_QuestStatus[team][1] == 30)
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            break;
        case BG_AV_QUEST_A_BOSS1:
        case BG_AV_QUEST_H_BOSS1:
            m_Team_QuestStatus[team][4] += 4;               // there are 2 quests where you can turn in 5 or 1 item.. ( + 4 cause +1 will be done some lines below)
            reputation = 4;
        case BG_AV_QUEST_A_BOSS2:
        case BG_AV_QUEST_H_BOSS2:
            m_Team_QuestStatus[team][4]++;
            reputation += 1;
            if (m_Team_QuestStatus[team][4] >= 200)
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            break;
        case BG_AV_QUEST_A_NEAR_MINE:
        case BG_AV_QUEST_H_NEAR_MINE:
            m_Team_QuestStatus[team][5]++;
            reputation = 2;
            if (m_Team_QuestStatus[team][5] == 28)
            {
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
                if (m_Team_QuestStatus[team][6] == 7)
                    sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here - ground assault ready", questid);
            }
            break;
        case BG_AV_QUEST_A_OTHER_MINE:
        case BG_AV_QUEST_H_OTHER_MINE:
            m_Team_QuestStatus[team][6]++;
            reputation = 3;
            if (m_Team_QuestStatus[team][6] == 7)
            {
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
                if (m_Team_QuestStatus[team][5] == 20)
                    sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here - ground assault ready", questid);
            }
            break;
        case BG_AV_QUEST_A_RIDER_HIDE:
        case BG_AV_QUEST_H_RIDER_HIDE:
            m_Team_QuestStatus[team][7]++;
            reputation = 1;
            if (m_Team_QuestStatus[team][7] == 25)
            {
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
                if (m_Team_QuestStatus[team][8] == 25)
                    sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here - rider assault ready", questid);
            }
            break;
        case BG_AV_QUEST_A_RIDER_TAME:
        case BG_AV_QUEST_H_RIDER_TAME:
            m_Team_QuestStatus[team][8]++;
            reputation = 1;
            if (m_Team_QuestStatus[team][8] == 25)
            {
                sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
                if (m_Team_QuestStatus[team][7] == 25)
                    sLog.outDebug("BattleGroundAV: Quest %i completed (need to implement some events here - rider assault ready", questid);
            }
            break;
        default:
            sLog.outDebug("BattleGroundAV: Quest %i completed but is not interesting for us", questid);
            return;
            break;
    }
    /*if (reputation)
        RewardReputationToTeam((player->GetTeam() == ALLIANCE) ? BG_ALLIANCE : BG_HORDE, reputation, player->GetTeam());*/
}

void BattleGroundAV::UpdateScore(BattleGroundTeamId team, int32 points )
{
    // note: to remove reinforcements points must be negative, for adding reinforcements points must be positive
    ASSERT( team == BG_TEAM_ALLIANCE || team == BG_TEAM_HORDE);
    m_TeamScores[team] += points;                      // m_TeamScores is int32 - so no problems here

    if (points < 0)
    {
        if (m_TeamScores[team] < 1)
        {
            m_TeamScores[team] = 0;
            // other team will win:
            EndBattleGround((team == BG_TEAM_ALLIANCE)? HORDE : ALLIANCE);
        }
        else if (!m_IsInformedNearLose[team] && m_TeamScores[team] < BG_AV_SCORE_NEAR_LOSE)
        {
            SendMessageToAll((team == BG_TEAM_HORDE) ? LANG_BG_AV_H_NEAR_LOSE : LANG_BG_AV_A_NEAR_LOSE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
            PlaySoundToAll(BG_AV_SOUND_NEAR_LOSE);
            m_IsInformedNearLose[team] = true;
        }
    }
    // must be called here, else it could display a negative value
    UpdateWorldState(((team == BG_TEAM_HORDE) ? BG_AV_Horde_Score : BG_AV_Alliance_Score), m_TeamScores[team]);
}

void BattleGroundAV::Update(uint32 diff)
{
    BattleGround::Update(diff);
	if (GetStatus() == STATUS_IN_PROGRESS)
    {
		    // add points from mine owning, and look if the neutral team can reclaim the mine
		m_Mine_Timer -=diff;
		for(uint8 mine = 0; mine <2; mine++)
		{
			if (m_Mine_Owner[mine] == ALLIANCE || m_Mine_Owner[mine] == HORDE)
			{
				if (m_Mine_Timer <= 0)
					UpdateScore(BattleGroundTeamId(m_Mine_Owner[mine]), 1);

				if (m_Mine_Reclaim_Timer[mine] > diff)
					m_Mine_Reclaim_Timer[mine] -= diff;
				else{
					ChangeMineOwner(mine, BG_AV_NEUTRAL_TEAM);
				}
			}
		}
		if (m_Mine_Timer <= 0)
			m_Mine_Timer = BG_AV_MINE_TICK_TIMER;                  // this is at the end, cause we need to update both mines

		team_captain_buff_Timer -= diff;
        if(team_captain_buff_Timer <= 0)
        {
            if(m_captainAlive[BG_TEAM_ALLIANCE] == true)
				CastSpellOnTeam(23693,ALLIANCE);

			if(m_captainAlive[BG_TEAM_HORDE] == true)
				CastSpellOnTeam(22751,HORDE);

            team_captain_buff_Timer = BG_AV_CAPTAIN_BUFF_TICK_TIMER;
        }

		for(int node = 0; node < BG_AV_NODES_MAX; ++node)
		{
			if(m_Nodes[node].Timer != 0)
			{
				if (m_Nodes[node].Timer > diff)
                    m_Nodes[node].Timer -= diff;
				else
				{
					m_Nodes[node].Timer = 0;
					uint8 teamindex = GetTeamIndexByTeamId(m_Nodes[node].Owner);
					DepopulateNode((BG_AV_Nodes)node);
					SpawnNodeEnd((BG_AV_Nodes)node);
					if(node == BG_AV_NODES_SNOWFALL_GRAVE)
						BG_CentralGrave_FirstAttack = false;
				}

			}
		}
	}
}

void BattleGroundAV::StartingEventCloseDoors()
{
	DoorClose(m_BgObjects[BG_AV_OBJECT_GATE_A]);
	DoorClose(m_BgObjects[BG_AV_OBJECT_GATE_H]);
	SpawnBGObject(m_BgObjects[BG_AV_OBJECT_GATE_A], RESPAWN_IMMEDIATELY);
    SpawnBGObject(m_BgObjects[BG_AV_OBJECT_GATE_H], RESPAWN_IMMEDIATELY);
}

void BattleGroundAV::OnCreatureRespawn(Creature* creature)
{
    uint32 level = creature->getLevel();
    if (level != 0)
        level += GetMaxLevel() - 60;                        // maybe we can do this more generic for custom level - range.. actually it's ok
    creature->SetLevel(level);
}

void BattleGroundAV::StartingEventOpenDoors()
{
	// Statut du cimetière des neiges :
	BG_CentralGrave_FirstAttack = true;

	// Ouverture des portes
	DoorOpen(BG_AV_OBJECT_GATE_A);
	DoorOpen(BG_AV_OBJECT_GATE_H);

	// On spawn les points capturables de départ
	SpawnBGObject(BG_AV_OBJECT_STORM_A_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_STORM_A_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_DUNB_A_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_DUNB_A_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_GIT_A_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_GIT_A_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_SWF_NEU_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_SWF_NEU_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_GSG_H_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_GSG_H_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FSW_H_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FSW_H_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FSH_H_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FSH_H_B, RESPAWN_IMMEDIATELY);

	/* Tours de départ */
	// Alliance
	SpawnBGObject(BG_AV_OBJECT_DBN_A_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_DBN_A_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_DBN_A_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_DBS_A_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_DBS_A_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_DBS_A_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FGP_A_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FGP_A_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FGP_A_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_ADG_A_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_ADG_A_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_ADG_A_AURA, RESPAWN_IMMEDIATELY);
	
	// Horde
	SpawnBGObject(BG_AV_OBJECT_GST_H_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_GST_H_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_GST_H_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_HAL_H_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_HAL_H_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_HAL_H_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FWO_H_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FWO_H_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FWO_H_AURA, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FWE_H_B, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FWE_H_T, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_AV_OBJECT_FWE_H_AURA, RESPAWN_IMMEDIATELY);

	// On mets les score au montant initial
	m_TeamScores[BG_TEAM_ALLIANCE] = BG_AV_SCORE_INITIAL_POINTS;
	m_TeamScores[BG_TEAM_HORDE] = BG_AV_SCORE_INITIAL_POINTS;
	// On met à jour tout l'affichage
	UpdateWorldState(BG_AV_SHOW_H_SCORE, 1);
    UpdateWorldState(BG_AV_SHOW_A_SCORE, 1);
	UpdateWorldState(BG_AV_Horde_Score,600);
	UpdateWorldState(BG_AV_Alliance_Score,600);

	
}

void BattleGroundAV::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundAVScore* sc = new BattleGroundAVScore;
 
    m_PlayerScores[plr->GetGUID()] = sc;
}

void BattleGroundAV::RemovePlayer(Player* /*plr*/,uint64 /*guid*/)
{
 
}

void BattleGroundAV::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
 
    uint32 SpellId = 0;
    switch(Trigger)
    {
        case 95:
        case 2606:
			if (Source->GetTeam() != HORDE)
                Source->GetSession()->SendAreaTriggerMessage("Seule la Horde peut utiliser ce portail");
            else
                Source->LeaveBattleground();
            break;
        case 2608:
			if (Source->GetTeam() != ALLIANCE)
                Source->GetSession()->SendAreaTriggerMessage("Seule l'Alliance peut utiliser ce portail");
            else
                Source->LeaveBattleground();
            break;
        case 3326:
        case 3327:
        case 3328:
        case 3329:
        case 3330:
        case 3331:
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
 
    if (SpellId)
        Source->CastSpell(Source, SpellId, true);
}

void BattleGroundAV::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{

    BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetGUID());
    if(itr == m_PlayerScores.end())                         // player not found...
        return;

    switch(type)
    {
        case SCORE_GRAVEYARDS_ASSAULTED:
            ((BattleGroundAVScore*)itr->second)->GraveyardsAssaulted += value;
            break;
        case SCORE_GRAVEYARDS_DEFENDED:
            ((BattleGroundAVScore*)itr->second)->GraveyardsDefended += value;
            break;
        case SCORE_TOWERS_ASSAULTED:
            ((BattleGroundAVScore*)itr->second)->TowersAssaulted += value;
            break;
        case SCORE_TOWERS_DEFENDED:
            ((BattleGroundAVScore*)itr->second)->TowersDefended += value;
            break;
        case SCORE_MINES_CAPTURED:
            ((BattleGroundAVScore*)itr->second)->MinesCaptured += value;
            break;
        case SCORE_LEADERS_KILLED:
            ((BattleGroundAVScore*)itr->second)->LeadersKilled += value;
            break;
        case SCORE_SECONDARY_OBJECTIVES:
            ((BattleGroundAVScore*)itr->second)->SecondaryObjectives += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(Source,type,value);
            break;
    }
}


bool BattleGroundAV::SetupBattleGround()
{
	// Portes du BG
	if (!AddObject(BG_AV_OBJECT_GATE_A,BG_AV_OBJECTID_GATE,BG_AV_DoorPositions[0][0],BG_AV_DoorPositions[0][1],BG_AV_DoorPositions[0][2],BG_AV_DoorPositions[0][3],BG_AV_DoorPositions[0][4],BG_AV_DoorPositions[0][5],BG_AV_DoorPositions[0][6],BG_AV_DoorPositions[0][7],RESPAWN_IMMEDIATELY)
        || !AddObject(BG_AV_OBJECT_GATE_H,BG_AV_OBJECTID_GATE,BG_AV_DoorPositions[1][0],BG_AV_DoorPositions[1][1],BG_AV_DoorPositions[1][2],BG_AV_DoorPositions[1][3],BG_AV_DoorPositions[1][4],BG_AV_DoorPositions[1][5],BG_AV_DoorPositions[1][6],BG_AV_DoorPositions[1][7],RESPAWN_IMMEDIATELY)
        )
    {
        sLog.outErrorDb("BattleGroundAV: Failed to spawn door object BattleGround not created!");
        return false;
    }
	/* Vérifié */
	//Auras
	if (!AddObject(BG_AV_OBJECT_DUNB_NEU_AURA,NEUTRAL_AURA_BANNER,BG_AV_Capturable_Points[0][0],BG_AV_Capturable_Points[0][1],BG_AV_Capturable_Points[0][2],BG_AV_Capturable_Points[0][3],BG_AV_Capturable_Points[0][4],BG_AV_Capturable_Points[0][5],BG_AV_Capturable_Points[0][6],BG_AV_Capturable_Points[0][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DUNB_H_AURA,HORDE_AURA_BANNER,BG_AV_Capturable_Points[0][0],BG_AV_Capturable_Points[0][1],BG_AV_Capturable_Points[0][2],BG_AV_Capturable_Points[0][3],BG_AV_Capturable_Points[0][4],BG_AV_Capturable_Points[0][5],BG_AV_Capturable_Points[0][6],BG_AV_Capturable_Points[0][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DUNB_A_AURA,ALLI_AURA_BANNER,BG_AV_Capturable_Points[0][0],BG_AV_Capturable_Points[0][1],BG_AV_Capturable_Points[0][2],BG_AV_Capturable_Points[0][3],BG_AV_Capturable_Points[0][4],BG_AV_Capturable_Points[0][5],BG_AV_Capturable_Points[0][6],BG_AV_Capturable_Points[0][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_STORM_NEU_AURA,NEUTRAL_AURA_BANNER,BG_AV_Capturable_Points[1][0],BG_AV_Capturable_Points[1][1],BG_AV_Capturable_Points[1][2],BG_AV_Capturable_Points[1][3],BG_AV_Capturable_Points[1][4],BG_AV_Capturable_Points[1][5],BG_AV_Capturable_Points[1][6],BG_AV_Capturable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_STORM_NEU_AURA,HORDE_AURA_BANNER,BG_AV_Capturable_Points[1][0],BG_AV_Capturable_Points[1][1],BG_AV_Capturable_Points[1][2],BG_AV_Capturable_Points[1][3],BG_AV_Capturable_Points[1][4],BG_AV_Capturable_Points[1][5],BG_AV_Capturable_Points[1][6],BG_AV_Capturable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_STORM_NEU_AURA,ALLI_AURA_BANNER,BG_AV_Capturable_Points[1][0],BG_AV_Capturable_Points[1][1],BG_AV_Capturable_Points[1][2],BG_AV_Capturable_Points[1][3],BG_AV_Capturable_Points[1][4],BG_AV_Capturable_Points[1][5],BG_AV_Capturable_Points[1][6],BG_AV_Capturable_Points[1][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_GIT_NEU_AURA,NEUTRAL_AURA_BANNER,BG_AV_Capturable_Points[2][0],BG_AV_Capturable_Points[2][1],BG_AV_Capturable_Points[2][2],BG_AV_Capturable_Points[2][3],BG_AV_Capturable_Points[2][4],BG_AV_Capturable_Points[2][5],BG_AV_Capturable_Points[2][6],BG_AV_Capturable_Points[2][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GIT_H_AURA,HORDE_AURA_BANNER,BG_AV_Capturable_Points[2][0],BG_AV_Capturable_Points[2][1],BG_AV_Capturable_Points[2][2],BG_AV_Capturable_Points[2][3],BG_AV_Capturable_Points[2][4],BG_AV_Capturable_Points[2][5],BG_AV_Capturable_Points[2][6],BG_AV_Capturable_Points[2][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GIT_A_AURA,ALLI_AURA_BANNER,BG_AV_Capturable_Points[2][0],BG_AV_Capturable_Points[2][1],BG_AV_Capturable_Points[2][2],BG_AV_Capturable_Points[2][3],BG_AV_Capturable_Points[2][4],BG_AV_Capturable_Points[2][5],BG_AV_Capturable_Points[2][6],BG_AV_Capturable_Points[2][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_SWF_NEU_AURA,NEUTRAL_AURA_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_H_AURA,HORDE_AURA_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_AURA,ALLI_AURA_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_GSG_NEU_AURA,NEUTRAL_AURA_BANNER,BG_AV_Capturable_Points[8][0],BG_AV_Capturable_Points[8][1],BG_AV_Capturable_Points[8][2],BG_AV_Capturable_Points[8][3],BG_AV_Capturable_Points[8][4],BG_AV_Capturable_Points[8][5],BG_AV_Capturable_Points[8][6],BG_AV_Capturable_Points[8][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GSG_H_AURA,HORDE_AURA_BANNER,BG_AV_Capturable_Points[8][0],BG_AV_Capturable_Points[8][1],BG_AV_Capturable_Points[8][2],BG_AV_Capturable_Points[8][3],BG_AV_Capturable_Points[8][4],BG_AV_Capturable_Points[8][5],BG_AV_Capturable_Points[8][6],BG_AV_Capturable_Points[8][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_AURA,ALLI_AURA_BANNER,BG_AV_Capturable_Points[8][0],BG_AV_Capturable_Points[8][1],BG_AV_Capturable_Points[8][2],BG_AV_Capturable_Points[8][3],BG_AV_Capturable_Points[8][4],BG_AV_Capturable_Points[8][5],BG_AV_Capturable_Points[8][6],BG_AV_Capturable_Points[8][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_FSW_NEU_AURA,NEUTRAL_AURA_BANNER,BG_AV_Capturable_Points[9][0],BG_AV_Capturable_Points[9][1],BG_AV_Capturable_Points[9][2],BG_AV_Capturable_Points[9][3],BG_AV_Capturable_Points[9][4],BG_AV_Capturable_Points[9][5],BG_AV_Capturable_Points[9][6],BG_AV_Capturable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSW_H_AURA,HORDE_AURA_BANNER,BG_AV_Capturable_Points[9][0],BG_AV_Capturable_Points[9][1],BG_AV_Capturable_Points[9][2],BG_AV_Capturable_Points[9][3],BG_AV_Capturable_Points[9][4],BG_AV_Capturable_Points[9][5],BG_AV_Capturable_Points[9][6],BG_AV_Capturable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSW_A_AURA,ALLI_AURA_BANNER,BG_AV_Capturable_Points[9][0],BG_AV_Capturable_Points[9][1],BG_AV_Capturable_Points[9][2],BG_AV_Capturable_Points[9][3],BG_AV_Capturable_Points[9][4],BG_AV_Capturable_Points[9][5],BG_AV_Capturable_Points[9][6],BG_AV_Capturable_Points[9][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_FSH_NEU_AURA,NEUTRAL_AURA_BANNER,BG_AV_Capturable_Points[10][0],BG_AV_Capturable_Points[10][1],BG_AV_Capturable_Points[10][2],BG_AV_Capturable_Points[10][3],BG_AV_Capturable_Points[10][4],BG_AV_Capturable_Points[10][5],BG_AV_Capturable_Points[10][6],BG_AV_Capturable_Points[10][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSH_H_AURA,HORDE_AURA_BANNER,BG_AV_Capturable_Points[10][0],BG_AV_Capturable_Points[10][1],BG_AV_Capturable_Points[10][2],BG_AV_Capturable_Points[10][3],BG_AV_Capturable_Points[10][4],BG_AV_Capturable_Points[10][5],BG_AV_Capturable_Points[10][6],BG_AV_Capturable_Points[10][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSH_A_AURA,ALLI_AURA_BANNER,BG_AV_Capturable_Points[10][0],BG_AV_Capturable_Points[10][1],BG_AV_Capturable_Points[10][2],BG_AV_Capturable_Points[10][3],BG_AV_Capturable_Points[10][4],BG_AV_Capturable_Points[10][5],BG_AV_Capturable_Points[10][6],BG_AV_Capturable_Points[10][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_DBN_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[1][0],BG_AV_Destroyable_Points[1][1],BG_AV_Destroyable_Points[1][2],BG_AV_Destroyable_Points[1][3],BG_AV_Destroyable_Points[1][4],BG_AV_Destroyable_Points[1][5],BG_AV_Destroyable_Points[1][6],BG_AV_Destroyable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBS_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[3][0],BG_AV_Destroyable_Points[3][1],BG_AV_Destroyable_Points[3][2],BG_AV_Destroyable_Points[3][3],BG_AV_Destroyable_Points[3][4],BG_AV_Destroyable_Points[3][5],BG_AV_Destroyable_Points[3][6],BG_AV_Destroyable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_ADG_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[5][0],BG_AV_Destroyable_Points[5][1],BG_AV_Destroyable_Points[5][2],BG_AV_Destroyable_Points[5][3],BG_AV_Destroyable_Points[5][4],BG_AV_Destroyable_Points[5][5],BG_AV_Destroyable_Points[5][6],BG_AV_Destroyable_Points[5][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FGP_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[7][0],BG_AV_Destroyable_Points[7][1],BG_AV_Destroyable_Points[7][2],BG_AV_Destroyable_Points[7][3],BG_AV_Destroyable_Points[7][4],BG_AV_Destroyable_Points[7][5],BG_AV_Destroyable_Points[7][6],BG_AV_Destroyable_Points[7][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBN_A_AURA,ALLI_AURA_BANNER,BG_AV_Destroyable_Points[1][0],BG_AV_Destroyable_Points[1][1],BG_AV_Destroyable_Points[1][2],BG_AV_Destroyable_Points[1][3],BG_AV_Destroyable_Points[1][4],BG_AV_Destroyable_Points[1][5],BG_AV_Destroyable_Points[1][6],BG_AV_Destroyable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBS_A_AURA,ALLI_AURA_BANNER,BG_AV_Destroyable_Points[3][0],BG_AV_Destroyable_Points[3][1],BG_AV_Destroyable_Points[3][2],BG_AV_Destroyable_Points[3][3],BG_AV_Destroyable_Points[3][4],BG_AV_Destroyable_Points[3][5],BG_AV_Destroyable_Points[3][6],BG_AV_Destroyable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_ADG_A_AURA,ALLI_AURA_BANNER,BG_AV_Destroyable_Points[5][0],BG_AV_Destroyable_Points[5][1],BG_AV_Destroyable_Points[5][2],BG_AV_Destroyable_Points[5][3],BG_AV_Destroyable_Points[5][4],BG_AV_Destroyable_Points[5][5],BG_AV_Destroyable_Points[5][6],BG_AV_Destroyable_Points[5][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FGP_A_AURA,ALLI_AURA_BANNER,BG_AV_Destroyable_Points[7][0],BG_AV_Destroyable_Points[7][1],BG_AV_Destroyable_Points[7][2],BG_AV_Destroyable_Points[7][3],BG_AV_Destroyable_Points[7][4],BG_AV_Destroyable_Points[7][5],BG_AV_Destroyable_Points[7][6],BG_AV_Destroyable_Points[7][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_GST_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[9][0],BG_AV_Destroyable_Points[9][1],BG_AV_Destroyable_Points[9][2],BG_AV_Destroyable_Points[9][3],BG_AV_Destroyable_Points[9][4],BG_AV_Destroyable_Points[9][5],BG_AV_Destroyable_Points[9][6],BG_AV_Destroyable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GST_H_AURA,HORDE_AURA_BANNER,BG_AV_Destroyable_Points[9][0],BG_AV_Destroyable_Points[9][1],BG_AV_Destroyable_Points[9][2],BG_AV_Destroyable_Points[9][3],BG_AV_Destroyable_Points[9][4],BG_AV_Destroyable_Points[9][5],BG_AV_Destroyable_Points[9][6],BG_AV_Destroyable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_HAL_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[11][0],BG_AV_Destroyable_Points[11][1],BG_AV_Destroyable_Points[11][2],BG_AV_Destroyable_Points[11][3],BG_AV_Destroyable_Points[11][4],BG_AV_Destroyable_Points[11][5],BG_AV_Destroyable_Points[11][6],BG_AV_Destroyable_Points[11][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_HAL_H_AURA,HORDE_AURA_BANNER,BG_AV_Destroyable_Points[11][0],BG_AV_Destroyable_Points[11][1],BG_AV_Destroyable_Points[11][2],BG_AV_Destroyable_Points[11][3],BG_AV_Destroyable_Points[11][4],BG_AV_Destroyable_Points[11][5],BG_AV_Destroyable_Points[11][6],BG_AV_Destroyable_Points[11][7],RESPAWN_IMMEDIATELY)
		
		|| !AddObject(BG_AV_OBJECT_FWO_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[13][0],BG_AV_Destroyable_Points[13][1],BG_AV_Destroyable_Points[13][2],BG_AV_Destroyable_Points[13][3],BG_AV_Destroyable_Points[13][4],BG_AV_Destroyable_Points[13][5],BG_AV_Destroyable_Points[13][6],BG_AV_Destroyable_Points[13][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWO_H_AURA,HORDE_AURA_BANNER,BG_AV_Destroyable_Points[13][0],BG_AV_Destroyable_Points[13][1],BG_AV_Destroyable_Points[13][2],BG_AV_Destroyable_Points[13][3],BG_AV_Destroyable_Points[13][4],BG_AV_Destroyable_Points[13][5],BG_AV_Destroyable_Points[13][6],BG_AV_Destroyable_Points[13][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWE_N_AURA,NEUTRAL_AURA_BANNER,BG_AV_Destroyable_Points[15][0],BG_AV_Destroyable_Points[15][1],BG_AV_Destroyable_Points[15][2],BG_AV_Destroyable_Points[13][3],BG_AV_Destroyable_Points[15][4],BG_AV_Destroyable_Points[15][5],BG_AV_Destroyable_Points[15][6],BG_AV_Destroyable_Points[15][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWE_H_AURA,HORDE_AURA_BANNER,BG_AV_Destroyable_Points[15][0],BG_AV_Destroyable_Points[15][1],BG_AV_Destroyable_Points[15][2],BG_AV_Destroyable_Points[15][3],BG_AV_Destroyable_Points[15][4],BG_AV_Destroyable_Points[15][5],BG_AV_Destroyable_Points[15][6],BG_AV_Destroyable_Points[15][7],RESPAWN_IMMEDIATELY)

		)
	{
		sLog.outErrorDb("BattleGroundAV: Failed to spawn Stormpike auras!");
		return false;
	}
	//Bannières
		/* Cimetières */
		// Stormpike
	if (!AddObject(BG_AV_OBJECT_STORM_H_B,HORDE_BANNER,BG_AV_Capturable_Points[1][0],BG_AV_Capturable_Points[1][1],BG_AV_Capturable_Points[1][2],BG_AV_Capturable_Points[1][3],BG_AV_Capturable_Points[1][4],BG_AV_Capturable_Points[1][5],BG_AV_Capturable_Points[1][6],BG_AV_Capturable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_STORM_A_B,ALLIANCE_BANNER,BG_AV_Capturable_Points[1][0],BG_AV_Capturable_Points[1][1],BG_AV_Capturable_Points[1][2],BG_AV_Capturable_Points[1][3],BG_AV_Capturable_Points[1][4],BG_AV_Capturable_Points[1][5],BG_AV_Capturable_Points[1][6],BG_AV_Capturable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_STORM_H_B_CONT,HORDE_CONT_BANNER,BG_AV_Capturable_Points[1][0],BG_AV_Capturable_Points[1][1],BG_AV_Capturable_Points[1][2],BG_AV_Capturable_Points[1][3],BG_AV_Capturable_Points[1][4],BG_AV_Capturable_Points[1][5],BG_AV_Capturable_Points[1][6],BG_AV_Capturable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_STORM_A_B_CONT,ALLIANCE_CONT_BANNER,BG_AV_Capturable_Points[1][0],BG_AV_Capturable_Points[1][1],BG_AV_Capturable_Points[1][2],BG_AV_Capturable_Points[1][3],BG_AV_Capturable_Points[1][4],BG_AV_Capturable_Points[1][5],BG_AV_Capturable_Points[1][6],BG_AV_Capturable_Points[1][7],RESPAWN_IMMEDIATELY)
		// DunBaldar
		|| !AddObject(BG_AV_OBJECT_DUNB_H_B,HORDE_BANNER,BG_AV_Capturable_Points[0][0],BG_AV_Capturable_Points[0][1],BG_AV_Capturable_Points[0][2],BG_AV_Capturable_Points[0][3],BG_AV_Capturable_Points[0][4],BG_AV_Capturable_Points[0][5],BG_AV_Capturable_Points[0][6],BG_AV_Capturable_Points[0][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DUNB_A_B,ALLIANCE_BANNER,BG_AV_Capturable_Points[0][0],BG_AV_Capturable_Points[0][1],BG_AV_Capturable_Points[0][2],BG_AV_Capturable_Points[0][3],BG_AV_Capturable_Points[0][4],BG_AV_Capturable_Points[0][5],BG_AV_Capturable_Points[0][6],BG_AV_Capturable_Points[0][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DUNB_H_B_CONT,HORDE_CONT_BANNER,BG_AV_Capturable_Points[0][0],BG_AV_Capturable_Points[0][1],BG_AV_Capturable_Points[0][2],BG_AV_Capturable_Points[0][3],BG_AV_Capturable_Points[0][4],BG_AV_Capturable_Points[0][5],BG_AV_Capturable_Points[0][6],BG_AV_Capturable_Points[0][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DUNB_A_B_CONT,ALLIANCE_CONT_BANNER,BG_AV_Capturable_Points[0][0],BG_AV_Capturable_Points[0][1],BG_AV_Capturable_Points[0][2],BG_AV_Capturable_Points[0][3],BG_AV_Capturable_Points[0][4],BG_AV_Capturable_Points[0][5],BG_AV_Capturable_Points[0][6],BG_AV_Capturable_Points[0][7],RESPAWN_IMMEDIATELY)
		// Gîtepierre
		|| !AddObject(BG_AV_OBJECT_GIT_H_B,HORDE_BANNER,BG_AV_Capturable_Points[2][0],BG_AV_Capturable_Points[2][1],BG_AV_Capturable_Points[2][2],BG_AV_Capturable_Points[2][3],BG_AV_Capturable_Points[2][4],BG_AV_Capturable_Points[2][5],BG_AV_Capturable_Points[2][6],BG_AV_Capturable_Points[2][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GIT_A_B,ALLIANCE_BANNER,BG_AV_Capturable_Points[2][0],BG_AV_Capturable_Points[2][1],BG_AV_Capturable_Points[2][2],BG_AV_Capturable_Points[2][3],BG_AV_Capturable_Points[2][4],BG_AV_Capturable_Points[2][5],BG_AV_Capturable_Points[2][6],BG_AV_Capturable_Points[2][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GIT_H_B_CONT,HORDE_CONT_BANNER,BG_AV_Capturable_Points[2][0],BG_AV_Capturable_Points[2][1],BG_AV_Capturable_Points[2][2],BG_AV_Capturable_Points[2][3],BG_AV_Capturable_Points[2][4],BG_AV_Capturable_Points[2][5],BG_AV_Capturable_Points[2][6],BG_AV_Capturable_Points[2][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GIT_A_B_CONT,ALLIANCE_CONT_BANNER,BG_AV_Capturable_Points[2][0],BG_AV_Capturable_Points[2][1],BG_AV_Capturable_Points[2][2],BG_AV_Capturable_Points[2][3],BG_AV_Capturable_Points[2][4],BG_AV_Capturable_Points[2][5],BG_AV_Capturable_Points[2][6],BG_AV_Capturable_Points[2][7],RESPAWN_IMMEDIATELY)
		/* SnowFall */
		// Main
		|| !AddObject(BG_AV_OBJECT_SWF_NEU_B,SNOWFALL_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_H_B,HORDE_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B,ALLIANCE_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_CONT,HORDE_CONT_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_CONT,ALLIANCE_CONT_BANNER,BG_AV_Capturable_Points[3][0],BG_AV_Capturable_Points[3][1],BG_AV_Capturable_Points[3][2],BG_AV_Capturable_Points[3][3],BG_AV_Capturable_Points[3][4],BG_AV_Capturable_Points[3][5],BG_AV_Capturable_Points[3][6],BG_AV_Capturable_Points[3][7],RESPAWN_IMMEDIATELY)
		// Spawn 1
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_1,SNOWFALL_BANNER_H,BG_AV_Capturable_Points[4][0],BG_AV_Capturable_Points[4][1],BG_AV_Capturable_Points[4][2],BG_AV_Capturable_Points[4][3],BG_AV_Capturable_Points[4][4],BG_AV_Capturable_Points[4][5],BG_AV_Capturable_Points[4][6],BG_AV_Capturable_Points[4][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_1,SNOWFALL_BANNER_A,BG_AV_Capturable_Points[4][0],BG_AV_Capturable_Points[4][1],BG_AV_Capturable_Points[4][2],BG_AV_Capturable_Points[4][3],BG_AV_Capturable_Points[4][4],BG_AV_Capturable_Points[4][5],BG_AV_Capturable_Points[4][6],BG_AV_Capturable_Points[4][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_CONT_1,SNOWFALL_BANNER_H_CONT,BG_AV_Capturable_Points[4][0],BG_AV_Capturable_Points[4][1],BG_AV_Capturable_Points[4][2],BG_AV_Capturable_Points[4][3],BG_AV_Capturable_Points[4][4],BG_AV_Capturable_Points[4][5],BG_AV_Capturable_Points[4][6],BG_AV_Capturable_Points[4][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_CONT_1,SNOWFALL_BANNER_A_CONT,BG_AV_Capturable_Points[4][0],BG_AV_Capturable_Points[4][1],BG_AV_Capturable_Points[4][2],BG_AV_Capturable_Points[4][3],BG_AV_Capturable_Points[4][4],BG_AV_Capturable_Points[4][5],BG_AV_Capturable_Points[4][6],BG_AV_Capturable_Points[4][7],RESPAWN_IMMEDIATELY)
		// Spawn 2
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_2,SNOWFALL_BANNER_H,BG_AV_Capturable_Points[5][0],BG_AV_Capturable_Points[5][1],BG_AV_Capturable_Points[5][2],BG_AV_Capturable_Points[5][3],BG_AV_Capturable_Points[5][4],BG_AV_Capturable_Points[5][5],BG_AV_Capturable_Points[5][6],BG_AV_Capturable_Points[5][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_2,SNOWFALL_BANNER_A,BG_AV_Capturable_Points[5][0],BG_AV_Capturable_Points[5][1],BG_AV_Capturable_Points[5][2],BG_AV_Capturable_Points[5][3],BG_AV_Capturable_Points[5][4],BG_AV_Capturable_Points[5][5],BG_AV_Capturable_Points[5][6],BG_AV_Capturable_Points[5][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_CONT_2,SNOWFALL_BANNER_H_CONT,BG_AV_Capturable_Points[5][0],BG_AV_Capturable_Points[5][1],BG_AV_Capturable_Points[5][2],BG_AV_Capturable_Points[5][3],BG_AV_Capturable_Points[5][4],BG_AV_Capturable_Points[5][5],BG_AV_Capturable_Points[5][6],BG_AV_Capturable_Points[5][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_CONT_2,SNOWFALL_BANNER_A_CONT,BG_AV_Capturable_Points[5][0],BG_AV_Capturable_Points[5][1],BG_AV_Capturable_Points[5][2],BG_AV_Capturable_Points[5][3],BG_AV_Capturable_Points[5][4],BG_AV_Capturable_Points[5][5],BG_AV_Capturable_Points[5][6],BG_AV_Capturable_Points[5][7],RESPAWN_IMMEDIATELY)
		// Spawn 3
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_3,SNOWFALL_BANNER_H,BG_AV_Capturable_Points[6][0],BG_AV_Capturable_Points[6][1],BG_AV_Capturable_Points[6][2],BG_AV_Capturable_Points[6][3],BG_AV_Capturable_Points[6][4],BG_AV_Capturable_Points[6][5],BG_AV_Capturable_Points[6][6],BG_AV_Capturable_Points[6][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_3,SNOWFALL_BANNER_A,BG_AV_Capturable_Points[6][0],BG_AV_Capturable_Points[6][1],BG_AV_Capturable_Points[6][2],BG_AV_Capturable_Points[6][3],BG_AV_Capturable_Points[6][4],BG_AV_Capturable_Points[6][5],BG_AV_Capturable_Points[6][6],BG_AV_Capturable_Points[6][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_CONT_3,SNOWFALL_BANNER_H_CONT,BG_AV_Capturable_Points[6][0],BG_AV_Capturable_Points[6][1],BG_AV_Capturable_Points[6][2],BG_AV_Capturable_Points[6][3],BG_AV_Capturable_Points[6][4],BG_AV_Capturable_Points[6][5],BG_AV_Capturable_Points[6][6],BG_AV_Capturable_Points[6][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_CONT_3,SNOWFALL_BANNER_A_CONT,BG_AV_Capturable_Points[6][0],BG_AV_Capturable_Points[6][1],BG_AV_Capturable_Points[6][2],BG_AV_Capturable_Points[6][3],BG_AV_Capturable_Points[6][4],BG_AV_Capturable_Points[6][5],BG_AV_Capturable_Points[6][6],BG_AV_Capturable_Points[6][7],RESPAWN_IMMEDIATELY)
		// Spawn 4
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_4,SNOWFALL_BANNER_H,BG_AV_Capturable_Points[7][0],BG_AV_Capturable_Points[7][1],BG_AV_Capturable_Points[7][2],BG_AV_Capturable_Points[7][3],BG_AV_Capturable_Points[7][4],BG_AV_Capturable_Points[7][5],BG_AV_Capturable_Points[7][6],BG_AV_Capturable_Points[7][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_4,SNOWFALL_BANNER_A,BG_AV_Capturable_Points[7][0],BG_AV_Capturable_Points[7][1],BG_AV_Capturable_Points[7][2],BG_AV_Capturable_Points[7][3],BG_AV_Capturable_Points[7][4],BG_AV_Capturable_Points[7][5],BG_AV_Capturable_Points[7][6],BG_AV_Capturable_Points[7][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_H_B_CONT_4,SNOWFALL_BANNER_H_CONT,BG_AV_Capturable_Points[7][0],BG_AV_Capturable_Points[7][1],BG_AV_Capturable_Points[7][2],BG_AV_Capturable_Points[7][3],BG_AV_Capturable_Points[7][4],BG_AV_Capturable_Points[7][5],BG_AV_Capturable_Points[7][6],BG_AV_Capturable_Points[7][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_SWF_A_B_CONT_4,SNOWFALL_BANNER_A_CONT,BG_AV_Capturable_Points[7][0],BG_AV_Capturable_Points[7][1],BG_AV_Capturable_Points[7][2],BG_AV_Capturable_Points[7][3],BG_AV_Capturable_Points[7][4],BG_AV_Capturable_Points[7][5],BG_AV_Capturable_Points[7][6],BG_AV_Capturable_Points[7][7],RESPAWN_IMMEDIATELY)
		// GlaceSang
		|| !AddObject(BG_AV_OBJECT_GSG_H_B,HORDE_BANNER,BG_AV_Capturable_Points[8][0],BG_AV_Capturable_Points[8][1],BG_AV_Capturable_Points[8][2],BG_AV_Capturable_Points[8][3],BG_AV_Capturable_Points[8][4],BG_AV_Capturable_Points[8][5],BG_AV_Capturable_Points[8][6],BG_AV_Capturable_Points[8][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GSG_A_B,ALLIANCE_BANNER,BG_AV_Capturable_Points[8][0],BG_AV_Capturable_Points[8][1],BG_AV_Capturable_Points[8][2],BG_AV_Capturable_Points[8][3],BG_AV_Capturable_Points[8][4],BG_AV_Capturable_Points[8][5],BG_AV_Capturable_Points[8][6],BG_AV_Capturable_Points[8][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GSG_H_B_CONT,HORDE_CONT_BANNER,BG_AV_Capturable_Points[8][0],BG_AV_Capturable_Points[8][1],BG_AV_Capturable_Points[8][2],BG_AV_Capturable_Points[8][3],BG_AV_Capturable_Points[8][4],BG_AV_Capturable_Points[8][5],BG_AV_Capturable_Points[8][6],BG_AV_Capturable_Points[8][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GSG_A_B_CONT,ALLIANCE_CONT_BANNER,BG_AV_Capturable_Points[8][0],BG_AV_Capturable_Points[8][1],BG_AV_Capturable_Points[8][2],BG_AV_Capturable_Points[8][3],BG_AV_Capturable_Points[8][4],BG_AV_Capturable_Points[8][5],BG_AV_Capturable_Points[8][6],BG_AV_Capturable_Points[8][7],RESPAWN_IMMEDIATELY)
		// Cimetière loup de givre
		|| !AddObject(BG_AV_OBJECT_FSW_H_B,HORDE_BANNER,BG_AV_Capturable_Points[9][0],BG_AV_Capturable_Points[9][1],BG_AV_Capturable_Points[9][2],BG_AV_Capturable_Points[9][3],BG_AV_Capturable_Points[9][4],BG_AV_Capturable_Points[9][5],BG_AV_Capturable_Points[9][6],BG_AV_Capturable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSW_A_B,ALLIANCE_BANNER,BG_AV_Capturable_Points[9][0],BG_AV_Capturable_Points[9][1],BG_AV_Capturable_Points[9][2],BG_AV_Capturable_Points[9][3],BG_AV_Capturable_Points[9][4],BG_AV_Capturable_Points[9][5],BG_AV_Capturable_Points[9][6],BG_AV_Capturable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSW_H_B_CONT,HORDE_CONT_BANNER,BG_AV_Capturable_Points[9][0],BG_AV_Capturable_Points[9][1],BG_AV_Capturable_Points[9][2],BG_AV_Capturable_Points[9][3],BG_AV_Capturable_Points[9][4],BG_AV_Capturable_Points[9][5],BG_AV_Capturable_Points[9][6],BG_AV_Capturable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSW_A_B_CONT,ALLIANCE_CONT_BANNER,BG_AV_Capturable_Points[9][0],BG_AV_Capturable_Points[9][1],BG_AV_Capturable_Points[9][2],BG_AV_Capturable_Points[9][3],BG_AV_Capturable_Points[9][4],BG_AV_Capturable_Points[9][5],BG_AV_Capturable_Points[9][6],BG_AV_Capturable_Points[9][7],RESPAWN_IMMEDIATELY)
		// Hutte de guérison Loup de Givre
		|| !AddObject(BG_AV_OBJECT_FSH_H_B,HORDE_BANNER,BG_AV_Capturable_Points[10][0],BG_AV_Capturable_Points[10][1],BG_AV_Capturable_Points[10][2],BG_AV_Capturable_Points[10][3],BG_AV_Capturable_Points[10][4],BG_AV_Capturable_Points[10][5],BG_AV_Capturable_Points[10][6],BG_AV_Capturable_Points[10][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSH_A_B,ALLIANCE_BANNER,BG_AV_Capturable_Points[10][0],BG_AV_Capturable_Points[10][1],BG_AV_Capturable_Points[10][2],BG_AV_Capturable_Points[10][3],BG_AV_Capturable_Points[10][4],BG_AV_Capturable_Points[10][5],BG_AV_Capturable_Points[10][6],BG_AV_Capturable_Points[10][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSH_H_B_CONT,HORDE_CONT_BANNER,BG_AV_Capturable_Points[10][0],BG_AV_Capturable_Points[10][1],BG_AV_Capturable_Points[10][2],BG_AV_Capturable_Points[10][3],BG_AV_Capturable_Points[10][4],BG_AV_Capturable_Points[10][5],BG_AV_Capturable_Points[10][6],BG_AV_Capturable_Points[10][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FSH_A_B_CONT,ALLIANCE_CONT_BANNER,BG_AV_Capturable_Points[10][0],BG_AV_Capturable_Points[10][1],BG_AV_Capturable_Points[10][2],BG_AV_Capturable_Points[10][3],BG_AV_Capturable_Points[10][4],BG_AV_Capturable_Points[10][5],BG_AV_Capturable_Points[10][6],BG_AV_Capturable_Points[10][7],RESPAWN_IMMEDIATELY)
		
		/* Tours */
		// Dun Baldar North
		|| !AddObject(BG_AV_OBJECT_DBN_A_B,ALLIANCE_D_BANNER,BG_AV_Destroyable_Points[0][0],BG_AV_Destroyable_Points[0][1],BG_AV_Destroyable_Points[0][2],BG_AV_Destroyable_Points[0][3],BG_AV_Destroyable_Points[0][4],BG_AV_Destroyable_Points[0][5],BG_AV_Destroyable_Points[0][6],BG_AV_Destroyable_Points[0][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBN_H_B,HORDE_CONT_D_BANNER,BG_AV_Destroyable_Points[0][0],BG_AV_Destroyable_Points[0][1],BG_AV_Destroyable_Points[0][2],BG_AV_Destroyable_Points[0][3],BG_AV_Destroyable_Points[0][4],BG_AV_Destroyable_Points[0][5],BG_AV_Destroyable_Points[0][6],BG_AV_Destroyable_Points[0][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBN_A_T,ALLIANCE_T_BANNER,BG_AV_Destroyable_Points[1][0],BG_AV_Destroyable_Points[1][1],BG_AV_Destroyable_Points[1][2],BG_AV_Destroyable_Points[1][3],BG_AV_Destroyable_Points[1][4],BG_AV_Destroyable_Points[1][5],BG_AV_Destroyable_Points[1][6],BG_AV_Destroyable_Points[1][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBN_H_T,HORDE_COND_T_BANNER,BG_AV_Destroyable_Points[1][0],BG_AV_Destroyable_Points[1][1],BG_AV_Destroyable_Points[1][2],BG_AV_Destroyable_Points[1][3],BG_AV_Destroyable_Points[1][4],BG_AV_Destroyable_Points[1][5],BG_AV_Destroyable_Points[1][6],BG_AV_Destroyable_Points[1][7],RESPAWN_IMMEDIATELY)
		// Dun Baldar South
		|| !AddObject(BG_AV_OBJECT_DBS_A_B,ALLIANCE_D_BANNER,BG_AV_Destroyable_Points[2][0],BG_AV_Destroyable_Points[2][1],BG_AV_Destroyable_Points[2][2],BG_AV_Destroyable_Points[2][3],BG_AV_Destroyable_Points[2][4],BG_AV_Destroyable_Points[2][5],BG_AV_Destroyable_Points[2][6],BG_AV_Destroyable_Points[2][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBS_H_B,HORDE_CONT_D_BANNER,BG_AV_Destroyable_Points[2][0],BG_AV_Destroyable_Points[2][1],BG_AV_Destroyable_Points[2][2],BG_AV_Destroyable_Points[2][3],BG_AV_Destroyable_Points[2][4],BG_AV_Destroyable_Points[2][5],BG_AV_Destroyable_Points[2][6],BG_AV_Destroyable_Points[2][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBS_A_T,ALLIANCE_T_BANNER,BG_AV_Destroyable_Points[3][0],BG_AV_Destroyable_Points[3][1],BG_AV_Destroyable_Points[3][2],BG_AV_Destroyable_Points[3][3],BG_AV_Destroyable_Points[3][4],BG_AV_Destroyable_Points[3][5],BG_AV_Destroyable_Points[3][6],BG_AV_Destroyable_Points[3][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_DBS_H_T,HORDE_COND_T_BANNER,BG_AV_Destroyable_Points[3][0],BG_AV_Destroyable_Points[3][1],BG_AV_Destroyable_Points[3][2],BG_AV_Destroyable_Points[3][3],BG_AV_Destroyable_Points[3][4],BG_AV_Destroyable_Points[3][5],BG_AV_Destroyable_Points[3][6],BG_AV_Destroyable_Points[3][7],RESPAWN_IMMEDIATELY)
		// Aile de Glace 
		|| !AddObject(BG_AV_OBJECT_ADG_A_B,ALLIANCE_D_BANNER,BG_AV_Destroyable_Points[4][0],BG_AV_Destroyable_Points[4][1],BG_AV_Destroyable_Points[4][2],BG_AV_Destroyable_Points[4][3],BG_AV_Destroyable_Points[4][4],BG_AV_Destroyable_Points[4][5],BG_AV_Destroyable_Points[4][6],BG_AV_Destroyable_Points[4][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_ADG_H_B,HORDE_CONT_D_BANNER,BG_AV_Destroyable_Points[4][0],BG_AV_Destroyable_Points[4][1],BG_AV_Destroyable_Points[4][2],BG_AV_Destroyable_Points[4][3],BG_AV_Destroyable_Points[4][4],BG_AV_Destroyable_Points[4][5],BG_AV_Destroyable_Points[4][6],BG_AV_Destroyable_Points[4][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_ADG_A_T,ALLIANCE_T_BANNER,BG_AV_Destroyable_Points[5][0],BG_AV_Destroyable_Points[5][1],BG_AV_Destroyable_Points[5][2],BG_AV_Destroyable_Points[5][3],BG_AV_Destroyable_Points[5][4],BG_AV_Destroyable_Points[5][5],BG_AV_Destroyable_Points[5][6],BG_AV_Destroyable_Points[5][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_ADG_H_T,HORDE_COND_T_BANNER,BG_AV_Destroyable_Points[5][0],BG_AV_Destroyable_Points[5][1],BG_AV_Destroyable_Points[5][2],BG_AV_Destroyable_Points[5][3],BG_AV_Destroyable_Points[5][4],BG_AV_Destroyable_Points[5][5],BG_AV_Destroyable_Points[5][6],BG_AV_Destroyable_Points[5][7],RESPAWN_IMMEDIATELY)
		// Gîtepierre
		|| !AddObject(BG_AV_OBJECT_FGP_A_B,ALLIANCE_D_BANNER,BG_AV_Destroyable_Points[6][0],BG_AV_Destroyable_Points[6][1],BG_AV_Destroyable_Points[6][2],BG_AV_Destroyable_Points[6][3],BG_AV_Destroyable_Points[6][4],BG_AV_Destroyable_Points[6][5],BG_AV_Destroyable_Points[6][6],BG_AV_Destroyable_Points[6][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FGP_H_B,HORDE_CONT_D_BANNER,BG_AV_Destroyable_Points[6][0],BG_AV_Destroyable_Points[6][1],BG_AV_Destroyable_Points[6][2],BG_AV_Destroyable_Points[6][3],BG_AV_Destroyable_Points[6][4],BG_AV_Destroyable_Points[6][5],BG_AV_Destroyable_Points[6][6],BG_AV_Destroyable_Points[6][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FGP_A_T,ALLIANCE_T_BANNER,BG_AV_Destroyable_Points[7][0],BG_AV_Destroyable_Points[7][1],BG_AV_Destroyable_Points[7][2],BG_AV_Destroyable_Points[7][3],BG_AV_Destroyable_Points[7][4],BG_AV_Destroyable_Points[7][5],BG_AV_Destroyable_Points[7][6],BG_AV_Destroyable_Points[7][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FGP_H_T,HORDE_COND_T_BANNER,BG_AV_Destroyable_Points[7][0],BG_AV_Destroyable_Points[7][1],BG_AV_Destroyable_Points[7][2],BG_AV_Destroyable_Points[7][3],BG_AV_Destroyable_Points[7][4],BG_AV_Destroyable_Points[7][5],BG_AV_Destroyable_Points[7][6],BG_AV_Destroyable_Points[7][7],RESPAWN_IMMEDIATELY)
		// GlaceSang
		|| !AddObject(BG_AV_OBJECT_GST_A_B,ALLIANCE_CONT_D_BANNER,BG_AV_Destroyable_Points[8][0],BG_AV_Destroyable_Points[8][1],BG_AV_Destroyable_Points[8][2],BG_AV_Destroyable_Points[8][3],BG_AV_Destroyable_Points[8][4],BG_AV_Destroyable_Points[8][5],BG_AV_Destroyable_Points[8][6],BG_AV_Destroyable_Points[8][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GST_H_B,HORDE_D_BANNER,BG_AV_Destroyable_Points[8][0],BG_AV_Destroyable_Points[8][1],BG_AV_Destroyable_Points[8][2],BG_AV_Destroyable_Points[8][3],BG_AV_Destroyable_Points[8][4],BG_AV_Destroyable_Points[8][5],BG_AV_Destroyable_Points[8][6],BG_AV_Destroyable_Points[8][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GST_A_T,ALLIANCE_CONT_T_BANNER,BG_AV_Destroyable_Points[9][0],BG_AV_Destroyable_Points[9][1],BG_AV_Destroyable_Points[9][2],BG_AV_Destroyable_Points[9][3],BG_AV_Destroyable_Points[9][4],BG_AV_Destroyable_Points[9][5],BG_AV_Destroyable_Points[9][6],BG_AV_Destroyable_Points[9][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_GST_H_T,HORDE_T_BANNER,BG_AV_Destroyable_Points[9][0],BG_AV_Destroyable_Points[9][1],BG_AV_Destroyable_Points[9][2],BG_AV_Destroyable_Points[9][3],BG_AV_Destroyable_Points[9][4],BG_AV_Destroyable_Points[9][5],BG_AV_Destroyable_Points[9][6],BG_AV_Destroyable_Points[9][7],RESPAWN_IMMEDIATELY)
		// Halte
		|| !AddObject(BG_AV_OBJECT_HAL_A_B,ALLIANCE_CONT_D_BANNER,BG_AV_Destroyable_Points[10][0],BG_AV_Destroyable_Points[10][1],BG_AV_Destroyable_Points[10][2],BG_AV_Destroyable_Points[10][3],BG_AV_Destroyable_Points[10][4],BG_AV_Destroyable_Points[10][5],BG_AV_Destroyable_Points[10][6],BG_AV_Destroyable_Points[10][7],RESPAWN_ONE_DAY)
		|| !AddObject(BG_AV_OBJECT_HAL_H_B,HORDE_D_BANNER,BG_AV_Destroyable_Points[10][0],BG_AV_Destroyable_Points[10][1],BG_AV_Destroyable_Points[10][2],BG_AV_Destroyable_Points[10][3],BG_AV_Destroyable_Points[10][4],BG_AV_Destroyable_Points[10][5],BG_AV_Destroyable_Points[10][6],BG_AV_Destroyable_Points[10][7],RESPAWN_ONE_DAY)
		|| !AddObject(BG_AV_OBJECT_HAL_A_T,ALLIANCE_CONT_T_BANNER,BG_AV_Destroyable_Points[11][0],BG_AV_Destroyable_Points[11][1],BG_AV_Destroyable_Points[11][2],BG_AV_Destroyable_Points[11][3],BG_AV_Destroyable_Points[11][4],BG_AV_Destroyable_Points[11][5],BG_AV_Destroyable_Points[11][6],BG_AV_Destroyable_Points[11][7],RESPAWN_ONE_DAY)
		|| !AddObject(BG_AV_OBJECT_HAL_H_T,HORDE_T_BANNER,BG_AV_Destroyable_Points[11][0],BG_AV_Destroyable_Points[11][1],BG_AV_Destroyable_Points[11][2],BG_AV_Destroyable_Points[11][3],BG_AV_Destroyable_Points[11][4],BG_AV_Destroyable_Points[11][5],BG_AV_Destroyable_Points[11][6],BG_AV_Destroyable_Points[11][7],RESPAWN_ONE_DAY)
		// FrostWolf Ouest
		|| !AddObject(BG_AV_OBJECT_FWO_A_B,ALLIANCE_CONT_D_BANNER,BG_AV_Destroyable_Points[12][0],BG_AV_Destroyable_Points[12][1],BG_AV_Destroyable_Points[12][2],BG_AV_Destroyable_Points[12][3],BG_AV_Destroyable_Points[12][4],BG_AV_Destroyable_Points[12][5],BG_AV_Destroyable_Points[12][6],BG_AV_Destroyable_Points[12][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWO_H_B,HORDE_D_BANNER,BG_AV_Destroyable_Points[12][0],BG_AV_Destroyable_Points[12][1],BG_AV_Destroyable_Points[12][2],BG_AV_Destroyable_Points[12][3],BG_AV_Destroyable_Points[12][4],BG_AV_Destroyable_Points[12][5],BG_AV_Destroyable_Points[12][6],BG_AV_Destroyable_Points[12][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWO_A_T,ALLIANCE_CONT_T_BANNER,BG_AV_Destroyable_Points[13][0],BG_AV_Destroyable_Points[13][1],BG_AV_Destroyable_Points[13][2],BG_AV_Destroyable_Points[13][3],BG_AV_Destroyable_Points[13][4],BG_AV_Destroyable_Points[13][5],BG_AV_Destroyable_Points[13][6],BG_AV_Destroyable_Points[13][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWO_H_T,HORDE_T_BANNER,BG_AV_Destroyable_Points[13][0],BG_AV_Destroyable_Points[13][1],BG_AV_Destroyable_Points[13][2],BG_AV_Destroyable_Points[13][3],BG_AV_Destroyable_Points[13][4],BG_AV_Destroyable_Points[13][5],BG_AV_Destroyable_Points[13][6],BG_AV_Destroyable_Points[13][7],RESPAWN_IMMEDIATELY)
		// FrostWolf Est
		|| !AddObject(BG_AV_OBJECT_FWE_A_B,ALLIANCE_CONT_D_BANNER,BG_AV_Destroyable_Points[14][0],BG_AV_Destroyable_Points[14][1],BG_AV_Destroyable_Points[14][2],BG_AV_Destroyable_Points[14][3],BG_AV_Destroyable_Points[14][4],BG_AV_Destroyable_Points[14][5],BG_AV_Destroyable_Points[14][6],BG_AV_Destroyable_Points[14][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWE_H_B,HORDE_D_BANNER,BG_AV_Destroyable_Points[14][0],BG_AV_Destroyable_Points[14][1],BG_AV_Destroyable_Points[14][2],BG_AV_Destroyable_Points[14][3],BG_AV_Destroyable_Points[14][4],BG_AV_Destroyable_Points[14][5],BG_AV_Destroyable_Points[14][6],BG_AV_Destroyable_Points[14][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWE_A_T,ALLIANCE_CONT_T_BANNER,BG_AV_Destroyable_Points[15][0],BG_AV_Destroyable_Points[15][1],BG_AV_Destroyable_Points[15][2],BG_AV_Destroyable_Points[15][3],BG_AV_Destroyable_Points[15][4],BG_AV_Destroyable_Points[15][5],BG_AV_Destroyable_Points[15][6],BG_AV_Destroyable_Points[15][7],RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_AV_OBJECT_FWE_H_T,HORDE_T_BANNER,BG_AV_Destroyable_Points[15][0],BG_AV_Destroyable_Points[15][1],BG_AV_Destroyable_Points[15][2],BG_AV_Destroyable_Points[15][3],BG_AV_Destroyable_Points[15][4],BG_AV_Destroyable_Points[15][5],BG_AV_Destroyable_Points[15][6],BG_AV_Destroyable_Points[15][7],RESPAWN_IMMEDIATELY)
		
		)
		{
			sLog.outErrorDb("BattleGroundAV: Failed to spawn Stormpike banners!");
			return false;
		}
		uint8 i;
		for(i=0;i<64;i++)
			AddObject(BG_AV_OBJECT_DBS_F+i,FLAMMES,BG_AV_Flammes[i][0],BG_AV_Flammes[i][1],BG_AV_Flammes[i][2],BG_AV_Flammes[i][3],BG_AV_Flammes[i][4],BG_AV_Flammes[i][5],BG_AV_Flammes[i][6],BG_AV_Flammes[i][7],RESPAWN_ONE_DAY);
		

		for(i=0;i<5;i++)
			AddObject(BG_AV_OBJECT_GGS_F+i,FLAMMES,BG_AV_FlammesForCaptains[i][0],BG_AV_FlammesForCaptains[i][1],BG_AV_FlammesForCaptains[i][2],BG_AV_FlammesForCaptains[i][3],BG_AV_FlammesForCaptains[i][4],BG_AV_FlammesForCaptains[i][5],BG_AV_FlammesForCaptains[i][6],BG_AV_FlammesForCaptains[i][7],RESPAWN_ONE_DAY);

		for(i=5;i<9;i++)
			AddObject(BG_AV_OBJECT_GGS_F+i,SMOKE,BG_AV_FlammesForCaptains[i][0],BG_AV_FlammesForCaptains[i][1],BG_AV_FlammesForCaptains[i][2],BG_AV_FlammesForCaptains[i][3],BG_AV_FlammesForCaptains[i][4],BG_AV_FlammesForCaptains[i][5],BG_AV_FlammesForCaptains[i][6],BG_AV_FlammesForCaptains[i][7],RESPAWN_ONE_DAY);

		for(i=9;i<14;i++)
			AddObject(BG_AV_OBJECT_GGS_F+i,FLAMMES,BG_AV_FlammesForCaptains[i][0],BG_AV_FlammesForCaptains[i][1],BG_AV_FlammesForCaptains[i][2],BG_AV_FlammesForCaptains[i][3],BG_AV_FlammesForCaptains[i][4],BG_AV_FlammesForCaptains[i][5],BG_AV_FlammesForCaptains[i][6],BG_AV_FlammesForCaptains[i][7],RESPAWN_ONE_DAY);

		for(i=14;i<17;i++)
			AddObject(BG_AV_OBJECT_GGS_F+i,SMOKE,BG_AV_FlammesForCaptains[i][0],BG_AV_FlammesForCaptains[i][1],BG_AV_FlammesForCaptains[i][2],BG_AV_FlammesForCaptains[i][3],BG_AV_FlammesForCaptains[i][4],BG_AV_FlammesForCaptains[i][5],BG_AV_FlammesForCaptains[i][6],BG_AV_FlammesForCaptains[i][7],RESPAWN_ONE_DAY);

		InitNode(BG_AV_NODES_FIRSTAID_STATION,ALLIANCE,false);
		InitNode(BG_AV_NODES_STORMPIKE_GRAVE,ALLIANCE,false);
		InitNode(BG_AV_NODES_STONEHEART_GRAVE,ALLIANCE,false);
		InitNode(BG_AV_NODES_SNOWFALL_GRAVE,0,false);
		InitNode(BG_AV_NODES_ICEBLOOD_GRAVE,HORDE,false);
		InitNode(BG_AV_NODES_FROSTWOLF_GRAVE,HORDE,false);
		InitNode(BG_AV_NODES_FROSTWOLF_HUT,HORDE,false);
		InitNode(BG_AV_NODES_DUNBALDAR_SOUTH,ALLIANCE,true);
		InitNode(BG_AV_NODES_DUNBALDAR_NORTH,ALLIANCE,true);
		InitNode(BG_AV_NODES_ICEWING_BUNKER,ALLIANCE,true);
		InitNode(BG_AV_NODES_STONEHEART_BUNKER,ALLIANCE,true);
		InitNode(BG_AV_NODES_ICEBLOOD_TOWER,HORDE,true);
		InitNode(BG_AV_NODES_TOWER_POINT,HORDE,true);
		InitNode(BG_AV_NODES_FROSTWOLF_ETOWER,HORDE,true);
		InitNode(BG_AV_NODES_FROSTWOLF_WTOWER,HORDE,true);
		m_captainAlive[BG_TEAM_ALLIANCE] = true;
		m_captainAlive[BG_TEAM_HORDE] = true;
		return true;
}




void BattleGroundAV::FillInitialWorldStates(WorldPacket& data)
{
    bool stateok;
    // graveyards
    for (uint32 i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_FROSTWOLF_HUT; ++i)
    {
        for (uint8 j = 1; j <= 3; j+=2)
        {
            // j=1=assaulted j=3=controled
            stateok = (m_Nodes[i].State == j);
            data << uint32(BG_AV_NodeWorldStates[i][GetWorldStateType(j,ALLIANCE)]) << uint32((m_Nodes[i].Owner == ALLIANCE && stateok) ? 1 : 0);
            data << uint32(BG_AV_NodeWorldStates[i][GetWorldStateType(j,HORDE)]) << uint32((m_Nodes[i].Owner == HORDE && stateok) ? 1 : 0);
        }
    }

    // towers
    for (uint8 i = BG_AV_NODES_DUNBALDAR_SOUTH; i <= BG_AV_NODES_MAX; i++)
        for (uint8 j = 1; j <= 3; j+=2)
        {
            // j=1=assaulted j=3=controled
            // i dont have j = 2=destroyed cause destroyed is the same like enemy - team controll
            stateok = (m_Nodes[i].State == j || (m_Nodes[i].State == POINT_DESTROYED && j == 3));
            data << uint32(BG_AV_NodeWorldStates[i][GetWorldStateType(j, ALLIANCE)]) << uint32((m_Nodes[i].Owner == ALLIANCE && stateok) ? 1 : 0);
            data << uint32(BG_AV_NodeWorldStates[i][GetWorldStateType(j, HORDE)]) << uint32((m_Nodes[i].Owner == HORDE && stateok) ? 1 : 0);
        }
    if( m_Nodes[BG_AV_NODES_SNOWFALL_GRAVE].Owner == BG_AV_NEUTRAL_TEAM )   // cause neutral teams aren't handled generic
        data << uint32(AV_SNOWFALL_N) << uint32(1);
    data << uint32(BG_AV_Alliance_Score)  << uint32(m_TeamScores[BG_TEAM_ALLIANCE]);
    data << uint32(BG_AV_Horde_Score) << uint32(m_TeamScores[BG_TEAM_HORDE]);
    if( GetStatus() == STATUS_IN_PROGRESS )                 // only if game is running the teamscores are displayed
    {
        data << uint32(BG_AV_SHOW_A_SCORE) << uint32(1);
        data << uint32(BG_AV_SHOW_H_SCORE) << uint32(1);
    }
    else
    {
        data << uint32(BG_AV_SHOW_A_SCORE) << uint32(0);
        data << uint32(BG_AV_SHOW_H_SCORE) << uint32(0);
    }
    SendMineWorldStates(BG_AV_NORTH_MINE);
    SendMineWorldStates(BG_AV_SOUTH_MINE);
}



uint8 BattleGroundAV::GetWorldStateType(uint8 state, uint32 team) const
{
	// neutral stuff cant get handled (currently its only snowfall) - if we want implement it we need a new column in this array
    ASSERT(team != BG_AV_NEUTRAL_TEAM);
    // a_c a_a h_c h_a the positions in worldstate - array
    // a_c==alliance-controlled, a_a==alliance-assaulted, h_c==horde-controlled, h_a==horde-assaulted
    if (team == ALLIANCE)
    {
        if (state == POINT_CONTROLLED || state == POINT_DESTROYED)
            return 0;
        if (state == POINT_ASSAULTED)
            return 1;
    }
    if (team == HORDE)
    {
        if (state == POINT_DESTROYED || state == POINT_CONTROLLED)
            return 2;
        if (state == POINT_ASSAULTED)
            return 3;
    }
    sLog.outError("BattleGroundAV: should update a strange worldstate state:%i team:%i", state, team);
    // we will crash the game with this.. so we have a good chance for a bugreport
    return 5;
}


void BattleGroundAV::SendMineWorldStates(uint32 mine)
{
    ASSERT(mine == BG_AV_NORTH_MINE || mine == BG_AV_SOUTH_MINE);
    ASSERT(m_Mine_PrevOwner[mine] == ALLIANCE || m_Mine_PrevOwner[mine] == HORDE || m_Mine_PrevOwner[mine] == BG_AV_NEUTRAL_TEAM);
    ASSERT(m_Mine_Owner[mine] == ALLIANCE || m_Mine_Owner[mine] == HORDE || m_Mine_Owner[mine] == BG_AV_NEUTRAL_TEAM);

    uint8 owner, prevowner;                                  // those variables are needed to access the right worldstate in the BG_AV_MineWorldStates array
    if (m_Mine_PrevOwner[mine] == ALLIANCE)
        prevowner = 0;
    else if (m_Mine_PrevOwner[mine] == HORDE)
        prevowner = 2;
    else
        prevowner = 1;
    if (m_Mine_Owner[mine] == ALLIANCE)
        owner = 0;
    else if (m_Mine_Owner[mine] == HORDE)
        owner = 2;
    else
        owner = 1;

    UpdateWorldState(BG_AV_MineWorldStates[mine][owner], 1);
    if (prevowner != owner)
        UpdateWorldState(BG_AV_MineWorldStates[mine][prevowner], 0);
}



void BattleGroundAV::EventPlayerClickedOnFlag(Player *Source, GameObject* target_obj)
{		 
	if (GetStatus() != STATUS_IN_PROGRESS)
		return;
	switch(target_obj->GetEntry())
	{
        case 178943:
        case ALLIANCE_BANNER:
        case SNOWFALL_BANNER:
		case ALLIANCE_D_BANNER:
            EventPlayerAssaultsPoint(Source);
            break;
        case HORDE_CONT_BANNER:
        case ALLIANCE_CONT_BANNER:
		case HORDE_CONT_D_BANNER:
		case ALLIANCE_CONT_D_BANNER:
            EventPlayerDefendsPoint(Source);
            break;
        default:
            break;
    }
}

bool BattleGroundAV::PlayerCanDoMineQuest(int32 GOId, uint32 team)
{
    if (GOId == BG_AV_OBJECTID_MINE_N)
         return (m_Mine_Owner[BG_AV_NORTH_MINE] == GetTeamIndexByTeamId(team));
    if (GOId == BG_AV_OBJECTID_MINE_S)
         return (m_Mine_Owner[BG_AV_SOUTH_MINE] == GetTeamIndexByTeamId(team));
    return true;                                            // cause it's no mine'object it is ok if this is true
}

void BattleGroundAV::AssaultNode(BG_AV_Nodes node, uint32 team)
{
    ASSERT(m_Nodes[node].TotalOwner != team);
    ASSERT(m_Nodes[node].Owner != team);
    ASSERT(m_Nodes[node].State != POINT_DESTROYED);
    ASSERT(m_Nodes[node].State != POINT_ASSAULTED || !m_Nodes[node].TotalOwner ); // only assault an assaulted node if no totalowner exists
    // the timer gets another time, if the previous owner was 0 == Neutral
    m_Nodes[node].Timer      = (m_Nodes[node].PrevOwner != BG_AV_NEUTRAL_TEAM) ? BG_AV_CAPTIME : BG_AV_SNOWFALL_FIRSTCAP;
    m_Nodes[node].PrevOwner  = m_Nodes[node].Owner;
    m_Nodes[node].Owner      = team;
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = POINT_ASSAULTED;
}


void BattleGroundAV::DestroyNode(BG_AV_Nodes node)
{
    ASSERT(m_Nodes[node].State == POINT_ASSAULTED);

    m_Nodes[node].TotalOwner = m_Nodes[node].Owner;
    m_Nodes[node].PrevOwner  = m_Nodes[node].Owner;
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = (m_Nodes[node].Tower) ? POINT_DESTROYED : POINT_CONTROLLED;
    m_Nodes[node].Timer      = 0;
}


void BattleGroundAV::InitNode(BG_AV_Nodes node, uint32 team, bool tower)
{
    m_Nodes[node].TotalOwner = team;
    m_Nodes[node].Owner      = team;
    m_Nodes[node].PrevOwner  = team;
    m_Nodes[node].State      = POINT_CONTROLLED;
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = POINT_CONTROLLED;
    m_Nodes[node].Timer      = 0;
    m_Nodes[node].Tower      = tower;
}


void BattleGroundAV::DefendNode(BG_AV_Nodes node, uint32 team)
{
    ASSERT(m_Nodes[node].TotalOwner == team);
    ASSERT(m_Nodes[node].Owner != team);
    ASSERT(m_Nodes[node].State != POINT_CONTROLLED && m_Nodes[node].State != POINT_DESTROYED);
    m_Nodes[node].PrevOwner  = m_Nodes[node].Owner;
    m_Nodes[node].Owner      = team;
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = POINT_CONTROLLED;
    m_Nodes[node].Timer      = 0;
}

const BG_AV_Nodes BattleGroundAV::GetNodeThroughPlayerPosition(Player* plr)
{
    for(uint32 i=0; i<=BG_AV_NODES_MAX; ++i)
    {
        if (plr->IsWithinDist2d(BG_AV_Nodes_Coords[i][0], BG_AV_Nodes_Coords[i][1], BG_AV_MAX_NODE_DISTANCE))
            return BG_AV_Nodes(i);
    }
    sLog.outError("BattleGroundAV: player isn't near to any node maybe a cheater? or you spawned a banner not near to a node - or there is a bug in the code");
    return BG_AV_NODES_ERROR;
}

void BattleGroundAV::EventPlayerDefendsPoint(Player* player)
{
    ASSERT(GetStatus() == STATUS_IN_PROGRESS);
    BG_AV_Nodes node = GetNodeThroughPlayerPosition(player);
    if (node == BG_AV_NODES_ERROR)
        return;

    uint32 team = player->GetTeam();

    if (m_Nodes[node].Owner == player->GetTeam() || m_Nodes[node].State != POINT_ASSAULTED)
        return;
    if( node == BG_AV_NODES_SNOWFALL_GRAVE && BG_CentralGrave_FirstAttack == true)    // initial snowfall capture
    {
        // until snowfall doesn't belong to anyone it is better handled in assault - code (best would be to have a special function
        // for neutral nodes.. but doing this just for snowfall will be a bit to much i think
        EventPlayerAssaultsPoint(player);
        return;
    }
    sLog.outDebug("BattleGroundAV: player defends node: %i", node);
    if (m_Nodes[node].PrevOwner != team)
    {
        sLog.outError("BattleGroundAV: player defends point which doesn't belong to his team %i", node);
        return;
    }

    DepopulateNode(node);
    PopulateNode(node, player);                                     // spawn node-creatures (defender for example)
	DefendNode(node,team);                                  // set the right variables for nodeinfo
    ModifySpawnMode(node,RESPAWN_IMMEDIATELY,team);
	SendYell2ToAll(791, LANG_UNIVERSAL, m_DB_Creature[BG_AV_CREATURE_HERALD],( team == ALLIANCE ) ? 757:758, GetNodeName(node));
        
	if (IsTower(node))
    {
        UpdatePlayerScore(player, SCORE_TOWERS_DEFENDED, 1);
        PlaySoundToAll(BG_AV_SOUND_BOTH_TOWER_DEFEND);
    }
    else if (IsGrave(node))
    {
        UpdatePlayerScore(player, SCORE_GRAVEYARDS_DEFENDED, 1);
		// update the statistic for the defending player
        PlaySoundToAll((team == ALLIANCE)?BG_AV_SOUND_ALLIANCE_GOOD:BG_AV_SOUND_HORDE_GOOD);
    }

}

uint32 BattleGroundAV::GetNodeName(BG_AV_Nodes node)
{
    switch (node)
    {
        case BG_AV_NODES_FIRSTAID_STATION:  return 768;
        case BG_AV_NODES_DUNBALDAR_SOUTH:   return 769;
        case BG_AV_NODES_DUNBALDAR_NORTH:   return 770;
        case BG_AV_NODES_STORMPIKE_GRAVE:   return 771;
        case BG_AV_NODES_ICEWING_BUNKER:    return 772;
        case BG_AV_NODES_STONEHEART_GRAVE:  return 773;
        case BG_AV_NODES_STONEHEART_BUNKER: return 774;
        case BG_AV_NODES_SNOWFALL_GRAVE:    return 775;
        case BG_AV_NODES_ICEBLOOD_TOWER:    return 776;
        case BG_AV_NODES_ICEBLOOD_GRAVE:    return 777;
        case BG_AV_NODES_TOWER_POINT:       return 778;
        case BG_AV_NODES_FROSTWOLF_GRAVE:   return 779;
        case BG_AV_NODES_FROSTWOLF_ETOWER:  return 780;
        case BG_AV_NODES_FROSTWOLF_WTOWER:  return 781;
        case BG_AV_NODES_FROSTWOLF_HUT:     return 782;
        default: return 0; break;
    }
}



void BattleGroundAV::EventPlayerAssaultsPoint(Player* player)
{
	BG_AV_Nodes node = GetNodeThroughPlayerPosition(player);
    if (node == BG_AV_NODES_ERROR)
        return;
	m_assault_in_progress = node; 
	uint32 team  = player->GetTeam();
    sLog.outDebug("BattleGroundAV: player assaults node %i", node);
    if (m_Nodes[node].Owner == team || team == m_Nodes[node].TotalOwner)
        return;

    DepopulateNode(node);
    ModifySpawnMode(node,RESPAWN_ONE_DAY,GetOtherTeam(team));
    PopulateNode(node, player);
    AssaultNode(node, team);                                // update nodeinfo variables

	SendYell2ToAll(764, LANG_UNIVERSAL, m_DB_Creature[BG_AV_CREATURE_HERALD], GetNodeName(node), ( team == ALLIANCE ) ? 757:758);
    
    if (IsTower(node))
		UpdatePlayerScore(player, SCORE_TOWERS_ASSAULTED, 1);

	else if (IsGrave(node))
	{
		UpdatePlayerScore(player, SCORE_GRAVEYARDS_ASSAULTED, 1);
	}
    PlaySoundToAll((team == ALLIANCE)?BG_AV_SOUND_ALLIANCE_GOOD:BG_AV_SOUND_HORDE_GOOD);
 
}

void BattleGroundAV::DepopulateNode(BG_AV_Nodes node)
{
    // graves will be handled extra, cause the defenders are not stored in the
    // m_NodeObjects.creature vector, cause they can change through a quest
    if (IsGrave(node))
	{
		if(m_Nodes[node].Owner != BG_AV_NEUTRAL_TEAM && m_Nodes[node].State == POINT_CONTROLLED)
		{
			uint32 team = GetTeamIndexByTeamId(m_Nodes[node].Owner);
			uint32 graveDefenderType;
			if (m_Team_QuestStatus[team][0] < 500 )
				graveDefenderType = 0;
			else if (m_Team_QuestStatus[team][0] < 1000 )
				graveDefenderType = 1;
			else if (m_Team_QuestStatus[team][0] < 1500 )
				graveDefenderType = 2;
			else
				graveDefenderType = 3;
		}
	}
	int i;
		switch(node)
		{
			case 0:
				for(i=9;i<=15;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 1:
				for(i=2;i<=8;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 2:
				for(i=16;i<=22;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 3:
				for(i=23;i<=47;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 4:
				for(i=48;i<=54;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 5:
				for(i=55;i<=61;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 6:
				for(i=62;i<=68;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 7:
				for(i=75;i<=80;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 8:
				for(i=69;i<=74;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 9:
				for(i=81;i<=86;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 10:
				for(i=87;i<=92;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 11:
				for(i=93;i<=98;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 12:
				for(i=99;i<=104;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 13:
				for(i=111;i<=116;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			case 14:
				for(i=105;i<=110;i++)
					SpawnBGObject(i,RESPAWN_ONE_DAY);
				break;
			default:
				break;

		}
}




void BattleGroundAV::SpawnNode(uint32 oldWS, uint32 newWS, uint32 aura, uint32 banner, uint32 banner2)
{
	UpdateWorldState(oldWS,0);
	UpdateWorldState(newWS,1);
	SpawnBGObject(aura,RESPAWN_IMMEDIATELY);
	SpawnBGObject(banner,RESPAWN_IMMEDIATELY);
	if(banner2 != 0)
		SpawnBGObject(banner2,RESPAWN_IMMEDIATELY);

}

void BattleGroundAV::ModifySpawnMode(BG_AV_Nodes node, uint32 respawn, uint32 team)
{

	uint8 teamindex = GetTeamIndexByTeamId(team);

    uint32 upgrade_mobs = 0;
    switch(m_Team_QuestStatus[team][0])
    {
    case 500:
        upgrade_mobs = 1;
        break;
    case 1000:
        upgrade_mobs = 2;
        break;
    case 1500:
        upgrade_mobs = 3;
        break;
    default:
        break;
    }
	BGCreatures::const_iterator itr = m_GraveCreatures[node][teamindex][upgrade_mobs].begin();
    switch(node)
    {
        case BG_AV_NODES_FIRSTAID_STATION:
        case BG_AV_NODES_STORMPIKE_GRAVE:
        case BG_AV_NODES_STONEHEART_GRAVE:
        case BG_AV_NODES_SNOWFALL_GRAVE:
        case BG_AV_NODES_ICEBLOOD_GRAVE:
        case BG_AV_NODES_FROSTWOLF_GRAVE:
        case BG_AV_NODES_FROSTWOLF_HUT:
			for(; itr != m_GraveCreatures[node][teamindex][upgrade_mobs].end(); ++itr)
                SpawnBGCreature(*itr,respawn);
            break;
        case BG_AV_NODES_DUNBALDAR_SOUTH:
        case BG_AV_NODES_DUNBALDAR_NORTH:
        case BG_AV_NODES_ICEWING_BUNKER:
        case BG_AV_NODES_STONEHEART_BUNKER:
        case BG_AV_NODES_ICEBLOOD_TOWER:
        case BG_AV_NODES_TOWER_POINT:
        case BG_AV_NODES_FROSTWOLF_ETOWER:
        case BG_AV_NODES_FROSTWOLF_WTOWER:
            break;

    }

}
void BattleGroundAV::SpawnNodeEnd(BG_AV_Nodes node)
{
	DestroyNode(node);
	uint32 owner = m_Nodes[node].Owner;
	if(IsTower(node))
	{
		SpawnFlammes(node);
		MapDestroyedTower(node);
		UpdateScore(BattleGroundTeamId(GetOtherTeam(owner)), (-1) * BG_AV_RES_TOWER);
		RewardReputationToTeam((owner == ALLIANCE) ? 730 : 729, BG_AV_REP_TOWER, owner);
		RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_TOWER), owner);
		SendYell2ToAll(759, LANG_UNIVERSAL, m_DB_Creature[BG_AV_CREATURE_HERALD], GetNodeName(node), ( owner == ALLIANCE ) ? 757:758);
	}
	else
	{
		MapCaptureGrave(node);
		SendYell2ToAll(761, LANG_UNIVERSAL, m_DB_Creature[BG_AV_CREATURE_HERALD], GetNodeName(node), ( owner == ALLIANCE ) ? 757:758);
		SpawnCapturedGrave(node,owner);
        ModifySpawnMode(node,RESPAWN_IMMEDIATELY,owner);
	}


}


void BattleGroundAV::SpawnForAlliance(BG_AV_Nodes node)
{
	switch(node)
	{
		case 0:
			SpawnNode(1328,1325,BG_AV_OBJECT_DUNB_A_AURA,BG_AV_OBJECT_DUNB_A_B,0);
			break;
		case 1:
			SpawnNode(1336,1333,BG_AV_OBJECT_STORM_A_AURA,BG_AV_OBJECT_STORM_A_B,0);
			break;
		case 2:
			SpawnNode(1303,1302,BG_AV_OBJECT_GIT_A_AURA,BG_AV_OBJECT_GIT_A_B,0);
			break;
		case 3:
			UpdateWorldState(1344,0);
			UpdateWorldState(1341,1);
			SpawnBGObject(26,RESPAWN_IMMEDIATELY);
			SpawnBGObject(28,RESPAWN_IMMEDIATELY);
			SpawnBGObject(32,RESPAWN_IMMEDIATELY);
			SpawnBGObject(36,RESPAWN_IMMEDIATELY);
			SpawnBGObject(40,RESPAWN_IMMEDIATELY);
			SpawnBGObject(45,RESPAWN_IMMEDIATELY);
			break;
		case 4:
			SpawnNode(1349,1346,BG_AV_OBJECT_GSG_A_AURA,BG_AV_OBJECT_GSG_A_B,0);
			break;
		case 5:
			SpawnNode(1340,1337,BG_AV_OBJECT_FSW_A_AURA,BG_AV_OBJECT_FSW_A_B,0);
			break;
		case 6:
			SpawnNode(1332,1329,BG_AV_OBJECT_FSH_A_AURA,BG_AV_OBJECT_FSH_A_B,0);
			break;
		case 7:
			SpawnNode(1378,1361,BG_AV_OBJECT_DBS_A_AURA,BG_AV_OBJECT_DBS_A_T,BG_AV_OBJECT_DBS_A_B);
			break;
		case 8:
			SpawnNode(1379,1362,BG_AV_OBJECT_DBN_A_AURA,BG_AV_OBJECT_DBN_A_B,BG_AV_OBJECT_DBN_A_T);
			break;
		case 9:
			SpawnNode(1380,1363,BG_AV_OBJECT_ADG_A_B,BG_AV_OBJECT_ADG_A_T,BG_AV_OBJECT_ADG_A_AURA);
			break;
		case 10:
			SpawnNode(1381,1364,BG_AV_OBJECT_FGP_A_AURA,BG_AV_OBJECT_FGP_A_B,BG_AV_OBJECT_FGP_A_T);
			break;
		default:
			break;
	}
}

void BattleGroundAV::SpawnForHorde(BG_AV_Nodes node)
{
	switch(node)
	{
		case 0:
			SpawnNode(1326,1327,BG_AV_OBJECT_DUNB_H_AURA,BG_AV_OBJECT_DUNB_H_B,0);
			break;
		case 1:
			SpawnNode(1335,1334,BG_AV_OBJECT_STORM_H_AURA,BG_AV_OBJECT_STORM_H_B,0);
			break;
		case 2:
			SpawnNode(1304,1301,BG_AV_OBJECT_GIT_H_AURA,BG_AV_OBJECT_GIT_H_B,0);
			break;
		case 3:
			UpdateWorldState(1343,0);
			UpdateWorldState(1342,1);
			SpawnBGObject(25,RESPAWN_IMMEDIATELY);
			SpawnBGObject(27,RESPAWN_IMMEDIATELY);
			SpawnBGObject(31,RESPAWN_IMMEDIATELY);
			SpawnBGObject(35,RESPAWN_IMMEDIATELY);
			SpawnBGObject(39,RESPAWN_IMMEDIATELY);
			SpawnBGObject(44,RESPAWN_IMMEDIATELY);
			break;
		case 4:
			SpawnNode(1348,1347,BG_AV_OBJECT_GSG_H_AURA,BG_AV_OBJECT_GSG_H_B,0);
			break;
		case 5:
			SpawnNode(1339,1338,BG_AV_OBJECT_FSW_H_AURA,BG_AV_OBJECT_FSW_H_B,0);
			break;
		case 6:
			SpawnNode(1331,1330,BG_AV_OBJECT_FSH_H_AURA,BG_AV_OBJECT_FSH_H_B,0);
			break;
		case 11:
			SpawnNode(1390,1385,BG_AV_OBJECT_GST_H_AURA,BG_AV_OBJECT_GST_H_B,BG_AV_OBJECT_GST_H_T);
			break;
		case 12:
			SpawnNode(1389,1384,BG_AV_OBJECT_HAL_H_AURA,BG_AV_OBJECT_HAL_H_B,BG_AV_OBJECT_HAL_H_T);
			break;
		case 13:
			SpawnNode(1388,1383,BG_AV_OBJECT_FWE_H_AURA,BG_AV_OBJECT_FWE_H_T,BG_AV_OBJECT_FWE_H_B);
			break;
		case 14:
			SpawnNode(1387,1382,BG_AV_OBJECT_FWO_H_AURA,BG_AV_OBJECT_FWO_H_T,BG_AV_OBJECT_FWO_H_B);
			break;
		default:
			break;
	}
}
void BattleGroundAV::SpawnCapturedGrave(BG_AV_Nodes node, uint32 owner)
{
	if(!node)
		return;
	if(owner == ALLIANCE)
		SpawnForAlliance(node);
	else 
		SpawnForHorde(node);
}

void BattleGroundAV::MapCaptureGrave(BG_AV_Nodes node)
{
	uint32 owner = m_Nodes[node].Owner;

	if(owner == ALLIANCE)
	{
		UpdateWorldState(BG_AV_NodeWorldStates[uint32(node)][1],0);
		UpdateWorldState(BG_AV_NodeWorldStates[uint32(node)][0],1);
	}
	else
	{
		UpdateWorldState(BG_AV_NodeWorldStates[uint32(node)][3],0);
		UpdateWorldState(BG_AV_NodeWorldStates[uint32(node)][2],1);
	}
}

void BattleGroundAV::MapDestroyedTower(BG_AV_Nodes node)
{
	switch(node)
	{
		case BG_AV_NODES_DUNBALDAR_SOUTH:
			UpdateWorldState(1378,0);
			UpdateWorldState(1370,1);
			break;
		case BG_AV_NODES_DUNBALDAR_NORTH:
			UpdateWorldState(1379,0);
			UpdateWorldState(1371,1);
			break;
		case BG_AV_NODES_ICEWING_BUNKER:
			UpdateWorldState(1380,0);
			UpdateWorldState(1372,1);
			break;
		case BG_AV_NODES_STONEHEART_BUNKER:
			UpdateWorldState(1381,0);
			UpdateWorldState(1373,1);
			break;
		case BG_AV_NODES_ICEBLOOD_TOWER:
			UpdateWorldState(1390,0);
			UpdateWorldState(1368,1);
			break;
		case BG_AV_NODES_TOWER_POINT:
			UpdateWorldState(1389,0);
			UpdateWorldState(1367,1);
			break;
		case BG_AV_NODES_FROSTWOLF_ETOWER:
			UpdateWorldState(1388,0);
			UpdateWorldState(1366,1);
			break;
		case BG_AV_NODES_FROSTWOLF_WTOWER:
			UpdateWorldState(1387,0);
			UpdateWorldState(1365,1);
			break;
		default:
				break;
	}

}
void BattleGroundAV::SpawnFlammes(BG_AV_Nodes node)
{
	uint16 i;
		for(i=200+8*uint16(node) ; i < 200+8*(uint16(node))+10 ; ++i)
			SpawnBGObject(i,RESPAWN_IMMEDIATELY);

}
void BattleGroundAV::PopulateNode(BG_AV_Nodes node, Player *plr)
{ 
	uint32 team = plr->GetTeam();
	ASSERT( team == ALLIANCE || team == HORDE);
	uint8 teamindex = GetTeamIndexByTeamId(team);
	if(teamindex == BG_TEAM_ALLIANCE && ((node == BG_AV_NODES_SNOWFALL_GRAVE && BG_CentralGrave_FirstAttack == true) ||(m_Nodes[node].Owner == 0) || (m_Nodes[node].Owner == HORDE && m_Nodes[node].State == POINT_CONTROLLED)))
	{
		switch(node)
		{
			case 0:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_DUNB_NEU_AURA,BG_AV_OBJECT_DUNB_A_B_CONT,0);
				break;
			case 1:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_STORM_NEU_AURA,BG_AV_OBJECT_STORM_A_B_CONT,0);
				break;
			case 2:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_GIT_NEU_AURA,BG_AV_OBJECT_GIT_A_B_CONT,0);
				break;
			case 3:
				UpdateWorldState(1966,0);
				UpdateWorldState(1342,0);
				UpdateWorldState(1343,1);
				SpawnBGObject(30,RESPAWN_IMMEDIATELY);
				SpawnBGObject(24,RESPAWN_IMMEDIATELY);
				SpawnBGObject(34,RESPAWN_IMMEDIATELY);
				SpawnBGObject(38,RESPAWN_IMMEDIATELY);
				SpawnBGObject(42,RESPAWN_IMMEDIATELY);
				SpawnBGObject(47,RESPAWN_IMMEDIATELY);
				break;
			case 4:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_GSG_NEU_AURA,BG_AV_OBJECT_GSG_A_B_CONT,0);
				break;
			case 5:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_FSW_NEU_AURA,BG_AV_OBJECT_FSW_A_B_CONT,0);
				break;
			case 6:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_FSH_NEU_AURA,BG_AV_OBJECT_FSH_A_B_CONT,0);
				break;
			case 11:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_GST_A_B,BG_AV_OBJECT_GST_A_T,BG_AV_OBJECT_GST_N_AURA);
				break;
			case 12:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_HAL_A_B,BG_AV_OBJECT_HAL_A_T,BG_AV_OBJECT_HAL_N_AURA);
				break;
			case 13:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_FWE_A_B,BG_AV_OBJECT_FWE_A_T,BG_AV_OBJECT_FWE_N_AURA);
				break;
			case 14:
				SpawnNode(BG_AV_NodeWorldStates[uint32(node)][2],BG_AV_NodeWorldStates[uint32(node)][1],BG_AV_OBJECT_FWO_A_B,BG_AV_OBJECT_FWO_A_T,BG_AV_OBJECT_FWO_N_AURA);
				break;
			default:
				break;
		}
	}
	else if(teamindex == BG_TEAM_HORDE && ((node == BG_AV_NODES_SNOWFALL_GRAVE && BG_CentralGrave_FirstAttack == true) || (m_Nodes[node].Owner == 0) || (m_Nodes[node].Owner == ALLIANCE && m_Nodes[node].State == POINT_CONTROLLED)))
	{
		switch(node)
		{
			case 0:
				SpawnNode(1325,1328,BG_AV_OBJECT_DUNB_NEU_AURA,BG_AV_OBJECT_DUNB_H_B_CONT,0);
				break;
			case 1:
				SpawnNode(1333,1336,BG_AV_OBJECT_STORM_NEU_AURA,BG_AV_OBJECT_STORM_H_B_CONT,0);
				break;
			case 2:
				SpawnNode(1302,1303,BG_AV_OBJECT_GIT_NEU_AURA,BG_AV_OBJECT_GIT_H_B_CONT,0);
				break;
			case 3:
				UpdateWorldState(1341,0);
				UpdateWorldState(1344,1);
				SpawnBGObject(29,RESPAWN_IMMEDIATELY);
				SpawnBGObject(24,RESPAWN_IMMEDIATELY);
				SpawnBGObject(33,RESPAWN_IMMEDIATELY);
				SpawnBGObject(37,RESPAWN_IMMEDIATELY);
				SpawnBGObject(41,RESPAWN_IMMEDIATELY);
				SpawnBGObject(46,RESPAWN_IMMEDIATELY);
				break;
			case 4:
				SpawnNode(1346,1349,BG_AV_OBJECT_GSG_NEU_AURA,BG_AV_OBJECT_GSG_H_B_CONT,0);
				break;
			case 5:
				SpawnNode(1337,1340,BG_AV_OBJECT_FSW_NEU_AURA,BG_AV_OBJECT_FSW_H_B_CONT,0);
				break;
			case 6:
				SpawnNode(1329,1332,BG_AV_OBJECT_FSH_NEU_AURA,BG_AV_OBJECT_FSH_H_B_CONT,0);
				break;
			case 7:
				SpawnNode(1361,1378,BG_AV_OBJECT_DBS_H_B,BG_AV_OBJECT_DBS_H_T,BG_AV_OBJECT_DBS_N_AURA);
				break;
			case 8:
				SpawnNode(1362,1379,BG_AV_OBJECT_DBN_H_B,BG_AV_OBJECT_DBN_H_T,BG_AV_OBJECT_DBN_N_AURA);
				break;
			case 9:
				SpawnNode(1363,1380,BG_AV_OBJECT_ADG_H_B,BG_AV_OBJECT_ADG_H_T,BG_AV_OBJECT_ADG_N_AURA);
				break;
			case 10:
				SpawnNode(1364,1381,BG_AV_OBJECT_FGP_H_B,BG_AV_OBJECT_FGP_H_T,BG_AV_OBJECT_FGP_N_AURA);
				break;
			default:
				break;
		}
	}
	else if(teamindex == BG_TEAM_HORDE && (m_Nodes[node].Owner == ALLIANCE) && m_Nodes[node].State == POINT_ASSAULTED)
		SpawnForHorde(node);
	else if(teamindex == BG_TEAM_ALLIANCE && m_Nodes[node].Owner == HORDE && m_Nodes[node].State == POINT_ASSAULTED)
		SpawnForAlliance(node);
}

WorldSafeLocsEntry const* BattleGroundAV::GetClosestGraveYard(Player *plr)
{
    float x = plr->GetPositionX();
    float y = plr->GetPositionY();
    uint32 team = plr->GetTeam();
    WorldSafeLocsEntry const* good_entry = NULL;
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // Is there any occupied node for this team?
        float mindist = 9999999.0f;
        for(uint8 i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_FROSTWOLF_HUT; ++i)
        {
            if (i == m_assault_in_progress)
                continue;
            if (m_Nodes[i].Owner != team || m_Nodes[i].State != POINT_CONTROLLED)
                continue;
            WorldSafeLocsEntry const * entry = sWorldSafeLocsStore.LookupEntry( BG_AV_GraveyardIds[i] );
            if (!entry)
                continue;
            float dist = (entry->x - x) * (entry->x - x) + (entry->y - y) * (entry->y - y);
            if (mindist > dist)
            {
                mindist = dist;
                good_entry = entry;
            }
        }
    }
    // If not, place ghost in the starting-cave
    if (!good_entry)
        good_entry = sWorldSafeLocsStore.LookupEntry( BG_AV_GraveyardIds[GetTeamIndexByTeamId(team) + 7] );

    return good_entry;
}


void BattleGroundAV::SpawnBGCreature(uint64 const& guid, uint32 respawntime)
{
	Creature* obj = ObjectAccessor::GetCreatureInWorld(guid);

    if (!obj)
        return;
    Map * map = MapManager::Instance().FindMap(GetMapId(),GetInstanceID());
    if (!map)
        return;
    if (respawntime == 0)
    {
        obj->SetPhaseMask(PHASEMASK_NORMAL,false);
        obj->setDeathState(ALIVE);
        obj->Respawn();
        map->Add(obj);
    }
    else
    {
        obj->SetPhaseMask(0,false);
        map->Add(obj);
		obj->ForcedDespawn();
    }
}

void BattleGroundAV::ChangeMineOwner(uint8 mine, uint32 team)
{
    // TODO implement quest 7122
    // mine=0 northmine, mine=1 southmine
    ASSERT(mine == BG_AV_NORTH_MINE || mine == BG_AV_SOUTH_MINE);
    if (m_Mine_Owner[mine] == team)
        return;

    if (team != ALLIANCE && team != HORDE)
        team = BG_AV_NEUTRAL_TEAM;

    m_Mine_PrevOwner[mine] = m_Mine_Owner[mine];
    m_Mine_Owner[mine] = team;
    uint32 index;

    SendMineWorldStates(mine);

    sLog.outDebug("BattleGroundAV: depopulating mine %i (0=north, 1=south)",mine);
    index = (m_Mine_PrevOwner[mine] == ALLIANCE) ? 0 : (m_Mine_PrevOwner[mine] == HORDE) ? 1 : 2;
	if (!m_MineCreatures[mine][index].empty())
        for(BGCreatures::const_iterator itr = m_MineCreatures[mine][index].begin(); itr != m_MineCreatures[mine][index].end(); ++itr)
            SpawnBGCreature(*itr, RESPAWN_ONE_DAY);

    sLog.outDebug("BattleGroundAV: populating mine %i owner %i, prevowner %i",mine,m_Mine_Owner[mine], m_Mine_PrevOwner[mine]);
    index = (m_Mine_Owner[mine] == ALLIANCE)?0:(m_Mine_Owner[mine] == HORDE)?1:2;

    // TODO : fixer les bornes de l'intervalle de pop des mobs
    if (!m_MineCreatures[mine][index].empty())
        for(BGCreatures::const_iterator itr = m_MineCreatures[mine][index].begin(); itr != m_MineCreatures[mine][index].end(); ++itr)
            SpawnBGCreature(*itr, RESPAWN_IMMEDIATELY);

    if (team == ALLIANCE || team == HORDE)
    {
        PlaySoundToAll((team == ALLIANCE) ? BG_AV_SOUND_ALLIANCE_GOOD : BG_AV_SOUND_HORDE_GOOD);
        m_Mine_Reclaim_Timer[mine] = BG_AV_MINE_RECLAIM_TIMER;
        SendYell2ToAll(765 , LANG_UNIVERSAL, m_DB_Creature[BG_AV_CREATURE_HERALD], (team == ALLIANCE ) ? 757 : 758, (mine == BG_AV_NORTH_MINE) ? 766 : 767);
    }
	SendMineWorldStates(BG_AV_NORTH_MINE);
	SendMineWorldStates(BG_AV_SOUTH_MINE);
}




const BG_AV_Nodes BattleGroundAV::GetNodeThroughCreaturePosition(Creature * cr)
{
    for(uint8 i=0; i<=BG_AV_NODES_MAX; ++i)
    {
		if (cr->IsWithinDist2d(BG_AV_Nodes_Coords[i][0], BG_AV_Nodes_Coords[i][1], BG_AV_MAX_NODE_DISTANCE))
            return BG_AV_Nodes(i);
    }
    return BG_AV_NODES_ERROR;
}

void BattleGroundAV::OnObjectDBLoad(Creature* creature)
{
    // it's important that creature which should be spawned at the beginning are
    // spawned here, cause they only get loaded if a player enters the grid..
    // and we can't tell when this happens (could even be after the beginning)
    uint8 graveDefenderType = 0;
    uint8 node_int = uint32(GetNodeThroughCreaturePosition(creature));
    switch(creature->GetEntry())
    {
        case BG_AV_CREATURE_ENTRY_H_BOSS:
            m_DB_Creature[BG_AV_CREATURE_H_BOSS] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_A_BOSS:
            m_DB_Creature[BG_AV_CREATURE_A_BOSS] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_N_HERALD:
            m_DB_Creature[BG_AV_CREATURE_HERALD] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_A_MARSHAL_SOUTH:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 0] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_A_MARSHAL_NORTH:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 1] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_A_MARSHAL_ICE:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 2] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_A_MARSHAL_STONE:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 3] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_H_MARSHAL_ICE:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 4] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_H_MARSHAL_TOWER:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 5] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_H_MARSHAL_ETOWER:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 6] = creature->GetGUID();
            break;
        case BG_AV_CREATURE_ENTRY_H_MARSHAL_WTOWER:
            m_DB_Creature[BG_AV_CREATURE_MARSHAL + 7] = creature->GetGUID();
            break;

        case BG_AV_NORTH_MINE_ALLIANCE_1:
        case BG_AV_NORTH_MINE_ALLIANCE_2:
        case BG_AV_NORTH_MINE_ALLIANCE_3:
        case BG_AV_NORTH_MINE_ALLIANCE_4:
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_MineCreatures[BG_AV_NORTH_MINE][0].push_back(creature->GetGUID());
            break;
        case BG_AV_NORTH_MINE_HORDE_1:
        case BG_AV_NORTH_MINE_HORDE_2:
        case BG_AV_NORTH_MINE_HORDE_3:
        case BG_AV_NORTH_MINE_HORDE_4:
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_MineCreatures[BG_AV_NORTH_MINE][1].push_back(creature->GetGUID());
            break;
        case BG_AV_NORTH_MINE_NEUTRAL_1:
        case BG_AV_NORTH_MINE_NEUTRAL_2:
        case BG_AV_NORTH_MINE_NEUTRAL_3:
        case BG_AV_NORTH_MINE_NEUTRAL_4:
            m_MineCreatures[BG_AV_NORTH_MINE][2].push_back(creature->GetGUID());
            break;
        case BG_AV_SOUTH_MINE_ALLIANCE_1:
        case BG_AV_SOUTH_MINE_ALLIANCE_2:
        case BG_AV_SOUTH_MINE_ALLIANCE_3:
        case BG_AV_SOUTH_MINE_ALLIANCE_4:
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_MineCreatures[BG_AV_SOUTH_MINE][0].push_back(creature->GetGUID());
            break;
        case BG_AV_SOUTH_MINE_HORDE_1:
        case BG_AV_SOUTH_MINE_HORDE_2:
        case BG_AV_SOUTH_MINE_HORDE_3:
        case BG_AV_SOUTH_MINE_HORDE_4:
            m_MineCreatures[BG_AV_SOUTH_MINE][1].push_back(creature->GetGUID());
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            break;
        case BG_AV_SOUTH_MINE_NEUTRAL_1:
        case BG_AV_SOUTH_MINE_NEUTRAL_2:
        case BG_AV_SOUTH_MINE_NEUTRAL_3:
        case BG_AV_SOUTH_MINE_NEUTRAL_4:
        case BG_AV_SOUTH_MINE_NEUTRAL_5:
            if (creature->GetEntry() == BG_AV_SOUTH_MINE_NEUTRAL_4)
                m_DB_Creature[BG_AV_CREATURE_SNIFFLE] = creature->GetGUID();
            m_MineCreatures[BG_AV_SOUTH_MINE][2].push_back(creature->GetGUID());
            break;
            // Alliance
        case BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_4:
             SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
             m_GraveCreatures[node_int][BG_TEAM_ALLIANCE][3].push_back(creature->GetGUID());
			break;
        case BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_3:
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_GraveCreatures[node_int][BG_TEAM_ALLIANCE][2].push_back(creature->GetGUID());
			break;
        case BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_2:
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_GraveCreatures[node_int][BG_TEAM_ALLIANCE][1].push_back(creature->GetGUID());
			break;
        case BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_1:
			if(node_int > 2)
                SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_GraveCreatures[node_int][BG_TEAM_ALLIANCE][0].push_back(creature->GetGUID());
			break;
            //Horde
        case BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_4:
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_GraveCreatures[node_int][BG_TEAM_HORDE][3].push_back(creature->GetGUID());
			break;
        case BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_3:
            SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_GraveCreatures[node_int][BG_TEAM_HORDE][2].push_back(creature->GetGUID());
			break;
        case BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_2:
                SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_GraveCreatures[node_int][BG_TEAM_HORDE][1].push_back(creature->GetGUID());
			break;
        case BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_1:
            if (node_int < 4 || node_int > 6)
                SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
            m_GraveCreatures[node_int][BG_TEAM_HORDE][0].push_back(creature->GetGUID());
			break;
        default:
            break;
    }


    uint32 level = creature->getLevel();
    if (level != 0)
        level += GetMaxLevel() - 60;                        // maybe we can do this more generic for custom level - range.. actually it's ok
    creature->SetLevel(level);
}