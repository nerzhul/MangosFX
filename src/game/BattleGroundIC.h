#ifndef __BATTLEGROUNDIC_H
#define __BATTLEGROUNDIC_H

#include "Formulas.h"

class BattleGround;

#define BG_IC_KILL_BOSS                 4

#define BG_IC_NEARWIN                  "The gates have been breached! Defend the keep!" //Same from both.

enum BG_IC_String
{
	LANG_BG_IC_START_TWO_MINUTES = 11025,
	LANG_BG_IC_START_ONE_MINUTE = 11026,
	LANG_BG_IC_START_HALF_MINUTE = 11027,
	LANG_BG_IC_HAS_BEGUN = 11028,
	LANG_BG_IC_NODE_TAKEN = 11029,
	LANG_BG_IC_NODE_WORKSHOP = 11030,
	LANG_BG_IC_NODE_QUARRY = 11031,
	LANG_BG_IC_NODE_REFINERY = 11032,
	LANG_BG_IC_NODE_DOCKS = 11033,
	LANG_BG_IC_NODE_HANGAR = 11034,
	LANG_BG_IC_NODE_CLAIMED = 11035,
	LANG_BG_IC_NODE_ASSAULTED = 11036,
	LANG_BG_IC_NODE_DEFENDED = 11037,
	LANG_BG_IC_ALLY = 11038,
	LANG_BG_IC_HORDE = 11039
};

enum BG_IC_CreaturePlace
    {
    IC_CPLACE_OVERLORD_AGMAR        = 0,
    IC_CPLACE_HALFORD_WYRMBANE      = 1,
    IC_STATICCPLACE_MAX             = 2
    };

enum BG_IC_CreatureIds
    {
    IC_NPC_A_BOSS          = 0,
    IC_NPC_H_BOSS          = 1,
    };

const uint32 BG_IC_StaticCreatureInfo[IC_STATICCPLACE_MAX][4] = {
    { 34924, 1973, 81, 81 },
    { 34922, 2054, 81, 81 }
};

const float BG_IC_StaticCreaturePos[IC_STATICCPLACE_MAX][5] = { //static creatures
    {224.982635f,-831.572937f,60.903355f,0.0f,0 },//Halford
    {1295.43921f,-765.732666f,70.054062f,0.0f,1 },//Agmar
};

enum BG_IC_WorldStates
{
    IC_Alliance_Score               = 3127,
    IC_Horde_Score                  = 3128,
    IC_SHOW_H_SCORE                 = 3133,
    IC_SHOW_A_SCORE                 = 3134,
};

const uint32 BG_IC_OP_NODESTATES[5] =    {1767, 1782, 1772, 1792, 1787};

const uint32 BG_IC_OP_NODEICONS[5]  =    {1842, 1846, 1845, 1844, 1843};

// For whatever reason we dont use actual banner IDs so heres our "fake" ids.
enum BG_IC_NodeObjectId
{
    BG_IC_OBJECTID_NODE_BANNER_0         = 599995,  // Workshop
    BG_IC_OBJECTID_NODE_BANNER_1         = 599996,  // Quarry
    BG_IC_OBJECTID_NODE_BANNER_2         = 599997,  //Refinery
    BG_IC_OBJECTID_NODE_BANNER_3         = 599998,  // Docks
    BG_IC_OBJECTID_NODE_BANNER_4         = 599999   // Hangar
};

enum BG_IC_ObjectType
{
    // for all 5 node points 8*5=40 objects
    BG_IC_OBJECT_BANNER_NEUTRAL            = 0,
    BG_IC_OBJECT_BANNER_CONT_A             = 1,
    BG_IC_OBJECT_BANNER_CONT_H             = 2,
    BG_IC_OBJECT_BANNER_ALLY               = 3,
    BG_IC_OBJECT_BANNER_HORDE              = 4,
    BG_IC_OBJECT_AURA_ALLY                 = 5,
    BG_IC_OBJECT_AURA_HORDE                = 6,
    BG_IC_OBJECT_AURA_CONTESTED            = 7,
    BG_IC_OBJECT_DESTRUCT_GATE_H_1         = 41,
    BG_IC_OBJECT_DESTRUCT_GATE_A_1         = 42,
    BG_IC_OBJECT_DESTRUCT_GATE_H_2         = 43,
    BG_IC_OBJECT_DESTRUCT_GATE_A_2         = 44,
    BG_IC_OBJECT_DESTRUCT_GATE_H_3         = 45,
    BG_IC_OBJECT_DESTRUCT_GATE_A_3         = 46,
    BG_IC_OBJECT_GATE_H_1                  = 47,
    BG_IC_OBJECT_GATE_A_1                  = 48,
    BG_IC_OBJECT_GATE_H_2                  = 49,
    BG_IC_OBJECT_GATE_A_2                  = 50,
    BG_IC_OBJECT_GATE_H_3                  = 51,
    BG_IC_OBJECT_GATE_A_3                  = 52,
    BG_IC_OBJECT_HORDE_PORTCULLIS          = 53,
    BG_IC_OBJECT_VRKYUL_PORTCULLIS         = 54,
    BG_IC_OBJECT_PORTCULLIS_1              = 55,
    BG_IC_OBJECT_PORTCULLIS_2              = 56,
    BG_IC_OBJECT_PORTCULLIS_3              = 57,
    BG_IC_OBJECT_TELEPORTER_1              = 58,
    BG_IC_OBJECT_TELEPORTER_2              = 61,
    BG_IC_OBJECT_TELEPORTER_3              = 64,
    BG_IC_OBJECT_TELEPORTER_4              = 67,
    BG_IC_OBJECT_TPEFFECT_A                = 70,
    BG_IC_OBJECT_GUNSHIP_P_E_A             = 76,
    BG_IC_OBJECT_GUNSHIP_P_E_H             = 80,
    BG_IC_OBJECT_GUNSHIP_P_A               = 81,
    BG_IC_OBJECT_SEAFOAM_BOMB_1            = 87,
    BG_IC_OBJECT_SEAFOAM_BOMB_2            = 89,
    BG_IC_OBJECT_SEAFOAM_BOMB_3            = 93,
    BG_IC_OBJECT_MAX                       = 95,
};

enum BG_IC_ObjectTypes
{
    BG_IC_OBJECTID_BANNER_A             = 180058,
    BG_IC_OBJECTID_BANNER_CONT_A        = 180059,
    BG_IC_OBJECTID_BANNER_H             = 180060,
    BG_IC_OBJECTID_BANNER_CONT_H        = 180061,

    BG_IC_OBJECTID_AURA_A               = 180100,
    BG_IC_OBJECTID_AURA_H               = 180101,
    BG_IC_OBJECTID_AURA_C               = 180102,

    BG_IC_OBJECTID_DESTRUCT_GATE_H_1    = 195494,
    BG_IC_OBJECTID_DESTRUCT_GATE_A_1    = 195698,
    BG_IC_OBJECTID_DESTRUCT_GATE_H_2    = 195495,
    BG_IC_OBJECTID_DESTRUCT_GATE_A_2    = 195699,
    BG_IC_OBJECTID_DESTRUCT_GATE_H_3    = 195496,
    BG_IC_OBJECTID_DESTRUCT_GATE_A_3    = 195700,
    // Yes, theres more efficent ways of handling this, No, I dont care.
    // If someone someday wants to streamline it, be my guest.
    BG_IC_OBJECTID_GATE_H_1             = 195491,
    BG_IC_OBJECTID_GATE_A_1             = 195703,
    BG_IC_OBJECTID_GATE_H_2             = 195491,
    BG_IC_OBJECTID_GATE_A_2             = 195703,
    BG_IC_OBJECTID_GATE_H_3             = 195491,
    BG_IC_OBJECTID_GATE_A_3             = 195703,
    BG_IC_OBJECTID_HORDE_PORTCULLIS     = 195223,
    BG_IC_OBJECTID_VRKYUL_PORTCULLIS    = 195437,
    BG_IC_OBJECTID_PORTCULLIS_1         = 195436,
    BG_IC_OBJECTID_PORTCULLIS_2         = 195452,
    BG_IC_OBJECTID_PORTCULLIS_3         = 195451,
    BG_IC_OBJECTID_TELEPORTER_1         = 195316,
    BG_IC_OBJECTID_TELEPORTER_2         = 195315,
    BG_IC_OBJECTID_TELEPORTER_3         = 195313,
    BG_IC_OBJECTID_TELEPORTER_4         = 195314,
    BG_IC_OBJECTID_TPEFFECT_A           = 195701,
    BG_IC_OBJECTID_GUNSHIP_P_E_A        = 195705,
    BG_IC_OBJECTID_GUNSHIP_P_E_H        = 195706,
    BG_IC_OBJECTID_GUNSHIP_P_A          = 195320,
    BG_IC_OBJECTID_SEAFOAM_BOMB_1       = 195232,
    BG_IC_OBJECTID_SEAFOAM_BOMB_2       = 195237,
    BG_IC_OBJECTID_SEAFOAM_BOMB_3       = 195333
};

enum BG_IC_Timers
{
    BG_IC_FLAG_CAPTURING_TIME           = 60000,
};

enum BG_IC_Score
{
    BG_IC_WARNING_NEAR_VICTORY_SCORE    = 1400,
    BG_IC_MAX_TEAM_SCORE                = 1600
};

// DO NOT mess with the order...
enum BG_IC_BattleGroundNodes
{
    BG_IC_NODE_WORKSHOP             = 0,
    BG_IC_NODE_QUARRY               = 1,
    BG_IC_NODE_REFINERY             = 2,
    BG_IC_NODE_DOCKS                = 3,
    BG_IC_NODE_HANGAR               = 4,

    BG_IC_DYNAMIC_NODES_COUNT       = 5,

    BG_IC_SPIRIT_ALIANCE            = 5,
    BG_IC_SPIRIT_HORDE              = 6,

    BG_IC_ALL_NODES_COUNT           = 7,
};

enum BG_IC_NodeStatus
{
    BG_IC_NODE_TYPE_NEUTRAL             = 0,
    BG_IC_NODE_TYPE_CONTESTED           = 1,
    BG_IC_NODE_STATUS_ALLY_CONTESTED    = 1,
    BG_IC_NODE_STATUS_HORDE_CONTESTED   = 2,
    BG_IC_NODE_TYPE_OCCUPIED            = 3,
    BG_IC_NODE_STATUS_ALLY_OCCUPIED     = 3,
    BG_IC_NODE_STATUS_HORDE_OCCUPIED    = 4
};

// Once again, ABs until we find ours.
enum BG_IC_Sounds
{
    BG_IC_SOUND_NODE_CLAIMED            = 8192,
    BG_IC_SOUND_NODE_CAPTURED_ALLIANCE  = 8173,
    BG_IC_SOUND_NODE_CAPTURED_HORDE     = 8213,
    BG_IC_SOUND_NODE_ASSAULTED_ALLIANCE = 8212,
    BG_IC_SOUND_NODE_ASSAULTED_HORDE    = 8174,
    BG_IC_SOUND_NEAR_VICTORY            = 8456
};

#define BG_IC_NotICBGWeekendHonorTicks      330
#define BG_IC_ICBGWeekendHonorTicks         200

// x, y, z, o
const float BG_IC_NodePositions[BG_IC_DYNAMIC_NODES_COUNT][4] = {
    {776.229004f, -804.2830200f, 6.45052000f, 1.6057020f},        // Workshop
    {251.015625f, -1159.316040f, 17.2376000f, -2.251473f},         // Quarry
    {1269.50179f, -400.8090210f, 37.6252520f, -1.762782f},        // Refinery
    {726.385010f, -360.2049870f, 17.8153000f, -1.605702f},        // Docks
    {807.779541f, -1000.067688f, 132.381409f, -1.919862f}        // Hangar
};

//DESTRUCTIBLE gates. Other gates coming later
// x, y, z, o, rot0, rot1, rot2, rot3
const float BG_IC_DoorPositions[6][8] = {
    {1150.902832f, -762.605896f, 47.507679f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f}, // H1
    {413.479156f, -833.949646f, 48.523758f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f},  // A1
    {1217.899292f, -676.947937f, 47.634075f, 1.570796f, 0.0f, 0.0f,  0.0f,  0.0f}, // H2
    {351.614594f, -762.750000f, 48.916248f, -1.570796f, 0.0f, 0.0f,  0.0f,  0.0f}, // A2
    {1218.743042f, -851.154541f, 48.253284f, -1.570796f, 0.0f, 0.0f, 0.0f, 0.0},   // H3
    {351.024323f, -903.326416f, 48.924717f, 1.570796f, 0.0f, 0.0f,  0.0f,  0.0f}   // A3
};

const float BG_IC_DestructDoorPositions[6][8] = {
    {1150.902832f, -762.605896f, 47.507679f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f}, //H1
    {413.479156f, -833.949646f, 48.523758f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f},  //A1
    {1217.899292f, -676.947937f, 47.634075f, 1.570796f, 0.0f, 0.0f,  0.0f,  0.0f}, //H2
    {351.614594f, -762.750000f, 48.916248f, -1.570796f, 0.0f, 0.0f,  0.0f,  0.0f}, //A2
    {1218.743042f, -851.154541f, 48.253284f, -1.570796f, 0.0f, 0.0f, 0.0f, 0.0},   //H3
    {351.024323f, -903.326416f, 48.924717f, 1.570796f, 0.0f, 0.0f,  0.0f,  0.0f}   //A3
};
const float BG_IC_OtherObjectPositions[46][8] = {
    {1283.046021f, -765.878479f, 50.829720f, -3.132858f, 0.0f, 0.0f,  0.0f,  0.0f}, // Horde Port
    {1157.047119f, -682.360413f, 48.632248f, -0.829132f, 0.0f, 0.0f,  0.0f,  0.0f}, // Vrykul Port
    {401.024048f, -780.723755f, 49.948219f, -2.528962f, 0.0f, 0.0f,  0.0f,  0.0f}, // Portcullis 1
    {399.801941f, -885.207520f, 50.193855f, 2.515995f, 0.0f, 0.0f,  0.0f,  0.0f}, // Portcullis 1
    {273.032715f, -832.199219f, 51.410851f, -0.026179f, 0.0f, 0.0f,  0.0f,  0.0f}, // portcullis2
    {288.159729f, -832.595337f, 51.410851f, -0.026179f, 0.0f, 0.0f,  0.0f,  0.0f}, // Portcullis3
    {397.088531f, -859.381958f, 48.899307f, 1.640607f, 0.0f, 0.0f,  0.0f,  0.0f}, // Teleporter 1
    {323.539948f, -888.361145f, 48.919735f, 0.034906f, 0.0f, 0.0f,  0.0f,  0.0f},
    {326.284729f, -777.366333f, 49.020832f, 3.124123f, 0.0f, 0.0f,  0.0f,  0.0f},
    {324.635406f, -749.128479f, 49.360184f, 0.017452f, 0.0f, 0.0f,  0.0f,  0.0f}, // Teleporter 2
    {425.675354f, -857.090271f, 48.510418f, -1.605702f, 0.0f, 0.0f,  0.0f,  0.0f},
    {311.920135f, -913.972229f, 48.815922f, 3.089183f, 0.0f, 0.0f,  0.0f,  0.0f}, 
    {1233.272583f, -844.526062f, 48.882416f, -0.017452f, 0.0f, 0.0f,  0.0f,  0.0f}, // Teleporter 3
    {1158.762207f, -746.182312f, 48.627678f, -1.518436f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1235.527832f, -683.871521f, 49.303974f, -3.089183f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1158.762207f, -746.182312f, 48.627678f, -1.518436f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1233.272583f, -844.526062f, 48.882416f, -0.017452f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1235.527832f, -683.871521f, 49.303974f, -3.089183f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1235.086792f, -857.897583f, 48.916256f, 3.071766f, 0.0f, 0.0f,  0.0f,  0.0f}, // Teleporter 4
    {1143.250000f, -779.598938f, 48.628956f, 1.640607f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1236.531250f, -669.414917f, 48.272854f, 0.104719f, 0.0f, 0.0f,  0.0f,  0.0f},
    {311.911469f, -913.986145f, 48.815662f, 3.089183f, 0.0f, 0.0f,  0.0f,  0.0f}, // Teleporter Effect #1
    {324.633698f, -749.147583f, 49.358974f, 0.017452f, 0.0f, 0.0f,  0.0f,  0.0f},
    {323.550354f, -888.347229f, 48.919788f, 0.017452f, 0.0f, 0.0f,  0.0f,  0.0f},
    {397.116333f, -859.378479f, 48.898945f, 1.640607f, 0.0f, 0.0f,  0.0f,  0.0f},
    {326.265625f, -777.347229f, 49.021545f, 3.124123f, 0.0f, 0.0f,  0.0f,  0.0f},
    {425.685760f, -857.092041f, 48.509960f, -1.623156f, 0.0f, 0.0f,  0.0f,  0.0f},
    {839.227478f, -1188.854492f, 278.930450f, 0.0f, 0.0f,  0.0f,  0.0f}, // Portal Effect Alliance
    {672.263916f, -1157.034790f, 133.705673f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f},
    {738.795166f, -1106.746582f, 134.742966f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f},
    {827.961792f, -994.475708f, 134.070267f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f},
    {827.958313f, -994.467041f, 134.071243f, 0.000000f, 0.0f, 0.0f,  0.0f,  0.0f}, // P_A
    {1233.246582f, -844.572937f, 48.883568f, 0.017452f, 0.0f, 0.0f,  0.0f,  0.0f}, // Portal Effect H
    {1235.071167f, -857.956604f, 48.916256f, 3.054327f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1150.902832f, -762.605896f, 47.507679f, 3.141593f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1158.640625f, -746.147583f, 48.627693f, -1.500983f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1235.595459f, -683.805542f, 49.302753f, -3.071766f, 0.0f, 0.0f,  0.0f,  0.0f},
    {1236.458374f, -669.343750f, 48.268406f, 0.087266f, 0.0f, 0.0f,  0.0f,  0.0f},
    {750.600708f, -864.597229f, 13.475407f, 1.937312f, 0.0f, 0.0f,  0.0f,  0.0f}, // Seaforium 3
    {785.508667f, -864.715271f, 13.399305f, 2.478367f, 0.0f, 0.0f,  0.0f,  0.0f},
    {300.371521f, -818.732666f, 48.916248f, 0.785397f, 0.0f, 0.0f,  0.0f,  0.0f}, // Bomb 6
    {302.135406f, -810.708313f, 48.916248f, -1.047198f, 0.0f, 0.0f,  0.0f,  0.0f},
    {297.321198f, -851.321167f, 48.916271f, -0.942477f, 0.0f, 0.0f,  0.0f,  0.0f},
    {298.104156f, -861.026062f, 48.916275f, -2.757613f, 0.0f, 0.0f,  0.0f,  0.0f},
    {750.571167f, -864.614563f, 13.482058f, 1.937312f, 0.0f, 0.0f,  0.0f,  0.0f}, // Bomb 1
    {785.479187f, -864.732666f, 13.396978f, 2.478367f, 0.0f, 0.0f,  0.0f,  0.0f},

};


// Quarry & Refinery dont have SGs
const uint32 BG_IC_GraveyardIds[BG_IC_ALL_NODES_COUNT] = {1482, 1482, 1482, 1480, 1481, 1485, 1486};

// x, y, z, o
const float BG_IC_SpiritGuidePos[BG_IC_ALL_NODES_COUNT][4] = {
    {779.902466f, -37.79542f ,9.762961f, 4.394047f},                      // Workshop
    {779.902466f, -37.79542f ,9.762961f, 4.394047f},                      // Workshop
    {779.902466f, -37.79542f ,9.762961f, 4.394047f},                      // Workshop
    {626.845337f, -279.852203f, 11.383606f, 0.051044f},                   // Docks
    {828.759230f, -994.176147f, 134.110992f, 3.169080f},                  // Hangar
    {278.559998f, -884.533447f, 49.923302f, 1.704314f},                   // Alliance Keep
    {1301.508667f, -833.072937f, 48.999580f, 1.553343f}                   // Horde Keep
};

struct BG_IC_BannerTimer
{
    uint32      timer;
    uint8       type;
    uint8       teamIndex;
};

class BattleGroundICScore : public BattleGroundScore
{
    public:
        BattleGroundICScore(): BasesAssaulted(0), BasesDefended(0) {};
        virtual ~BattleGroundICScore() {};
        uint32 BasesAssaulted;
        uint32 BasesDefended;
};

class BattleGroundIC : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        BattleGroundIC();
        ~BattleGroundIC();

        void Update(uint32 diff);
        void AddPlayer(Player *plr);
        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();
        void RemovePlayer(Player *plr,uint64 guid);
        void HandleAreaTrigger(Player *Source, uint32 Trigger);
        Creature* AddICCreature(uint16 cinfoid, uint16 type);
        virtual bool SetupBattleGround();
        const uint16 GetBonusHonor(uint8 kills);
        virtual void Reset();
        void EndBattleGround(uint32 winner);
        virtual WorldSafeLocsEntry const* GetClosestGraveYard(Player* player);
        void HandleKillUnit(Creature *unit, Player *killer);

        virtual void UpdatePlayerScore(Player *Source, uint32 type, uint32 value);

        virtual void FillInitialWorldStates(WorldPacket& data);

        virtual void EventPlayerClickedOnFlag(Player *source, GameObject* target_obj);

        private:
        void _CreateBanner(uint8 node, uint8 type, uint8 teamIndex, bool delay);
        void _DelBanner(uint8 node, uint8 type, uint8 teamIndex);
        void _SendNodeUpdate(uint8 node);

        void _NodeOccupied(uint8 node,Team team);
        void _NodeDeOccupied(uint8 node);

        int32 _GetNodeNameId(uint8 node);

        uint8 m_MaxLevel;

        uint8               m_Nodes[BG_IC_DYNAMIC_NODES_COUNT];
        uint8               m_prevNodes[BG_IC_DYNAMIC_NODES_COUNT];
        BG_IC_BannerTimer   m_BannerTimers[BG_IC_DYNAMIC_NODES_COUNT];
        uint32              m_NodeTimers[BG_IC_DYNAMIC_NODES_COUNT];
        uint32              m_lastTick[BG_TEAMS_COUNT];
        uint32              m_HonorScoreTics[BG_TEAMS_COUNT];
        bool                m_IsInformedNearVictory;
        uint32              m_HonorTics;
       int32 m_Team_Scores[2];
};
#endif

