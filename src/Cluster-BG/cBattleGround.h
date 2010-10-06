#ifndef __C_BATTLEGROUND_H_
#define __C_BATTLEGROUND_H_

class cBattleGround
{
	public:
		void SendWarningToAll(int32 entry, ...);
		void SendWarningToAll(std::string str);

		typedef std::map<uint64, BattleGroundPlayer> BattleGroundPlayerMap;
        BattleGroundPlayerMap const& GetPlayers() const { return m_Players; }
        uint32 GetPlayersSize() const { return m_Players.size(); }
	protected:
		/* Player lists, those need to be accessible by inherited classes */
        BattleGroundPlayerMap  m_Players;
	private:

};

#endif