#ifndef __C_BATTLEGROUND_MGR_H_
#define __C_BATTLEGROUND_MGR_H_

#include <Common.h>
#include <BattleGround.h>
#include "cBattleGround.h"

typedef std::map<uint64,cBattleGround*> cBattleGroundMap;
class cBattleGroundMgr
{
	public:
		cBattleGroundMgr() {}
		~cBattleGroundMgr() {}
		uint64 CreateBattleGround();
		void Update(uint32 diff);
		void DropBattleGround(uint64 id);
		cBattleGround* getBattleGround(uint64 id);

		void BuildPlaySoundPacket(WorldPacket *data, uint32 soundid);
		void BuildUpdateWorldStatePacket(WorldPacket *data, uint32 field, uint32 value);
		void BuildPlayerLeftBattleGroundPacket(WorldPacket *data, const uint64& guid);
		BattleGroundQueueTypeId BGQueueTypeId(BattleGroundTypeId bgTypeId, uint8 arenaType);
		uint32 GetPrematureFinishTime() const;
	private:
		cBattleGroundMap m_BGMap;
};

#define sClusterBGMgr MaNGOS::Singleton<cBattleGroundMgr>::Instance()

#endif