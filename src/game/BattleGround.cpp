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
#include "BattleGroundMgr.h"
#include "Creature.h"
#include "MapManager.h"
#include "Language.h"
#include "SpellAuras.h"
#include "ArenaTeam.h"
#include "World.h"
#include "Group.h"
#include "ObjectDefines.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "Util.h"
#include "Formulas.h"
#include "GridNotifiersImpl.h"
#include "cSocketTCP.h"
#include "cClusterMgr.h"
#include "cPacketOpcodes.h"

namespace MaNGOS
{
    class BattleGroundChatBuilder
    {
        public:
            BattleGroundChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, va_list* args = NULL)
                : i_msgtype(msgtype), i_textId(textId), i_source(source), i_args(args) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);

                if (i_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap,*i_args);

                    char str [2048];
                    vsnprintf(str,2048,text, ap );
                    va_end(ap);

                    do_helper(data,&str[0]);
                }
                else
                    do_helper(data,text);
            }
        private:
            void do_helper(WorldPacket& data, char const* text)
            {
                uint64 target_guid = i_source  ? i_source ->GetGUID() : 0;

                data << uint8(i_msgtype);
                data << uint32(LANG_UNIVERSAL);
                data << uint64(target_guid);                // there 0 for BG messages
                data << uint32(0);                          // can be chat msg group or something
                data << uint64(target_guid);
                data << uint32(strlen(text)+1);
                data << text;
                data << uint8(i_source ? i_source->chatTag() : uint8(0));
            }

            ChatMsg i_msgtype;
            int32 i_textId;
            Player const* i_source;
            va_list* i_args;
    };

    class BattleGroundYellBuilder
    {
        public:
            BattleGroundYellBuilder(uint32 language, int32 textId, Creature const* source, va_list* args = NULL)
                : i_language(language), i_textId(textId), i_source(source), i_args(args) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);

                if(i_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap,*i_args);

                    char str [2048];
                    vsnprintf(str,2048,text, ap );
                    va_end(ap);

                    do_helper(data,&str[0]);
                }
                else
                    do_helper(data,text);
            }
        private:
            void do_helper(WorldPacket& data, char const* text)
            {
                //copyied from BuildMonsterChat
                data << (uint8)CHAT_MSG_MONSTER_YELL;
                data << (uint32)i_language;
                data << (uint64)i_source->GetGUID();
                data << (uint32)0;                                     //2.1.0
                data << (uint32)(strlen(i_source->GetName())+1);
                data << i_source->GetName();
                data << (uint64)0;                            //Unit Target - isn't important for bgs
                data << (uint32)strlen(text)+1;
                data << text;
                data << (uint8)0;                                      // ChatTag - for bgs allways 0?
            }

            uint32 i_language;
            int32 i_textId;
            Creature const* i_source;
            va_list* i_args;
    };


    class BattleGround2ChatBuilder
    {
        public:
            BattleGround2ChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, int32 arg1, int32 arg2)
                : i_msgtype(msgtype), i_textId(textId), i_source(source), i_arg1(arg1), i_arg2(arg2) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);
                char const* arg1str = i_arg1 ? sObjectMgr.GetMangosString(i_arg1,loc_idx) : "";
                char const* arg2str = i_arg2 ? sObjectMgr.GetMangosString(i_arg2,loc_idx) : "";

                char str [2048];
                snprintf(str,2048,text, arg1str, arg2str );

                uint64 target_guid = i_source  ? i_source ->GetGUID() : 0;

                data << uint8(i_msgtype);
                data << uint32(LANG_UNIVERSAL);
                data << uint64(target_guid);                // there 0 for BG messages
                data << uint32(0);                          // can be chat msg group or something
                data << uint64(target_guid);
                data << uint32(strlen(str)+1);
                data << str;
                data << uint8(i_source ? i_source->chatTag() : uint8(0));
            }
        private:

            ChatMsg i_msgtype;
            int32 i_textId;
            Player const* i_source;
            int32 i_arg1;
            int32 i_arg2;
    };

    class BattleGround2YellBuilder
    {
        public:
            BattleGround2YellBuilder(uint32 language, int32 textId, Creature const* source, int32 arg1, int32 arg2)
                : i_language(language), i_textId(textId), i_source(source), i_arg1(arg1), i_arg2(arg2) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);
                char const* arg1str = i_arg1 ? sObjectMgr.GetMangosString(i_arg1,loc_idx) : "";
                char const* arg2str = i_arg2 ? sObjectMgr.GetMangosString(i_arg2,loc_idx) : "";

                char str [2048];
                snprintf(str,2048,text, arg1str, arg2str );
                //copyied from BuildMonsterChat
                data << (uint8)CHAT_MSG_MONSTER_YELL;
                data << (uint32)i_language;
                data << (uint64)i_source->GetGUID();
                data << (uint32)0;                                     //2.1.0
                data << (uint32)(strlen(i_source->GetName())+1);
                data << i_source->GetName();
                data << (uint64)0;                            //Unit Target - isn't important for bgs
                data << (uint32)strlen(str)+1;
                data << str;
                data << (uint8)0;                                      // ChatTag - for bgs allways 0?
            }
        private:

            uint32 i_language;
            int32 i_textId;
            Creature const* i_source;
            int32 i_arg1;
            int32 i_arg2;
    };
}                                                           // namespace MaNGOS

template<class Do>
void BattleGround::BroadcastWorker(Do& _do)
{
    /*for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player *plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
            _do(plr);*/
}

BattleGround::BattleGround()
{
    m_Map               = NULL;

    m_BgRaids[BG_TEAM_ALLIANCE]         = NULL;
    m_BgRaids[BG_TEAM_HORDE]            = NULL;

	m_Id = 0;
	Packet pck;
	pck << uint16(C_CMSG_GET_BG_ID);
	m_Id = sClusterMgr.getUint64Value(&pck,C_BG);
	sLog.outBasic("BattleGround Id set to %u",GUID_LOPART(m_Id));

}

BattleGround::~BattleGround()
{
    if (GetInstanceID())                                    // not spam by useless queries in case BG templates
    {
        // delete creature and go respawn times
        WorldDatabase.PExecute("DELETE FROM creature_respawn WHERE instance = '%u'",GetInstanceID());
        WorldDatabase.PExecute("DELETE FROM gameobject_respawn WHERE instance = '%u'",GetInstanceID());
        // delete instance from db
        CharacterDatabase.PExecute("DELETE FROM instance WHERE id = '%u'",GetInstanceID());
        // remove from battlegrounds
    }

    // unload map
    // map can be null at bg destruction
    if (m_Map)
        m_Map->SetUnload();
}

void BattleGround::Update(uint32 diff)
{
}

void BattleGround::SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O)
{
	Packet pck;
	pck << uint16(C_CMSG_SET_TEAM_START_LOC);
	pck << uint64(m_Id) << uint32(TeamID) << float(X) << float(Y) << float(Z) << float(O);
	sClusterMgr.getNullValue(&pck,C_BG);
    /*BattleGroundTeamId idx = GetTeamIndexByTeamId(TeamID);
    m_TeamStartLocX[idx] = X;
    m_TeamStartLocY[idx] = Y;
    m_TeamStartLocZ[idx] = Z;
    m_TeamStartLocO[idx] = O;*/
}

void BattleGround::SendPacketToAll(WorldPacket *packet)
{
}

void BattleGround::SendPacketToTeam(uint32 TeamID, WorldPacket *packet, Player *sender, bool self)
{
}

void BattleGround::PlaySoundToAll(uint32 SoundID)
{
    WorldPacket data;
    sBattleGroundMgr.BuildPlaySoundPacket(&data, SoundID);
    SendPacketToAll(&data);
}

void BattleGround::PlaySoundToTeam(uint32 SoundID, uint32 TeamID)
{
}

void BattleGround::CastSpellOnTeam(uint32 SpellID, uint32 TeamID)
{
}

void BattleGround::RewardHonorToTeam(uint32 Honor, uint32 TeamID)
{
}

void BattleGround::RewardHonorTeamDaily(uint32 WinningTeamID)
{
}

void Player::RewardHonorEndBattlegroud(bool win)
{
	uint32 hk = 0;
	uint32 guid = GetGUIDLow();
	if(!win)
		hk = 5;
	else
	{
		if(HasDoneRandomBattleGround())
			hk = 15;
		else
		{
			hk = 30;
			SetRandomBGDone(true);
			CharacterDatabase.PExecute("INSERT INTO character_battleground_status VALUES (%u, %u)", guid, uint64(time(NULL)));
			ModifyArenaPoints(25);
		}
	}
	
	if(hk)
		RewardHonor(NULL, 1, MaNGOS::Honor::hk_honor_at_level(getLevel(),hk));
		
}

void BattleGround::RewardReputationToTeam(uint32 faction_id, uint32 Reputation, uint32 TeamID)
{
}

void BattleGround::RewardXpToTeam(uint32 Xp, float percentOfLevel, uint32 TeamID)
{
}

void BattleGround::UpdateWorldState(uint32 Field, uint32 Value,Player *Source)
{
    WorldPacket data;
    sBattleGroundMgr.BuildUpdateWorldStatePacket(&data, Field, Value);
	if(Source)
		Source->GetSession()->SendPacket(&data);
	else
		SendPacketToAll(&data);
}

void BattleGround::EndBattleGround(uint32 winner)
{
    RemoveFromBGFreeSlotQueue();

    ArenaTeam * winner_arena_team = NULL;
    ArenaTeam * loser_arena_team = NULL;
    uint32 loser_rating = 0;
    uint32 winner_rating = 0;
    WorldPacket data;
    int32 winmsg_id = 0;

    SetStatus(STATUS_WAIT_LEAVE);

    // arena rating calculation
    if (winmsg_id)
        SendMessageToAll(winmsg_id, CHAT_MSG_BG_SYSTEM_NEUTRAL);
}

uint32 BattleGround::GetBonusHonorFromKill(uint32 kills) const
{
    //variable kills means how many honorable kills you scored (so we need kills * honor_for_one_kill)
    return MaNGOS::Honor::hk_honor_at_level(GetMaxLevel(), kills);
}

uint32 BattleGround::GetBattlemasterEntry() const
{
    switch(GetTypeID(true))
    {
        case BATTLEGROUND_AV: return 15972;
        case BATTLEGROUND_WS: return 14623;
        case BATTLEGROUND_AB: return 14879;
        case BATTLEGROUND_EY: return 22516;
        case BATTLEGROUND_NA: return 20200;
        default:              return 0;
    }
}

void BattleGround::RewardMark(Player *plr,uint32 count)
{
}

void BattleGround::RewardSpellCast(Player *plr, uint32 spell_id)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spell_id);
    if(!spellInfo)
    {
        sLog.outError("Battleground reward casting spell %u not exist.",spell_id);
        return;
    }

    plr->CastSpell(plr, spellInfo, true);
}

void BattleGround::RewardItem(Player *plr, uint32 item_id, uint32 count)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    ItemPosCountVec dest;
    uint32 no_space_count = 0;
    uint8 msg = plr->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, item_id, count, &no_space_count );

    if( msg == EQUIP_ERR_ITEM_NOT_FOUND)
    {
        sLog.outErrorDb("Battleground reward item (Entry %u) not exist in `item_template`.",item_id);
        return;
    }

    if( msg != EQUIP_ERR_OK )                               // convert to possible store amount
        count -= no_space_count;

    if( count != 0 && !dest.empty())                        // can add some
        if (Item* item = plr->StoreNewItem( dest, item_id, true, 0))
            plr->SendNewItem(item,count,true,false);

    if (no_space_count > 0)
        SendRewardMarkByMail(plr,item_id,no_space_count);
}

void BattleGround::SendRewardMarkByMail(Player *plr,uint32 mark, uint32 count)
{
}

void BattleGround::RewardQuestComplete(Player *plr)
{
    uint32 quest;
    switch(GetTypeID(true))
    {
        case BATTLEGROUND_AV:
            quest = SPELL_AV_QUEST_REWARD;
            break;
        case BATTLEGROUND_WS:
            quest = SPELL_WS_QUEST_REWARD;
            break;
        case BATTLEGROUND_AB:
            quest = SPELL_AB_QUEST_REWARD;
            break;
        case BATTLEGROUND_EY:
            quest = SPELL_EY_QUEST_REWARD;
            break;
        default:
            return;
    }

    RewardSpellCast(plr, quest);
}

void BattleGround::BlockMovement(Player *plr)
{
    plr->SetClientControl(plr, 0);                          // movement disabled NOTE: the effect will be automatically removed by client when the player is teleported from the battleground, so no need to send with uint8(1) in RemovePlayerAtLeave()
}

void BattleGround::RemovePlayerAtLeave(uint64 guid, bool Transport, bool SendPacket)
{
    uint32 team = GetPlayerTeam(guid);
	bool participant = IsPlayerInBattleGround(guid);
	
    // Remove from lists/maps
	Packet pck;
	pck << uint16(C_CMSG_BG_RM_PLR_LEAVE) << uint64(m_Id) << uint64(guid);
	sClusterMgr.getNullValue(&pck,C_BG);
    /* TODO: call this function
	BattleGroundPlayerMap::iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
    {
        UpdatePlayersCountByTeam(team, true);               // -1 player
        m_Players.erase(itr);
        // check if the player was a participant of the match, or only entered through gm command (goname)
        participant = true;
    }*/

    BattleGroundScoreMap::iterator itr2 = m_PlayerScores.find(guid);
    if (itr2 != m_PlayerScores.end())
    {
        delete itr2->second;                                // delete player's score
        m_PlayerScores.erase(itr2);
    }

    Player *plr = sObjectMgr.GetPlayer(guid);

    // should remove spirit of redemption
    if (plr && plr->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        plr->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

    if(plr && !plr->isAlive())                              // resurrect on exit
    {
        plr->ResurrectPlayer(1.0f);
        plr->SpawnCorpseBones();
    }

    RemovePlayer(plr, guid);                                // BG subclass specific code

    if (plr)
    {
        // Do next only if found in battleground
        plr->SetBattleGroundId(0, BATTLEGROUND_TYPE_NONE);  // We're not in BG.
        // reset destination bg team
        plr->SetBGTeam(0);

        if (Transport)
            plr->TeleportToBGEntryPoint();

        sLog.outDetail("BATTLEGROUND: Removed player %s from BattleGround.", plr->GetName());
    }

    //battleground object will be deleted next BattleGround::Update() call
}

// this method is called when no players remains in battleground
void BattleGround::Reset()
{
}

void BattleGround::StartBattleGround()
{
}

void BattleGround::AddPlayer(Player *plr)
{
    // remove afk from player
    if (plr->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK))
        plr->ToggleAFK();

    // score struct must be created in inherited class

    uint64 guid = plr->GetGUID();
    uint32 team = plr->GetBGTeam();

    /*BattleGroundPlayer bp;
    bp.OfflineRemoveTime = 0;
    bp.Team = team;*/

    // Add to list/maps
	Packet pkt;
	pkt << uint16(C_CMSG_BG_REG_PLAYER);
	pkt << uint64(m_Id) << uint64(guid) << uint32(0) << uint32(team);
	sClusterMgr.getNullValue(&pkt,C_BG);

    //m_Players[guid] = bp;

    //UpdatePlayersCountByTeam(team, false);                  // +1 player

    WorldPacket data;
    sBattleGroundMgr.BuildPlayerJoinedBattleGroundPacket(&data, plr);
    SendPacketToTeam(team, &data, plr, false);

	plr->CastSpell(plr, SPELL_AURA_PVP_HEALING,true); 
	plr->Unmount();
	plr->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

    // add arena specific auras

    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());

    // setup BG group membership
    PlayerAddedToBGCheckIfBGIsRunning(plr);
    AddOrSetPlayerToCorrectBgGroup(plr, guid, team);

    // Log
    sLog.outDetail("BATTLEGROUND: Player %s joined the battle.", plr->GetName());
}

/* this method adds player to his team's bg group, or sets his correct group if player is already in bg group */
void BattleGround::AddOrSetPlayerToCorrectBgGroup(Player *plr, uint64 plr_guid, uint32 team)
{
    Group* group = GetBgRaid(team);
    if(!group)                                      // first player joined
    {
        group = new Group;
        SetBgRaid(team, group);
        group->Create(plr_guid, plr->GetName());
    }
    else                                            // raid already exist
    {
        if (group->IsMember(plr_guid))
        {
            uint8 subgroup = group->GetMemberGroup(plr_guid);
            plr->SetBattleGroundRaid(group, subgroup);
        }
        else
        {
            group->AddMember(plr_guid, plr->GetName());
            if (Group* originalGroup = plr->GetOriginalGroup())
                if (originalGroup->IsLeader(plr_guid))
                    group->ChangeLeader(plr_guid);
        }
    }
}

// This method should be called when player logs into running battleground
void BattleGround::EventPlayerLoggedIn(Player* player, uint64 plr_guid)
{
    // player is correct pointer
    for(std::deque<uint64>::iterator itr = m_OfflineQueue.begin(); itr != m_OfflineQueue.end(); ++itr)
    {
        if (*itr == plr_guid)
        {
            m_OfflineQueue.erase(itr);
            break;
        }
    }

	Packet pkt;
	pkt << uint16(C_CMSG_BG_REG_PLAYER);
	pkt << uint64(m_Id) << uint64(plr_guid) << uint32(0) << uint32(player->GetBGTeam());
	sClusterMgr.getNullValue(&pkt,C_BG);
    //m_Players[plr_guid].OfflineRemoveTime = 0;
    PlayerAddedToBGCheckIfBGIsRunning(player);
    // if battleground is starting, then add preparation aura
    // we don't have to do that, because preparation aura isn't removed when player logs out
}

// This method should be called when player logs out from running battleground
void BattleGround::EventPlayerLoggedOut(Player* player)
{
    // player is correct pointer, it is checked in WorldSession::LogoutPlayer()
    m_OfflineQueue.push_back(player->GetGUID());

	Packet pkt;
	pkt << uint16(C_CMSG_BG_REG_PLAYER);
	pkt << uint64(m_Id) << uint64(player->GetGUID()) << uint32(sWorld.GetGameTime() + MAX_OFFLINE_TIME) << uint32(player->GetBGTeam());
	sClusterMgr.getNullValue(&pkt,C_BG);

    //m_Players[player->GetGUID()].OfflineRemoveTime = sWorld.GetGameTime() + MAX_OFFLINE_TIME;
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // drop flag and handle other cleanups
        RemovePlayer(player, player->GetGUID());
    }
}

/* This method should be called only once ... it adds pointer to queue */
void BattleGround::AddToBGFreeSlotQueue()
{
}

/* This method removes this battleground from free queue - it must be called when deleting battleground - not used now*/
void BattleGround::RemoveFromBGFreeSlotQueue()
{
}

// get the number of free slots for team
// returns the number how many players can join battleground to MaxPlayersPerTeam
uint32 BattleGround::GetFreeSlotsForTeam(uint32 Team) const
{
	return 0;
}

bool BattleGround::HasFreeSlots()
{
	Packet pck;
	pck << uint16(C_CMSG_BG_HAS_FREE_SLOTS) << uint64(m_Id);
	return sClusterMgr.getBoolValue(&pck,C_BG);
}

void BattleGround::UpdatePlayerScore(Player *Source, uint32 type, uint32 value)
{
    //this procedure is called from virtual function implemented in bg subclass
    BattleGroundScoreMap::const_iterator itr = m_PlayerScores.find(Source->GetGUID());

    if(itr == m_PlayerScores.end())                         // player not found...
        return;

    switch(type)
    {
        case SCORE_KILLING_BLOWS:                           // Killing blows
            itr->second->KillingBlows += value;
            break;
        case SCORE_DEATHS:                                  // Deaths
            itr->second->Deaths += value;
            break;
        case SCORE_HONORABLE_KILLS:                         // Honorable kills
            itr->second->HonorableKills += value;
			if(itr->second->HonorableKills >= 0)
				RewardAchievementToPlayer(Source,229);
            break;
        case SCORE_BONUS_HONOR:                             // Honor bonus
            break;
            //used only in EY, but in MSG_PVP_LOG_DATA opcode
        case SCORE_DAMAGE_DONE:                             // Damage Done
            itr->second->DamageDone += value;
            break;
        case SCORE_HEALING_DONE:                            // Healing Done
            itr->second->HealingDone += value;
            break;
        default:
            sLog.outError("BattleGround: Unknown player score type %u", type);
            break;
    }
}

uint32 BattleGround::GetPlayerScore(Player *Source, uint32 type)
{
    BattleGroundScoreMap::const_iterator itr = m_PlayerScores.find(Source->GetGUID());

    if(itr == m_PlayerScores.end()) // player not found...
        return 0;

    switch(type)
    {
        case SCORE_KILLING_BLOWS: // Killing blows
            return itr->second->KillingBlows;
        case SCORE_DEATHS: // Deaths
            return itr->second->Deaths;
        case SCORE_DAMAGE_DONE: // Damage Done
            return itr->second->DamageDone;
        case SCORE_HEALING_DONE: // Healing Done
            return itr->second->HealingDone;
        default:
            sLog.outError("BattleGround: Unknown player score type %u", type);
            return 0;
    }
}

uint32 BattleGround::GetDamageDoneForTeam(uint32 TeamID)
{
	uint32 finaldamage = 0;
	return finaldamage;
}

bool BattleGround::AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime)
{
    // must be created this way, adding to godatamap would add it to the base map of the instance
    // and when loading it (in go::LoadFromDB()), a new guid would be assigned to the object, and a new object would be created
    // so we must create it specific for this instance
    GameObject * go = new GameObject;
    if(!go->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT),entry, GetBgMap(),
        PHASEMASK_NORMAL, x,y,z,o,rotation0,rotation1,rotation2,rotation3,100,GO_STATE_READY))
    {
        sLog.outErrorDb("Gameobject template %u not found in database! BattleGround not created!", entry);
        sLog.outError("Cannot create gameobject template %u! BattleGround not created!", entry);
        delete go;
        return false;
    }
/*
    uint32 guid = go->GetGUIDLow();

    // without this, UseButtonOrDoor caused the crash, since it tried to get go info from godata
    // iirc that was changed, so adding to go data map is no longer required if that was the only function using godata from GameObject without checking if it existed
    GameObjectData& data = sObjectMgr.NewGOData(guid);

    data.id             = entry;
    data.mapid          = GetMapId();
    data.posX           = x;
    data.posY           = y;
    data.posZ           = z;
    data.orientation    = o;
    data.rotation0      = rotation0;
    data.rotation1      = rotation1;
    data.rotation2      = rotation2;
    data.rotation3      = rotation3;
    data.spawntimesecs  = respawnTime;
    data.spawnMask      = 1;
    data.animprogress   = 100;
    data.go_state       = 1;
*/
    // add to world, so it can be later looked up from HashMapHolder
    go->AddToWorld();
    m_BgObjects[type] = go->GetGUID();
    return true;
}

//some doors aren't despawned so we cannot handle their closing in gameobject::update()
//it would be nice to correctly implement GO_ACTIVATED state and open/close doors in gameobject code
void BattleGround::DoorClose(uint64 const& guid)
{
    GameObject *obj = GetBgMap()->GetGameObject(guid);
    if (obj)
    {
        //if doors are open, close it
        if (obj->getLootState() == GO_ACTIVATED && obj->GetGoState() != GO_STATE_READY)
        {
            //change state to allow door to be closed
            obj->SetLootState(GO_READY);
            obj->UseDoorOrButton(RESPAWN_ONE_DAY);
        }
    }
    else
    {
        sLog.outError("BattleGround: Door object not found (cannot close doors)");
    }
}

void BattleGround::DoorOpen(uint64 const& guid)
{
    GameObject *obj = GetBgMap()->GetGameObject(guid);
    if (obj)
    {
        //change state to be sure they will be opened
        obj->SetLootState(GO_READY);
        obj->UseDoorOrButton(RESPAWN_ONE_DAY);
    }
    else
    {
        sLog.outError("BattleGround: Door object not found! - doors will be closed.");
    }
}

void BattleGround::OnObjectDBLoad(Creature* creature)
{
    const BattleGroundEventIdx eventId = sBattleGroundMgr.GetCreatureEventIndex(creature->GetDBTableGUIDLow());
    if (eventId.event1 == BG_EVENT_NONE)
        return;
    m_EventObjects[MAKE_PAIR32(eventId.event1, eventId.event2)].creatures.push_back(creature->GetGUID());
    if (!IsActiveEvent(eventId.event1, eventId.event2))
        SpawnBGCreature(creature->GetGUID(), RESPAWN_ONE_DAY);
}

uint64 BattleGround::GetSingleCreatureGuid(uint8 event1, uint8 event2)
{
    BGCreatures::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.begin();
    if (itr != m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.end())
        return *itr;
    return 0;
}

void BattleGround::OnObjectDBLoad(GameObject* obj)
{
    const BattleGroundEventIdx eventId = sBattleGroundMgr.GetGameObjectEventIndex(obj->GetDBTableGUIDLow());
    if (eventId.event1 == BG_EVENT_NONE)
        return;
    m_EventObjects[MAKE_PAIR32(eventId.event1, eventId.event2)].gameobjects.push_back(obj->GetGUID());
    if (!IsActiveEvent(eventId.event1, eventId.event2))
    {
        SpawnBGObject(obj->GetGUID(), RESPAWN_ONE_DAY);
    }
    else
    {
        // it's possible, that doors aren't spawned anymore (wsg)
        if (GetStatus() >= STATUS_IN_PROGRESS && IsDoor(eventId.event1, eventId.event2))
            DoorOpen(obj->GetGUID());
    }
}

bool BattleGround::IsDoor(uint8 event1, uint8 event2)
{
    if (event1 == BG_EVENT_DOOR)
    {
        if (event2 > 0)
        {
            sLog.outError("BattleGround too high event2 for event1:%i", event1);
            return false;
        }
        return true;
    }
    return false;
}

void BattleGround::OpenDoorEvent(uint8 event1, uint8 event2 /*=0*/)
{
    if (!IsDoor(event1, event2))
    {
        sLog.outError("BattleGround:OpenDoorEvent this is no door event1:%u event2:%u", event1, event2);
        return;
    }
    if (!IsActiveEvent(event1, event2))                 // maybe already despawned (eye)
    {
        sLog.outError("BattleGround:OpenDoorEvent this event isn't active event1:%u event2:%u", event1, event2);
        return;
    }
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr)
        DoorOpen(*itr);
}

void BattleGround::SpawnEvent(uint8 event1, uint8 event2, bool spawn)
{
    // stop if we want to spawn something which was already spawned
    // or despawn something which was already despawned
    if (event2 == BG_EVENT_NONE || (spawn && m_ActiveEvents[event1] == event2)
        || (!spawn && m_ActiveEvents[event1] != event2))
        return;

    if (spawn)
    {
        // if event gets spawned, the current active event mus get despawned
        SpawnEvent(event1, m_ActiveEvents[event1], false);
        m_ActiveEvents[event1] = event2;                    // set this event to active
    }
    else
        m_ActiveEvents[event1] = BG_EVENT_NONE;             // no event active if event2 gets despawned

    BGCreatures::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.end(); ++itr)
        SpawnBGCreature(*itr, (spawn) ? RESPAWN_IMMEDIATELY : RESPAWN_ONE_DAY);
    BGObjects::const_iterator itr2 = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr2 != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr2)
        SpawnBGObject(*itr2, (spawn) ? RESPAWN_IMMEDIATELY : RESPAWN_ONE_DAY);
}

void BattleGround::SpawnBGObject(uint64 const& guid, uint32 respawntime)
{
    Map* map = GetBgMap();

    GameObject *obj = map->GetGameObject(guid);
    if(!obj)
        return;
    if (respawntime == 0)
    {
        //we need to change state from GO_JUST_DEACTIVATED to GO_READY in case battleground is starting again
        if (obj->getLootState() == GO_JUST_DEACTIVATED)
            obj->SetLootState(GO_READY);
        obj->SetRespawnTime(0);
        map->Add(obj);
    }
    else
    {
        map->Add(obj);
        obj->SetRespawnTime(respawntime);
        obj->SetLootState(GO_JUST_DEACTIVATED);
    }
}

void BattleGround::SpawnBGCreature(uint64 const& guid, uint32 respawntime)
{
    Map* map = GetBgMap();

    Creature* obj = map->GetCreature(guid);
    if (!obj)
        return;
    if (respawntime == 0)
    {
        obj->Respawn();
        map->Add(obj);
    }
    else
    {
        map->Add(obj);
        obj->setDeathState(JUST_DIED);
        obj->SetRespawnDelay(respawntime);
        obj->RemoveCorpse();
    }
}

bool BattleGround::DelObject(uint32 type)
{
    if (!m_BgObjects[type])
        return true;

    GameObject *obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if (!obj)
    {
        sLog.outError("Can't find gobject guid: %u",GUID_LOPART(m_BgObjects[type]));
        return false;
    }

    obj->SetRespawnTime(0);                                 // not save respawn time
    obj->Delete();
    m_BgObjects[type] = 0;
    return true;
}

void BattleGround::SendMessageToAll(int32 entry, ChatMsg type, Player const* source)
{
    MaNGOS::BattleGroundChatBuilder bg_builder(type, entry, source);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::SendYellToAll(int32 entry, uint32 language, uint64 const& guid)
{
    Creature* source = GetBgMap()->GetCreature(guid);
    if(!source)
        return;
    MaNGOS::BattleGroundYellBuilder bg_builder(language, entry, source);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGroundYellBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...)
{
    va_list ap;
    va_start(ap, source);

    MaNGOS::BattleGroundChatBuilder bg_builder(type, entry, source, &ap);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);

    va_end(ap);
}

void BattleGround::SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 arg1, int32 arg2)
{
    MaNGOS::BattleGround2ChatBuilder bg_builder(type, entry, source, arg1, arg2);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGround2ChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::SendYell2ToAll(int32 entry, uint32 language, uint64 const& guid, int32 arg1, int32 arg2)
{
    Creature* source = GetBgMap()->GetCreature(guid);
    if(!source)
        return;
    MaNGOS::BattleGround2YellBuilder bg_builder(language, entry, source, arg1, arg2);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGround2YellBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::EndNow()
{
}

/*
important notice:
buffs aren't spawned/despawned when players captures anything
buffs are in their positions when battleground starts
*/
void BattleGround::HandleTriggerBuff(uint64 const& go_guid)
{
    GameObject *obj = GetBgMap()->GetGameObject(go_guid);
    if (!obj || obj->GetGoType() != GAMEOBJECT_TYPE_TRAP || !obj->isSpawned())
        return;

    // static buffs are already handled just by database and don't need
    // battleground code
    if (!m_BuffChange)
    {
        obj->SetLootState(GO_JUST_DEACTIVATED);             // can be despawned or destroyed
        return;
    }

    // change buff type, when buff is used:
    // TODO this can be done when poolsystem works for instances
    int32 index = m_BgObjects.size() - 1;
    while (index >= 0 && m_BgObjects[index] != go_guid)
        index--;
    if (index < 0)
    {
        sLog.outError("BattleGround (Type: %u) has buff gameobject (Guid: %u Entry: %u Type:%u) but it hasn't that object in its internal data",GetTypeID(),GUID_LOPART(go_guid),obj->GetEntry(),obj->GetGoType());
        return;
    }

    //randomly select new buff
    uint8 buff = urand(0, 2);
    uint32 entry = obj->GetEntry();
    if (m_BuffChange && entry != Buff_Entries[buff])
    {
        //despawn current buff
        SpawnBGObject(m_BgObjects[index], RESPAWN_ONE_DAY);
        //set index for new one
        for (uint8 currBuffTypeIndex = 0; currBuffTypeIndex < 3; ++currBuffTypeIndex)
        {
            if (entry == Buff_Entries[currBuffTypeIndex])
            {
                index -= currBuffTypeIndex;
                index += buff;
            }
        }
    }

    SpawnBGObject(m_BgObjects[index], BUFF_RESPAWN_TIME);
}

void BattleGround::HandleKillPlayer( Player *player, Player *killer )
{
}

// return the player's team based on battlegroundplayer info
// used in same faction arena matches mainly
uint32 BattleGround::GetPlayerTeam(uint64 guid)
{
	Packet pkt;
	pkt << uint16(C_CMSG_GET_BG_TEAM) << uint64(m_Id) << uint64(guid);
	return sClusterMgr.getUint32Value(&pkt,C_BG);

    /*BattleGroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr!=m_Players.end())
        return itr->second.Team;
    return 0;*/
}

bool BattleGround::IsPlayerInBattleGround(uint64 guid)
{
	Packet pkt;
	pkt << uint16(C_CMSG_IS_IN_BG) << uint64(m_Id) << uint64(guid);
	return sClusterMgr.getBoolValue(&pkt,C_BG);

    /*BattleGroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return true;
    return false;*/
}

void BattleGround::PlayerAddedToBGCheckIfBGIsRunning(Player* plr)
{
    BlockMovement(plr);
	WorldPacket data(0);
    sBattleGroundMgr.BuildPvpLogDataPacket(&data, this);
    plr->GetSession()->SendPacket(&data);

}

uint32 BattleGround::GetAlivePlayersCountByTeam(uint32 Team)
{
    int count = 0;
    return count;
}

void BattleGround::CheckArenaWinConditions()
{
	Packet pck;
	pck << uint16(C_GET_PL_NB_TEAM) << uint64(m_Id) << uint32(ALLIANCE);
	uint32 pAcount = sClusterMgr.getUint32Value(&pck,C_BG);
	Packet pck2;
	pck2 << uint16(C_GET_PL_NB_TEAM) << uint64(m_Id) << uint32(HORDE);
	uint32 pHcount = sClusterMgr.getUint32Value(&pck2,C_BG);
    if (!pAcount && pHcount)
	{
		RewardAchievementToTeam(HORDE,397);
        EndBattleGround(HORDE);
	}
    else if (pAcount && !GetAlivePlayersCountByTeam(HORDE))
	{
        EndBattleGround(ALLIANCE);
		RewardAchievementToTeam(ALLIANCE,397);
	}
}

void BattleGround::SetBgRaid( uint32 TeamID, Group *bg_raid )
{
    Group* &old_raid = TeamID == ALLIANCE ? m_BgRaids[BG_TEAM_ALLIANCE] : m_BgRaids[BG_TEAM_HORDE];
    if(old_raid) old_raid->SetBattlegroundGroup(NULL);
    if(bg_raid) bg_raid->SetBattlegroundGroup(this);
    old_raid = bg_raid;
}

WorldSafeLocsEntry const* BattleGround::GetClosestGraveYard( Player* player )
{
    return sObjectMgr.GetClosestGraveYard( player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetTeam() );
}

bool BattleGround::IsTeamScoreInRange(uint32 team, uint32 minScore, uint32 maxScore) const
{
    BattleGroundTeamId team_idx = GetTeamIndexByTeamId(team);
    uint32 score = (m_TeamScores[team_idx] < 0) ? 0 : uint32(m_TeamScores[team_idx]);
    return score >= minScore && score <= maxScore;
}

void BattleGround::SetBracket( PvPDifficultyEntry const* bracketEntry )
{
}

void BattleGround::UpdateArenaWorldState()
{
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
}

GameObject* BattleGround::GetBGObject(uint32 type)
{
    GameObject *obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if(!obj)
        sLog.outError("couldn't get gameobject %i",type);
    return obj;
}

bool BattleGround::DelCreature(uint32 type)
{
    if (!m_BgCreatures[type])
        return true;

    Creature *cr = GetBgMap()->GetCreature(m_BgCreatures[type]);
    if (!cr)
    {
        sLog.outError("Can't find creature guid: %u",GUID_LOPART(m_BgCreatures[type]));
        return false;
    }
    cr->AddObjectToRemoveList();
    m_BgCreatures[type] = 0;
    return true;
}

Creature* BattleGround::AddCreature(uint32 entry, uint32 type, uint32 teamval, float x, float y, float z, float o, uint32 respawntime)
{
    Map* map = GetBgMap();
    if (!map)
        return NULL;

    Creature* pCreature = new Creature;
    if (!pCreature->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), map, PHASEMASK_NORMAL, entry, teamval, 0))
    {
        sLog.outError("Can't create creature entry: %u",entry);
        delete pCreature;
        return NULL;
    }

	pCreature->Relocate(x, y, z, o);
    pCreature->SetSummonPoint(x, y, z, o);
    //pCreature->SetHomePosition(x, y, z, o);

    //pCreature->SetDungeonDifficulty(0);

    map->Add(pCreature);
    m_BgCreatures[type] = pCreature->GetGUID();

    return  pCreature;
}

bool BattleGround::AddSpiritGuide(uint32 type, float x, float y, float z, float o, uint32 team)
{
    uint32 entry = 0;

    if (team == ALLIANCE)
        entry = 13116;
    else
        entry = 13117;

    Creature* pCreature = AddCreature(entry,type,team,x,y,z,o);
    if (!pCreature)
    {
        sLog.outError("Can't create Spirit guide. BattleGround not created!");
        EndNow();
        return false;
    }

    pCreature->setDeathState(DEAD);

    pCreature->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, pCreature->GetGUID());
    // aura
    //TODO: Fix display here
    //pCreature->SetVisibleAura(0, SPELL_SPIRIT_HEAL_CHANNEL);
    // casting visual effect
    pCreature->SetUInt32Value(UNIT_CHANNEL_SPELL, 22011);
    // correct cast speed
    pCreature->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);

    //pCreature->CastSpell(pCreature, SPELL_SPIRIT_HEAL_CHANNEL, true);

    return true;
}

Creature* BattleGround::GetBGCreature(uint32 type)
{
    Creature *creature = GetBgMap()->GetCreature(m_BgCreatures[type]);
    if(!creature)
        sLog.outError("couldn't get creature %i",type);
    return creature;
}

void BattleGround::SendWarningToAll(int32 entry, ...)
{
    const char *format = sObjectMgr.GetMangosStringForDBCLocale(entry);
    va_list ap;
    char str [1024];
    va_start(ap, entry);
    vsnprintf(str,1024,format, ap);
    va_end(ap);
    std::string msg = (std::string)str;

    WorldPacket data(SMSG_MESSAGECHAT, 200);

    data << (uint8)CHAT_MSG_RAID_BOSS_EMOTE;
    data << (uint32)LANG_UNIVERSAL;
    data << (uint64)0;
    data << (uint32)0;                                     // 2.1.0
    data << (uint32)1;
    data << (uint8)0; 
    data << (uint64)0;
    data << (uint32)(strlen(msg.c_str())+1);
    data << msg.c_str();
    data << (uint8)0;
    /*for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player *plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
            if (plr->GetSession())
                plr->GetSession()->SendPacket(&data);*/
}

void BattleGround::SendWarningToAll(std::string msg)
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);

    data << (uint8)CHAT_MSG_RAID_BOSS_EMOTE;
    data << (uint32)LANG_UNIVERSAL;
    data << (uint64)0;
    data << (uint32)0;                                     // 2.1.0
    data << (uint32)1;
    data << (uint8)0; 
    data << (uint64)0;
    data << (uint32)(strlen(msg.c_str())+1);
    data << msg.c_str();
    data << (uint8)0;
    /*for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player *plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
            if (plr->GetSession())
                plr->GetSession()->SendPacket(&data);*/
}

void BattleGround::RewardAchievementToPlayer(Player* plr, uint32 entry)
{
	if(!plr)
		return;

	AchievementEntry const* pAE = GetAchievementStore()->LookupEntry(entry);
	if (!pAE)
    {
        sLog.outError("DoCompleteAchievement called for not existing achievement %u", entry);
        return;
    }
	
	plr->GetAchievementMgr().DoCompleteAchivement(pAE);
}

void BattleGround::RewardAchievementToTeam(uint32 team, uint32 entry)
{
}