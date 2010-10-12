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

#include <Player.h>
#include "cBattleGround.h"
#include "cBattleGroundABG.h"
#include <Language.h>

cBattleGroundABG::cBattleGroundABG()
{
    //TODO FIX ME!
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_WS_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;
}

cBattleGroundABG::~cBattleGroundABG()
{

}

void cBattleGroundABG::Update(uint32 diff)
{
    cBattleGround::Update(diff);
}

void cBattleGroundABG::StartingEventCloseDoors()
{
}

void cBattleGroundABG::StartingEventOpenDoors()
{
}

void cBattleGroundABG::AddPlayer(Player *plr)
{
    cBattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundABGScore* sc = new BattleGroundABGScore;

    //m_PlayerScores[plr->GetGUID()] = sc; export this
}

void cBattleGroundABG::RemovePlayer(Player* /*plr*/,uint64 /*guid*/)
{

}

void cBattleGroundABG::HandleAreaTrigger(Player * /*Source*/, uint32 /*Trigger*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
}

void cBattleGroundABG::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{

	/*BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetGUID());

    if(itr == m_PlayerScores.end())                         // player not found...
        return;*/

    cBattleGround::UpdatePlayerScore(Source,type,value);
}
