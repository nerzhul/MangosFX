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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

#include "Log.h"
#include "Object.h"
#include "Bag.h"
#include "Creature.h"
#include "Player.h"
#include "GameObject.h"
#include "Corpse.h"
#include "QuestDef.h"
#include "Path.h"
#include "ItemPrototype.h"
#include "NPCHandler.h"
#include "Database/DatabaseEnv.h"
#include "Mail.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "Policies/Singleton.h"
#include "Database/SQLStorage.h"

#include <string>
#include <map>
#include <limits>

extern SQLStorage sCreatureStorage;
extern SQLStorage sCreatureDataAddonStorage;
extern SQLStorage sCreatureInfoAddonStorage;
extern SQLStorage sCreatureModelStorage;
extern SQLStorage sEquipmentStorage;
extern SQLStorage sGOStorage;
extern SQLStorage sPageTextStore;
extern SQLStorage sItemStorage;
extern SQLStorage sInstanceTemplate;

class Group;
class Guild;
class ArenaTeam;
class TransportPath;
class Item;

struct GameTele
{
    float  position_x;
    float  position_y;
    float  position_z;
    float  orientation;
    uint32 mapId;
    std::string name;
    std::wstring wnameLow;
};

typedef UNORDERED_MAP<uint32, GameTele > GameTeleMap;

enum eScriptCommand
{
    SCRIPT_COMMAND_TALK                     = 0,            // source = WorldObject, target = any/none, datalong (see enum ChatType for supported CHAT_TYPE_'s)
                                                            // datalong2 = creature entry (searching for a buddy, closest to source), datalong3 = creature search radius, datalong4 = language
                                                            // data_flags = flag_target_player_as_source    = 0x01
                                                            //              flag_original_source_as_target  = 0x02
                                                            //              flag_buddy_as_target            = 0x04
                                                            // dataint = text entry from db_script_string -table. dataint2-4 optional for random selected text.
    SCRIPT_COMMAND_EMOTE                    = 1,            // source = unit, datalong = emote_id
    SCRIPT_COMMAND_FIELD_SET                = 2,            // source = any, datalong = field_id, datalong2 = value
    SCRIPT_COMMAND_MOVE_TO                  = 3,            // source = Creature, datalong2 = time, x/y/z
    SCRIPT_COMMAND_FLAG_SET                 = 4,            // source = any, datalong = field_id, datalong2 = bitmask
    SCRIPT_COMMAND_FLAG_REMOVE              = 5,            // source = any, datalong = field_id, datalong2 = bitmask
    SCRIPT_COMMAND_TELEPORT_TO              = 6,            // source or target with Player, datalong = map_id, x/y/z
    SCRIPT_COMMAND_QUEST_EXPLORED           = 7,            // one from source or target must be Player, another GO/Creature, datalong=quest_id, datalong2=distance or 0
    SCRIPT_COMMAND_KILL_CREDIT              = 8,            // source or target with Player, datalong = creature entry, datalong2 = bool (0=personal credit, 1=group credit)
    SCRIPT_COMMAND_RESPAWN_GAMEOBJECT       = 9,            // source = any (summoner), datalong=db_guid, datalong2=despawn_delay
    SCRIPT_COMMAND_TEMP_SUMMON_CREATURE     = 10,           // source = any (summoner), datalong=creature entry, datalong2=despawn_delay
    SCRIPT_COMMAND_OPEN_DOOR                = 11,           // source = unit, datalong=db_guid, datalong2=reset_delay
    SCRIPT_COMMAND_CLOSE_DOOR               = 12,           // source = unit, datalong=db_guid, datalong2=reset_delay
    SCRIPT_COMMAND_ACTIVATE_OBJECT          = 13,           // source = unit, target=GO
    SCRIPT_COMMAND_REMOVE_AURA              = 14,           // source (datalong2!=0) or target (datalong==0) unit, datalong = spell_id
    SCRIPT_COMMAND_CAST_SPELL               = 15,           // source/target cast spell at target/source (script->datalong2: 0: s->t 1: s->s 2: t->t 3: t->s
    SCRIPT_COMMAND_PLAY_SOUND               = 16,           // source = any object, target=any/player, datalong (sound_id), datalong2 (bitmask: 0/1=anyone/target, 0/2=with distance dependent, so 1|2 = 3 is target with distance dependent)
    SCRIPT_COMMAND_CREATE_ITEM              = 17,           // source or target must be player, datalong = item entry, datalong2 = amount
    SCRIPT_COMMAND_DESPAWN_SELF             = 18,           // source or target must be creature, datalong = despawn delay
    SCRIPT_COMMAND_PLAY_MOVIE               = 19,           // target can only be a player, datalog = movie id
    SCRIPT_COMMAND_MOVEMENT                 = 20,           // source or target must be creature. datalong = MovementType (0:idle, 1:random or 2:waypoint)
                                                            // datalong2 = creature entry (searching for a buddy, closest to source), datalong3 = creature search radius
    SCRIPT_COMMAND_SET_ACTIVEOBJECT         = 21,           // source=any, target=creature
                                                            // datalong=bool 0=off, 1=on
                                                            // datalong2=creature entry, datalong3=search radius
    SCRIPT_COMMAND_SET_FACTION              = 22,           // source=any, target=creature
                                                            // datalong=factionId,
                                                            // datalong2=creature entry, datalong3=search radius
    SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL  = 23,           // source=any, target=creature
                                                            // datalong=creature entry/modelid (depend on data_flags)
                                                            // datalong2=creature entry, datalong3=search radius
                                                            // dataflags= 0x01 to use datalong value as modelid explicit
    SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL  = 24,           // source=any, target=creature
                                                            // datalong=creature entry/modelid (depend on data_flags)
                                                            // datalong2=creature entry, datalong3=search radius
                                                            // dataflags= 0x01 to use datalong value as modelid explicit
    SCRIPT_COMMAND_SET_RUN                  = 25,           // source=any, target=creature
                                                            // datalong= bool 0=off, 1=on
                                                            // datalong2=creature entry, datalong3=search radius
};

#define MAX_TEXT_ID 4                                       // used for SCRIPT_COMMAND_TALK

struct ScriptInfo
{
    uint32 id;
    uint32 delay;
    uint32 command;

    union
    {
        struct                                              // SCRIPT_COMMAND_TALK (0)
        {
            uint32 chatType;                                // datalong
            uint32 creatureEntry;                           // datalong2
            uint32 searchRadius;                            // datalong3
            uint32 language;                                // datalong4
            uint32 flags;                                   // data_flags
            int32  textId[MAX_TEXT_ID];                     // dataint to dataint4
        } talk;

        struct                                              // SCRIPT_COMMAND_EMOTE (1)
        {
            uint32 emoteId;                                 // datalong
        } emote;

        struct                                              // SCRIPT_COMMAND_FIELD_SET (2)
        {
            uint32 fieldId;                                 // datalong
            uint32 fieldValue;                              // datalong2
        } setField;

        struct                                              // SCRIPT_COMMAND_MOVE_TO (3)
        {
            uint32 unused1;                                 // datalong
            uint32 travelTime;                              // datalong2
        } moveTo;

        struct                                              // SCRIPT_COMMAND_FLAG_SET (4)
        {
            uint32 fieldId;                                 // datalong
            uint32 fieldValue;                              // datalong2
        } setFlag;

        struct                                              // SCRIPT_COMMAND_FLAG_REMOVE (5)
        {
            uint32 fieldId;                                 // datalong
            uint32 fieldValue;                              // datalong2
        } removeFlag;

        struct                                              // SCRIPT_COMMAND_TELEPORT_TO (6)
        {
            uint32 mapId;                                   // datalong
        } teleportTo;

        struct                                              // SCRIPT_COMMAND_QUEST_EXPLORED (7)
        {
            uint32 questId;                                 // datalong
            uint32 distance;                                // datalong2
        } questExplored;

        struct                                              // SCRIPT_COMMAND_KILL_CREDIT (8)
        {
            uint32 creatureEntry;                           // datalong
            uint32 isGroupCredit;                           // datalong2
        } killCredit;

        struct                                              // SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (9)
        {
            uint32 goGuid;                                  // datalong
            int32 despawnDelay;                             // datalong2
        } respawnGo;

        struct                                              // SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (10)
        {
            uint32 creatureEntry;                           // datalong
            uint32 despawnDelay;                            // datalong2
            uint32 unused1;                                 // datalong3
            uint32 unused2;                                 // datalong4
            uint32 flags;                                   // data_flags
        } summonCreature;

        struct                                              // SCRIPT_COMMAND_OPEN_DOOR (11)
        {
            uint32 goGuid;                                  // datalong
            int32 resetDelay;                               // datalong2
        } openDoor;

        struct                                              // SCRIPT_COMMAND_CLOSE_DOOR (12)
        {
            uint32 goGuid;                                  // datalong
            int32 resetDelay;                               // datalong2
        } closeDoor;

                                                            // SCRIPT_COMMAND_ACTIVATE_OBJECT (13)

        struct                                              // SCRIPT_COMMAND_REMOVE_AURA (14)
        {
            uint32 spellId;                                 // datalong
            uint32 isSourceTarget;                          // datalong2
        } removeAura;

        struct                                              // SCRIPT_COMMAND_CAST_SPELL (15)
        {
            uint32 spellId;                                 // datalong
            uint32 flags;                                   // datalong2
        } castSpell;

        struct                                              // SCRIPT_COMMAND_PLAY_SOUND (16)
        {
            uint32 soundId;                                 // datalong
            uint32 flags;                                   // datalong2
        } playSound;

        struct                                              // SCRIPT_COMMAND_CREATE_ITEM (17)
        {
            uint32 itemEntry;                               // datalong
            uint32 amount;                                  // datalong2
        } createItem;

        struct                                              // SCRIPT_COMMAND_DESPAWN_SELF (18)
        {
            uint32 despawnDelay;                            // datalong
        } despawn;

        struct                                              // SCRIPT_COMMAND_PLAY_MOVIE (19)
        {
            uint32 movieId;                                 // datalong
        } playMovie;

        struct                                              // SCRIPT_COMMAND_MOVEMENT (20)
        {
            uint32 movementType;                            // datalong
            uint32 creatureEntry;                           // datalong2
            uint32 searchRadius;                            // datalong3
        } movement;

        struct                                              // SCRIPT_COMMAND_SET_ACTIVEOBJECT (21)
        {
            uint32 activate;                                // datalong
            uint32 creatureEntry;                           // datalong2
            uint32 searchRadius;                            // datalong3
        } activeObject;

        struct                                              // SCRIPT_COMMAND_SET_FACTION (22)
        {
            uint32 factionId;                               // datalong
            uint32 creatureEntry;                           // datalong2
            uint32 searchRadius;                            // datalong3
        } faction;

        struct                                              // SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL (23)
        {
            uint32 creatureOrModelEntry;                    // datalong
            uint32 creatureEntry;                           // datalong2
            uint32 searchRadius;                            // datalong3
            uint32 empty1;                                  // datalong4
            uint32 flags;                                   // data_flags
        } morph;

        struct                                              // SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL (24)
        {
            uint32 creatureOrModelEntry;                    // datalong
            uint32 creatureEntry;                           // datalong2
            uint32 searchRadius;                            // datalong3
            uint32 empty1;                                  // datalong4
            uint32 flags;                                   // data_flags
        } mount;

        struct                                              // SCRIPT_COMMAND_SET_RUN (25)
        {
            uint32 run;                                     // datalong
            uint32 creatureEntry;                           // datalong2
            uint32 searchRadius;                            // datalong3
        } run;

        struct
        {
            uint32 data[9];
        } raw;
    };

    float x;
    float y;
    float z;
    float o;

    // helpers
    uint32 GetGOGuid() const
    {
        switch(command)
        {
            case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT: return respawnGo.goGuid;
            case SCRIPT_COMMAND_OPEN_DOOR: return openDoor.goGuid;
            case SCRIPT_COMMAND_CLOSE_DOOR: return closeDoor.goGuid;
            default: return 0;
        }
    }
};

typedef std::multimap<uint32, ScriptInfo> ScriptMap;
typedef std::map<uint32, ScriptMap > ScriptMapMap;
extern ScriptMapMap sQuestEndScripts;
extern ScriptMapMap sQuestStartScripts;
extern ScriptMapMap sSpellScripts;
extern ScriptMapMap sGameObjectScripts;
extern ScriptMapMap sEventScripts;
extern ScriptMapMap sGossipScripts;

struct SpellClickInfo
{
    uint32 spellId;
    uint32 questStart;                                      // quest start (quest must be active or rewarded for spell apply)
    uint32 questEnd;                                        // quest end (quest don't must be rewarded for spell apply)
    bool   questStartCanActive;                             // if true then quest start can be active (not only rewarded)
    uint8 castFlags;

    // helpers
    bool IsFitToRequirements(Player const* player) const;
};

typedef std::multimap<uint32, SpellClickInfo> SpellClickInfoMap;
typedef std::pair<SpellClickInfoMap::const_iterator,SpellClickInfoMap::const_iterator> SpellClickInfoMapBounds;

struct AreaTrigger
{
    uint8  requiredLevel;
    uint32 requiredItem;
    uint32 requiredItem2;
    uint32 heroicKey;
    uint32 heroicKey2;
    uint32 requiredQuest;
    uint32 requiredQuestHeroic;
    std::string requiredFailedText;
    uint32 target_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
};

typedef std::set<uint32> CellGuidSet;
typedef std::map<uint32/*player guid*/,uint32/*instance*/> CellCorpseSet;
struct CellObjectGuids
{
    CellGuidSet creatures;
    CellGuidSet gameobjects;
    CellCorpseSet corpses;
};
typedef UNORDERED_MAP<uint32/*cell_id*/,CellObjectGuids> CellObjectGuidsMap;
typedef UNORDERED_MAP<uint32/*(mapid,spawnMode) pair*/,CellObjectGuidsMap> MapObjectGuids;

typedef UNORDERED_MAP<uint64/*(instance,guid) pair*/,time_t> RespawnTimes;


// mangos string ranges
#define MIN_MANGOS_STRING_ID           1                    // 'mangos_string'
#define MAX_MANGOS_STRING_ID           2000000000
#define MIN_DB_SCRIPT_STRING_ID        MAX_MANGOS_STRING_ID // 'db_script_string'
#define MAX_DB_SCRIPT_STRING_ID        2000010000
#define MIN_CREATURE_AI_TEXT_STRING_ID (-1)                 // 'creature_ai_texts'
#define MAX_CREATURE_AI_TEXT_STRING_ID (-1000000)

struct MangosStringLocale
{
    std::vector<std::string> Content;                       // 0 -> default, i -> i-1 locale index
};

typedef UNORDERED_MAP<uint32,CreatureData> CreatureDataMap;
typedef UNORDERED_MAP<uint32,GameObjectData> GameObjectDataMap;
typedef UNORDERED_MAP<uint32,CreatureLocale> CreatureLocaleMap;
typedef UNORDERED_MAP<uint32,GameObjectLocale> GameObjectLocaleMap;
typedef UNORDERED_MAP<uint32,ItemLocale> ItemLocaleMap;
typedef UNORDERED_MAP<uint32,QuestLocale> QuestLocaleMap;
typedef UNORDERED_MAP<uint32,NpcTextLocale> NpcTextLocaleMap;
typedef UNORDERED_MAP<uint32,PageTextLocale> PageTextLocaleMap;
typedef UNORDERED_MAP<int32,MangosStringLocale> MangosStringLocaleMap;
typedef UNORDERED_MAP<uint32,GossipMenuItemsLocale> GossipMenuItemsLocaleMap;
typedef UNORDERED_MAP<uint32,PointOfInterestLocale> PointOfInterestLocaleMap;

typedef std::multimap<uint32,uint32> QuestRelations;
typedef std::multimap<uint32,ItemRequiredTarget> ItemRequiredTargetMap;
typedef std::pair<ItemRequiredTargetMap::const_iterator, ItemRequiredTargetMap::const_iterator>  ItemRequiredTargetMapBounds;

struct PetLevelInfo
{
    PetLevelInfo() : health(0), mana(0) { for(int i=0; i < MAX_STATS; ++i ) stats[i] = 0; }

    uint16 stats[MAX_STATS];
    uint16 health;
    uint16 mana;
    uint16 armor;
};

struct MailLevelReward
{
    MailLevelReward() : raceMask(0), mailTemplateId(0), senderEntry(0) {}
    MailLevelReward(uint32 _raceMask, uint32 _mailTemplateId, uint32 _senderEntry) : raceMask(_raceMask), mailTemplateId(_mailTemplateId), senderEntry(_senderEntry) {}

    uint32 raceMask;
    uint32 mailTemplateId;
    uint32 senderEntry;
};

typedef std::list<MailLevelReward> MailLevelRewardList;
typedef UNORDERED_MAP<uint8,MailLevelRewardList> MailLevelRewardMap;

struct ReputationOnKillEntry
{
    uint32 repfaction1;
    uint32 repfaction2;
    bool is_teamaward1;
    uint32 reputation_max_cap1;
    int32 repvalue1;
    bool is_teamaward2;
    uint32 reputation_max_cap2;
    int32 repvalue2;
    bool team_dependent;
};

struct PointOfInterest
{
    uint32 entry;
    float x;
    float y;
    uint32 icon;
    uint32 flags;
    uint32 data;
    std::string icon_name;
};

struct GossipMenuItems
{
    uint32          menu_id;
    uint32          id;
    uint8           option_icon;
    std::string     option_text;
    uint32          option_id;
    uint32          npc_option_npcflag;
    uint32          action_menu_id;
    uint32          action_poi_id;
    uint32          action_script_id;
    bool            box_coded;
    uint32          box_money;
    std::string     box_text;
    uint16          cond_1;
    uint16          cond_2;
    uint16          cond_3;
};

struct GossipMenus
{
    uint32          entry;
    uint32          text_id;
    uint16          cond_1;
    uint16          cond_2;
};

typedef std::multimap<uint32,GossipMenus> GossipMenusMap;
typedef std::pair<GossipMenusMap::const_iterator, GossipMenusMap::const_iterator> GossipMenusMapBounds;
typedef std::multimap<uint32,GossipMenuItems> GossipMenuItemsMap;
typedef std::pair<GossipMenuItemsMap::const_iterator, GossipMenuItemsMap::const_iterator> GossipMenuItemsMapBounds;

struct QuestPOIPoint
{
    int32 x;
    int32 y;

    QuestPOIPoint() : x(0), y(0) {}
    QuestPOIPoint(int32 _x, int32 _y) : x(_x), y(_y) {}
    uint32          npc_option_npcflag;
    uint32          action_menu_id;
    uint32          action_poi_id;
    uint32          action_script_id;
    bool            box_coded;
    uint32          box_money;
    std::string     box_text;
    uint16          cond_1;
    uint16          cond_2;
    uint16          cond_3;
};

struct QuestPOI
{
    int32 ObjectiveIndex;
    uint32 MapId;
    uint32 mapAreaId;
    uint32 floodId;
    uint32 Unk3;
    uint32 Unk4;
    std::vector<QuestPOIPoint> points;

    QuestPOI() : ObjectiveIndex(0), MapId(0), mapAreaId(0), floodId(0), Unk3(0), Unk4(0) {}
    QuestPOI(int32 objIndex, uint32 mapId, uint32 _mapAreaId, uint32 _floorId, uint32 unk3, uint32 unk4) : ObjectiveIndex(objIndex), MapId(mapId), mapAreaId(_mapAreaId), floodId(_floorId), Unk3(unk3), Unk4(unk4) {}
};

typedef std::vector<QuestPOI> QuestPOIVector;
typedef UNORDERED_MAP<uint32, QuestPOIVector> QuestPOIMap;

#define WEATHER_SEASONS 4
struct WeatherSeasonChances
{
    uint32 rainChance;
    uint32 snowChance;
    uint32 stormChance;
};

struct WeatherZoneChances
{
    WeatherSeasonChances data[WEATHER_SEASONS];
};

struct GraveYardData
{
    uint32 safeLocId;
    uint32 team;
};
typedef std::multimap<uint32,GraveYardData> GraveYardMap;

enum ConditionType
{                                                           // value1       value2  for the Condition enumed
    CONDITION_NONE                  = 0,                    // 0            0
    CONDITION_AURA                  = 1,                    // spell_id     effindex
    CONDITION_ITEM                  = 2,                    // item_id      count
    CONDITION_ITEM_EQUIPPED         = 3,                    // item_id      0
    CONDITION_AREAID                = 4,                    // zone_id      0
    CONDITION_REPUTATION_RANK       = 5,                    // faction_id   min_rank
    CONDITION_TEAM                  = 6,                    // player_team  0,      (469 - Alliance 67 - Horde)
    CONDITION_SKILL                 = 7,                    // skill_id     skill_value
    CONDITION_QUESTREWARDED         = 8,                    // quest_id     0
    CONDITION_QUESTTAKEN            = 9,                    // quest_id     0,      for condition true while quest active.
    CONDITION_AD_COMMISSION_AURA    = 10,                   // 0            0,      for condition true while one from AD commission aura active
    CONDITION_NO_AURA               = 11,                   // spell_id     effindex
    CONDITION_ACTIVE_EVENT          = 12,                   // event_id     0
    CONDITION_AREA_FLAG             = 13,                   // area_flag    area_flag_not
    CONDITION_RACE_CLASS            = 14,                   // race_mask    class_mask
    CONDITION_LEVEL                 = 15,                   // player_level 0, 1 or 2 (0: equal to, 1: equal or higher than, 2: equal or less than)
	CONDITION_NOITEM                = 16,                   // item_id      count
	CONDITION_SPELL                 = 17,                   // spell_id     0, 1 (0: has spell, 1: hasn't spell)
	CONDITION_INSTANCE_SCRIPT       = 18,                   // map_id       instance_condition_id (instance script specific enum)
	CONDITION_QUESTAVAILABLE        = 19,                   // quest_id     0       for case when loot/gossip possible only if player can start quest
	CONDITION_ACHIEVEMENT           = 20,                   // ach_id       0, 1 (0: has achievement, 1: hasn't achievement) for player
	CONDITION_ACHIEVEMENT_REALM     = 21,                   // ach_id       0, 1 (0: has achievement, 1: hasn't achievement) for server
};

#define MAX_CONDITION                 22                    // maximum value in ConditionType enum

struct PlayerCondition
{
    ConditionType condition;                                // additional condition type
    uint32  value1;                                         // data for the condition - see ConditionType definition
    uint32  value2;

    PlayerCondition(uint8 _condition = 0, uint32 _value1 = 0, uint32 _value2 = 0)
        : condition(ConditionType(_condition)), value1(_value1), value2(_value2) {}

    static bool IsValid(ConditionType condition, uint32 value1, uint32 value2);
    // Checks correctness of values
    bool Meets(Player const * APlayer) const;               // Checks if the player meets the condition
    bool operator == (PlayerCondition const& lc) const
    {
        return (lc.condition == condition && lc.value1 == value1 && lc.value2 == value2);
    }
};

// NPC gossip text id
typedef UNORDERED_MAP<uint32, uint32> CacheNpcTextIdMap;

typedef UNORDERED_MAP<uint32, VendorItemData> CacheVendorItemMap;
typedef UNORDERED_MAP<uint32, TrainerSpellData> CacheTrainerSpellMap;

enum SkillRangeType
{
    SKILL_RANGE_LANGUAGE,                                   // 300..300
    SKILL_RANGE_LEVEL,                                      // 1..max skill for level
    SKILL_RANGE_MONO,                                       // 1..1, grey monolite bar
    SKILL_RANGE_RANK,                                       // 1..skill for known rank
    SKILL_RANGE_NONE,                                       // 0..0 always
};

SkillRangeType GetSkillRangeType(SkillLineEntry const *pSkill, bool racial);

#define MAX_PLAYER_NAME          12                         // max allowed by client name length
#define MAX_INTERNAL_PLAYER_NAME 15                         // max server internal player name length ( > MAX_PLAYER_NAME for support declined names )
#define MAX_PET_NAME             12                         // max allowed by client name length
#define MAX_CHARTER_NAME         24                         // max allowed by client name length

bool normalizePlayerName(std::string& name);

struct MANGOS_DLL_SPEC LanguageDesc
{
    Language lang_id;
    uint32   spell_id;
    uint32   skill_id;
};

extern LanguageDesc lang_description[LANGUAGES_COUNT];
MANGOS_DLL_SPEC LanguageDesc const* GetLanguageDescByID(uint32 lang);

class PlayerDumpReader;

// vehicle system
#define MAX_VEHICLE_SPELLS 6

struct VehicleDataStructure
{
    uint32 v_flags;                                         // vehicle flags, see enum CustomVehicleFLags
    uint32 v_spells[MAX_VEHICLE_SPELLS];                    // spells
    uint32 req_aura;                                        // requieres aura on player to enter (eg. in wintergrasp)
};

typedef UNORDERED_MAP<uint32, VehicleDataStructure> VehicleDataMap;
typedef std::map<uint32,uint32> VehicleSeatDataMap;

class ObjectMgr
{
    friend class PlayerDumpReader;

    public:
        ObjectMgr();
        ~ObjectMgr();

        typedef UNORDERED_MAP<uint32, Item*> ItemMap;

        typedef UNORDERED_MAP<uint32, Group*> GroupMap;

        typedef UNORDERED_MAP<uint32, Guild *> GuildMap;

        typedef UNORDERED_MAP<uint32, ArenaTeam*> ArenaTeamMap;

        typedef UNORDERED_MAP<uint32, Quest*> QuestMap;

        typedef UNORDERED_MAP<uint32, AreaTrigger> AreaTriggerMap;

        typedef UNORDERED_MAP<uint32, uint32> AreaTriggerScriptMap;

		typedef UNORDERED_MAP<uint32, AccessRequirement> AccessRequirementMap;//Merging

        typedef UNORDERED_MAP<uint32, ReputationOnKillEntry> RepOnKillMap;
        typedef UNORDERED_MAP<uint32, PointOfInterest> PointOfInterestMap;

        typedef UNORDERED_MAP<uint32, WeatherZoneChances> WeatherZoneMap;

        typedef std::vector<std::string> ScriptNameMap;

        Player* GetPlayer(const char* name) const { return ObjectAccessor::FindPlayerByName(name);}
        Player* GetPlayer(uint64 guid) const { return ObjectAccessor::FindPlayer(guid); }
		Player* GetPlayerByLowGUID(uint32 lowguid) const;

        static GameObjectInfo const *GetGameObjectInfo(uint32 id) { return sGOStorage.LookupEntry<GameObjectInfo>(id); }

        void LoadGameobjectInfo(bool ClusterIgnore = false);
        void AddGameobjectInfo(GameObjectInfo *goinfo);

        void PackGroupIds();
		Group* GetGroupById(uint32 id) const;
        void AddGroup(Group* group);
        void RemoveGroup(Group* group);
		
        Guild* GetGuildByLeader(uint64 const&guid) const;
        Guild* GetGuildById(uint32 GuildId) const;
        Guild* GetGuildByName(const std::string& guildname) const;
        std::string GetGuildNameById(uint32 GuildId) const;
        void AddGuild(Guild* guild);
        void RemoveGuild(uint32 Id);

        ArenaTeam* GetArenaTeamById(uint32 arenateamid) const;
        ArenaTeam* GetArenaTeamByName(const std::string& arenateamname) const;
        ArenaTeam* GetArenaTeamByCaptain(uint64 const& guid) const;
        void AddArenaTeam(ArenaTeam* arenaTeam);
        void RemoveArenaTeam(uint32 Id);
        ArenaTeamMap::iterator GetArenaTeamMapBegin() { return mArenaTeamMap.begin(); }
        ArenaTeamMap::iterator GetArenaTeamMapEnd()   { return mArenaTeamMap.end(); }

        static CreatureInfo const *GetCreatureTemplate( uint32 id );
        CreatureModelInfo const *GetCreatureModelInfo( uint32 modelid );
        CreatureModelInfo const* GetCreatureModelRandomGender(uint32 display_id);
		uint32 GetCreatureModelAlternativeModel(uint32 modelId);
		uint32 GetCreatureModelOtherTeamModel(uint32 modelId);
        uint32 ChooseDisplayId(uint32 team, const CreatureInfo *cinfo, const CreatureData *data = NULL);
        EquipmentInfo const *GetEquipmentInfo( uint32 entry );
        static CreatureDataAddon const *GetCreatureAddon( uint32 lowguid )
        {
            return sCreatureDataAddonStorage.LookupEntry<CreatureDataAddon>(lowguid);
        }

        static CreatureDataAddon const *GetCreatureTemplateAddon( uint32 entry )
        {
            return sCreatureInfoAddonStorage.LookupEntry<CreatureDataAddon>(entry);
        }

        static ItemPrototype const* GetItemPrototype(uint32 id) { return sItemStorage.LookupEntry<ItemPrototype>(id); }

        static InstanceTemplate const* GetInstanceTemplate(uint32 map)
        {
            return sInstanceTemplate.LookupEntry<InstanceTemplate>(map);
        }

        PetLevelInfo const* GetPetLevelInfo(uint32 creature_id, uint32 level) const;

        PlayerClassInfo const* GetPlayerClassInfo(uint32 class_) const
        {
            if(class_ >= MAX_CLASSES) return NULL;
            return &playerClassInfo[class_];
        }
        void GetPlayerClassLevelInfo(uint32 class_,uint32 level, PlayerClassLevelInfo* info) const;

        PlayerInfo const* GetPlayerInfo(uint32 race, uint32 class_) const
        {
            if(race   >= MAX_RACES)   return NULL;
            if(class_ >= MAX_CLASSES) return NULL;
            PlayerInfo const* info = &playerInfo[race][class_];
            if(info->displayId_m==0 || info->displayId_f==0) return NULL;
            return info;
        }
        void GetPlayerLevelInfo(uint32 race, uint32 class_,uint32 level, PlayerLevelInfo* info) const;

        uint64 GetPlayerGUIDByName(std::string name) const;
        bool GetPlayerNameByGUID(const uint64 &guid, std::string &name) const;
        uint32 GetPlayerTeamByGUID(const uint64 &guid) const;
        uint32 GetPlayerAccountIdByGUID(const uint64 &guid) const;
        uint32 GetPlayerAccountIdByPlayerName(const std::string& name) const;

        uint32 GetNearestTaxiNode( float x, float y, float z, uint32 mapid, uint32 team );
        void GetTaxiPath( uint32 source, uint32 destination, uint32 &path, uint32 &cost);
        uint32 GetTaxiMountDisplayId( uint32 id, uint32 team, bool allowed_alt_team = false);
        //void GetTaxiPathNodes( uint32 path, Path &pathnodes, std::vector<uint32>& mapIds );
        void GetTransportPathNodes( uint32 path, TransportPath &pathnodes );

        Quest const* GetQuestTemplate(uint32 quest_id) const
        {
            QuestMap::const_iterator itr = mQuestTemplates.find(quest_id);
            return itr != mQuestTemplates.end() ? itr->second : NULL;
        }
        QuestMap const& GetQuestTemplates() const { return mQuestTemplates; }

        uint32 GetQuestForAreaTrigger(uint32 Trigger_ID) const
        {
            QuestAreaTriggerMap::const_iterator itr = mQuestAreaTriggerMap.find(Trigger_ID);
            if(itr != mQuestAreaTriggerMap.end())
                return itr->second;
            return 0;
        }
        bool IsTavernAreaTrigger(uint32 Trigger_ID) const
        {
            return mTavernAreaTriggerSet.find(Trigger_ID) != mTavernAreaTriggerSet.end();
        }

        bool IsGameObjectForQuests(uint32 entry) const
        {
            return mGameObjectForQuestSet.find(entry) != mGameObjectForQuestSet.end();
        }

        GossipText const* GetGossipText(uint32 Text_ID) const;

        WorldSafeLocsEntry const *GetClosestGraveYard(float x, float y, float z, uint32 MapId, uint32 team);
        bool AddGraveYardLink(uint32 id, uint32 zone, uint32 team, bool inDB = true);
        void LoadGraveyardZones();
        GraveYardData const* FindGraveYardData(uint32 id, uint32 zone);

        AreaTrigger const* GetAreaTrigger(uint32 trigger) const
        {
            AreaTriggerMap::const_iterator itr = mAreaTriggers.find( trigger );
            if( itr != mAreaTriggers.end( ) )
                return &itr->second;
            return NULL;
        }

		AccessRequirement const* GetAccessRequirement(uint32 mapid, Difficulty difficulty) const // Merging
        {
            AccessRequirementMap::const_iterator itr = mAccessRequirements.find(MAKE_PAIR32(mapid,difficulty));
            if (itr != mAccessRequirements.end())
                return &itr->second;
            return NULL;
        } //End Merging

        AreaTrigger const* GetGoBackTrigger(uint32 Map) const;
        AreaTrigger const* GetMapEntranceTrigger(uint32 Map) const;

        uint32 GetAreaTriggerScriptId(uint32 trigger_id);

        ReputationOnKillEntry const* GetReputationOnKilEntry(uint32 id) const
        {
            RepOnKillMap::const_iterator itr = mRepOnKill.find(id);
            if(itr != mRepOnKill.end())
                return &itr->second;
            return NULL;
        }

        PointOfInterest const* GetPointOfInterest(uint32 id) const
        {
            PointOfInterestMap::const_iterator itr = mPointsOfInterest.find(id);
            if(itr != mPointsOfInterest.end())
                return &itr->second;
            return NULL;
        }

        QuestPOIVector const* GetQuestPOIVector(uint32 questId)
        {
            QuestPOIMap::const_iterator itr = mQuestPOIMap.find(questId);
            if(itr != mQuestPOIMap.end())
                return &itr->second;
            return NULL;
        }

        void LoadGuilds();
        void LoadArenaTeams();
        void LoadGroups();
        void LoadQuests(bool ClusterIgnore = false);
        void LoadQuestRelations()
        {
            LoadGameobjectQuestRelations();
            LoadGameobjectInvolvedRelations();
            LoadCreatureQuestRelations();
            LoadCreatureInvolvedRelations();
        }
        void LoadGameobjectQuestRelations();
        void LoadGameobjectInvolvedRelations();
        void LoadCreatureQuestRelations();
        void LoadCreatureInvolvedRelations();

        QuestRelations mGOQuestRelations;
        QuestRelations mGOQuestInvolvedRelations;
        QuestRelations mCreatureQuestRelations;
        QuestRelations mCreatureQuestInvolvedRelations;

        void LoadGameObjectScripts();
        void LoadQuestEndScripts();
        void LoadQuestStartScripts();
        void LoadEventScripts();
        void LoadSpellScripts();
        void LoadGossipScripts();

        bool LoadMangosStrings(DatabaseType& db, char const* table, int32 min_value, int32 max_value);
        bool LoadMangosStrings() { return LoadMangosStrings(LocalisationDatabase,"mangos_string",MIN_MANGOS_STRING_ID,MAX_MANGOS_STRING_ID); }
        void LoadDbScriptStrings();
        void LoadCreatureLocales();
        void LoadCreatureTemplates(bool ClusterIgnore = false);
        void LoadCreatures();
        void LoadCreatureRespawnTimes();
        void LoadCreatureAddons();
        void LoadCreatureModelInfo();
        void LoadEquipmentTemplates();
        void LoadGameObjectLocales();
        void LoadGameobjects();
        void LoadGameobjectRespawnTimes();
        void LoadItemPrototypes(bool ClusterIgnore = false);
        void LoadItemRequiredTarget();
        void LoadItemLocales();
        void LoadQuestLocales();
        void LoadNpcTextLocales();
        void LoadPageTextLocales();
        void LoadGossipMenuItemsLocales();
        void LoadPointOfInterestLocales();
        void LoadInstanceTemplate();
        void LoadMailLevelRewards();

        void LoadGossipText();

        void LoadAreaTriggerTeleports();
        void LoadQuestAreaTriggers();
        void LoadAreaTriggerScripts();
        void LoadTavernAreaTriggers();
        void LoadGameObjectForQuests();

        void LoadItemTexts();
        void LoadPageTexts();

        void LoadPlayerInfo();
        void LoadPetLevelInfo();
        void LoadExplorationBaseXP();
        void LoadPetNames();
        void LoadPetNumber();
        void LoadCorpses();
        void LoadFishingBaseSkillLevel();

        void LoadReputationOnKill();
        void LoadPointsOfInterest();
        void LoadQuestPOI();

        void LoadNPCSpellClickSpells();

        void LoadWeatherZoneChances();
        void LoadGameTele();

        void LoadNpcTextId();

        void LoadGossipMenu();
        void LoadGossipMenuItems();

        void LoadVendors();
        void LoadTrainerSpell();

        std::string GeneratePetName(uint32 entry);
        uint32 GetBaseXP(uint32 level) const;
        uint32 GetXPForLevel(uint32 level) const;
        uint32 GetXPForPetLevel(uint32 level) const { return GetXPForLevel(level)/20; }

        int32 GetFishingBaseSkillLevel(uint32 entry) const
        {
            FishingBaseSkillMap::const_iterator itr = mFishingBaseForArea.find(entry);
            return itr != mFishingBaseForArea.end() ? itr->second : 0;
        }

        void ReturnOrDeleteOldMails(bool serverUp);

        void SetHighestGuids();
        uint32 GenerateLowGuid(HighGuid guidhigh);
        uint32 GenerateArenaTeamId();
        uint32 GenerateAuctionID();
		uint32 GenerateCalendarEventId();
        uint64 GenerateEquipmentSetGuid();
        uint32 GenerateGuildId();
		uint32 GenerateGroupId();
        //uint32 GenerateItemTextID();
        uint32 GenerateMailID();
        uint32 GeneratePetNumber();

        void CreateItemText(uint32 guid, std::string text);
        std::string GetItemText( uint32 id )
        {
            ItemTextMap::const_iterator itr = mItemTexts.find( id );
            if ( itr != mItemTexts.end() )
                return itr->second;
            else
                return "There is no info for this item";
        }

        typedef std::multimap<int32, uint32> ExclusiveQuestGroups;
        ExclusiveQuestGroups mExclusiveQuestGroups;

        MailLevelReward const* GetMailLevelReward(uint32 level,uint32 raceMask)
        {
            MailLevelRewardMap::const_iterator map_itr = m_mailLevelRewardMap.find(level);
            if (map_itr == m_mailLevelRewardMap.end())
                return NULL;

            for(MailLevelRewardList::const_iterator set_itr = map_itr->second.begin(); set_itr != map_itr->second.end(); ++set_itr)
                if (set_itr->raceMask & raceMask)
                    return &*set_itr;

            return NULL;
        }

        WeatherZoneChances const* GetWeatherChances(uint32 zone_id) const
        {
            WeatherZoneMap::const_iterator itr = mWeatherZoneMap.find(zone_id);
            if(itr != mWeatherZoneMap.end())
                return &itr->second;
            else
                return NULL;
        }

        CellObjectGuids const& GetCellObjectGuids(uint16 mapid, uint8 spawnMode, uint32 cell_id)
        {
            return mMapObjectGuids[MAKE_PAIR32(mapid,spawnMode)][cell_id];
        }

        CreatureData const* GetCreatureData(uint32 guid) const
        {
            CreatureDataMap::const_iterator itr = mCreatureDataMap.find(guid);
            if(itr==mCreatureDataMap.end()) return NULL;
            return &itr->second;
        }
        CreatureData& NewOrExistCreatureData(uint32 guid) { return mCreatureDataMap[guid]; }
        void DeleteCreatureData(uint32 guid);
        CreatureLocale const* GetCreatureLocale(uint32 entry) const
        {
            CreatureLocaleMap::const_iterator itr = mCreatureLocaleMap.find(entry);
            if(itr==mCreatureLocaleMap.end()) return NULL;
            return &itr->second;
        }
        GameObjectLocale const* GetGameObjectLocale(uint32 entry) const
        {
            GameObjectLocaleMap::const_iterator itr = mGameObjectLocaleMap.find(entry);
            if(itr==mGameObjectLocaleMap.end()) return NULL;
            return &itr->second;
        }
        ItemLocale const* GetItemLocale(uint32 entry) const
        {
            ItemLocaleMap::const_iterator itr = mItemLocaleMap.find(entry);
            if(itr==mItemLocaleMap.end()) return NULL;
            return &itr->second;
        }
        QuestLocale const* GetQuestLocale(uint32 entry) const
        {
            QuestLocaleMap::const_iterator itr = mQuestLocaleMap.find(entry);
            if(itr==mQuestLocaleMap.end()) return NULL;
            return &itr->second;
        }
        NpcTextLocale const* GetNpcTextLocale(uint32 entry) const
        {
            NpcTextLocaleMap::const_iterator itr = mNpcTextLocaleMap.find(entry);
            if(itr==mNpcTextLocaleMap.end()) return NULL;
            return &itr->second;
        }
        PageTextLocale const* GetPageTextLocale(uint32 entry) const
        {
            PageTextLocaleMap::const_iterator itr = mPageTextLocaleMap.find(entry);
            if(itr==mPageTextLocaleMap.end()) return NULL;
            return &itr->second;
        }
        GossipMenuItemsLocale const* GetGossipMenuItemsLocale(uint32 entry) const
        {
            GossipMenuItemsLocaleMap::const_iterator itr = mGossipMenuItemsLocaleMap.find(entry);
            if(itr==mGossipMenuItemsLocaleMap.end()) return NULL;
            return &itr->second;
        }
        PointOfInterestLocale const* GetPointOfInterestLocale(uint32 poi_id) const
        {
            PointOfInterestLocaleMap::const_iterator itr = mPointOfInterestLocaleMap.find(poi_id);
            if(itr==mPointOfInterestLocaleMap.end()) return NULL;
            return &itr->second;
        }

        GameObjectData const* GetGOData(uint32 guid) const
        {
            GameObjectDataMap::const_iterator itr = mGameObjectDataMap.find(guid);
            if(itr==mGameObjectDataMap.end()) return NULL;
            return &itr->second;
        }
        GameObjectData& NewGOData(uint32 guid) { return mGameObjectDataMap[guid]; }
        void DeleteGOData(uint32 guid);

        MangosStringLocale const* GetMangosStringLocale(int32 entry) const
        {
            MangosStringLocaleMap::const_iterator itr = mMangosStringLocaleMap.find(entry);
            if(itr==mMangosStringLocaleMap.end()) return NULL;
            return &itr->second;
        }
        const char *GetMangosString(int32 entry, int locale_idx) const;
        const char *GetMangosStringForDBCLocale(int32 entry) const { return GetMangosString(entry,DBCLocaleIndex); }
        int32 GetDBCLocaleIndex() const { return DBCLocaleIndex; }
        void SetDBCLocaleIndex(uint32 lang) { DBCLocaleIndex = GetIndexForLocale(LocaleConstant(lang)); }

        void AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance);
        void DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid);

        time_t GetCreatureRespawnTime(uint32 loguid, uint32 instance) { return mCreatureRespawnTimes[MAKE_PAIR64(loguid,instance)]; }
        void SaveCreatureRespawnTime(uint32 loguid, uint32 instance, time_t t);
        time_t GetGORespawnTime(uint32 loguid, uint32 instance) { return mGORespawnTimes[MAKE_PAIR64(loguid,instance)]; }
        void SaveGORespawnTime(uint32 loguid, uint32 instance, time_t t);
        void DeleteRespawnTimeForInstance(uint32 instance);

        // grid objects
        void AddCreatureToGrid(uint32 guid, CreatureData const* data);
        void RemoveCreatureFromGrid(uint32 guid, CreatureData const* data);
        void AddGameobjectToGrid(uint32 guid, GameObjectData const* data);
        void RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data);

        // reserved names
        void LoadReservedPlayersNames();
        bool IsReservedName(const std::string& name) const;

        // name with valid structure and symbols
        static uint8 CheckPlayerName( const std::string& name, bool create = false );
        static PetNameInvalidReason CheckPetName( const std::string& name );
        static bool IsValidCharterName( const std::string& name );

        static bool CheckDeclinedNames(std::wstring mainpart, DeclinedName const& names);

        int GetIndexForLocale(LocaleConstant loc);
        LocaleConstant GetLocaleForIndex(int i);

        uint16 GetConditionId(ConditionType condition, uint32 value1, uint32 value2);
        bool IsPlayerMeetToCondition(Player const* player, uint16 condition_id) const
        {
            if(condition_id >= mConditions.size())
                return false;

            return mConditions[condition_id].Meets(player);
        }

        GameTele const* GetGameTele(uint32 id) const
        {
            GameTeleMap::const_iterator itr = m_GameTeleMap.find(id);
            if(itr==m_GameTeleMap.end()) return NULL;
            return &itr->second;
        }
        GameTele const* GetGameTele(const std::string& name) const;
        GameTeleMap const& GetGameTeleMap() const { return m_GameTeleMap; }
        bool AddGameTele(GameTele& data);
        bool DeleteGameTele(const std::string& name);

        uint32 GetNpcGossip(uint32 entry) const
        {
            CacheNpcTextIdMap::const_iterator iter = m_mCacheNpcTextIdMap.find(entry);
            if(iter == m_mCacheNpcTextIdMap.end())
                return 0;

            return iter->second;
        }

        TrainerSpellData const* GetNpcTrainerSpells(uint32 entry) const
        {
            CacheTrainerSpellMap::const_iterator  iter = m_mCacheTrainerSpellMap.find(entry);
            if(iter == m_mCacheTrainerSpellMap.end())
                return NULL;

            return &iter->second;
        }

        VendorItemData const* GetNpcVendorItemList(uint32 entry) const
        {
            CacheVendorItemMap::const_iterator  iter = m_mCacheVendorItemMap.find(entry);
            if(iter == m_mCacheVendorItemMap.end())
                return NULL;

            return &iter->second;
        }
        void AddVendorItem(uint32 entry,uint32 item, uint32 maxcount, uint32 incrtime, uint32 ExtendedCost);
        bool RemoveVendorItem(uint32 entry,uint32 item);
        bool IsVendorItemValid( uint32 vendor_entry, uint32 item, uint32 maxcount, uint32 ptime, uint32 ExtendedCost, Player* pl = NULL, std::set<uint32>* skip_vendors = NULL ) const;

        void LoadScriptNames();
        ScriptNameMap &GetScriptNames() { return m_scriptNames; }
        const char * GetScriptName(uint32 id) { return id < m_scriptNames.size() ? m_scriptNames[id].c_str() : ""; }
        uint32 GetScriptId(const char *name);

        int GetOrNewIndexForLocale(LocaleConstant loc);

        VehicleDataMap mVehicleData;
        VehicleSeatDataMap mVehicleSeatData;

        uint32 GetSeatFlags(uint32 seatid)
        {
            VehicleSeatDataMap::iterator i = mVehicleSeatData.find(seatid);
            if(i == mVehicleSeatData.end())
                return NULL;
            else
                return i->second;
        }
        VehicleDataStructure const* GetVehicleData(uint32 entry) const
        {
            VehicleDataMap::const_iterator itr = mVehicleData.find(entry);
            if(itr==mVehicleData.end()) return NULL;
            return &itr->second;
        }

        SpellClickInfoMapBounds GetSpellClickInfoMapBounds(uint32 creature_id) const
        {
            return SpellClickInfoMapBounds(mSpellClickInfoMap.lower_bound(creature_id),mSpellClickInfoMap.upper_bound(creature_id));
        }

        ItemRequiredTargetMapBounds GetItemRequiredTargetMapBounds(uint32 uiItemEntry) const
        {
            return ItemRequiredTargetMapBounds(m_ItemRequiredTarget.lower_bound(uiItemEntry),m_ItemRequiredTarget.upper_bound(uiItemEntry));
        }

        GossipMenusMapBounds GetGossipMenusMapBounds(uint32 uiMenuId) const
        {
            return GossipMenusMapBounds(m_mGossipMenusMap.lower_bound(uiMenuId),m_mGossipMenusMap.upper_bound(uiMenuId));
        }

        GossipMenuItemsMapBounds GetGossipMenuItemsMapBounds(uint32 uiMenuId) const
        {
            return GossipMenuItemsMapBounds(m_mGossipMenuItemsMap.lower_bound(uiMenuId),m_mGossipMenuItemsMap.upper_bound(uiMenuId));
        }

		// fss outdoor
		GraveYardMap        mGraveYardMap;
		uint32 AddGOData(uint32 entry, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay = 0, float rotation0 = 0, float rotation1 = 0, float rotation2 = 0, float rotation3 = 0);
        uint32 AddCreData(uint32 entry, uint32 team, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay = 0);
		void RemoveGraveYardLink(uint32 id, uint32 zone, uint32 team, bool inDB = false);
    protected:

        // first free id for selected id type
        uint32 m_arenaTeamId;
        uint32 m_auctionid;
		uint32 m_calendareventid;
        uint64 m_equipmentSetGuid;
        uint32 m_guildId;
		uint32 m_groupId;
        uint32 m_mailid;
        uint32 m_hiPetNumber;

        // first free low guid for seelcted guid type
        uint32 m_hiCharGuid;
        uint32 m_hiCreatureGuid;
        uint32 m_hiItemGuid;
        uint32 m_hiGoGuid;
        uint32 m_hiCorpseGuid;
		uint32 m_hiVehicleGuid;

        QuestMap            mQuestTemplates;

        typedef UNORDERED_MAP<uint32, GossipText> GossipTextMap;
        typedef UNORDERED_MAP<uint32, uint32> QuestAreaTriggerMap;
        typedef UNORDERED_MAP<uint32, std::string> ItemTextMap;
        typedef std::set<uint32> TavernAreaTriggerSet;
        typedef std::set<uint32> GameObjectForQuestSet;

        GroupMap            mGroupMap;
        GuildMap            mGuildMap;
        ArenaTeamMap        mArenaTeamMap;
		AccessRequirementMap  mAccessRequirements; // Merging

        ItemTextMap         mItemTexts;

        QuestAreaTriggerMap mQuestAreaTriggerMap;
        TavernAreaTriggerSet mTavernAreaTriggerSet;
        GameObjectForQuestSet mGameObjectForQuestSet;
        GossipTextMap       mGossipText;
        AreaTriggerMap      mAreaTriggers;
        AreaTriggerScriptMap  mAreaTriggerScripts;

        RepOnKillMap        mRepOnKill;

        GossipMenusMap      m_mGossipMenusMap;
        GossipMenuItemsMap  m_mGossipMenuItemsMap;
        PointOfInterestMap  mPointsOfInterest;

        QuestPOIMap         mQuestPOIMap;

        WeatherZoneMap      mWeatherZoneMap;

        //character reserved names
        typedef std::set<std::wstring> ReservedNamesMap;
        ReservedNamesMap    m_ReservedNames;

		// for wintergrasp
		//GraveYardMap        mGraveYardMap;

        GameTeleMap         m_GameTeleMap;

        ScriptNameMap       m_scriptNames;

        SpellClickInfoMap   mSpellClickInfoMap;

        ItemRequiredTargetMap m_ItemRequiredTarget;

        typedef             std::vector<LocaleConstant> LocalForIndex;
        LocalForIndex        m_LocalForIndex;

        int DBCLocaleIndex;

    private:
        void LoadScripts(ScriptMapMap& scripts, char const* tablename);
        void CheckScripts(ScriptMapMap const& scripts,std::set<int32>& ids);
        void LoadCreatureAddons(SQLStorage& creatureaddons, char const* entryName, char const* comment);
        void ConvertCreatureAddonAuras(CreatureDataAddon* addon, char const* table, char const* guidEntryStr);
        void LoadQuestRelationsHelper(QuestRelations& map,char const* table);

        MailLevelRewardMap m_mailLevelRewardMap;

        typedef std::map<uint32,PetLevelInfo*> PetLevelInfoMap;
        // PetLevelInfoMap[creature_id][level]
        PetLevelInfoMap petInfo;                            // [creature_id][level]

        PlayerClassInfo playerClassInfo[MAX_CLASSES];

        void BuildPlayerLevelInfo(uint8 race, uint8 class_, uint8 level, PlayerLevelInfo* plinfo) const;
        PlayerInfo playerInfo[MAX_RACES][MAX_CLASSES];

        typedef std::vector<uint32> PlayerXPperLevel;       // [level]
        PlayerXPperLevel mPlayerXPperLevel;

        typedef std::map<uint32,uint32> BaseXPMap;          // [area level][base xp]
        BaseXPMap mBaseXPTable;

        typedef std::map<uint32,int32> FishingBaseSkillMap; // [areaId][base skill level]
        FishingBaseSkillMap mFishingBaseForArea;

        typedef std::map<uint32,std::vector<std::string> > HalfNameMap;
        HalfNameMap PetHalfName0;
        HalfNameMap PetHalfName1;

        MapObjectGuids mMapObjectGuids;
        CreatureDataMap mCreatureDataMap;
        CreatureLocaleMap mCreatureLocaleMap;
        GameObjectDataMap mGameObjectDataMap;
        GameObjectLocaleMap mGameObjectLocaleMap;
        ItemLocaleMap mItemLocaleMap;
        QuestLocaleMap mQuestLocaleMap;
        NpcTextLocaleMap mNpcTextLocaleMap;
        PageTextLocaleMap mPageTextLocaleMap;
        MangosStringLocaleMap mMangosStringLocaleMap;
        GossipMenuItemsLocaleMap mGossipMenuItemsLocaleMap;
        PointOfInterestLocaleMap mPointOfInterestLocaleMap;
        RespawnTimes mCreatureRespawnTimes;
        RespawnTimes mGORespawnTimes;

        // Storage for Conditions. First element (index 0) is reserved for zero-condition (nothing required)
        typedef std::vector<PlayerCondition> ConditionStore;
        ConditionStore mConditions;

        CacheNpcTextIdMap m_mCacheNpcTextIdMap;
        CacheVendorItemMap m_mCacheVendorItemMap;
        CacheTrainerSpellMap m_mCacheTrainerSpellMap;
};

#define sObjectMgr MaNGOS::Singleton<ObjectMgr>::Instance()

// scripting access functions
MANGOS_DLL_SPEC bool LoadMangosStrings(DatabaseType& db, char const* table,int32 start_value = MAX_CREATURE_AI_TEXT_STRING_ID, int32 end_value = std::numeric_limits<int32>::min());
MANGOS_DLL_SPEC uint32 GetAreaTriggerScriptId(uint32 trigger_id);
MANGOS_DLL_SPEC uint32 GetScriptId(const char *name);
MANGOS_DLL_SPEC ObjectMgr::ScriptNameMap& GetScriptNames();
MANGOS_DLL_SPEC CreatureInfo const* GetCreatureTemplateStore(uint32 entry);
MANGOS_DLL_SPEC Quest const* GetQuestTemplateStore(uint32 entry);

#endif
