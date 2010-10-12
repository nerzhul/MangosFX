#ifndef _C_BATTLEGROUNDBE_H_
#define _C_BATTLEGROUNDBE_H_

#include "cBattleGround.h"

class BattleGroundBEScore : public BattleGroundScore
{
public:
	BattleGroundBEScore() {};
	virtual ~BattleGroundBEScore() {};
};

class cBattleGroundBE : public cBattleGround
{
    friend class BattleGroundMgr;
	
public:
	cBattleGroundBE();
	~cBattleGroundBE();
	void Update(uint32 diff);
	
	/* inherited from BattlegroundClass */
	virtual void AddPlayer(Player *plr);
	virtual void StartingEventCloseDoors();
	virtual void StartingEventOpenDoors();
	
	void RemovePlayer(Player *plr, uint64 guid);
	void HandleAreaTrigger(Player *Source, uint32 Trigger);
	bool SetupBattleGround();
	virtual void Reset();
	virtual void FillInitialWorldStates(WorldPacket &d, uint32& count);
	void HandleKillPlayer(Player* player, Player *killer);
	bool HandlePlayerUnderMap(Player * plr);
	
	/* Scorekeeping */
	void UpdatePlayerScore(Player *Source, uint32 type, uint32 value);
};
#endif
