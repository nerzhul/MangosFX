#ifndef __C_BATTLEGROUND_H_
#define __C_BATTLEGROUND_H_

#include <Common.h>
#include <BattleGround.h>

class cBattleGround
{
	public:
		cBattleGround();
		~cBattleGround() {}
		void SendWarningToAll(int32 entry, ...);
		void SendWarningToAll(std::string str);

		virtual void Reset();

		typedef std::map<uint64, BattleGroundPlayer> BattleGroundPlayerMap;
        BattleGroundPlayerMap const& GetPlayers() const { return m_Players; }
        uint32 GetPlayersSize() const { return m_Players.size(); }
		std::vector<uint64> getPlayerList();
		bool IsPlayerInBattleGround(uint64 guid);
		uint32 GetPlayerTeam(uint64 guid);

		virtual void RemovePlayerAtLeave(uint64 guid, bool Transport, bool SendPacket);

		static BattleGroundTeamId GetTeamIndexByTeamId(uint32 Team) { return Team == ALLIANCE ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE; }
		void UpdatePlayersCountByTeam(uint32 Team, bool remove)
        {
            if (remove)
                --m_PlayersCount[GetTeamIndexByTeamId(Team)];
            else
                ++m_PlayersCount[GetTeamIndexByTeamId(Team)];
        }

		void setId(uint64 id) { m_Id = id; }
		uint64 getId() { return m_Id; }
	protected:
		uint64 m_Id;
		/* Player lists, those need to be accessible by inherited classes */
        BattleGroundPlayerMap  m_Players;

		/* Players count by team */
        uint32 m_PlayersCount[BG_TEAMS_COUNT];
};

#endif