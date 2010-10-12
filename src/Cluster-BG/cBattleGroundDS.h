#ifndef _C_BATTLEGROUNDDS_H_
#define _C_BATTLEGROUNDDS_H_

#include "cBattleGround.h"

class BattleGroundDSScore : public BattleGroundScore
{
public:
	BattleGroundDSScore() {};
	virtual ~BattleGroundDSScore() {};
	//TODO fix me
};

class cBattleGroundDS : public cBattleGround
{
    friend class BattleGroundMgr;
	
public:
	cBattleGroundDS();
	~cBattleGroundDS();
	void Update(uint32 diff);
	
	/* inherited from BattlegroundClass */
	virtual void AddPlayer(Player *plr);
	virtual void StartingEventCloseDoors();
	virtual void StartingEventOpenDoors();
	virtual void Reset(Player * /*Source*/, uint32 /*Trigger*/);
	virtual void FillInitialWorldStates(WorldPacket &d);
	
	void RemovePlayer(Player *plr, uint64 guid);
	void HandleAreaTrigger(Player *Source, uint32 Trigger);
	bool SetupBattleGround();
	void HandleKillPlayer(Player* player, Player *killer);
	bool HandlePlayerUnderMap(Player * plr);
private:
	uint32 m_uiKnockback;
};
#endif
