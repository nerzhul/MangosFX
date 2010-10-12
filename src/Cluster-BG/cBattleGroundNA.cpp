#include <Player.h>
#include "cBattleGround.h"
#include "cBattleGroundNA.h"
#include <Language.h>

cBattleGroundNA::cBattleGroundNA()
{
    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

cBattleGroundNA::~cBattleGroundNA()
{
	
}

void cBattleGroundNA::Update(uint32 diff)
{
    cBattleGround::Update(diff);
	
    /*if (GetStatus() == STATUS_IN_PROGRESS)
	 {
	 // update something
	 }*/
}

void cBattleGroundNA::StartingEventCloseDoors()
{
}

void cBattleGroundNA::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
}

void cBattleGroundNA::AddPlayer(Player *plr)
{
    cBattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundNAScore* sc = new cBattleGroundNAScore;
	
    // m_PlayerScores[plr->GetGUID()] = sc; export this
	
    UpdateArenaWorldState();
}

void cBattleGroundNA::RemovePlayer(Player* /*plr*/, uint64 /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;
	
    UpdateArenaWorldState();
	CheckArenaWinConditions();
}

void cBattleGroundNA::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    if (!killer)
    {
        sLog.outError("cBattleGroundNA: Killer player not found");
        return;
    }
	
    BattleGround::HandleKillPlayer(player,killer);
	
    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

bool cBattleGroundNA::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(),4055.504395,2919.660645,13.611241,player->GetOrientation(),false);
    return true;
}

void cBattleGroundNA::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    //uint32 SpellId = 0;
    //uint64 buff_guid = 0;
    switch(Trigger)
    {
        case 4536:                                          // buff trigger?
        case 4537:                                          // buff trigger?
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
	
    //if (buff_guid)
    //    HandleTriggerBuff(buff_guid,Source);
}

void cBattleGroundNA::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xa0f, GetAlivePlayersCountByTeam(ALLIANCE));
	FillInitialWorldState(data, count, 0xa10, GetAlivePlayersCountByTeam(HORDE));
	FillInitialWorldState(data, count, 0xa11, 1);
}

void cBattleGroundNA::Reset()
{
    //call parent's class reset
    cBattleGround::Reset();
}

bool cBattleGroundNA::SetupBattleGround()
{
    return true;
}

