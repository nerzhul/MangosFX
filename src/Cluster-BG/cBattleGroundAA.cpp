#include <Player.h>
#include "cBattleGround.h"
#include "cBattleGroundAA.h"
#include <Language.h>

cBattleGroundAA::cBattleGroundAA()
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

cBattleGroundAA::~cBattleGroundAA()
{

}

void cBattleGroundAA::Update(uint32 diff)
{
    cBattleGround::Update(diff);
}

void cBattleGroundAA::StartingEventCloseDoors()
{
}

void cBattleGroundAA::StartingEventOpenDoors()
{
}

void cBattleGroundAA::AddPlayer(Player *plr)
{
    cBattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundAAScore* sc = new BattleGroundAAScore;

    //m_PlayerScores[plr->GetGUID()] = sc; export this
}

void cBattleGroundAA::RemovePlayer(Player * /*plr*/, uint64 /*guid*/)
{
}

void cBattleGroundAA::HandleKillPlayer(Player* player, Player* killer)
{
   cBattleGround::HandleKillPlayer(player, killer);
}

void cBattleGroundAA::HandleAreaTrigger(Player * /*Source*/, uint32 /*Trigger*/)
{
}

bool cBattleGroundAA::SetupBattleGround()
{
    return true;
}
