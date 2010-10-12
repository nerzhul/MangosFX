#include <Player.h>
#include "cBattleGround.h"
#include "cBattleGroundSA.h"
#include <Language.h>
#include <ObjectMgr.h>
#include <WorldPacket.h>
#include <Chat.h>
#include <Vehicle.h>

cBattleGroundSA::cBattleGroundSA()
{
	m_BgCreatures.resize(BG_SA_MAX_GY);
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
	AllDemolishersSet.clear();
	AllSpiritGuidsSet.clear();
	BoatSet[0].clear();
	BoatSet[1].clear();
	for(uint8 i=0;i<BG_SA_MAX_GATES-1;i++)
		SigilGUID[i] = 0;
	
	TitanRelicGUID = 0;
	GobelinGUID[0] = GobelinGUID[1] = 0;
	
	for(uint8 i=0;i<3;i++)
		for(uint8 j=0;j<2;j++)
			GraveyardFlag[i][j] = BG_TEAM_NEUTRAL;
	
	for(uint8 i=0;i<3;i++)
		for(uint8 j=0;j<2;j++)
			SpiritGuidesGUID[i][j] = 0;
	
	status = BG_SA_NOTSTARTED;
	RoundLenght = 0;
}

cBattleGroundSA::~cBattleGroundSA()
{
	
}

void cBattleGroundSA::Reset()
{
	cBattleGround::Reset();
	
    attackers = ( (urand(0,1)) ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE);
    for(uint8 i=BG_SA_GREEN_GATE;i<=BG_SA_ANCIENT_GATE;i++)
		GateStatus[i] = BG_SA_GATE_OK;
	GraveyardStatus[BG_SA_BEACH_GY] = attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
	for(uint8 i=BG_SA_DEFENDER_LAST_GY;i<BG_SA_MAX_GY;i++)
		GraveyardStatus[i] = attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
	
	TotalTime = 0;
	round = 0;
    ShipsStarted = false;
	OnLeftBoat = true;
    status = BG_SA_WARMUP;
	WarmupTimer = BG_SA_WARMUPFIRSTROUND;
	RoundLenght = BG_SA_ROUNDLENGTH;
}

void cBattleGroundSA::InitAllObjects()
{
	uint32 defFaction = (attackers == BG_TEAM_ALLIANCE) ? BG_SA_Factions[BG_TEAM_HORDE] : BG_SA_Factions[BG_TEAM_ALLIANCE];
	uint32 attFaction = BG_SA_Factions[attackers];
	
	// Turrets to defender
	/*for(GUIDSet::iterator itr = TurretSet.begin(); itr != TurretSet.end(); ++itr)
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
	
	for(GUIDSet::iterator itr = TeleportSet.begin(); itr != TeleportSet.end(); ++itr)
		if(GameObject* go = GetBgMap()->GetGameObject(*itr))
			go->SetFaction(defFaction);
	
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
	
	for(uint8 i=0;i<3;i++)
		for(uint8 j=0;j<2;j++)
		{
			if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[i][j]))
			{
				if(attackers == j)
					go->SetPhaseMask(2,true);
				else
					go->SetPhaseMask(1,true);
			}
		}*/
	
}	

void cBattleGroundSA::Update(uint32 diff)
{
    cBattleGround::Update(diff);
	
	if(round > 1)
		return;
	
    TotalTime += diff;
    if(status == BG_SA_WARMUP || status == BG_SA_SECOND_WARMUP)
    {
        if(TotalTime >= WarmupTimer)
        {
            TotalTime = 0;
            ToggleTimer();
			LoadSpiritGuids();
			ResetGraveyards();
			LoadDemolishers();
			UpdateCatapults(true);
			SendWarningToAll(LANG_BG_SA_HAS_BEGUN);
            status = (status == BG_SA_WARMUP) ? BG_SA_ROUND_ONE : BG_SA_ROUND_TWO;
			RelocateAllPlayers(false);
        }
        if(TotalTime >= BG_SA_BOAT_START)
            StartShips();
        return;
    }
    else if(status == BG_SA_ROUND_ONE)
    {
        if(TotalTime >= RoundLenght)
        {
			TotalTime = RoundLenght;
            EndRound();
			UpdateCatapults(false);
            return;
        }
    }
    else if(status == BG_SA_ROUND_TWO)
    {
        if(TotalTime >= RoundLenght)
        {
			TotalTime = RoundLenght;
			EndRound();
			UpdateCatapults(false);
            return;
        }
    }
	else if(status == BG_SA_STUCK)
	{
		if(TotalTime >= 5000)
		{
			status = BG_SA_TELEPORT;
			TotalTime = 0;
			ApplyStuckBuffOnPlayers();
			return;
		}
	}
	else if(status == BG_SA_TELEPORT)
	{
		if(TotalTime > 3000)
		{
			status = BG_SA_SECOND_WARMUP;
			TotalTime = 0;
			RelocateAllPlayers(true);
			return;
		}
	}
	
	UpdateTimer();
}

void cBattleGroundSA::ApplyStuckBuffOnPlayers()
{
    for(BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end();++itr)
	 {
	 /*if(Player* p = sObjectMgr.GetPlayer(itr->first))
	 {
	 p->ResurrectPlayer(100.0f);
	 p->CastStop();
	 p->ExitVehicle();
	 p->CastSpell(p,BG_SA_END_ROUND,false);
	 }*/
	 }
}
void cBattleGroundSA::RelocateAllPlayers(bool reseting)
{
    for(BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end();++itr)
	 {
	/* if(Player* p = sObjectMgr.GetPlayer(itr->first*itr))
	 {
	 if(reseting || (p->GetTeam() == ALLIANCE && attackers == BG_TEAM_ALLIANCE || 
	 p->GetTeam() == HORDE && attackers == BG_TEAM_HORDE))
	 TeleportPlayer(p);
	 p->ResurrectPlayer(100.0f);
	 }*/
	 }
}

void cBattleGroundSA::UpdateCatapults(bool usable)
{
	/*for(GUIDSet::iterator itr = NWDemolisherSet.begin(); itr != NWDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{	
			if(usable)
			{
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			}
			else
			{
				cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			}
			
			if(cr->GetVehicleKit())
				cr->GetVehicleKit()->RemoveAllPassengers();
			
			cr->SetPhaseMask(1,true);
		}
	
	for(GUIDSet::iterator itr = NEDemolisherSet.begin(); itr != NEDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			if(usable)
			{
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			}
			else
				cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			
			if(cr->GetVehicleKit())
				cr->GetVehicleKit()->RemoveAllPassengers();
			
			cr->SetPhaseMask(1,true);
		}
	
	for(GUIDSet::iterator itr = SWDemolisherSet.begin(); itr != SWDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			if(usable)
			{
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			}
			else
				cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			
			if(cr->GetVehicleKit())
				cr->GetVehicleKit()->RemoveAllPassengers();
			
			cr->SetPhaseMask(2,true);
		}
	
	for(GUIDSet::iterator itr = SEDemolisherSet.begin(); itr != SEDemolisherSet.end(); ++itr)
		if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			if(usable)
			{
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			}
			else
				cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			
			if(cr->GetVehicleKit())
				cr->GetVehicleKit()->RemoveAllPassengers();
			
			cr->SetPhaseMask(2,true);
		}*/
}

void cBattleGroundSA::UpdateTimer()
{
	if(status == BG_SA_ROUND_ONE || status == BG_SA_ROUND_TWO)
    {
        //Send Time
        uint32 end_of_round = (RoundLenght - TotalTime);
        UpdateWorldState(BG_SA_TIMER_MINS, end_of_round/60000);
        UpdateWorldState(BG_SA_TIMER_SEC_TENS, (end_of_round%60000)/10000);
        UpdateWorldState(BG_SA_TIMER_SEC_DECS, ((end_of_round%60000)%10000)/1000);
    }
}

void cBattleGroundSA::EndRound()
{
	RoundScores[round].time = TotalTime;
	
	if(TotalTime <= 240000)
	 RewardAchievementToTeam(attackers,1310);
    
    ToggleTimer();
	
	bool WallLoose = false;
	
	if(round)
	{
		// define winner
		if(RoundScores[round].time < BG_SA_ROUNDLENGTH)
			RoundScores[round].winner = attackers;
		else
			RoundScores[round].winner = (attackers == BG_TEAM_ALLIANCE) ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
		
		for(uint8 i=BG_SA_GREEN_GATE;i<=BG_SA_ANCIENT_GATE;i++)
		 if(GateStatus[i] == BG_SA_GATE_OK)
		 RewardHonorToTeam(BG_SA_HONOR_GATE_DESTROYED,attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE);
		 else
		 WallLoose = true;
		
		if(!WallLoose)
		 RewardAchievementToTeam(attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE,1757);
		
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
		RoundLenght = RoundScores[round].time;
		if(RoundScores[round].time < BG_SA_ROUNDLENGTH)
			RoundScores[round].winner = attackers;
		else
			RoundScores[round].winner = (attackers == BG_TEAM_ALLIANCE) ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
		
		// Reinit
		attackers = (attackers == BG_TEAM_ALLIANCE) ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
		
		InitAllObjects();
		ResetGraveyards();
		
		for(uint8 i=BG_SA_GREEN_GATE;i<=BG_SA_ANCIENT_GATE;i++)
		 if(GateStatus[i] == BG_SA_GATE_OK)
		 RewardHonorToTeam(BG_SA_HONOR_GATE_DESTROYED,attackers);
		 else
		 {
		 GateStatus[i] = BG_SA_GATE_OK;
		 WallLoose = true;
		 }
		
		if(!WallLoose)
		 RewardAchievementToTeam(attackers,1757);
		
		ResetWorldStates();
		
		status = BG_SA_STUCK;
		WarmupTimer = BG_SA_WARMUPSECONDROUND;		
	}
	round++;
}

void cBattleGroundSA::ResetWorldStates()
{
	UpdateWorldState(BG_SA_HORDE_ATTACKS, uint32(attackers == BG_TEAM_HORDE ? 1 : 0));
	UpdateWorldState(BG_SA_ALLY_ATTACKS, uint32(attackers == BG_TEAM_ALLIANCE ? 1 : 0));
	UpdateWorldState(BG_SA_PURPLE_GATEWS, uint32(GateStatus[BG_SA_PURPLE_GATE]));
	UpdateWorldState(BG_SA_RED_GATEWS, uint32(GateStatus[BG_SA_RED_GATE]));
	UpdateWorldState(BG_SA_BLUE_GATEWS, uint32(GateStatus[BG_SA_BLUE_GATE]));
	UpdateWorldState(BG_SA_GREEN_GATEWS, uint32(GateStatus[BG_SA_GREEN_GATE]));
	UpdateWorldState(BG_SA_YELLOW_GATEWS, uint32(GateStatus[BG_SA_YELLOW_GATE]));
	UpdateWorldState(BG_SA_ANCIENT_GATEWS, uint32(GateStatus[BG_SA_ANCIENT_GATE]));
	UpdateWorldState(BG_SA_RIGHT_GY_HORDE, uint32(GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == BG_TEAM_HORDE?1:0 ));
	UpdateWorldState(BG_SA_LEFT_GY_HORDE, uint32(GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == BG_TEAM_HORDE?1:0 ));
	UpdateWorldState(BG_SA_ANCIENT_GATEWS, uint32(GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == BG_TEAM_HORDE?1:0 ));
	UpdateWorldState(BG_SA_RIGHT_GY_ALLIANCE, uint32(GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == BG_TEAM_ALLIANCE?1:0 ));
	UpdateWorldState(BG_SA_LEFT_GY_ALLIANCE, uint32(GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == BG_TEAM_ALLIANCE?1:0 ));
	UpdateWorldState(BG_SA_CENTER_GY_ALLIANCE, uint32(GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == BG_TEAM_ALLIANCE?1:0 ));
}

void cBattleGroundSA::StartingEventCloseDoors()
{	
	InitAllObjects();
	LoadSpiritGuids();
	ResetGraveyards();
	LoadDemolishers();
	UpdateCatapults(true);
}

void cBattleGroundSA::StartingEventOpenDoors()
{
	status = BG_SA_ROUND_ONE;
	WarmupTimer = BG_SA_WARMUPFIRSTROUND;
}

void cBattleGroundSA::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    cBattleGroundSAScore* sc = new cBattleGroundSAScore;
	
    //m_PlayerScores[plr->GetGUID()] = sc; export this
	
	TeleportPlayer(plr);
}

void cBattleGroundSA::TeleportPlayer(Player* plr)
{
	if(GetTeamIndexByTeamId(plr->GetTeam()) == attackers)
	{
		uint8 boat = OnLeftBoat ? 1 : 0;
		if(status == BG_SA_ROUND_ONE || status == BG_SA_ROUND_TWO)
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

WorldLocation cBattleGroundSA::GetTPDest(Player* plr)
{
	if(GetTeamIndexByTeamId(plr->GetTeam()) == attackers)
	{
		uint8 boat = OnLeftBoat ? 1 : 0;
		if(status == BG_SA_ROUND_ONE || status == BG_SA_ROUND_TWO)
		{
			if(OnLeftBoat)
				return WorldLocation(607,1602.27f,-99.248f,8.873f,4.109f);
			else
				return WorldLocation(607,1612.16f,44.134f,7.579f,2.352f);
		}
		else
		{
			if(OnLeftBoat)
				return WorldLocation(607,2686.42f,-829.99f,18.092f,2.630f);
			else
				return WorldLocation(607,2578.95f,986.802f,16.991f,4.001f);
		}
		
		OnLeftBoat = !OnLeftBoat;
	}
	else
		return WorldLocation(607,1198.54f,-66.007f,70.084f,3.194f);
}

void cBattleGroundSA::RemovePlayer(Player* /*plr*/,uint64 /*guid*/)
{
	
}

void cBattleGroundSA::HandleAreaTrigger(Player * /*Source*/, uint32 /*Trigger*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
}

void cBattleGroundSA::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{
	 BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetGUID());
	 if(itr == m_PlayerScores.end())                         // player not found...
	 return;
	 if(type == SCORE_DESTROYED_DEMOLISHER)
	 ((BattleGroundSAScore*)itr->second)->demolishers_destroyed += value;
	 else if(type == SCORE_DESTROYED_WALL)
	 ((BattleGroundSAScore*)itr->second)->gates_destroyed += value;
	 else
	 BattleGround::UpdatePlayerScore(Source,type,value);
}

WorldSafeLocsEntry const* cBattleGroundSA::GetClosestGraveYard(Player* player)
{
    uint32 safeloc = 0;
    WorldSafeLocsEntry const* ret;
    WorldSafeLocsEntry const* closest;
    float dist, nearest;
    float x,y,z;
	
    player->GetPosition(x,y,z);
	
    if (GetTeamIndexByTeamId(player->GetTeam()) == attackers)
        safeloc = BG_SA_GYEntries[BG_SA_BEACH_GY];
    else
        safeloc = BG_SA_GYEntries[BG_SA_DEFENDER_LAST_GY];
	
    closest = sWorldSafeLocsStore.LookupEntry(safeloc);
    nearest = sqrt((closest->x - x)*(closest->x - x) + (closest->y - y)*(closest->y - y)+(closest->z - z)*(closest->z - z));
	
    for (uint8 i = BG_SA_RIGHT_CAPTURABLE_GY; i < BG_SA_MAX_GY; i++)
    {
        if (GraveyardStatus[i] != GetTeamIndexByTeamId(player->GetTeam()))
            continue;
		
        ret = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);
        dist = sqrt((ret->x - x)*(ret->x - x) + (ret->y - y)*(ret->y - y)+(ret->z - z)*(ret->z - z));
        if (dist < nearest)
        {
            closest = ret;
            nearest = dist;
        }
    }
	
    return closest;
}

void cBattleGroundSA::CaptureGraveyard(BG_SA_Graveyards i, Player *Source)
{
    GraveyardStatus[i] = attackers;
    WorldSafeLocsEntry const *sg = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);
	if(GetTeamIndexByTeamId(Source->GetTeam()) != attackers)
		return;
	
    switch(i)
    {
        case BG_SA_LEFT_CAPTURABLE_GY:
			/*if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[i][attackers]))
				cr->SetPhaseMask(1,true);
			
			if(GraveyardStatus[i] == BG_TEAM_ALLIANCE)
			{
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[0][BG_TEAM_ALLIANCE]))
					go->SetPhaseMask(1,true);
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[0][BG_TEAM_HORDE]))
					go->SetPhaseMask(2,true);
			}
			else
			{
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[0][BG_TEAM_ALLIANCE]))
					go->SetPhaseMask(2,true);
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[0][BG_TEAM_HORDE]))
					go->SetPhaseMask(1,true);
			}
			for(GUIDSet::iterator itr = NWDemolisherSet.begin(); itr != NWDemolisherSet.end(); ++itr)
				if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
					cr->SetPhaseMask(2,true);
			
			for(GUIDSet::iterator itr = SWDemolisherSet.begin(); itr != SWDemolisherSet.end(); ++itr)
				if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
					cr->SetPhaseMask(1,true);*/
			
            UpdateWorldState(BG_SA_LEFT_GY_ALLIANCE, (GraveyardStatus[i] == BG_TEAM_ALLIANCE? 1:0));
            UpdateWorldState(BG_SA_LEFT_GY_HORDE, (GraveyardStatus[i] == BG_TEAM_ALLIANCE? 0:1));
            if (Source->GetTeam() == ALLIANCE)
                SendWarningToAll("Le Cimetiere de l'Ouest a ete pris par l'Alliance");
            else
                SendWarningToAll("Le Cimetiere de l'Ouest a ete pris par la Horde");
            break;
        case BG_SA_RIGHT_CAPTURABLE_GY:
			/*if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[i][attackers]))
				cr->SetPhaseMask(1,true);
			
			for(GUIDSet::iterator itr = NEDemolisherSet.begin(); itr != NEDemolisherSet.end(); ++itr)
				if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
					cr->SetPhaseMask(2,true);
			
			for(GUIDSet::iterator itr = SEDemolisherSet.begin(); itr != SEDemolisherSet.end(); ++itr)
				if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
					cr->SetPhaseMask(1,true);
			
            if(GraveyardStatus[i] == BG_TEAM_ALLIANCE)
			{
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[1][BG_TEAM_ALLIANCE]))
					go->SetPhaseMask(1,true);
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[1][BG_TEAM_HORDE]))
					go->SetPhaseMask(2,true);
			}
			else
			{
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[1][BG_TEAM_ALLIANCE]))
					go->SetPhaseMask(2,true);
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[1][BG_TEAM_HORDE]))
					go->SetPhaseMask(1,true);
			}*/
			
            UpdateWorldState(BG_SA_RIGHT_GY_ALLIANCE, (GraveyardStatus[i] == BG_TEAM_ALLIANCE? 1:0));
            UpdateWorldState(BG_SA_RIGHT_GY_HORDE, (GraveyardStatus[i] == BG_TEAM_ALLIANCE? 0:1));
            if (Source->GetTeam() == ALLIANCE)
                SendWarningToAll("Le Cimetiere de l'Est a ete pris par l'Alliance");
            else
                SendWarningToAll("Le Cimetiere de l'Est a ete pris par la Horde");
            break;
        case BG_SA_CENTRAL_CAPTURABLE_GY:
			/*if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[i][attackers]))
				cr->SetPhaseMask(1,true);
			
            if(GraveyardStatus[i] == BG_TEAM_ALLIANCE)
			{
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[2][BG_TEAM_ALLIANCE]))
					go->SetPhaseMask(1,true);
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[2][BG_TEAM_HORDE]))
					go->SetPhaseMask(2,true);
			}
			else
			{
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[2][BG_TEAM_ALLIANCE]))
					go->SetPhaseMask(2,true);
				if(GameObject* go = GetBgMap()->GetGameObject(GraveyardFlag[2][BG_TEAM_HORDE]))
					go->SetPhaseMask(1,true);
			}*/
			
            UpdateWorldState(BG_SA_CENTER_GY_ALLIANCE, (GraveyardStatus[i] == BG_TEAM_ALLIANCE? 1:0));
            UpdateWorldState(BG_SA_CENTER_GY_HORDE, (GraveyardStatus[i] == BG_TEAM_ALLIANCE? 0:1));
            if (Source->GetTeam() == ALLIANCE)
                SendWarningToAll("Le Cimetiere du Sud a ete pris par l'Alliance");
            else
                SendWarningToAll("Le Cimetiere du Sud a ete pris par la Horde");
            break;
        default:
            break;
    };
}

void cBattleGroundSA::FillInitialWorldStates(WorldPacket& data, uint32& count)
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

void cBattleGroundSA::ToggleTimer()
{
    TimerEnabled = !TimerEnabled;
    UpdateWorldState(BG_SA_ENABLE_TIMER, (TimerEnabled) ? 1 : 0);
}

void cBattleGroundSA::StartShips()
{
	if(ShipsStarted)
		return;
	
	for(GUIDSet::iterator itr = BoatSet[attackers].begin(); itr != BoatSet[attackers].end(); ++itr)
		DoorOpen(*itr);
	
	ShipsStarted = true;
}

bool cBattleGroundSA::SetupBattleGround()
{
	ResetGraveyards();
	return true;
}

void cBattleGroundSA::HandleKillUnit(Creature *unit, Player *killer)
{
	if (!unit)
        return;
	
    if (unit->GetEntry() == 28781)  //Demolisher
        UpdatePlayerScore(killer, SCORE_DESTROYED_DEMOLISHER, 1);
}

void cBattleGroundSA::ResetGraveyards()
{
	GraveyardStatus[BG_SA_BEACH_GY] = attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
	for(uint8 i=BG_SA_DEFENDER_LAST_GY;i<BG_SA_MAX_GY;i++)
		GraveyardStatus[i] = attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
	
	/*if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_BEACH_GY][attackers]))
		cr->SetPhaseMask(1,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_BEACH_GY][attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE]))
		cr->SetPhaseMask(2,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_DEFENDER_LAST_GY][attackers]))
		cr->SetPhaseMask(2,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_DEFENDER_LAST_GY][attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE]))
		cr->SetPhaseMask(1,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_RIGHT_CAPTURABLE_GY][attackers]))
		cr->SetPhaseMask(2,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_RIGHT_CAPTURABLE_GY][attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE]))
		cr->SetPhaseMask(1,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_LEFT_CAPTURABLE_GY][attackers]))
		cr->SetPhaseMask(2,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_LEFT_CAPTURABLE_GY][attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE]))
		cr->SetPhaseMask(1,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_CENTRAL_CAPTURABLE_GY][attackers]))
		cr->SetPhaseMask(2,true);
	
	if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(SpiritGuidesGUID[BG_SA_CENTRAL_CAPTURABLE_GY][attackers == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE]))
		cr->SetPhaseMask(1,true);*/
}

void cBattleGroundSA::EventPlayerClickedOnFlag(Player *Source, GameObject *target_obj)
{
	switch(target_obj->GetEntry())
	{
			// titan relic
		case 192834:
		{
			if(status != BG_SA_ROUND_ONE && status != BG_SA_ROUND_TWO)
				return;
			
			const char* teamName = Source->GetTeam() == ALLIANCE ? "'Alliance" : "a Horde";
			SendWarningToAll(fmtstring("L%s a pris la relique !",teamName));
			//RewardHonorToTeam(BG_SA_HONOR_RELIC_CAPTURED,Source->GetTeam() == ALLIANCE ? BG_TEAM_ALLIANCE : BG_TEAM_HORDE);
			EndRound();
			break;
		}
			// graveyards
			// left
		case 191307:
		case 191308:
			if (GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED)
                CaptureGraveyard(BG_SA_LEFT_CAPTURABLE_GY, Source);
			break;
			// right
		case 191305:
		case 191306:
			if (GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED)
                CaptureGraveyard(BG_SA_RIGHT_CAPTURABLE_GY, Source);
			break;
			// central
		case 191309:
		case 191310:
			if ((GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED) && (GateStatus[BG_SA_RED_GATE] == BG_SA_GATE_DESTROYED || GateStatus[BG_SA_PURPLE_GATE] == BG_SA_GATE_DESTROYED))
                CaptureGraveyard(BG_SA_CENTRAL_CAPTURABLE_GY, Source);
			break;
	}
}


void cBattleGroundSA::EndBattleGround(uint32 winner)
{
	//honor reward for winning
    if (winner == ALLIANCE)
	 RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);
	 else if (winner == HORDE)
	 RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);
	
	RewardHonorTeamDaily(winner);
	
    //complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(2), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(2), HORDE);
	
    cBattleGround::EndBattleGround(winner);
}

uint32 cBattleGroundSA::GetGateIDFromDestroyEventID(uint32 id)
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

uint32 cBattleGroundSA::GetWorldStateFromGateID(uint32 id)
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

const char* cBattleGroundSA::GetDoorNameFromGateID(uint32 id)
{
    switch(id)
    {
        case BG_SA_GREEN_GATE:   return "de l'Emeraude Verte";
        case BG_SA_YELLOW_GATE:  return "de la Lune Jaune";
        case BG_SA_BLUE_GATE:    return "du Saphir Bleu";
        case BG_SA_RED_GATE:     return "du Soleil Rouge";
        case BG_SA_PURPLE_GATE:  return "de l'Amethyste Violette";
        case BG_SA_ANCIENT_GATE: return "de la Chambre des Reliques";
    }
    return "";
}

void cBattleGroundSA::LoadDemolishers()
{
	for(GUIDSet::iterator itr = AllDemolishersSet.begin(); itr != AllDemolishersSet.end(); ++itr)
	{
		/*if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			if(cr->GetDistance2d(1611.597656,-117.270073) < 6.0f || cr->GetDistance2d(1575.562500,-158.421875) < 6.0f)
				NEDemolisherSet.insert(cr->GetGUID());
			else if(cr->GetDistance2d(1618.047729,61.424641) < 6.0f || cr->GetDistance2d(1575.103149,98.873344) < 6.0f)
				NWDemolisherSet.insert(cr->GetGUID());
			else if(cr->GetDistance2d(1371.055786,-317.071136) < 6.0f || cr->GetDistance2d(1391.213f,-284.105) < 6.0f)
				SEDemolisherSet.insert(cr->GetGUID());
			else if(cr->GetDistance2d(1353.139893,223.745438) < 6.0f || cr->GetDistance2d(1377.583f,182.722f) < 6.0f)
				SWDemolisherSet.insert(cr->GetGUID());
		}*/
	}
	
	for(GUIDSet::iterator itr = NWDemolisherSet.begin(); itr != NWDemolisherSet.end(); ++itr)
		AllDemolishersSet.erase(*itr);
	
	for(GUIDSet::iterator itr = NEDemolisherSet.begin(); itr != NEDemolisherSet.end(); ++itr)
		AllDemolishersSet.erase(*itr);
	
	for(GUIDSet::iterator itr = SWDemolisherSet.begin(); itr != SWDemolisherSet.end(); ++itr)
		AllDemolishersSet.erase(*itr);
	
	for(GUIDSet::iterator itr = SEDemolisherSet.begin(); itr != SEDemolisherSet.end(); ++itr)
		AllDemolishersSet.erase(*itr);
}

void cBattleGroundSA::LoadSpiritGuids()
{
	for(GUIDSet::iterator itr = AllSpiritGuidsSet.begin(); itr != AllSpiritGuidsSet.end(); ++itr)
	{
		/*if(Creature* cr = GetBgMap()->GetCreatureOrPetOrVehicle(*itr))
		{
			if(cr->GetDistance2d(1456.9f,-52.255f) < 25.0f)
			{
				SpiritGuidesGUID[BG_SA_BEACH_GY][cr->GetEntry() == 13116 ? 0 : 1] = cr->GetGUID();
				if(GraveyardStatus[BG_SA_BEACH_GY] == BG_TEAM_ALLIANCE)
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(1,true);
					else
						cr->SetPhaseMask(2,true);
				}
				else
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(2,true);
					else
						cr->SetPhaseMask(1,true);
				}	
			}
			else if(cr->GetDistance2d(964.843f,-189.878f) < 25.0f)
			{
				SpiritGuidesGUID[BG_SA_DEFENDER_LAST_GY][cr->GetEntry() == 13116 ? 0 : 1] = cr->GetGUID();
				if(GraveyardStatus[BG_SA_DEFENDER_LAST_GY] == BG_TEAM_ALLIANCE)
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(1,true);
					else
						cr->SetPhaseMask(2,true);
				}
				else
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(2,true);
					else
						cr->SetPhaseMask(1,true);
				}
			}
			else if(cr->GetDistance2d(1398.79f,-288.838f) < 25.0f)
			{
				SpiritGuidesGUID[BG_SA_LEFT_CAPTURABLE_GY][cr->GetEntry() == 13116 ? 0 : 1] = cr->GetGUID();
				if(GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == BG_TEAM_ALLIANCE)
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(1,true);
					else
						cr->SetPhaseMask(2,true);
				}
				else
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(2,true);
					else
						cr->SetPhaseMask(1,true);
				}
			}
			else if(cr->GetDistance2d(1388.42f,203.042f) < 25.0f)
			{
				SpiritGuidesGUID[BG_SA_RIGHT_CAPTURABLE_GY][cr->GetEntry() == 13116 ? 0 : 1] = cr->GetGUID();
				if(GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == BG_TEAM_ALLIANCE)
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(1,true);
					else
						cr->SetPhaseMask(2,true);
				}
				else
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(2,true);
					else
						cr->SetPhaseMask(1,true);
				}
			}
			else if(cr->GetDistance2d(1121.95f,4.48f) < 25.0f)
			{
				SpiritGuidesGUID[BG_SA_CENTRAL_CAPTURABLE_GY][cr->GetEntry() == 13116 ? 0 : 1] = cr->GetGUID();
				if(GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == BG_TEAM_ALLIANCE)
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(1,true);
					else
						cr->SetPhaseMask(2,true);
				}
				else
				{
					if(cr->GetEntry() == 13116)
						cr->SetPhaseMask(2,true);
					else
						cr->SetPhaseMask(1,true);
				}
			}
		}*/
	}
	
	for(uint8 i=0;i<5;i++)
		for(uint8 j=0;j<2;j++)
			AllSpiritGuidsSet.erase(SpiritGuidesGUID[i][j]);
}

void cBattleGroundSA::OnCreatureCreate(Creature* cr)
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
			AllDemolishersSet.insert(cr->GetGUID());
			if (status != BG_SA_ROUND_ONE && status != BG_SA_ROUND_TWO)
			{
                cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				cr->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			}
            else
			{
                cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				cr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			}
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
			// Spirit Guides
		case 13116:
		case 13117:
			AllSpiritGuidsSet.insert(cr->GetGUID());
			break;
	}
}

void cBattleGroundSA::OnGameObjectCreate(GameObject* go)
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
			DoorClose(go->GetGUID());
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(2,true);
			else
				go->SetPhaseMask(1,true);
			break;
		case 190722:
			GatesGUID[0] = go->GetGUID();
			go->SetFaction(defFaction);
			break;
		case 190727:
			GatesGUID[1] = go->GetGUID();
			go->SetFaction(defFaction);
			break;
		case 190724:
			GatesGUID[2] = go->GetGUID();
			go->SetFaction(defFaction);
			break;
		case 190726:
			GatesGUID[3] = go->GetGUID();
			go->SetFaction(defFaction);
			break;
		case 190723:
			GatesGUID[4] = go->GetGUID();
			go->SetFaction(defFaction);
			break;
		case 192549:
			GatesGUID[5] = go->GetGUID();
			go->SetFaction(defFaction);
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
			go->SetFaction(attFaction);
			break;
		case 191308:
			GraveyardFlag[0][BG_TEAM_ALLIANCE] = go->GetGUID();
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(2,true);
			else
				go->SetPhaseMask(1,true);
			break;
		case 191307:
			GraveyardFlag[0][BG_TEAM_HORDE] = go->GetGUID();
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(1,true);
			else
				go->SetPhaseMask(2,true);
			break;
		case 191306:
			GraveyardFlag[1][BG_TEAM_ALLIANCE] = go->GetGUID();
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(2,true);
			else
				go->SetPhaseMask(1,true);
			break;
		case 191305:
			GraveyardFlag[1][BG_TEAM_HORDE] = go->GetGUID();
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(1,true);
			else
				go->SetPhaseMask(2,true);
			break;
		case 191310:
			GraveyardFlag[2][BG_TEAM_ALLIANCE] = go->GetGUID();
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(2,true);
			else
				go->SetPhaseMask(1,true);
			break;
		case 191309:
			GraveyardFlag[2][BG_TEAM_HORDE] = go->GetGUID();
			if(attackers == BG_TEAM_ALLIANCE)
				go->SetPhaseMask(1,true);
			else
				go->SetPhaseMask(2,true);
			break;
		case 192819:
			go->SetFaction(defFaction);
			TeleportSet.insert(go->GetGUID());
			break;
	}
}
void cBattleGroundSA::EventPlayerDamageGO(Player *player, GameObject* target_obj, uint32 eventId)
{
	switch(eventId)
	{
			// Damage Events
		case 21630:
			switch(target_obj->GetEntry())
		{
			case 190722:
				GateStatus[BG_SA_GREEN_GATE] = BG_SA_GATE_DAMAGED;
				UpdateWorldState(GetWorldStateFromGateID(BG_SA_GREEN_GATE), GateStatus[BG_SA_GREEN_GATE]);
				SendWarningToAll("L'ennemi attaque la porte de l'Emeraude Verte !");
				break;
			case 190724:
				GateStatus[BG_SA_BLUE_GATE] = BG_SA_GATE_DAMAGED;
				UpdateWorldState(GetWorldStateFromGateID(BG_SA_BLUE_GATE), GateStatus[BG_SA_BLUE_GATE]);
				SendWarningToAll("L'ennemi attaque la porte du Saphir Bleu !");
				break;
			case 190726:
				GateStatus[BG_SA_RED_GATE] = BG_SA_GATE_DAMAGED;
				UpdateWorldState(GetWorldStateFromGateID(BG_SA_RED_GATE), GateStatus[BG_SA_RED_GATE]);
				SendWarningToAll("L'ennemi attaque la porte du Soleil Rouge !");
				break;
			case 190723:
				GateStatus[BG_SA_PURPLE_GATE] = BG_SA_GATE_DAMAGED;
				UpdateWorldState(GetWorldStateFromGateID(BG_SA_PURPLE_GATE), GateStatus[BG_SA_PURPLE_GATE]);
				SendWarningToAll("L'ennemi attaque la porte de l'Amethyste Violette !");
				break;
			case 190727:
				GateStatus[BG_SA_YELLOW_GATE] = BG_SA_GATE_DAMAGED;
				UpdateWorldState(GetWorldStateFromGateID(BG_SA_YELLOW_GATE), GateStatus[BG_SA_YELLOW_GATE]);
				SendWarningToAll("L'ennemi attaque la porte de la Lune Jaune !");
				break;
			case 192691:
				GateStatus[BG_SA_ANCIENT_GATE] = BG_SA_GATE_DAMAGED;
				UpdateWorldState(GetWorldStateFromGateID(BG_SA_ANCIENT_GATE), GateStatus[BG_SA_ANCIENT_GATE]);
				SendWarningToAll("L'ennemi attaque la Salle de la Relique !");
				break;
		}
			return;
	}
	
	uint32 gateId = GetGateIDFromDestroyEventID(eventId);
	GateStatus[gateId] = BG_SA_GATE_DESTROYED;
	uint32 uws = GetWorldStateFromGateID(gateId);
	if(uws)
		UpdateWorldState(uws, GateStatus[gateId]);
	if(gateId != BG_SA_ANCIENT_GATE)
		SendWarningToAll(fmtstring("La porte %s est detruite !",GetDoorNameFromGateID(gateId)));
	else
		SendWarningToAll("La relique des titans est vulnerable !");
	
	cBattleGround::RewardHonorToTeam(BG_SA_HONOR_GATE_DESTROYED,attackers);
	
	UpdatePlayerScore(player,SCORE_DESTROYED_WALL, 1);
    UpdatePlayerScore(player,SCORE_BONUS_HONOR,(GetBonusHonorFromKill(1)));
	switch(target_obj->GetEntry())
	{
		/*case 190722:
			if(GameObject* go = GetBgMap()->GetGameObject(SigilGUID[0]))
				go->SetPhaseMask(2,true);
			break;
		case 190727:
			if(GameObject* go = GetBgMap()->GetGameObject(SigilGUID[1]))
				go->SetPhaseMask(2,true);
			break;
		case 190724:
			if(GameObject* go = GetBgMap()->GetGameObject(SigilGUID[2]))
				go->SetPhaseMask(2,true);
			break;
		case 190726:
			if(GameObject* go = GetBgMap()->GetGameObject(SigilGUID[3]))
				go->SetPhaseMask(2,true);
			break;
		case 190723:
			if(GameObject* go = GetBgMap()->GetGameObject(SigilGUID[4]))
				go->SetPhaseMask(2,true);
			break;*/
	}
	
}