#ifndef _C_BATTLEGROUNDAA_H_
#define _C_BATTLEGROUNDAA_H_

class BattleGround;

class BattleGroundAAScore : public BattleGroundScore
{
    public:
        BattleGroundAAScore() {};
        virtual ~BattleGroundAAScore() {};
        //TODO fix me
};

class cBattleGroundAA : public cBattleGround
{
    friend class BattleGroundMgr;

    public:
        cBattleGroundAA();
        ~cBattleGroundAA();
        void Update(uint32 diff);

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player *plr);
        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();

        void RemovePlayer(Player *plr, uint64 guid);
        void HandleAreaTrigger(Player *Source, uint32 Trigger);
        bool SetupBattleGround();
        void HandleKillPlayer(Player* player, Player *killer);
};
#endif
