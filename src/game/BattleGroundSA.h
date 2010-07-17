/*
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "BattleGround.h"
class BattleGround;

class BattleGroundSAScore : public BattleGroundScore
{
    public:
        BattleGroundSAScore(): demolishers_destroyed(0), gates_destroyed(0) {};
        virtual ~BattleGroundSAScore() {};
    uint8 demolishers_destroyed;
    uint8 gates_destroyed;
};

#define BG_SA_FLAG_AMOUNT 3
#define BG_SA_DEMOLISHER_AMOUNT 4
#define BG_SA_MAX_GATES 6

enum BG_SA_Status
  {
    BG_SA_NOTSTARTED = 0,
    BG_SA_WARMUP,
    BG_SA_ROUND_ONE,
	BG_SA_STUCK,
	BG_SA_TELEPORT,
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

enum BG_SA_Timers
  {
    BG_SA_BOAT_START  =  60000,
    BG_SA_WARMUPFIRSTROUND = 120000,
	BG_SA_WARMUPSECONDROUND = 70000,
    BG_SA_ROUNDLENGTH = 600000
  };

enum BG_SA_WorldStates
  {
    BG_SA_TIMER_MINS				= 3559,
    BG_SA_TIMER_SEC_TENS			= 3560,
    BG_SA_TIMER_SEC_DECS			= 3561,
    BG_SA_ALLY_ATTACKS				= 4352,
    BG_SA_HORDE_ATTACKS				= 4353,

    BG_SA_PURPLE_GATEWS				= 3614,
    BG_SA_RED_GATEWS				= 3617,
    BG_SA_BLUE_GATEWS				= 3620,
    BG_SA_GREEN_GATEWS				= 3623,
    BG_SA_YELLOW_GATEWS				= 3638,
    BG_SA_ANCIENT_GATEWS			= 3849,


    BG_SA_LEFT_GY_ALLIANCE			= 3635,
    BG_SA_RIGHT_GY_ALLIANCE			= 3636,
    BG_SA_CENTER_GY_ALLIANCE		= 3637,

    BG_SA_RIGHT_ATT_TOKEN_ALL		= 3627,
    BG_SA_LEFT_ATT_TOKEN_ALL		= 3626,

    BG_SA_LEFT_ATT_TOKEN_HRD		= 3629,
    BG_SA_RIGHT_ATT_TOKEN_HRD		= 3628,

    BG_SA_HORDE_DEFENCE_TOKEN		= 3631,
    BG_SA_ALLIANCE_DEFENCE_TOKEN	= 3630,

    BG_SA_RIGHT_GY_HORDE			= 3632,
    BG_SA_LEFT_GY_HORDE				= 3633,
    BG_SA_CENTER_GY_HORDE			= 3634,

    BG_SA_BONUS_TIMER				= 3571,
    BG_SA_ENABLE_TIMER				= 3564,
  };

enum BG_SA_Objects
  {
    BG_SA_GREEN_GATE = 0,
    BG_SA_YELLOW_GATE,
    BG_SA_BLUE_GATE,
    BG_SA_RED_GATE,
    BG_SA_PURPLE_GATE,
    BG_SA_ANCIENT_GATE,
    BG_SA_TITAN_RELIC,
    BG_SA_BOAT_ONE,
    BG_SA_BOAT_TWO,
    BG_SA_SIGIL_1,
    BG_SA_SIGIL_2,
    BG_SA_SIGIL_3,
    BG_SA_SIGIL_4,
    BG_SA_SIGIL_5,
    BG_SA_CENTRAL_FLAGPOLE,
    BG_SA_RIGHT_FLAGPOLE,
    BG_SA_LEFT_FLAGPOLE,
    BG_SA_CENTRAL_FLAG,
    BG_SA_RIGHT_FLAG,
    BG_SA_LEFT_FLAG,
    BG_SA_MAXOBJ
};

enum BG_SA_Event
{
	SA_EVENT_INIT_SPAWN	= 0,
	SA_EVENT_LEFT_GRAVE = 1,
	SA_EVENT_RIGHT_GRAVE = 2,
	SA_EVENT_CENTER_GRAVE = 3
};

const uint32 BG_SA_Factions[2] =
  {
    1732,
    1735,
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

enum BG_SA_Miscellaneous
{
	BG_SA_END_ROUND		=	52459,
};

const uint32 BG_SA_GYEntries[BG_SA_MAX_GY] =
  {
    1350,
    1349,
    1347,
    1346,
    1348,
  };

const float BG_SA_GYOrientation[BG_SA_MAX_GY] =
  {
    6.202f,
    1.926f, //right capturable GY
    3.917f, //left capturable GY
    3.104f, //center, capturable
    6.148f, //defender last GY
  };

struct BG_SA_RoundScore
{
  BattleGroundTeamId winner;
  uint32 time;
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
		virtual bool SetupBattleGround();
		virtual void OnCreatureCreate(Creature* cr);
		virtual void OnGameObjectCreate(GameObject* go);
		virtual void Reset();
		virtual void FillInitialWorldStates(WorldPacket& data, uint32& count);
		virtual void EventPlayerDamageGO(Player *player, GameObject* target_obj, uint32 eventId);
		virtual void HandleKillUnit(Creature* unit, Player* killer);
		virtual WorldSafeLocsEntry const* GetClosestGraveYard(Player* player);
		virtual void EventPlayerClickedOnFlag(Player *Source, GameObject* target_obj);
		//virtual void EventPlayerUsedGO(Player* Source, GameObject* object);
		
        uint32 GetWorldStateFromGateID(uint32 id);
	    void EndBattleGround(uint32 winner);
        void RemovePlayer(Player *plr,uint64 guid);
        void HandleAreaTrigger(Player *Source, uint32 Trigger);

        /* Scorekeeping */
        void UpdatePlayerScore(Player *Source, uint32 type, uint32 value);

    private:
		
		void StartShips();
		
		uint32 BG_SA_ENDROUNDTIME;
		
		
		bool TimerEnabled;
		uint32 UpdateWaitTimer;//5secs before starting the 1min countdown for second round
		bool SignaledRoundTwo;
		bool SignaledRoundTwoHalfMin;
		bool InitSecondRound;


		// All validate private objects
		void InitAllObjects();
		void TeleportPlayer(Player* plr);
		void CaptureGraveyard(BG_SA_Graveyards i, Player *Source);
		void EndRound();
		void RelocatePlayers();
		void ToggleTimer();
		void UpdateTimer();
		void UpdateCatapults(bool usable);
		void ResetGraveyards();
		void ApplyStuckBuffOnPlayers();
		void RelocateAllPlayers(bool reseting);
		uint32 GetGateIDFromDestroyEventID(uint32 id);
		const char* GetDoorNameFromGateID(uint32 gateid);

		BattleGroundTeamId attackers;
		BG_SA_GateState GateStatus[BG_SA_MAX_GATES];
		BG_SA_Status status;
		BG_SA_RoundScore RoundScores[2];
		BattleGroundTeamId GraveyardStatus[BG_SA_MAX_GY];

		uint32 TotalTime;
		uint32 WarmupTimer;

		// All guid storage
		typedef std::set<uint64> GUIDSet;
		// Vehicles
		GUIDSet	TurretSet;
		GUIDSet NWDemolisherSet;
		GUIDSet NEDemolisherSet;
		GUIDSet SWDemolisherSet;
		GUIDSet SEDemolisherSet;
		GUIDSet BoatSet[2];
		// GameObjects
		uint64 GatesGUID[BG_SA_MAX_GATES];
		uint64 SigilGUID[BG_SA_MAX_GATES-1];
		uint64 TitanRelicGUID;
		uint64 GraveyardFlag[3][2];
		// Npcs
		uint64 GobelinGUID[2];
		uint64 SpiritGuidesGUID[BG_SA_MAX_GY][2];
		// Tests
		bool OnLeftBoat;
		bool ShipsStarted;
		uint8 round;
};
#endif