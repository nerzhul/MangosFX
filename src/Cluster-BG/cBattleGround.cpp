#include <ObjectMgr.h>
#include <WorldPacket.h>
#include <BattleGround.h>
#include "cBattleGround.h"
#include "cBattleGroundMgr.h"

namespace ClusterFX
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
}                                                           // namespace ClusterFX

void cBattleGround::SendWarningToAll(int32 entry, ...)
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
    for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player *plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
            if (plr->GetSession())
                plr->GetSession()->SendPacket(&data);
}

void cBattleGround::SendWarningToAll(std::string msg)
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
    for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player *plr = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(itr->first, 0, HIGHGUID_PLAYER)))
            if (plr->GetSession())
                plr->GetSession()->SendPacket(&data);
}

cBattleGround::cBattleGround(): m_Id(0)
{
	m_TypeID            = BattleGroundTypeId(0);
    m_RandomTypeID      = BattleGroundTypeId(0);

	m_Status            = STATUS_NONE;

	m_ArenaType         = 0;
	m_StartTime         = 0;
	m_EndTime           = 0;

	m_IsArena           = false;
	m_IsRated           = false;

	m_ArenaTeamIds[BG_TEAM_ALLIANCE]   = 0;
    m_ArenaTeamIds[BG_TEAM_HORDE]      = 0;

	m_ArenaTeamRatingChanges[BG_TEAM_ALLIANCE]   = 0;
    m_ArenaTeamRatingChanges[BG_TEAM_HORDE]      = 0;

	m_PlayersCount[BG_TEAM_ALLIANCE] = 0;
	m_PlayersCount[BG_TEAM_HORDE] = 0;
	
	m_LevelMin          = 0;
    m_LevelMax          = 0;

	m_MaxPlayersPerTeam = 0;
    m_MaxPlayers        = 0;
    m_MinPlayersPerTeam = 0;
    m_MinPlayers        = 0;

	m_TeamStartLocX[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocX[BG_TEAM_HORDE]      = 0;

    m_TeamStartLocY[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocY[BG_TEAM_HORDE]      = 0;

    m_TeamStartLocZ[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocZ[BG_TEAM_HORDE]      = 0;

    m_TeamStartLocO[BG_TEAM_ALLIANCE]   = 0;
    m_TeamStartLocO[BG_TEAM_HORDE]      = 0;


	m_Players.clear();
}

cBattleGround::~cBattleGround()
{
}

void cBattleGround::Update(uint32 diff)
{
}

void cBattleGround::SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O)
{
    BattleGroundTeamId idx = GetTeamIndexByTeamId(TeamID);
    m_TeamStartLocX[idx] = X;
    m_TeamStartLocY[idx] = Y;
    m_TeamStartLocZ[idx] = Z;
    m_TeamStartLocO[idx] = O;
}

void cBattleGround::SendPacketToAll(WorldPacket *packet)
{
	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
        /*Player *plr = sObjectMgr.GetPlayer(/*itr->first*itr);
        if (plr)
            plr->GetSession()->SendPacket(packet);
        else
            sLog.outError("BattleGround:SendPacketToAll: Player (GUID: %u) not found!", GUID_LOPART(/*itr->first*itr));*/
    }

}

void cBattleGround::SendPacketToTeam(uint32 TeamID, WorldPacket *packet, Player *sender, bool self)
{
	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
        /*Player *plr = sObjectMgr.GetPlayer(/*itr->firs/*itr);
        if (!plr)
        {
            sLog.outError("BattleGround:SendPacketToTeam: Player (GUID: %u) not found!", GUID_LOPART(/*itr->first*itr));
            continue;
        }

        if (!self && sender == plr)
            continue;

		// todo: handle team
        uint32 team = GetPlayerTeam(*itr)/*itr->second.Team;
        if(!team) team = plr->GetTeam();

        if (team == TeamID)
            plr->GetSession()->SendPacket(packet);*/
    }
}

void cBattleGround::PlaySoundToAll(uint32 SoundID)
{
	WorldPacket data;
    sClusterBGMgr.BuildPlaySoundPacket(&data, SoundID);
    SendPacketToAll(&data);
}

void cBattleGround::PlaySoundToTeam(uint32 SoundID, uint32 TeamID)
{
	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
        /*Player *plr = sObjectMgr.GetPlayer(/*itr->first*itr);

        if (!plr)
        {
            sLog.outError("BattleGround:PlaySoundToTeam: Player (GUID: %u) not found!", GUID_LOPART(/*itr->first*itr));
            continue;
        }

		// TODO : utiliser la retransmission directe via le cluster
        uint32 team = GetPlayerTeam(*itr)/*itr->second.Team;
        if(!team) team = plr->GetTeam();

        if (team == TeamID)
        {
            sClusterBGMgr.BuildPlaySoundPacket(&data, SoundID);
            plr->GetSession()->SendPacket(&data);
        }*/
    }
}

void cBattleGround::CastSpellOnTeam(uint32 SpellID, uint32 TeamID)
{
	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
        /*Player *plr = sObjectMgr.GetPlayer(/*itr->first*itr);

        if (!plr)
        {
            sLog.outError("BattleGround:CastSpellOnTeam: Player (GUID: %u) not found!", GUID_LOPART(/*itr->first**itr));
            continue;
        }

        uint32 team = GetPlayerTeam(*itr)/*itr->second.Team;
        if(!team) team = plr->GetTeam();

        if (team == TeamID)
            plr->CastSpell(plr, SpellID, true);*/
    }
}

void cBattleGround::RewardHonorToTeam(uint32 Honor, uint32 TeamID)
{
	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
        /*Player *plr = sObjectMgr.GetPlayer(/*itr->first*itr);

        if (!plr)
        {
            sLog.outError("BattleGround:RewardHonorToTeam: Player (GUID: %u) not found!", GUID_LOPART(/*itr->first*itr));
            continue;
        }

        uint32 team = GetPlayerTeam(*itr)/*itr->second.Team;
        if(!team) team = plr->GetTeam();

        if (team == TeamID)
            UpdatePlayerScore(plr, SCORE_BONUS_HONOR, Honor);*/
    }
}

void cBattleGround::RewardHonorTeamDaily(uint32 WinningTeamID)
{
	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
	{
		if (itr->second.OfflineRemoveTime)
			continue;
		
		/*Player *plr = sObjectMgr.GetPlayer(/*itr->first*itr);
		
		if (!plr)
			continue;
		
		uint32 team = GetPlayerTeam(*itr)/*itr->second.Team;
		if(!team) 
			team = plr->GetTeam();
		plr->RewardHonorEndBattlegroud(team == WinningTeamID);*/
	}
}

void cBattleGround::RewardReputationToTeam(uint32 faction_id, uint32 Reputation, uint32 TeamID)
{
	FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

    if (!factionEntry)
        return;

    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
        /*Player *plr = sObjectMgr.GetPlayer(/*itr->first*itr);

        if (!plr)
        {
            sLog.outError("BattleGround:RewardReputationToTeam: Player (GUID: %u) not found!", GUID_LOPART(/*itr->first*itr));
            continue;
        }

        uint32 team = GetPlayerTeam(*itr)/*itr->second.Team;
        if(!team) team = plr->GetTeam();

        if (team == TeamID)
            plr->GetReputationMgr().ModifyReputation(factionEntry, Reputation);*/
    }
}

void cBattleGround::RewardXpToTeam(uint32 Xp, float percentOfLevel, uint32 TeamID)
{
	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
        /*Player *plr = sObjectMgr.GetPlayer(/*itr->first*itr);

        if (!plr)
        {
            sLog.outError("BattleGround:RewardXpToTeam: Player (GUID: %u) not found!", GUID_LOPART(/*itr->first*itr));
            continue;
        }

        uint32 team = GetPlayerTeam(*itr)/*itr->second.Team;
        if(!team) team = plr->GetTeam();

        if (team == TeamID)
        {
            uint32 gain = Xp;
            if(gain == 0 && percentOfLevel != 0)
            {
                percentOfLevel = percentOfLevel / 100;
                gain = uint32(float(plr->GetUInt32Value(PLAYER_NEXT_LEVEL_XP))*percentOfLevel);
            }
            plr->GiveXP(gain, NULL);
        }*/
    }
}

void cBattleGround::UpdateWorldState(uint32 Field, uint32 Value,Player *Source)
{
	WorldPacket data;
    sClusterBGMgr.BuildUpdateWorldStatePacket(&data, Field, Value);
	/*if(Source)
		Source->GetSession()->SendPacket(&data);
	else*/
		SendPacketToAll(&data);
}

void cBattleGround::EndBattleGround(uint32 winner)
{
}

uint32 cBattleGround::GetBonusHonorFromKill(uint32 kills) const
{
    //variable kills means how many honorable kills you scored (so we need kills * honor_for_one_kill)
    return ClusterFX::Honor::hk_honor_at_level(GetMaxLevel(), kills);
}

uint32 cBattleGround::GetBattlemasterEntry() const
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

void cBattleGround::RewardSpellCast(Player *plr, uint32 spell_id)
{
	// 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    /*if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spell_id);
    if(!spellInfo)
    {
        sLog.outError("Battleground reward casting spell %u not exist.",spell_id);
        return;
    }

    plr->CastSpell(plr, spellInfo, true);*/
}

void cBattleGround::RewardItem(Player *plr, uint32 item_id, uint32 count)
{
	// 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    /*if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
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
        SendRewardMarkByMail(plr,item_id,no_space_count);*/
}

void cBattleGround::SendRewardMarkByMail(Player *plr,uint32 mark, uint32 count)
{
    uint32 bmEntry = GetBattlemasterEntry();
    if (!bmEntry)
        return;

    /*ItemPrototype const* markProto = ObjectMgr::GetItemPrototype(mark);
    if (!markProto)
        return;

    if (Item* markItem = Item::CreateItem(mark,count,plr))
    {
        // save new item before send
        markItem->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted

        // subject: item name
        std::string subject = markProto->Name1;
        int loc_idx = plr->GetSession()->GetSessionDbLocaleIndex();
        if (loc_idx >= 0 )
            if (ItemLocale const *il = sObjectMgr.GetItemLocale(markProto->ItemId))
                if (il->Name.size() > size_t(loc_idx) && !il->Name[loc_idx].empty())
                    subject = il->Name[loc_idx];

        // text
        std::string textFormat = plr->GetSession()->GetMangosString(LANG_BG_MARK_BY_MAIL);
        char textBuf[300];
        snprintf(textBuf,300,textFormat.c_str(),GetName(),GetName());

        MailDraft(subject, textBuf)
            .AddItem(markItem)
            .SendMailTo(plr, MailSender(MAIL_CREATURE, bmEntry));
    }*/
}

void cBattleGround::RewardQuestComplete(Player *plr)
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

void cBattleGround::BlockMovement(Player *plr)
{
    //plr->SetClientControl(plr, 0);                          // movement disabled NOTE: the effect will be automatically removed by client when the player is teleported from the battleground, so no need to send with uint8(1) in RemovePlayerAtLeave()
}

void cBattleGround::RemovePlayerAtLeave(uint64 guid, bool Transport, bool SendPacket)
{
	uint32 team = GetPlayerTeam(guid);
	bool participant = IsPlayerInBattleGround(guid);

	BattleGroundPlayerMap::iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
    {
        UpdatePlayersCountByTeam(team, true);               // -1 player
        m_Players.erase(itr);
	}

	BattleGroundScoreMap::iterator itr2 = m_PlayerScores.find(guid);
    if (itr2 != m_PlayerScores.end())
    {
        delete itr2->second;                                // delete player's score
        m_PlayerScores.erase(itr2);
    }

    /*Player *plr = sObjectMgr.GetPlayer(guid);

    // should remove spirit of redemption
    if (plr && plr->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        plr->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

    if(plr && !plr->isAlive())                              // resurrect on exit
    {
        plr->ResurrectPlayer(1.0f);
        plr->SpawnCorpseBones();
    }*/

    //RemovePlayer(plr, guid);                                // BG subclass specific code

    if(participant) // if the player was a match participant, remove auras, calc rating, update queue
    {
        BattleGroundTypeId bgTypeId = GetTypeID();
        BattleGroundQueueTypeId bgQueueTypeId = sClusterBGMgr.BGQueueTypeId(GetTypeID(), GetArenaType());
        /*if (plr)
        {
            plr->ClearAfkReports();

            if(!team) team = plr->GetTeam();

			plr->RemoveAurasDueToSpell(SPELL_AURA_PVP_HEALING); 

            // if arena, remove the specific arena auras
            if (isArena())
            {
                plr->RemoveArenaAuras(true);                // removes debuffs / dots etc., we don't want the player to die after porting out
                bgTypeId=BATTLEGROUND_AA;                   // set the bg type to all arenas (it will be used for queue refreshing)

                // unsummon current and summon old pet if there was one and there isn't a current pet
                plr->RemovePet(NULL, PET_SAVE_NOT_IN_SLOT);
                plr->ResummonPetTemporaryUnSummonedIfAny();

                if (isRated() && GetStatus() == STATUS_IN_PROGRESS)
                {
                    //left a rated match while the encounter was in progress, consider as loser
                    ArenaTeam * winner_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
                    ArenaTeam * loser_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(team));
                    if (winner_arena_team && loser_arena_team)
                        loser_arena_team->MemberLost(plr,winner_arena_team->GetRating());
                }
            }
            if (SendPacket)
            {
                WorldPacket data;
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, plr->GetBattleGroundQueueIndex(bgQueueTypeId), STATUS_NONE, 0, 0, 0);
                plr->GetSession()->SendPacket(&data);
            }

            // this call is important, because player, when joins to battleground, this method is not called, so it must be called when leaving bg
            plr->RemoveBattleGroundQueueId(bgQueueTypeId);
        }
        else
        // removing offline participant
        {
            if (isRated() && GetStatus() == STATUS_IN_PROGRESS)
            {
                //left a rated match while the encounter was in progress, consider as loser
                ArenaTeam * others_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
                ArenaTeam * players_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(team));
                if (others_arena_team && players_arena_team)
                    players_arena_team->OfflineMemberLost(guid, others_arena_team->GetRating());
            }
        }*/

        // remove from raid group if player is member
        /*if (Group *group = GetBgRaid(team))
        {
            if( !group->RemoveMember(guid, 0) )             // group was disbanded
            {
                SetBgRaid(team, NULL);
                delete group;
            }
        }*/
        //DecreaseInvitedCount(team);
        //we should update battleground queue, but only if bg isn't ending
        if (isBattleGround() && GetStatus() < STATUS_WAIT_LEAVE)
        {
            // a player has left the battleground, so there are free slots -> add to queue
            AddToBGFreeSlotQueue();
            //sBattleGroundMgr.ScheduleQueueUpdate(0, 0, bgQueueTypeId, bgTypeId, GetBracketId());
        }

        // Let others know
        WorldPacket data;
        sClusterBGMgr.BuildPlayerLeftBattleGroundPacket(&data, guid);
        //SendPacketToTeam(team, &data, plr, false);
    }

    /*if (plr)
    {
        // Do next only if found in battleground
        plr->SetBattleGroundId(0, BATTLEGROUND_TYPE_NONE);  // We're not in BG.
        // reset destination bg team
        plr->SetBGTeam(0);

        if (Transport)
            plr->TeleportToBGEntryPoint();

        sLog.outDetail("BATTLEGROUND: Removed player %s from BattleGround.", plr->GetName());
    }*/

    //battleground object will be deleted next BattleGround::Update() call
}

void cBattleGround::Reset()
{
	SetWinner(WINNER_NONE);
    SetStatus(STATUS_WAIT_QUEUE);
    SetStartTime(0);
    SetEndTime(0);
    SetArenaType(0);
    SetRated(false);

    //m_Events = 0;

    // door-event2 is always 0
    //m_ActiveEvents[BG_EVENT_DOOR] = 0;
    if (isArena())
    {
        //m_ActiveEvents[ARENA_BUFF_EVENT] = BG_EVENT_NONE;
        //m_ArenaBuffSpawned = false;
    }

    /*if (m_InvitedAlliance > 0 || m_InvitedHorde > 0)
        sLog.outError("BattleGround system: bad counter, m_InvitedAlliance: %d, m_InvitedHorde: %d", m_InvitedAlliance, m_InvitedHorde);*/

    /*m_InvitedAlliance = 0;
    m_InvitedHorde = 0;
    m_InBGFreeSlotQueue = false;*/

    // need do the same
	m_Players.clear();

    for(BattleGroundScoreMap::const_iterator itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
        delete itr->second;
    m_PlayerScores.clear();
}

void cBattleGround::StartBattleGround()
{
}

void cBattleGround::AddPlayer(Player *plr)
{
}

void cBattleGround::AddOrSetPlayerToCorrectBgGroup(Player *plr, uint64 plr_guid, uint32 team)
{
}

void cBattleGround::EventPlayerLoggedIn(Player* player, uint64 plr_guid)
{
}

void cBattleGround::EventPlayerLoggedOut(Player* player)
{
}

void cBattleGround::AddToBGFreeSlotQueue()
{
}

void cBattleGround::RemoveFromBGFreeSlotQueue()
{
}

uint32 cBattleGround::GetFreeSlotsForTeam(uint32 Team) const
{
	return 0;
}

bool cBattleGround::HasFreeSlots()
{
	return GetPlayersSize() < GetMaxPlayers();
}

void cBattleGround::UpdatePlayerScore(Player *Source, uint32 type, uint32 value)
{
}

uint32 BattleGround::GetPlayerScore(Player *Source, uint32 type)
{
	return 0;
}

uint32 cBattleGround::GetDamageDoneForTeam(uint32 TeamID)
{
	return 0;
}

bool cBattleGround::AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime)
{
	return true;
}

void cBattleGround::DoorClose(uint64 const& guid)
{
}

void BattleGround::DoorOpen(uint64 const& guid)
{
}

void cBattleGround::OnObjectDBLoad(Creature* creature)
{
}

uint64 cBattleGround::GetSingleCreatureGuid(uint8 event1, uint8 event2)
{
	return 0;
}

void cBattleGround::OnObjectDBLoad(GameObject* obj)
{
}

bool cBattleGround::IsDoor(uint8 event1, uint8 event2)
{
	return true;
}

void cBattleGround::OpenDoorEvent(uint8 event1, uint8 event2 /*=0*/)
{
}

void cBattleGround::SpawnEvent(uint8 event1, uint8 event2, bool spawn)
{
}

void cBattleGround::SpawnBGObject(uint64 const& guid, uint32 respawntime)
{
}

void cBattleGround::SpawnBGCreature(uint64 const& guid, uint32 respawntime)
{
}

bool cBattleGround::DelObject(uint32 type)
{
	return true;
}

void cBattleGround::SendMessageToAll(int32 entry, ChatMsg type, Player const* source)
{
}

void cBattleGround::SendYellToAll(int32 entry, uint32 language, uint64 const& guid)
{
}

void cBattleGround::PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...)
{
}

void cBattleGround::SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 arg1, int32 arg2)
{
}

void cBattleGround::SendYell2ToAll(int32 entry, uint32 language, uint64 const& guid, int32 arg1, int32 arg2)
{
}

void cBattleGround::EndNow()
{
}

void cBattleGround::HandleTriggerBuff(uint64 const& go_guid)
{
}

void cBattleGround::HandleKillPlayer(Player *player, Player *killer)
{
}

uint32 cBattleGround::GetPlayerTeam(uint64 guid)
{
    BattleGroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr!=m_Players.end())
        return itr->second.Team;
    return 0;
}

bool cBattleGround::IsPlayerInBattleGround(uint64 guid)
{
    BattleGroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return true;
    return false;
}

void cBattleGround::PlayerAddedToBGCheckIfBGIsRunning(Player* plr)
{
}

uint32 cBattleGround::GetAlivePlayersCountByTeam(uint32 Team)
{
	return 0;
}

void cBattleGround::CheckArenaWinConditions()
{
}

void cBattleGround::SetBgRaid( uint32 TeamID, Group *bg_raid )
{
}

WorldSafeLocsEntry const* BattleGround::GetClosestGraveYard( Player* player )
{
	return NULL;
}

bool cBattleGround::IsTeamScoreInRange(uint32 team, uint32 minScore, uint32 maxScore) const
{
    return true;
}

void cBattleGround::SetBracket( PvPDifficultyEntry const* bracketEntry )
{
}

void cBattleGround::UpdateArenaWorldState()
{
}

GameObject* cBattleGround::GetBGObject(uint32 type)
{
    return NULL;
}

bool cBattleGround::DelCreature(uint32 type)
{
    return true;
}

Creature* cBattleGround::AddCreature(uint32 entry, uint32 type, uint32 teamval, float x, float y, float z, float o, uint32 respawntime)
{
    return  NULL;
}

bool cBattleGround::AddSpiritGuide(uint32 type, float x, float y, float z, float o, uint32 team)
{
    return true;
}

Creature* cBattleGround::GetBGCreature(uint32 type)
{
    return NULL;
}

void cBattleGround::RewardAchievementToPlayer(Player* plr, uint32 entry)
{
}

void cBattleGround::RewardAchievementToTeam(uint32 team, uint32 entry)
{
}

// Cluster special functions

std::vector<uint64> cBattleGround::getPlayerList()
{
	std::vector<uint64> players;
	players.clear();

	for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;
		players.push_back(itr->first);
	}
	return players;
}

uint32 cBattleGround::GetPlayerOfflineTime(uint64 guid)
{
	BattleGroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return uint32(itr->second.OfflineRemoveTime);
    return 0;
}



void cBattleGround::SetPlayerValues(uint64 guid, uint32 offlineTime, uint32 team)
{
	if(!IsPlayerInBattleGround(guid))
		UpdatePlayersCountByTeam(team,false);

	cBattleGroundPlayer bp;
    bp.OfflineRemoveTime = offlineTime;
    bp.Team = team;

	m_Players[guid] = bp;
}



