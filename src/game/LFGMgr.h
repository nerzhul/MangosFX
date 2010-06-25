#ifndef __LFGMGR_H
#define __LFGMGR_H
 
#include "Platform/Define.h"
#include "Object.h"
#include "Common.h"
#include "Policies/Singleton.h"
#include "BattleGround.h"

enum LFG_Role
{
	ROLE_NONE	=	0x00,
	ROLE_MASTER	=	0x01,
	ROLE_TANK	=	0x02,
	ROLE_HEAL	=	0x04,
	ROLE_DPS	=	0x08,
};

enum LfgType
{
 	LFG_TYPE_DUNGEON = 1,
	LFG_TYPE_RAID    = 2,
	LFG_TYPE_QUEST   = 3,
	LFG_TYPE_ZONE    = 4,
	LFG_TYPE_HEROIC  = 5,
	LFG_TYPE_RANDOM  = 6,
};

enum LfgLockStatusType
{
    LFG_LOCKSTATUS_OK                        = 0,           // Internal use only
    LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION    = 1,
    LFG_LOCKSTATUS_TOO_LOW_LEVEL             = 2,
    LFG_LOCKSTATUS_TOO_HIGH_LEVEL            = 3,
    LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE        = 4,
    LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE       = 5,
    LFG_LOCKSTATUS_RAID_LOCKED               = 6,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL  = 1001,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL = 1002,
    LFG_LOCKSTATUS_QUEST_NOT_COMPLETED       = 1022,
    LFG_LOCKSTATUS_MISSING_ITEM              = 1025,
    LFG_LOCKSTATUS_NOT_IN_SEASON             = 1031,
};

enum LfgRandomDungeonEntries
{
    LFG_ALL_DUNGEONS       = 0,
    LFG_RANDOM_CLASSIC     = 258,
    LFG_RANDOM_BC_NORMAL   = 259,
    LFG_RANDOM_BC_HEROIC   = 260,
    LFG_RANDOM_LK_NORMAL   = 261,
    LFG_RANDOM_LK_HEROIC   = 262,
};

enum LfgRewardEnums
{
    LFG_REWARD_LEVEL0      = 10,
    LFG_REWARD_LEVEL1      = 0,
    LFG_REWARD_LEVEL2      = 1,
    LFG_REWARD_LEVEL3      = 2,
    LFG_REWARD_LEVEL4      = 3,
    LFG_REWARD_LEVEL5      = 4,
    LFG_REWARD_BC_NORMAL   = 5,
    LFG_REWARD_BC_HEROIC   = 6,
    LFG_REWARD_LK_NORMAL   = 7,
    LFG_REWARD_LK_NORMAL80 = 7,
    LFG_REWARD_LK_HEROIC   = 8,
    LFG_REWARD_DATA_SIZE   = 10,
};

const uint32 RewardDungeonData[LFG_REWARD_DATA_SIZE+1][5] =
{ // XP, money, item, item display, count
    {310, 3500, 51999, 56915, 1},                           // Classic 15-23
    {470, 7000, 52000, 56915, 1},                           // Classic 24-34
    {825, 13000, 52001, 56915, 1},                          // Classic 35-45
    {12250, 16500, 52002, 56915, 1},                        // Classic 46-55
    {14300, 18000, 52003, 56915, 1},                        // Classic 56-60
    {1600, 62000, 52004, 56915, 1},                         // BC Normal
    {1900, 88000, 52005, 56915, 1},                         // BC Heroic
    {33100, 148000, 47241, 62232, 2},                       // LK Normal
    {0, 198600, 47241, 62232, 2},                           // LK Normal - Level 80
    {0, 264600, 49426, 64062, 2},                           // LK Heroic
    {0, 0, 0, 0, 0},                                        // Classic - No level
};

const uint32 RewardDungeonDoneData[LFG_REWARD_DATA_SIZE][5] =
{ // XP, money, item, item display, count
    {200, 1800, 51999, 56915, 1},                           // Classic 15-23
    {310, 3500, 52000, 56915, 1},                           // Classic 24-34
    {550, 6500, 52001, 56915, 1},                           // Classic 35-45
    {8150, 8500, 52002, 56915, 1},                          // Classic 46-55
    {9550, 9000, 52003, 56915, 1},                          // Classic 56-60
    {1100, 31000, 52004, 56915, 1},                         // BC Normal
    {12650, 44000, 52005, 56915, 1},                        // BC Heroic
    {16550, 74000, 0, 0, 0},                                // LK Normal
    {0, 99300, 0, 0, 0},                                    // LK Normal - Level 80
    {0, 132300, 47241, 62232, 2},                           // LK Heroic
};

// Dungeon and reason why player can't join
struct LfgLockStatus
{
    uint32 dungeon;
    LfgLockStatusType lockstatus;
};

// Reward info
struct LfgReward
{
    uint32 strangers;
    uint32 baseMoney;
    uint32 baseXP;
    uint32 variableMoney;
    uint32 variableXP;
    uint32 itemId;
    uint32 displayId;
    uint32 stackCount;
};
enum LfgUpdateType
{
    LFG_UPDATETYPE_LEADER               = 1,
    LFG_UPDATETYPE_ROLECHECK_ABORTED    = 4,
    LFG_UPDATETYPE_JOIN_PROPOSAL        = 5,
    LFG_UPDATETYPE_ROLECHECK_FAILED     = 6,
    LFG_UPDATETYPE_REMOVED_FROM_QUEUE   = 7,
    LFG_UPDATETYPE_PROPOSAL_FAILED      = 8,
    LFG_UPDATETYPE_PROPOSAL_DECLINED    = 9,
    LFG_UPDATETYPE_GROUP_FOUND          = 10,
    LFG_UPDATETYPE_ADDED_TO_QUEUE       = 12,
    LFG_UPDATETYPE_PROPOSAL_FOUND       = 13,
    LFG_UPDATETYPE_CLEAR_LOCK_LIST      = 14,
    LFG_UPDATETYPE_GROUP_MEMBER_OFFLINE = 15,
    LFG_UPDATETYPE_GROUP_DISBAND        = 16,
};

#define MAX_DPS 3
#define MAX_GROUP_SIZE 5

enum LFGGroupAnswerType
{
	LFG_ANSW_DENY	=	0x0,
	LFG_ANSW_ACCEPT	=	0x1,
	LFG_ANSW_NONE	=	0x2,
};

class LFGGroup
{
	public:
		LFGGroup();
		~LFGGroup();

		void SendLfgProposalUpdate();

		void SetTank(uint64 guid) { Tank = guid; }
		void SetHeal(uint64 guid) { Heal = guid; }
		void SetMaster(uint64 guid) { Master = guid; }
		void SetDps(uint64 guid);

		bool SetRole(uint64 guid, LFG_Role role);
		LFG_Role TryToGiveRole(LFG_Role role);
		void RemovePlayer(uint64 guid);
		void ResetAnswers();

		uint8 GetTankNb() { return Tank ? 1 : 0; }
		uint8 GetHealNb() { return Heal ? 1 : 0; }
		uint8 GetDpsNb();
		Player* GetPlayerByRole(LFG_Role role, uint8 place = 0);
		LFG_Role GetRoleBySlot(uint8 slot);
		Player* GetPlayerBySlot(uint8 slot);
		
		bool IsFull();

	private:
		uint64 Tank;
		uint64 Heal;
		uint64 Dps[MAX_DPS];
		uint64 Master;
		LFGGroupAnswerType groupAnswers[MAX_GROUP_SIZE];
};

typedef std::set<uint32> LfgDungeonSet;
typedef std::set<LfgLockStatus*> LfgLockStatusSet;
typedef std::vector<LfgReward*> LfgRewardList;
typedef std::map<uint64, LfgLockStatusSet*> LfgLockStatusMap;
typedef std::map<uint32, LfgDungeonSet*> LfgDungeonMap;
typedef std::set<Player*> PlayerSet;
typedef std::set<LFGGroup*> LFGGroupSet;

struct LookingForGroup
{
    LookingForGroup(): roles(0)
    {
        donerandomDungeons.clear();
        applyDungeons.clear();
    }
    std::string comment;
    int8 roles;
	uint32 waited;

    bool isDungeonDone(const uint32 entry)
    {
        return donerandomDungeons.find(entry) != donerandomDungeons.end();
    }

    LfgDungeonSet applyDungeons;                            // Dungeons the player have applied for
    LfgDungeonSet donerandomDungeons;                       // Finished random Dungeons (to calculate the bonus);
	LFGGroup* group;
};

class LFGMgr
{
	public:
		LFGMgr();
		~LFGMgr();

		void InitLFG();
		void SendLfgPlayerInfo(Player *plr);
		void SendLfgPartyInfo(Player *plr);
		void Update(uint32 diff);
		void TeleportPlayerToInstance(Player* plr);
		void RemovePlayerFromRandomQueue(Player* plr);
		void AddPlayerToRandomQueue(Player* plr, LFG_Role role);
		void SendLfgRoleCheckResult(Player* plr, bool accept);
	private:
		void BuildAvailableRandomDungeonList(WorldPacket &data, Player *plr);
		void BuildRewardBlock(WorldPacket &data, uint32 dungeon, Player *plr);
		void BuildPlayerLockDungeonBlock(WorldPacket &data, LfgLockStatusSet *lockSet);
		void BuildPartyLockDungeonBlock(WorldPacket &data, LfgLockStatusMap *lockMap);
		void SendLfgQueueStatusUpdate(Player *plr, LFGGroup* grp);
		LfgLockStatusMap* GetPartyLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons);
		LfgLockStatusSet* GetPlayerLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons);
		LfgDungeonSet* GetRandomDungeons(uint8 level, uint8 expansion);
		LfgDungeonSet* GetDungeonsByRandom(uint32 randomdungeon);
		LfgDungeonSet* GetAllDungeons();
		LfgReward* GetRandomDungeonReward(uint32 dungeon, bool done, uint8 level);

		bool PlayerJoinRandomQueue(Player* plr);
		uint32 GenerateRandomDungeon();

		LFGGroup* SearchGroup(LFG_Role role, uint8 team);

		LfgRewardList m_RewardList;
		LfgRewardList m_RewardDoneList;
		LfgDungeonMap m_DungeonsMap;
		std::vector<LFGGroup*> m_LFGGroupList[BG_TEAMS_COUNT];
		uint32 middleTime; // seconds
		uint32 Update_Timer;

};

#define sLFGMgr MaNGOS::Singleton<LFGMgr>::Instance()
#endif