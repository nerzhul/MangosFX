/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __BATTLEGROUNDSA_H
#define __BATTLEGROUNDSA_H

class BattleGround;

enum BG_SA_WorldStates
{
    BG_SA_TIMER_MINS = 3559,
    BG_SA_TIMER_SEC_TENS = 3560,
    BG_SA_TIMER_SEC_DECS = 3561,
    BG_SA_ALLY_ATTACKS  = 4352,
    BG_SA_HORDE_ATTACKS = 4353,
    
    BG_SA_PURPLE_GATEWS = 3614,
    BG_SA_RED_GATEWS = 3617,
    BG_SA_BLUE_GATEWS = 3620,
    BG_SA_GREEN_GATEWS = 3623,
    BG_SA_YELLOW_GATEWS = 3638,
    BG_SA_ANCIENT_GATEWS = 3849,
    
    BG_SA_LEFT_GY_ALLIANCE = 3635,
    BG_SA_RIGHT_GY_ALLIANCE = 3636,
    BG_SA_CENTER_GY_ALLIANCE = 3637,


    BG_SA_RIGHT_ATT_TOKEN_ALL = 3627,
    BG_SA_LEFT_ATT_TOKEN_ALL = 3626,


    BG_SA_LEFT_ATT_TOKEN_HRD = 3629,
    BG_SA_RIGHT_ATT_TOKEN_HRD = 3628,


    BG_SA_HORDE_DEFENCE_TOKEN = 3631,
    BG_SA_ALLIANCE_DEFENCE_TOKEN = 3630,


    BG_SA_RIGHT_GY_HORDE = 3632,
    BG_SA_LEFT_GY_HORDE = 3633,
    BG_SA_CENTER_GY_HORDE = 3634,


    BG_SA_BONUS_TIMER = 0xdf3,
    BG_SA_ENABLE_TIMER = 3564,
};

enum BG_SA_Graveyards
{
    BG_SA_BEACH_GY = 0,
    BG_SA_DEFENDER_LAST_GY,
    BG_SA_RIGHT_CAPTURABLE_GY,
    BG_SA_LEFT_CAPTURABLE_GY,
    BG_SA_CENTRAL_CAPTURABLE_GY,
    BG_SA_MAX_GY
};
enum BG_SA_Status
{
    BG_SA_NOTSTARTED = 0,
    BG_SA_WARMUP,
    BG_SA_ROUND_ONE,
    BG_SA_SECOND_WARMUP,
    BG_SA_ROUND_TWO,
    BG_SA_BONUS_ROUND
};

enum BG_SA_GateState
{
    BG_SA_GATE_OK = 1,
    BG_SA_GATE_DAMAGED = 2,
    BG_SA_GATE_DESTROYED = 3
};

enum BG_SA_Gates
{
	BG_SA_ANCIENT_GATE	= 0,
	BG_SA_YELLOW_GATE	= 1,
	BG_SA_GREEN_GATE	= 2,
	BG_SA_BLUE_GATE		= 3,
	BG_SA_RED_GATE		= 4,
	BG_SA_PURPLE_GATE	= 5
};

enum BG_SA_BOAT
{
	BG_SA_BOAT_ONE	=	0,
	BG_SA_BOAT_TWO	=	1,
};

enum BG_SA_Timers
{
    BG_SA_BOAT_START  =  60000,
    BG_SA_WARMUPLENGTH = 120000,
    BG_SA_ROUNDLENGTH = 600000
};
struct BG_SA_RoundScore
{
  BattleGroundTeamId winner;
  uint32 time;
};
class BattleGroundSAScore : public BattleGroundScore
{
    public:
        BattleGroundSAScore(): DemolishersDestroyed(0), GatesDestroyed(0) {};
        virtual ~BattleGroundSAScore() {};
        uint32 DemolishersDestroyed;
        uint32 GatesDestroyed;
};

class BattleGroundSA : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        BattleGroundSA();
        ~BattleGroundSA();
        void Update(uint32 diff);

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player *plr);
        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();
		virtual void Reset();
        void ToggleTimer();
		void StartShips();

        void RemovePlayer(Player *plr,uint64 guid);
        void HandleAreaTrigger(Player *Source, uint32 Trigger);
        //bool SetupBattleGround();

        /* Scorekeeping */
        void UpdatePlayerScore(Player *Source, uint32 type, uint32 value);

        //world states
        virtual void FillInitialWorldStates(WorldPacket& data, uint32& count);

    private:
        BattleGroundTeamId attackers;
        BG_SA_GateState GateStatus[6];
        BG_SA_Status status;
        BattleGroundTeamId GraveyardStatus[BG_SA_MAX_GY];
        bool TimerEnabled;
        bool ShipsStarted;
        uint32 TotalTime;
        BG_SA_RoundScore RoundScores[2];
};
#endif
