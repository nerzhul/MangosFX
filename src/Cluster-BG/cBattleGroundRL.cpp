#include <Player.h>
#include "cBattleGround.h"
#include "ccBattleGroundRL.h"
#include <WorldPacket.h>

cBattleGroundRL::cBattleGroundRL()
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

cBattleGroundRL::~cBattleGroundRL()
{
	
}

void cBattleGroundRL::Update(uint32 diff)
{
    cBattleGround::Update(diff);
	
    /*if (GetStatus() == STATUS_IN_PROGRESS)
	 {
	 // update something
	 }*/
}

void cBattleGroundRL::StartingEventCloseDoors()
{
}

void cBattleGroundRL::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
}

void cBattleGroundRL::AddPlayer(Player *plr)
{
    cBattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    cBattleGroundRLScore* sc = new cBattleGroundRLScore;
	
    //m_PlayerScores[plr->GetGUID()] = sc; export this
	
    UpdateArenaWorldState();
}

void cBattleGroundRL::RemovePlayer(Player* /*plr*/, uint64 /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;
	
    UpdateArenaWorldState();
	
    CheckArenaWinConditions();
}

void cBattleGroundRL::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    if (!killer)
    {
        sLog.outError("Killer player not found");
        return;
    }
	
    cBattleGround::HandleKillPlayer(player,killer);
	
    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

bool cBattleGroundRL::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(),1285.810547,1667.896851,39.957642,player->GetOrientation(),false);
    return true;
}

void cBattleGroundRL::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    //uint32 SpellId = 0;
    //uint64 buff_guid = 0;
    switch(Trigger)
    {
        case 4696:                                          // buff trigger?
        case 4697:                                          // buff trigger?
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
	
    //if (buff_guid)
    //    HandleTriggerBuff(buff_guid,Source);
}

void cBattleGroundRL::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xbb8, GetAlivePlayersCountByTeam(ALLIANCE));
	FillInitialWorldState(data, count, 0xbb9, GetAlivePlayersCountByTeam(HORDE));
	FillInitialWorldState(data, count, 0xbba, 1);
}

void cBattleGroundRL::Reset()
{
    //call parent's reset
    cBattleGround::Reset();
}

bool cBattleGroundRL::SetupBattleGround()
{
    return true;
}