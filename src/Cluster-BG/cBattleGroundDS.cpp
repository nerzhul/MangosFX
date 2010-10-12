#include <Player.h>
#include "cBattleGround.h"
#include "cBattleGroundDS.h"
#include <Language.h>

cBattleGroundDS::cBattleGroundDS()
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

cBattleGroundDS::~cBattleGroundDS()
{
}

void cBattleGroundDS::Update(uint32 diff)
{
    cBattleGround::Update(diff);
	if (GetStatus() == STATUS_IN_PROGRESS)
	{
		// knockback
		if(m_uiKnockback < diff)
		{
    		for(BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
			 {
			 /*Player * plr = sObjectMgr.GetPlayer(itr->firstitr);
			 if (plr && plr->IsWithinLOS(1214,765,14) && plr->GetDistance2d(1214,765) <= 50)
			 plr->KnockBackPlayerWithAngle(6.40f,55,7);
			 if (plr && plr->IsWithinLOS(1369,817,14) && plr->GetDistance2d(1369,817) <= 50)
			 plr->KnockBackPlayerWithAngle(3.03f,55,7);*/
			 }
			m_uiKnockback = 1000;
		}
		else
			m_uiKnockback -= diff;
	}
}

void cBattleGroundDS::StartingEventCloseDoors()
{
}

void cBattleGroundDS::StartingEventOpenDoors()
{
	OpenDoorEvent(BG_EVENT_DOOR);
}

void cBattleGroundDS::AddPlayer(Player *plr)
{
    cBattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundDSScore* sc = new cBattleGroundDSScore;
	
	UpdateArenaWorldState();
	
    //m_PlayerScores[plr->GetGUID()] = sc; export this
}

void cBattleGroundDS::RemovePlayer(Player * /*plr*/, uint64 /*guid*/)
{
	if (GetStatus() == STATUS_WAIT_LEAVE)
        return;
	
    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void cBattleGroundDS::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    if (!killer)
    {
        sLog.outError("cBattleGroundDS: Killer player not found");
        return;
    }
	
    cBattleGround::HandleKillPlayer(player,killer);
	
    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void cBattleGroundDS::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    switch(Trigger)
    {
        case 5347:
        case 5348:
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

bool cBattleGroundDS::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(), 1299.046, 784.825, 9.338, 2.422, false);
    return true;
}

void cBattleGroundDS::FillInitialWorldStates(WorldPacket &data)
{
    data << uint32(3610) << uint32(1);           // 9
    UpdateArenaWorldState();
}

void cBattleGroundDS::Reset(Player * /*Source*/, uint32 /*Trigger*/)
{
	cBattleGround::Reset();
	m_uiKnockback = 5000;
}

bool cBattleGroundDS::SetupBattleGround()
{
    return true;
}
