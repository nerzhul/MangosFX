#ifndef __C_BATTLEGROUND_H_
#define __C_BATTLEGROUND_H_

#include <Common.h>
#include <BattleGround.h>

struct cBattleGroundPlayer
{
    time_t  OfflineRemoveTime;                              // for tracking and removing offline players from queue after 5 minutes
    uint32  Team;                                           // Player's team
};

class cBattleGround
{
	public:
		cBattleGround();
		~cBattleGround() {}
		void SendWarningToAll(int32 entry, ...);
		void SendWarningToAll(std::string str);

		virtual void Reset();

		BattleGroundTypeId GetTypeID(bool GetRandom = false) const { return GetRandom ? m_RandomTypeID : m_TypeID; }
		void SetTypeID(BattleGroundTypeId TypeID) { m_TypeID = TypeID; }
		void SetRandomTypeID(BattleGroundTypeId TypeID) { m_RandomTypeID = TypeID; }

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