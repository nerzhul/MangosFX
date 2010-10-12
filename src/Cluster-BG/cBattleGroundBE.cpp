#include <Player.h>
#include "cBattleGround.h"
#include "cBattleGroundBE.h"
#include <Language.h>

cBattleGroundBE::cBattleGroundBE()
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

cBattleGroundBE::~cBattleGroundBE()
{
	
}

void cBattleGroundBE::Update(uint32 diff)
{
    cBattleGround::Update(diff);
	
    /*if (GetStatus() == STATUS_IN_PROGRESS)
	 {
	 // update something
	 }*/
}

void cBattleGroundBE::StartingEventCloseDoors()
{
}

void cBattleGroundBE::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
}

void cBattleGroundBE::AddPlayer(Player *plr)
{
    cBattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    cBattleGroundBEScore* sc = new cBattleGroundBEScore;
	
    //m_PlayerScores[plr->GetGUID()] = sc; export this
	
    UpdateArenaWorldState();
}

void cBattleGroundBE::RemovePlayer(Player* /*plr*/, uint64 /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;
	
    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void cBattleGroundBE::HandleKillPlayer(Player *player, Player *killer)
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

bool cBattleGroundBE::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(),6238.930176,262.963470,0.889519,player->GetOrientation(),false);
    return true;
}

void cBattleGroundBE::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
	
    //uint32 SpellId = 0;
    //uint64 buff_guid = 0;
    switch(Trigger)
    {
        case 4538:                                          // buff trigger?
            //buff_guid = m_BgObjects[BG_BE_OBJECT_BUFF_1];
            break;
        case 4539:                                          // buff trigger?
            //buff_guid = m_BgObjects[BG_BE_OBJECT_BUFF_2];
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
	
    //if (buff_guid)
    //    HandleTriggerBuff(buff_guid,Source);
}

void cBattleGroundBE::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0x9f1, GetAlivePlayersCountByTeam(ALLIANCE));
	FillInitialWorldState(data, count, 0x9f0, GetAlivePlayersCountByTeam(HORDE));
	FillInitialWorldState(data, count, 0x9f3, 1);
}

void cBattleGroundBE::Reset()
{
    //call parent's class reset
    cBattleGround::Reset();
}

bool cBattleGroundBE::SetupBattleGround()
{
    return true;
}

void cBattleGroundBE::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{
	
	 BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetGUID());
	 if(itr == m_PlayerScores.end())                         // player not found...
	 return;
	
    //there is nothing special in this score
    cBattleGround::UpdatePlayerScore(Source,type,value);
	
}