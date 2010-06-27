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

#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundSA.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"

BattleGroundSA::BattleGroundSA()
{
    //TODO FIX ME!
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SA_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_SA_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_SA_HAS_BEGUN;
    TimerEnabled = false;
	for(uint8 i=0;i<BG_SA_MAX_GATES;i++)
		GatesGUID[i] = 0;
	TurretSet.clear();
	NWDemolisherSet.clear();
	NEDemolisherSet.clear();
	SWDemolisherSet.clear();
	SEDemolisherSet.clear();
	BoatSet[0].clear();
	BoatSet[1].clear();
	for(uint8 i=0;i<BG_SA_MAX_GATES-1;i++)
		SigilGUID[i] = 0;

	TitanRelicGUID = 0;
	GobelinGUID[0] = GobelinGUID[1] = 0;
}

BattleGroundSA::~BattleGroundSA()
{

}

void BattleGroundSA::Reset()
{
	BattleGround::Reset();

    attackers = ( (urand(0,1)) ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE);
    for(uint8 i = 0; i <= 5; i++)
    {
        GateStatus[i] = BG_SA_GATE_OK;
    }
	TotalTime = 0;
	round = 0;
    ShipsStarted = false;
	OnLeftBoat = true;
    status = BG_SA_WARMUP;
}

void BattleGroundSA::InitAllObjects()
{
	uint32 defFaction = (attackers == BG_TEAM_ALLIANCE) ? BG_SA_Factions[BG_TEAM_HORDE] : BG_SA_Factions[BG_TEAM_ALLIANCE];
	uint32 attFaction = BG_SA_Factions[attackers];

	// Turrets to defender
	for(GUIDSet::iterator itr = TurretSet.begin(); itr != TurretSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			if(!cr->isAlive())
				cr->Respawn();

			cr->setFaction(defFaction);
			cr->SetHealth(cr->GetMaxHealth());
		}

	for(GUIDSet::iterator itr = NWDemolisherSet.begin(); itr != NWDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{	
			cr->Respawn();
			cr->SetPhaseMask(1,true);
			cr->setFaction(attFaction);
		}

	for(GUIDSet::iterator itr = NEDemolisherSet.begin(); itr != NEDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			cr->Respawn();
			cr->SetPhaseMask(1,true);
			cr->setFaction(attFaction);
		}

	for(GUIDSet::iterator itr = SWDemolisherSet.begin(); itr != SWDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			cr->Respawn();
			cr->SetPhaseMask(2,true);
			cr->setFaction(attFaction);
		}

	for(GUIDSet::iterator itr = SEDemolisherSet.begin(); itr != SEDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			cr->Respawn();
			cr->SetPhaseMask(2,true);
			cr->setFaction(attFaction);
		}

	for(GUIDSet::iterator itr = BoatSet[BG_TEAM_ALLIANCE].begin(); itr != BoatSet[BG_TEAM_ALLIANCE].end(); ++itr)
		if(GameObject* go = GetBgMap()->GetGameObject(*itr))
		{
			DoorClose(go->GetGUID());
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(2,true);
			else
				go->SetPhaseMask(2,true);
		}

	for(GUIDSet::iterator itr = BoatSet[BG_TEAM_HORDE].begin(); itr != BoatSet[BG_TEAM_HORDE].end(); ++itr)
		if(GameObject* go = GetBgMap()->GetGameObject(*itr))
		{
			DoorClose(go->GetGUID());
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(2,true);
			else
				go->SetPhaseMask(1,true);
		}

	for(uint8 i=0;i<BG_SA_MAX_GATES;i++)
		if(GameObject* go = GetBgMap()->GetGameObject(GatesGUID[i]))
		{
			go->Respawn();
			go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
		}

	for(uint8 i=0;i<BG_SA_MAX_GATES-1;i++)
		if(GameObject* go = GetBgMap()->GetGameObject(SigilGUID[i]))
		{
			go->Respawn();
			go->SetPhaseMask(1,true);
		}

	if(GameObject* go = GetBgMap()->GetGameObject(TitanRelicGUID))
		go->SetUInt32Value(GAMEOBJECT_FACTION, attFaction);

	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(GobelinGUID[0]))
		cr->setFaction(defFaction);

	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(GobelinGUID[1]))
		cr->setFaction(defFaction);

}	

void BattleGroundSA::Update(uint32 diff)
{
    BattleGround::Update(diff);

	if(round > 1)
		return;

    TotalTime += diff;

    if(status == BG_SA_WARMUP || status == BG_SA_SECOND_WARMUP)
    {
        if(TotalTime >= BG_SA_WARMUPLENGTH)
        {
            TotalTime = 0;
            ToggleTimer();
            status = (status == BG_SA_WARMUP) ? BG_SA_ROUND_ONE : BG_SA_ROUND_TWO;
        }
        if(TotalTime >= BG_SA_BOAT_START)
            StartShips();
        return;
    }
    else if(status == BG_SA_ROUND_ONE)
    {
        if(TotalTime >= BG_SA_ROUNDLENGTH)
        {
			TotalTime = BG_SA_ROUNDLENGTH;
            EndRound();
            return;
        }
    }
    else if(status == BG_SA_ROUND_TWO)
    {
        if(TotalTime >= BG_SA_ROUNDLENGTH)
        {
			TotalTime = BG_SA_ROUNDLENGTH;
			EndRound();
            return;
        }
    }

	UpdateTimer();
}

void BattleGroundSA::UpdateTimer()
{
	if(status == BG_SA_ROUND_ONE || status == BG_SA_ROUND_TWO)
    {
        //Send Time
        uint32 end_of_round = (BG_SA_ROUNDLENGTH - TotalTime);
        UpdateWorldState(BG_SA_TIMER_MINS, end_of_round/60000);
        UpdateWorldState(BG_SA_TIMER_SEC_TENS, (end_of_round%60000)/10000);
        UpdateWorldState(BG_SA_TIMER_SEC_DECS, ((end_of_round%60000)%10000)/1000);
    }
}

void BattleGroundSA::RelocatePlayers()
{
	for(BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end();++itr)
	{
		if(Player* p = sObjectMgr.GetPlayer(itr->first))
		{
			// TODO : play sound
			p->ResurrectPlayer(100.0f);
			TeleportPlayer(p);
		}
	}
}

void BattleGroundSA::EndRound()
{
	RoundScores[round].time = TotalTime;
    
    ToggleTimer();
	if(round)
	{
		// define winner
		if(RoundScores[round].time < BG_SA_ROUNDLENGTH)
			RoundScores[round].winner = attackers;
		else
			RoundScores[round].winner = (attackers == BG_TEAM_ALLIANCE) ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;

		// define the winter of the BG
		if(RoundScores[0].time < RoundScores[1].time)
			EndBattleGround(RoundScores[0].winner == BG_TEAM_ALLIANCE ? ALLIANCE : HORDE);
		else
			EndBattleGround(RoundScores[1].winner == BG_TEAM_ALLIANCE ? ALLIANCE : HORDE);
	}
	else
	{
		// define time & winner of the round
		TotalTime = 0;
		if(RoundScores[round].time < BG_SA_ROUNDLENGTH)
			RoundScores[round].winner = attackers;
		else
			RoundScores[round].winner = (attackers == BG_TEAM_ALLIANCE) ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;

		// Reinit
		InitAllObjects();
		status = BG_SA_SECOND_WARMUP;
		attackers = (attackers == BG_TEAM_ALLIANCE) ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
		
	}
	round++;
}

void BattleGroundSA::StartingEventCloseDoors()
{	
	InitAllObjects();
}

void BattleGroundSA::StartingEventOpenDoors()
{
	status = BG_SA_ROUND_ONE;
}

void BattleGroundSA::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundSAScore* sc = new BattleGroundSAScore;

    m_PlayerScores[plr->GetGUID()] = sc;

	TeleportPlayer(plr);
}

void BattleGroundSA::TeleportPlayer(Player* plr)
{
	if(plr->GetBGTeam() == attackers)
	{
		uint8 boat = OnLeftBoat ? 1 : 0;
		if(status == BG_SA_ROUND_ONE || status == BG_SA_ROUND_ONE)
		{
			if(OnLeftBoat)
				plr->TeleportTo(607,1602.27f,-99.248f,8.873f,4.109f);
			else
				plr->TeleportTo(607,1612.16f,44.134f,7.579f,2.352f);
		}
		else
		{
			if(OnLeftBoat)
				plr->TeleportTo(607,2686.42f,-829.99f,18.092f,2.630f);
			else
				plr->TeleportTo(607,2578.95f,986.802f,16.991f,4.001f);
		}

		OnLeftBoat = !OnLeftBoat;
	}
	else
		plr->TeleportTo(607,1198.54f,-66.007f,70.084f,3.194f);
}

void BattleGroundSA::RemovePlayer(Player* /*plr*/,uint64 /*guid*/)
{

}

void BattleGroundSA::HandleAreaTrigger(Player * /*Source*/, uint32 /*Trigger*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
}

void BattleGroundSA::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{

    BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetGUID());
    if(itr == m_PlayerScores.end())                         // player not found...
        return;
    if(type == SCORE_DESTROYED_DEMOLISHER)
		((BattleGroundSAScore*)itr->second)->demolishers_destroyed += value;
    else if(type == SCORE_DESTROYED_WALL)
		((BattleGroundSAScore*)itr->second)->gates_destroyed += value;
    
    BattleGround::UpdatePlayerScore(Source,type,value);

}
void BattleGroundSA::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    uint32 ally_attacks = uint32(attackers == BG_TEAM_ALLIANCE ? 1 : 0);
    uint32 horde_attacks = uint32(attackers == BG_TEAM_HORDE ? 1 : 0);

	FillInitialWorldState(data,count, uint32(BG_SA_ANCIENT_GATEWS), uint32(GateStatus[BG_SA_ANCIENT_GATE]));
    FillInitialWorldState(data,count, uint32(BG_SA_YELLOW_GATEWS), uint32(GateStatus[BG_SA_YELLOW_GATE]));
    FillInitialWorldState(data,count, uint32(BG_SA_GREEN_GATEWS), uint32(GateStatus[BG_SA_GREEN_GATE]));
    FillInitialWorldState(data,count, uint32(BG_SA_BLUE_GATEWS), uint32(GateStatus[BG_SA_BLUE_GATE]));
    FillInitialWorldState(data,count, uint32(BG_SA_RED_GATEWS), uint32(GateStatus[BG_SA_RED_GATE]));
    FillInitialWorldState(data,count, uint32(BG_SA_PURPLE_GATEWS), uint32(GateStatus[BG_SA_PURPLE_GATE]));

    FillInitialWorldState(data,count, uint32(BG_SA_BONUS_TIMER), uint32(0));

    FillInitialWorldState(data,count, uint32(BG_SA_HORDE_ATTACKS), horde_attacks);
    FillInitialWorldState(data,count, uint32(BG_SA_ALLY_ATTACKS), ally_attacks);

    //Time will be sent on first update...
    FillInitialWorldState(data,count, uint32(BG_SA_ENABLE_TIMER), ((TimerEnabled) ? uint32(1) : uint32(0)));
    FillInitialWorldState(data,count, uint32(BG_SA_TIMER_MINS), uint32(0));
    FillInitialWorldState(data,count, uint32(BG_SA_TIMER_SEC_TENS), uint32(0));
    FillInitialWorldState(data,count, uint32(BG_SA_TIMER_SEC_DECS), uint32(0));

    FillInitialWorldState(data,count, uint32(BG_SA_RIGHT_GY_HORDE), uint32(GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == BG_TEAM_HORDE?1:0 ));
    FillInitialWorldState(data,count, uint32(BG_SA_LEFT_GY_HORDE), uint32(GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == BG_TEAM_HORDE?1:0 ));
    FillInitialWorldState(data,count, uint32(BG_SA_CENTER_GY_HORDE), uint32(GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == BG_TEAM_HORDE?1:0 ));

    FillInitialWorldState(data,count, uint32(BG_SA_RIGHT_GY_ALLIANCE), uint32(GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == BG_TEAM_ALLIANCE?1:0 ));
    FillInitialWorldState(data,count, uint32(BG_SA_LEFT_GY_ALLIANCE), uint32(GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == BG_TEAM_ALLIANCE?1:0 ));
    FillInitialWorldState(data,count, uint32(BG_SA_CENTER_GY_ALLIANCE), uint32(GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == BG_TEAM_ALLIANCE?1:0 ));

    FillInitialWorldState(data,count, uint32(BG_SA_HORDE_DEFENCE_TOKEN), ally_attacks);
    FillInitialWorldState(data,count, uint32(BG_SA_ALLIANCE_DEFENCE_TOKEN), horde_attacks);

    FillInitialWorldState(data,count, uint32(BG_SA_LEFT_ATT_TOKEN_HRD), horde_attacks);
    FillInitialWorldState(data,count, uint32(BG_SA_RIGHT_ATT_TOKEN_HRD), horde_attacks);
    FillInitialWorldState(data,count, uint32(BG_SA_RIGHT_ATT_TOKEN_ALL),  ally_attacks);
    FillInitialWorldState(data,count, uint32(BG_SA_LEFT_ATT_TOKEN_ALL),  ally_attacks);
}

void BattleGroundSA::ToggleTimer()
{
    TimerEnabled = !TimerEnabled;
    UpdateWorldState(BG_SA_ENABLE_TIMER, (TimerEnabled) ? 1 : 0);
}

void BattleGroundSA::StartShips()
{
	if(ShipsStarted)
		return;

	for(GUIDSet::iterator itr = BoatSet[attackers].begin(); itr != BoatSet[attackers].end(); ++itr)
	{
		DoorOpen(*itr);
		for(BattleGroundPlayerMap::const_iterator itr2 = GetPlayers().begin(); itr2 != GetPlayers().end();++itr2)
		{
			if(Player* p = sObjectMgr.GetPlayer(itr2->first))
			{
				if(p->GetBGTeam() != attackers)
					continue;

				UpdateData data;
				WorldPacket pkt;
				if(GameObject* go = GetBgMap()->GetGameObject(*itr))
					go->BuildValuesUpdateBlockForPlayer(&data, p);
				data.BuildPacket(&pkt);
				p->GetSession()->SendPacket(&pkt);
			}
		}
	}
	  
	ShipsStarted = true;
}

bool BattleGroundSA::SetupBattleGround()
{
	return true;
}

void BattleGroundSA::HandleKillUnit(Creature *unit, Player *killer)
{
}

void BattleGroundSA::EventPlayerClickedOnFlag(Player *Source, GameObject *target_obj)
{
	switch(target_obj->GetEntry())
	{
		// titan relic
		case 192834:
			EndRound();
			break;
		// graveyards
		// left
		case 191307:
		case 191308:
			break;
		// right
		case 191305:
		case 191306:
			break;
		// central
		case 191309:
		case 191310:
			break;
	}
}

void BattleGroundSA::EndBattleGround(uint32 winner)
{
}

uint32 BattleGroundSA::GetGateIDFromDestroyEventID(uint32 id)
{
    uint32 i = 0;
    switch(id)
    {
        case 19046: i = BG_SA_GREEN_GATE;   break; //Green gate destroyed
        case 19045: i = BG_SA_BLUE_GATE;    break; //blue gate
        case 19047: i = BG_SA_RED_GATE;     break; //red gate
        case 19048: i = BG_SA_PURPLE_GATE;  break; //purple gate
        case 19049: i = BG_SA_YELLOW_GATE;  break; //yellow gate
        case 19837: i = BG_SA_ANCIENT_GATE; break; //ancient gate
    }
    return i;
}

uint32 BattleGroundSA::GetWorldStateFromGateID(uint32 id)
{
	uint32 uws = 0;
    switch(id)
    {
        case BG_SA_GREEN_GATE:   uws = BG_SA_GREEN_GATEWS;   break;
        case BG_SA_YELLOW_GATE:  uws = BG_SA_YELLOW_GATEWS;  break;
        case BG_SA_BLUE_GATE:    uws = BG_SA_BLUE_GATEWS;    break;
        case BG_SA_RED_GATE:     uws = BG_SA_RED_GATEWS;     break;
        case BG_SA_PURPLE_GATE:  uws = BG_SA_PURPLE_GATEWS;  break;
        case BG_SA_ANCIENT_GATE: uws = BG_SA_ANCIENT_GATEWS; break;
    }
    return uws;
}

void BattleGroundSA::OnCreatureCreate(Creature* cr)
{
	uint32 defFaction = (attackers == BG_TEAM_ALLIANCE) ? BG_SA_Factions[BG_TEAM_HORDE] : BG_SA_Factions[BG_TEAM_ALLIANCE];
	uint32 attFaction = BG_SA_Factions[attackers];

	switch(cr->GetEntry())
	{
		case 27894:
			TurretSet.insert(cr->GetGUID());
			cr->setFaction(defFaction);
			break;
		case 28781:
			if(cr->GetDistance2d(1611.597656,-117.270073) < 3.0f || cr->GetDistance2d(1575.562500,-158.421875) < 3.0f)
				NEDemolisherSet.insert(cr->GetGUID());
			else if(cr->GetDistance2d(1618.047729,61.424641) < 3.0f || cr->GetDistance2d(1575.103149,98.873344) < 3.0f)
				NWDemolisherSet.insert(cr->GetGUID());
			else if(cr->GetDistance2d(1371.055786,-317.071136) < 3.0f || cr->GetDistance2d(1391.213f,-284.105) < 3.0f)
				SEDemolisherSet.insert(cr->GetGUID());
			else if(cr->GetDistance2d(1353.139893,223.745438) < 3.0f || cr->GetDistance2d(1377.583f,182.722f) < 3.0f)
				SWDemolisherSet.insert(cr->GetGUID());
			cr->setFaction(attFaction);
			break;
		// right gobelin
		case 29260:
			GobelinGUID[0] = cr->GetGUID();
			cr->setFaction(attFaction);
			break;
		// Left Gobelin
		case 29262:
			GobelinGUID[1] = cr->GetGUID();
			cr->setFaction(attFaction);
			break;
	}
}

void BattleGroundSA::OnGameObjectCreate(GameObject* go)
{
	uint32 defFaction = (attackers == BG_TEAM_ALLIANCE) ? BG_SA_Factions[BG_TEAM_HORDE] : BG_SA_Factions[BG_TEAM_ALLIANCE];
	uint32 attFaction = BG_SA_Factions[attackers];

	switch(go->GetEntry())
	{
		case 193182:
		case 193185:
			BoatSet[BG_TEAM_ALLIANCE].insert(go->GetGUID());
			DoorClose(go->GetGUID());
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(1,true);
			else
				go->SetPhaseMask(2,true);
			break;
		case 193183:
		case 193184:
			BoatSet[BG_TEAM_HORDE].insert(go->GetGUID());
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(2,true);
			else
				go->SetPhaseMask(1,true);
			break;
		case 190722:
			GatesGUID[0] = go->GetGUID();
			go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
			break;
		case 190727:
			GatesGUID[1] = go->GetGUID();
			go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
			break;
		case 190724:
			GatesGUID[2] = go->GetGUID();
			go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
			break;
		case 190726:
			GatesGUID[3] = go->GetGUID();
			go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
			break;
		case 190723:
			GatesGUID[4] = go->GetGUID();
			go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
			break;
		case 192549:
			GatesGUID[5] = go->GetGUID();
			go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
			break;
		case 192687:
			SigilGUID[0] = go->GetGUID();
			break;
		case 192685:
			SigilGUID[1] = go->GetGUID();
			break;
		case 192689:
			SigilGUID[2] = go->GetGUID();
			break;
		case 192690:
			SigilGUID[3] = go->GetGUID();
			break;
		case 192691:
			SigilGUID[4] = go->GetGUID();
			break;
		case 192834:
			TitanRelicGUID = go->GetGUID();
			go->SetUInt32Value(GAMEOBJECT_FACTION, attFaction);
			break;
	}
}
void BattleGroundSA::EventPlayerDamageGO(Player *player, GameObject* target_obj, uint32 eventId)
{
}