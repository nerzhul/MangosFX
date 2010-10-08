#ifndef __C_BATTLEGROUND_H_
#define __C_BATTLEGROUND_H_

#include <Common.h>
#include <BattleGround.h>

namespace ClusterFX
{
    namespace Honor
    {
        inline uint32 hk_honor_at_level(uint32 level, uint32 count=1)
        {
            return (uint32)ceil(count*(-0.53177f + 0.59357f * exp((level +23.54042f) / 23.07859f )));
        }
    }
}

struct cBattleGroundPlayer
{
    time_t  OfflineRemoveTime;                              // for tracking and removing offline players from queue after 5 minutes
    uint32  Team;                                           // Player's team
};

class cBattleGround
{
	public:
		cBattleGround();
		~cBattleGround();

		virtual void Update(uint32 diff);                   // must be implemented in BG subclass of BG specific update code, but must in begginning call parent version
        virtual bool SetupBattleGround()                    // must be implemented in BG subclass
        {
            return true;
        }
        virtual void Reset();                               // resets all common properties for battlegrounds, must be implemented and called in BG subclass
        virtual void StartingEventCloseDoors() {}
        virtual void StartingEventOpenDoors() {}
		virtual void OnCreatureCreate(Creature* /*cr*/) {}
		virtual void OnGameObjectCreate(GameObject* /*go*/) {}
		virtual void EventPlayerDamageGO(Player* /*player*/, GameObject* /*target_obj*/, uint32 /*eventId*/) {}

		void SendPacketToAll(WorldPacket *packet);

		void SendPacketToTeam(uint32 TeamID, WorldPacket *packet, Player *sender = NULL, bool self = true);
		void PlaySoundToAll(uint32 SoundID);
		void PlaySoundToTeam(uint32 SoundID, uint32 TeamID);
		void CastSpellOnTeam(uint32 SpellID, uint32 TeamID);
		void RewardHonorToTeam(uint32 Honor, uint32 TeamID);
		void RewardHonorTeamDaily(uint32 TeamID);
		void RewardReputationToTeam(uint32 faction_id, uint32 Reputation, uint32 TeamID);
		void RewardXpToTeam(uint32 Xp, float percentOfLevel, uint32 TeamID);
		void UpdateWorldState(uint32 Field, uint32 Value,Player *Source = NULL);
		void EndBattleGround(uint32 winner);
		uint32 GetBonusHonorFromKill(uint32 kills) const;
		uint32 GetBattlemasterEntry() const;
		void RewardSpellCast(Player *plr, uint32 spell_id);
		void RewardItem(Player *plr, uint32 item_id, uint32 count);
		void RewardQuestComplete(Player *plr);
		void BlockMovement(Player *plr);

		void StartBattleGround();
		void EndNow();

		void AddPlayer(Player *plr);

		void AddOrSetPlayerToCorrectBgGroup(Player *plr, uint64 plr_guid, uint32 team);
		void EventPlayerLoggedIn(Player* player, uint64 plr_guid);
		void EventPlayerLoggedOut(Player* player);
		void AddToBGFreeSlotQueue();
		void RemoveFromBGFreeSlotQueue();
		uint32 GetFreeSlotsForTeam(uint32 Team) const;
		void UpdatePlayerScore(Player *Source, uint32 type, uint32 value);
		uint32 GetPlayerScore(Player *Source, uint32 type);
		uint32 GetDamageDoneForTeam(uint32 TeamID);

		bool AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime = 0);
		void DoorClose(uint64 const& guid);
		void DoorOpen(uint64 const& guid);
		void OnObjectDBLoad(Creature* /*creature*/);
		uint64 GetSingleCreatureGuid(uint8 event1, uint8 event2);
		void OnObjectDBLoad(GameObject* /*obj*/);

		bool IsDoor(uint8 event1, uint8 event2);
		void OpenDoorEvent(uint8 event1, uint8 event2 = 0);

		void SpawnEvent(uint8 event1, uint8 event2, bool spawn);
		void SpawnBGObject(uint64 const& guid, uint32 respawntime);
		void SpawnBGCreature(uint64 const& guid, uint32 respawntime);

		bool DelObject(uint32 type);

		void SendMessageToAll(int32 entry, ChatMsg type, Player const* source = NULL);
		void SendYellToAll(int32 entry, uint32 language, uint64 const& guid);
		void PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...  );

		// specialized version with 2 string id args
        void SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 strId1 = 0, int32 strId2 = 0);
        void SendYell2ToAll(int32 entry, uint32 language, uint64 const& guid, int32 arg1, int32 arg2);

		void SendWarningToAll(int32 entry, ...);
		void SendWarningToAll(std::string str);

		void HandleTriggerBuff(uint64 const& go_guid);
		virtual void HandleKillPlayer(Player *player, Player *killer);
        virtual void HandleKillUnit(Creature* /*unit*/, Player* /*killer*/) { return; };

		void PlayerAddedToBGCheckIfBGIsRunning(Player* plr);
		uint32 GetAlivePlayersCountByTeam(uint32 Team);
		void CheckArenaWinConditions();

		void SetBgRaid(uint32 TeamID, Group *bg_raid);

		/* Death related */
        virtual WorldSafeLocsEntry const* GetClosestGraveYard(Player* player);

		void UpdateArenaWorldState();
		GameObject* GetBGObject(uint32 type);
		bool DelCreature(uint32 type);
		Creature* AddCreature(uint32 entry, uint32 type, uint32 teamval, float x, float y, float z, float o, uint32 respawntime = 0);
		bool AddSpiritGuide(uint32 type, float x, float y, float z, float o, uint32 team);
		Creature* GetBGCreature(uint32 type);

		void SetBracket(PvPDifficultyEntry const* bracketEntry);

		bool IsTeamScoreInRange(uint32 team, uint32 minScore, uint32 maxScore) const;

		BattleGroundTypeId GetTypeID(bool GetRandom = false) const { return GetRandom ? m_RandomTypeID : m_TypeID; }
		void SetTypeID(BattleGroundTypeId TypeID) { m_TypeID = TypeID; }
		void SetRandomTypeID(BattleGroundTypeId TypeID) { m_RandomTypeID = TypeID; }

		void RewardAchievementToPlayer(Player* plr, uint32 entry);
		void RewardAchievementToTeam(uint32 team, uint32 entry);

		typedef std::map<uint64, cBattleGroundPlayer> BattleGroundPlayerMap;
        BattleGroundPlayerMap const& GetPlayers() const { return m_Players; }
        uint32 GetPlayersSize() const { return m_Players.size(); }
		std::vector<uint64> getPlayerList();

		bool IsPlayerInBattleGround(uint64 guid);
		uint32 GetPlayerTeam(uint64 guid);
		uint32 GetPlayerOfflineTime(uint64 guid);

		void SetPlayerValues(uint64 guid, uint32 offlineTime, uint32 team);

		virtual void RemovePlayerAtLeave(uint64 guid, bool Transport, bool SendPacket);

		static BattleGroundTeamId GetTeamIndexByTeamId(uint32 Team) { return Team == ALLIANCE ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE; }

		uint32 GetPlayersCountByTeam(uint32 Team) const { return m_PlayersCount[GetTeamIndexByTeamId(Team)]; }
		void UpdatePlayersCountByTeam(uint32 Team, bool remove)
        {
            if (remove)
                --m_PlayersCount[GetTeamIndexByTeamId(Team)];
            else
                ++m_PlayersCount[GetTeamIndexByTeamId(Team)];
        }

		void SetArenaTeamIdForTeam(uint32 Team, uint32 ArenaTeamId) { m_ArenaTeamIds[GetTeamIndexByTeamId(Team)] = ArenaTeamId; }
        uint32 GetArenaTeamIdForTeam(uint32 Team) const             { return m_ArenaTeamIds[GetTeamIndexByTeamId(Team)]; }
		void SetArenaTeamRatingChangeForTeam(uint32 Team, int32 RatingChange) { m_ArenaTeamRatingChanges[GetTeamIndexByTeamId(Team)] = RatingChange; }
        int32 GetArenaTeamRatingChangeForTeam(uint32 Team) const    { return m_ArenaTeamRatingChanges[GetTeamIndexByTeamId(Team)]; }

		uint32 GetMaxPlayers() const        { return m_MaxPlayers; }
        uint32 GetMinPlayers() const        { return m_MinPlayers; }

		uint32 GetMinLevel() const          { return m_LevelMin; }
        uint32 GetMaxLevel() const          { return m_LevelMax; }

		uint32 GetMaxPlayersPerTeam() const { return m_MaxPlayersPerTeam; }
        uint32 GetMinPlayersPerTeam() const { return m_MinPlayersPerTeam; }

		void SetMaxPlayersPerTeam(uint32 MaxPlayers) { m_MaxPlayersPerTeam = MaxPlayers; }
        void SetMinPlayersPerTeam(uint32 MinPlayers) { m_MinPlayersPerTeam = MinPlayers; }

		void SetMaxPlayers(uint32 MaxPlayers) { m_MaxPlayers = MaxPlayers; }
        void SetMinPlayers(uint32 MinPlayers) { m_MinPlayers = MinPlayers; }
        void SetLevelRange(uint32 min, uint32 max) { m_LevelMin = min; m_LevelMax = max; }

		bool HasFreeSlots();

		void SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O);
		void GetTeamStartLoc(uint32 TeamID, float &X, float &Y, float &Z, float &O) const
        {
            BattleGroundTeamId idx = GetTeamIndexByTeamId(TeamID);
            X = m_TeamStartLocX[idx];
            Y = m_TeamStartLocY[idx];
            Z = m_TeamStartLocZ[idx];
            O = m_TeamStartLocO[idx];
        }

		void setId(uint64 id) { m_Id = id; }
		uint64 getId() { return m_Id; }
	protected:
		uint64 m_Id;
		/* Player lists, those need to be accessible by inherited classes */
        BattleGroundPlayerMap  m_Players;
	private:
		BattleGroundTypeId m_TypeID;
        BattleGroundTypeId m_RandomTypeID;

		/* Players count by team */
        uint32 m_PlayersCount[BG_TEAMS_COUNT];

		/* Arena team ids by team */
        uint32 m_ArenaTeamIds[BG_TEAMS_COUNT];

		int32 m_ArenaTeamRatingChanges[BG_TEAMS_COUNT];

		/* Limits */
		uint32 m_LevelMin;
        uint32 m_LevelMax;
        uint32 m_MaxPlayersPerTeam;
        uint32 m_MaxPlayers;
        uint32 m_MinPlayersPerTeam;
        uint32 m_MinPlayers;

		/* Start location */
		float m_TeamStartLocX[BG_TEAMS_COUNT];
        float m_TeamStartLocY[BG_TEAMS_COUNT];
        float m_TeamStartLocZ[BG_TEAMS_COUNT];
        float m_TeamStartLocO[BG_TEAMS_COUNT];
};

#endif