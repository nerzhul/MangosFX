#ifndef _C_BATTLEGROUNDRL_H_
#define _C_BATTLEGROUNDRL_H_

#include "cBattleGround.h"

class BattleGroundRLScore : public BattleGroundScore
{
public:
	BattleGroundRLScore() {};
	virtual ~BattleGroundRLScore() {};
	//TODO fix me
};

class cBattleGroundRL : public cBattleGround
{
    friend class BattleGroundMgr;
	
public:
	cBattleGroundRL();
	~cBattleGroundRL();
	void Update(uint32 diff);
	
	/* inherited from BattlegroundClass */
	virtual void AddPlayer(Player *plr);
	virtual void Reset();
	virtual void FillInitialWorldStates(WorldPacket &d, uint32& count);
	virtual void StartingEventCloseDoors();
	virtual void StartingEventOpenDoors();
	
	void RemovePlayer(Player *plr, uint64 guid);
	void HandleAreaTrigger(Player *Source, uint32 Trigger);
	bool SetupBattleGround();
	void HandleKillPlayer(Player* player, Player *killer);
	bool HandlePlayerUnderMap(Player * plr);
};
#endif
