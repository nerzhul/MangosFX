#ifndef _C_BATTLEGROUNDNA_H_
#define _C_BATTLEGROUNDNA_H_

#include "cBattleGround.h"

class BattleGroundNAScore : public BattleGroundScore
{
public:
	BattleGroundNAScore() {};
	virtual ~BattleGroundNAScore() {};
	//TODO fix me
};

class cBattleGroundNA : public cBattleGround
{
    friend class BattleGroundMgr;
	
public:
	cBattleGroundNA();
	~cBattleGroundNA();
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
};
#endif
