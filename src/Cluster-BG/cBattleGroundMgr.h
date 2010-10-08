#ifndef __C_BATTLEGROUND_MGR_H_
#define __C_BATTLEGROUND_MGR_H_

#include <Common.h>
#include "cBattleGround.h"

typedef std::map<uint64,cBattleGround*> cBattleGroundMap;
class cBattleGroundMgr
{
	public:
		cBattleGroundMgr() {}
		~cBattleGroundMgr() {}
		uint64 CreateBattleGround();
		void DropBattleGround(uint64 id);
		cBattleGround* getBattleGround(uint64 id);

		void BuildPlaySoundPacket(WorldPacket *data, uint32 soundid);
		void BuildUpdateWorldStatePacket(WorldPacket *data, uint32 field, uint32 value);
	private:
		cBattleGroundMap m_BGMap;
};

#define sClusterBGMgr MaNGOS::Singleton<cBattleGroundMgr>::Instance()

#endif