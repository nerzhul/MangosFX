#include <Player.h>
#include "cBattleGround.h"
#include "ccBattleGroundRV.h"
#include <GameObject.h>
#include <Language.h>

cBattleGroundRV::cBattleGroundRV()
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

cBattleGroundRV::~cBattleGroundRV()
{
	
}

void cBattleGroundRV::Update(uint32 diff)
{
    cBattleGround::Update(diff);
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // teleport buggers
        if(m_uiTeleport < diff)
        {
            for(BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
			 {
			 /*Player * plr = sObjectMgr.GetPlayer(itr->first);
			 if (plr && plr->GetPositionZ() < 27)
			 plr->TeleportTo(618, plr->GetPositionX(), plr->GetPositionY(), 29, plr->GetOrientation(), false);
			 if (plr && plr->GetPositionZ() < 27)
			 plr->TeleportTo(618, plr->GetPositionX(), plr->GetPositionY(), 29, plr->GetOrientation(), false);*/
			 }
            m_uiTeleport = 1000;
        }
        else
            m_uiTeleport -= diff;
    }
}

void cBattleGroundRV::StartingEventCloseDoors()
{
}

void cBattleGroundRV::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
}

void cBattleGroundRV::AddPlayer(Player *plr)
{
    cBattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    cBattleGroundRVScore* sc = new cBattleGroundRVScore;
	
    // m_PlayerScores[plr->GetGUID()] = sc; export this
	
    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
}

void cBattleGroundRV::RemovePlayer(Player * /*plr*/, uint64 /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;
	
    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
	
    CheckArenaWinConditions();
}

void cBattleGroundRV::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    if (!killer)
    {
        sLog.outError("cBattleGroundRV: Killer player not found");
        return;
    }
	
    cBattleGround::HandleKillPlayer(player, killer);
	
    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
	
    CheckArenaWinConditions();
}

bool cBattleGroundRV::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(), 763.5f, -284, 28.276f, player->GetOrientation(), false);
    return true;
}

void cBattleGroundRV::HandleAreaTrigger(Player * Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    switch(Trigger)
    {
        case 5224:
        case 5226:
        case 5473:
        case 5474:
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

void cBattleGroundRV::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    FillInitialWorldState(data, count, 0xe10, GetAlivePlayersCountByTeam(HORDE));
    FillInitialWorldState(data, count, 0xe1a, 1);
}

void cBattleGroundRV::Reset()
{
    //call parent's class reset
    cBattleGround::Reset();
    m_uiTeleport = 22000;
}

bool cBattleGroundRV::SetupBattleGround()
{
    return true;
}
