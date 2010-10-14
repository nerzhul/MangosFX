#ifndef __C_BATTLEGROUND_MGR_H_
#define __C_BATTLEGROUND_MGR_H_

#include <Common.h>
#include <BattleGround.h>
#include <Utilities/EventProcessor.h>
#include <ace/Recursive_Thread_Mutex.h>
#include "cBattleGround.h"

typedef std::map<uint64,cBattleGround*> cBattleGroundSet;
typedef UNORDERED_MAP<uint32, BattleGroundTypeId> BattleMastersMap;
typedef UNORDERED_MAP<uint32, BattleGroundEventIdx> CreatureBattleEventIndexesMap;
typedef UNORDERED_MAP<uint32, BattleGroundEventIdx> GameObjectBattleEventIndexesMap;

#define BATTLEGROUND_ARENA_POINT_DISTRIBUTION_DAY 86400     // seconds in a day
#define COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME 10

enum BattleGroundQueueGroupTypes
{
    BG_QUEUE_PREMADE_ALLIANCE   = 0,
    BG_QUEUE_PREMADE_HORDE      = 1,
    BG_QUEUE_NORMAL_ALLIANCE    = 2,
    BG_QUEUE_NORMAL_HORDE       = 3
};
#define BG_QUEUE_GROUP_TYPES_COUNT 4

//this container can't be deque, because deque doesn't like removing the last element - if you remove it, it invalidates next iterator and crash appears
typedef std::list<cBattleGround*> BGFreeSlotQueueType;
struct GroupQueueInfo;                                      // type predefinition
struct PlayerQueueInfo                                      // stores information for players in queue
{
    uint32  LastOnlineTime;                                 // for tracking and removing offline players from queue after 5 minutes
    GroupQueueInfo * GroupInfo;                             // pointer to the associated groupqueueinfo
};

struct GroupQueueInfo                                       // stores information about the group in queue (also used when joined as solo!)
{
    std::map<uint64, PlayerQueueInfo*> Players;             // player queue info map
    uint32  Team;                                           // Player team (ALLIANCE/HORDE)
    BattleGroundTypeId BgTypeId;                            // battleground type id
    bool    IsRated;                                        // rated
    uint8   ArenaType;                                      // 2v2, 3v3, 5v5 or 0 when BG
    uint32  ArenaTeamId;                                    // team id if rated match
    uint32  JoinTime;                                       // time when group was added
    uint32  RemoveInviteTime;                               // time when we will remove invite for players in group
    uint32  IsInvitedToBGInstanceGUID;                      // was invited to certain BG
    uint32  ArenaTeamRating;                                // if rated match, inited to the rating of the team
    uint32  OpponentsTeamRating;                            // for rated arena matches
};

class BattleGroundQueue
{
    public:
        BattleGroundQueue();
        ~BattleGroundQueue();

        void Update(BattleGroundTypeId bgTypeId, BattleGroundBracketId bracket_id, uint8 arenaType = 0, bool isRated = false, uint32 minRating = 0);

        void FillPlayersToBG(cBattleGround* bg, BattleGroundBracketId bracket_id);
        bool CheckPremadeMatch(BattleGroundBracketId bracket_id, uint32 MaxPlayersPerTeam);
        bool CheckNormalMatch(cBattleGround* bg_template, BattleGroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers);
        bool CheckSkirmishForSameFaction(BattleGroundBracketId bracket_id, uint32 minPlayersPerTeam);
        GroupQueueInfo * AddGroup(Player* leader, Group* group, BattleGroundTypeId bgTypeId, PvPDifficultyEntry const*  backetEntry, uint8 ArenaType, bool isRated, bool isPremade, uint32 ArenaRating, uint32 ArenaTeamId = 0);
        void RemovePlayer(const uint64& guid, bool decreaseInvitedCount);
        bool IsPlayerInvited(const uint64& pl_guid, const uint32 bgInstanceGuid, const uint32 removeTime);
        bool GetPlayerGroupInfoData(const uint64& guid, GroupQueueInfo* ginfo);
        void PlayerInvitedToBGUpdateAverageWaitTime(GroupQueueInfo* ginfo, BattleGroundBracketId bracket_id);
        uint32 GetAverageQueueWaitTime(GroupQueueInfo* ginfo, BattleGroundBracketId bracket_id);

    private:
        //mutex that should not allow changing private data, nor allowing to update Queue during private data change.
        ACE_Recursive_Thread_Mutex  m_Lock;


        typedef std::map<uint64, PlayerQueueInfo> QueuedPlayersMap;
        QueuedPlayersMap m_QueuedPlayers;

        //we need constant add to begin and constant remove / add from the end, therefore deque suits our problem well
        typedef std::list<GroupQueueInfo*> GroupsQueueType;

        /*
        This two dimensional array is used to store All queued groups
        First dimension specifies the bgTypeId
        Second dimension specifies the player's group types -
             BG_QUEUE_PREMADE_ALLIANCE  is used for premade alliance groups and alliance rated arena teams
             BG_QUEUE_PREMADE_HORDE     is used for premade horde groups and horde rated arena teams
             BG_QUEUE_NORMAL_ALLIANCE   is used for normal (or small) alliance groups or non-rated arena matches
             BG_QUEUE_NORMAL_HORDE      is used for normal (or small) horde groups or non-rated arena matches
        */
        GroupsQueueType m_QueuedGroups[MAX_BATTLEGROUND_BRACKETS][BG_QUEUE_GROUP_TYPES_COUNT];

        // class to select and invite groups to bg
        class SelectionPool
        {
        public:
            void Init();
            bool AddGroup(GroupQueueInfo *ginfo, uint32 desiredCount);
            bool KickGroup(uint32 size);
            uint32 GetPlayerCount() const {return PlayerCount;}
        public:
            GroupsQueueType SelectedGroups;
        private:
            uint32 PlayerCount;
        };

        //one selection pool for horde, other one for alliance
        SelectionPool m_SelectionPools[BG_TEAMS_COUNT];

        bool InviteGroupToBG(GroupQueueInfo * ginfo, cBattleGround * bg, uint32 side);
        uint32 m_WaitTimes[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS][COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME];
        uint32 m_WaitTimeLastPlayer[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS];
        uint32 m_SumOfWaitTimes[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS];
};

class BGQueueInviteEvent : public BasicEvent
{
    public:
        BGQueueInviteEvent(const uint64& pl_guid, uint32 BgInstanceGUID, BattleGroundTypeId BgTypeId, uint8 arenaType, uint32 removeTime) :
          m_PlayerGuid(pl_guid), m_BgInstanceGUID(BgInstanceGUID), m_BgTypeId(BgTypeId), m_ArenaType(arenaType), m_RemoveTime(removeTime)
          {
          };
        virtual ~BGQueueInviteEvent() {};

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        uint64 m_PlayerGuid;
        uint32 m_BgInstanceGUID;
        BattleGroundTypeId m_BgTypeId;
        uint8  m_ArenaType;
        uint32 m_RemoveTime;
};

/*
    This class is used to remove player from BG queue after 1 minute 20 seconds from first invitation
    We must store removeInvite time in case player left queue and joined and is invited again
    We must store bgQueueTypeId, because battleground can be deleted already, when player entered it
*/
class BGQueueRemoveEvent : public BasicEvent
{
    public:
        BGQueueRemoveEvent(const uint64& pl_guid, uint32 bgInstanceGUID, BattleGroundTypeId BgTypeId, BattleGroundQueueTypeId bgQueueTypeId, uint32 removeTime)
            : m_PlayerGuid(pl_guid), m_BgInstanceGUID(bgInstanceGUID), m_RemoveTime(removeTime), m_BgTypeId(BgTypeId), m_BgQueueTypeId(bgQueueTypeId)
        {}

        virtual ~BGQueueRemoveEvent() {}

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        uint64 m_PlayerGuid;
        uint32 m_BgInstanceGUID;
        uint32 m_RemoveTime;
        BattleGroundTypeId m_BgTypeId;
        BattleGroundQueueTypeId m_BgQueueTypeId;
};

class cBattleGroundMgr
{
	public:
		cBattleGroundMgr();
		~cBattleGroundMgr();
		uint64 CreateBattleGround();
		void Update(uint32 diff);
		void DropBattleGround(uint64 id);
		cBattleGround* getBattleGround(uint64 id);

		/* Packet Building */
		void BuildPlaySoundPacket(WorldPacket *data, uint32 soundid);
		void BuildUpdateWorldStatePacket(WorldPacket *data, uint32 field, uint32 value);
		void BuildGroupJoinedBattlegroundPacket(WorldPacket *data, GroupJoinBattlegroundResult result);
		void BuildPlayerLeftBattleGroundPacket(WorldPacket *data, const uint64& guid);
		void BuildBattleGroundStatusPacket(WorldPacket *data, cBattleGround *bg, uint8 QueueSlot, uint8 StatusID, uint32 Time1, uint32 Time2, uint8 arenatype);
		void BuildPlayerJoinedBattleGroundPacket(WorldPacket *data, Player *plr);
        void BuildBattleGroundListPacket(WorldPacket *data, const uint64& guid, Player *plr, BattleGroundTypeId bgTypeId, uint8 fromWhere);
        void BuildPvpLogDataPacket(WorldPacket *data, cBattleGround *bg);


		BattleGroundQueueTypeId BGQueueTypeId(BattleGroundTypeId bgTypeId, uint8 arenaType);
		uint32 GetPrematureFinishTime() const;

		void RemoveBattleGround(uint64 instanceID, BattleGroundTypeId bgTypeId) { m_BattleGrounds[bgTypeId].erase(instanceID); }
		void AddBattleGround(uint64 InstanceID, BattleGroundTypeId bgTypeId, cBattleGround* BG) { m_BattleGrounds[bgTypeId][InstanceID] = BG; }

		/* Battlegrounds */
        cBattleGround* GetBattleGroundThroughClientInstance(uint32 instanceId, BattleGroundTypeId bgTypeId);
        cBattleGround* GetBattleGround(uint32 InstanceID, BattleGroundTypeId bgTypeId); //there must be uint32 because MAX_BATTLEGROUND_TYPE_ID means unknown

        cBattleGround* GetBattleGroundTemplate(BattleGroundTypeId bgTypeId);
        cBattleGround* CreateNewBattleGround(BattleGroundTypeId bgTypeId, PvPDifficultyEntry const* bracketEntry, uint8 arenaType, bool isRated);

        uint32 CreateBattleGround(BattleGroundTypeId bgTypeId, bool IsArena, uint32 MinPlayersPerTeam, uint32 MaxPlayersPerTeam, uint32 LevelMin, uint32 LevelMax, uint32 MapID, float Team1StartLocX, float Team1StartLocY, float Team1StartLocZ, float Team1StartLocO, float Team2StartLocX, float Team2StartLocY, float Team2StartLocZ, float Team2StartLocO);

        uint32 CreateClientVisibleInstanceId(BattleGroundTypeId bgTypeId, BattleGroundBracketId bracket_id);

        void CreateInitialBattleGrounds();
        void DeleteAllBattleGrounds();

        void SendToBattleGround(Player *pl, uint32 InstanceID, BattleGroundTypeId bgTypeId);

        /* Battleground queues */
        //these queues are instantiated when creating BattlegroundMrg
        BattleGroundQueue m_BattleGroundQueues[MAX_BATTLEGROUND_QUEUE_TYPES]; // public, because we need to access them in BG handler code

        BGFreeSlotQueueType BGFreeSlotQueue[MAX_BATTLEGROUND_TYPE_ID];

        void ScheduleQueueUpdate(uint32 arenaRating, uint8 arenaType, BattleGroundQueueTypeId bgQueueTypeId, BattleGroundTypeId bgTypeId, BattleGroundBracketId bracket_id);
        uint32 GetMaxRatingDifference() const;
        uint32 GetRatingDiscardTimer()  const;
        
        void InitAutomaticArenaPointDistribution();
        void DistributeArenaPoints();
        void ToggleArenaTesting();
        void ToggleTesting();

        void LoadBattleMastersEntry();
        BattleGroundTypeId GetBattleMasterBG(uint32 entry) const
        {
            BattleMastersMap::const_iterator itr = mBattleMastersMap.find(entry);
            if (itr != mBattleMastersMap.end())
                return itr->second;
            return BATTLEGROUND_TYPE_NONE;
        }

        void LoadBattleEventIndexes();
        const BattleGroundEventIdx GetCreatureEventIndex(uint32 dbTableGuidLow) const
        {
            CreatureBattleEventIndexesMap::const_iterator itr = m_CreatureBattleEventIndexMap.find(dbTableGuidLow);
            if(itr != m_CreatureBattleEventIndexMap.end())
                return itr->second;
            return m_CreatureBattleEventIndexMap.find(-1)->second;
        }
        const BattleGroundEventIdx GetGameObjectEventIndex(uint32 dbTableGuidLow) const
        {
            GameObjectBattleEventIndexesMap::const_iterator itr = m_GameObjectBattleEventIndexMap.find(dbTableGuidLow);
            if(itr != m_GameObjectBattleEventIndexMap.end())
                return itr->second;
            return m_GameObjectBattleEventIndexMap.find(-1)->second;
        }

		bool GetBGAccessByLevel(BattleGroundTypeId bgTypeId,uint32 level) const;

		static bool IsArenaType(BattleGroundTypeId bgTypeId);
        static bool IsBattleGroundType(BattleGroundTypeId bgTypeId) { return !cBattleGroundMgr::IsArenaType(bgTypeId); }
        static BattleGroundTypeId BGTemplateId(BattleGroundQueueTypeId bgQueueTypeId);
        static uint8 BGArenaType(BattleGroundQueueTypeId bgQueueTypeId);

		static HolidayIds BGTypeToWeekendHolidayId(BattleGroundTypeId bgTypeId);
		static BattleGroundTypeId WeekendHolidayIdToBGType(HolidayIds holiday);
        static bool IsBGWeekend(BattleGroundTypeId bgTypeId);
	private:
		cBattleGroundSet m_BGMap;

		/* Battlegrounds */
        cBattleGroundSet m_BattleGrounds[MAX_BATTLEGROUND_TYPE_ID];

		ACE_Thread_Mutex    SchedulerLock;
        BattleMastersMap    mBattleMastersMap;
        CreatureBattleEventIndexesMap m_CreatureBattleEventIndexMap;
        GameObjectBattleEventIndexesMap m_GameObjectBattleEventIndexMap;

        /* Battlegrounds */
        std::vector<uint64> m_QueueUpdateScheduler;
        std::set<uint32> m_ClientBattleGroundIds[MAX_BATTLEGROUND_TYPE_ID][MAX_BATTLEGROUND_BRACKETS]; //the instanceids just visible for the client
        uint32 m_NextRatingDiscardUpdate;
        time_t m_NextAutoDistributionTime;
        uint32 m_AutoDistributionTimeChecker;
};

#define sClusterBGMgr MaNGOS::Singleton<cBattleGroundMgr>::Instance()

#endif