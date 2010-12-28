#include "PlayerBot.h"
#include <Policies/SingletonImp.h>
#include "ObjectMgr.h"
#include "BattleGroundWS.h"

INSTANTIATE_SINGLETON_1( PlayerBotMgr );

PlayerBotMgr::PlayerBotMgr()
{
	m_choiceChances.clear();
}

PlayerBotMgr::~PlayerBotMgr()
{
	m_choiceChances.clear();
}

void PlayerBotMgr::LoadBotChoiceChances()
{
	uint32 count = 0;
	m_choiceChances.clear();
	if(QueryResult* result = WorldDatabase.PQuery("SELECT idchoice,chance FROM playerbot_choice_chance"))
	{
		do
		{
			Field* fields = result->Fetch();
			m_choiceChances[fields[0].GetUInt32()] = fields[1].GetFloat();
			count++;
		} while(result->NextRow());
	}
	sLog.outString(">> Loaded %u PlayerBot chances by choice",count);
}

void PlayerBotMgr::CleanCoordinates()
{
	for(BotCoords::iterator itr = mail_a.begin(); itr != mail_a.end();)
	{
		BotCoords::iterator next = ++itr;
		--itr;
		delete itr->second;
		mail_a.erase(itr);
		itr = next;
	}

	for(BotCoords::iterator itr = mail_h.begin(); itr != mail_h.end();)
	{
		BotCoords::iterator next = ++itr;
		--itr;
		delete itr->second;
		mail_h.erase(itr);
		itr = next;
	}
}

void PlayerBotMgr::LoadBotCoordinates()
{
	uint32 count = 0;
	CleanCoordinates();
	if(QueryResult* result = WorldDatabase.PQuery("SELECT type,faction,map,x,y,z,id FROM playerbot_coordinates"))
	{
		do
		{
			Field* fields = result->Fetch();
			switch(BotCoordType(fields[0].GetUInt32()))
			{
				case BCOORD_MAIL:
				{
					BotCoord* bc = new BotCoord();
					bc->mapId = fields[2].GetUInt32();
					bc->x = fields[3].GetFloat();
					bc->y = fields[4].GetFloat();
					bc->z = fields[5].GetFloat();
					if(fields[1].GetUInt32() == 0)
						mail_a[fields[6].GetUInt32()] = bc;
					else
						mail_h[fields[6].GetUInt32()] = bc;
					break;
				}
				default:
					break;
			}
			count++;
		} while(result->NextRow());
	}
	sLog.outString(">> Loaded %u PlayerBot Coordinates",count);
}

uint32 PlayerBotMgr::GetRandomPoint(uint32 faction, BotCoordType bcType)
{
	if(faction == ALLIANCE)
	{
		switch(bcType)
		{
			case BCOORD_MAIL:
			{
				uint32 mcount = mail_a.size();
				uint32 pos = 0;
				uint32 idxchosen = urand(0,mail_a.size()-1);
				for(BotCoords::const_iterator itr = mail_a.begin(); itr != mail_a.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
			}
		}
	}
	else if(faction == HORDE)
	{
		switch(bcType)
		{
			case BCOORD_MAIL:
			{
				uint32 mcount = mail_h.size();
				uint32 pos = 0;
				uint32 idxchosen = urand(0,mail_h.size()-1);
				for(BotCoords::const_iterator itr = mail_h.begin(); itr != mail_h.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
			}
		}
	}

	return 0;
}

BotCoord* PlayerBotMgr::GetPoint(uint32 faction, BotCoordType bcType, uint32 idx)
{
	if(faction == ALLIANCE)
	{
		switch(bcType)
		{
			case BCOORD_MAIL:
			{
				BotCoords::iterator itr = mail_a.find(idx);
				if(itr != mail_a.end())
					return itr->second;
			}
		}
	}
	else if(faction == HORDE)
	{
		switch(bcType)
		{
			case BCOORD_MAIL:
			{
				BotCoords::iterator itr = mail_h.find(idx);
				if(itr != mail_h.end())
					return itr->second;
			}
		}
	}

	return 0;
}

PlayerBot::PlayerBot(WorldSession* session)//: Player(session)
{
	specIdx = 0;
	m_decideToFight = false;
	m_sheduledBGJoin = DAY*HOUR;
	bgTypeId = BATTLEGROUND_TYPE_NONE;
	m_ginfo = 0;
	sheduledBG = NULL;
	chosen_point = 0;

	choice_Timer = 0;
	mode_Timer = 5000;
	act_Timer = 1000;
	react_Timer = 100;
}

PlayerBot::~PlayerBot()
{
	delete bot;
}

void PlayerBot::GoToCacIfIsnt(Unit* target)
{
	if(!target)
		return;

	if(bot->GetDistance2d(target) >= 4.5f)
		bot->GetMotionMaster()->MoveChase(target,3.0f);
}

void PlayerBot::Stay()
{
	bot->GetMotionMaster()->Clear();
}

void PlayerBot::JoinBGQueueIfNotIn()
{
	if(bot->IsInWorld() && !bot->InBattleGroundQueue() && !bot->GetBattleGround())
	{
		BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[BATTLEGROUND_QUEUE_RANDOM];
		BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(BATTLEGROUND_RB);
		if(!bg)	return;

		PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(),bot->getLevel());
		if(!bracketEntry) return;

		bgQueue.AddGroup(bot, NULL, BATTLEGROUND_RB, bracketEntry, 0, false, false, 0);
		bot->AddBattleGroundQueueId(BATTLEGROUND_QUEUE_RANDOM);
		sBattleGroundMgr.ScheduleQueueUpdate(0, 0, BATTLEGROUND_QUEUE_RANDOM, BATTLEGROUND_RB, bracketEntry->GetBracketId());
	}
}

void PlayerBot::SendToBg()
{
	BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[BATTLEGROUND_QUEUE_RANDOM];
	if (!bot->InBattleGround())
		bot->SetBattleGroundEntryPoint();

	// resurrect the player
	if (!bot->isAlive())
	{
		bot->ResurrectPlayer(1.0f);
		bot->SpawnCorpseBones();
	}

	// stop taxi flight at port
	if (bot->isInFlight())
	{
		bot->GetMotionMaster()->MovementExpired();
		bot->m_taxi.ClearTaxiDestinations();
	}

	bgQueue.RemovePlayer(bot->GetGUID(), false);
	if (BattleGround *currentBg = bot->GetBattleGround())
		currentBg->RemovePlayerAtLeave(bot->GetGUID(), false, true);

	bot->SetBattleGroundId(sheduledBG->GetInstanceID(), bgTypeId);
	bot->SetBGTeam(m_ginfo->Team);
	sBattleGroundMgr.SendToBattleGround(bot, m_ginfo->IsInvitedToBGInstanceGUID, bgTypeId);
	bot->GetSession()->HandleMoveWorldportAckOpcode();
}

void PlayerBot::SheduleSendToBG(BattleGround* bg, BattleGroundTypeId btId, GroupQueueInfo* ginfo)
{
	sheduledBG = bg;
	bgTypeId = btId;
	m_ginfo = ginfo;
	m_sheduledBGJoin = 1000;
}

void PlayerBot::Update(uint32 diff)
{
	ASSERT(bot);
	
	if(react_Timer <= diff)
	{
		react_Timer = 1200;
		if(bot->isDead() && !bot->GetBattleGround())
		{
			HandleGoToCorpse();
			return;
		}
		
		if(!bot->isDead() && isInHostileZoneWithoutLevel())
		{
			HandleFearZone();
			return;
		}
	}
	else
		react_Timer -= diff;


	if(m_sheduledBGJoin < DAY*HOUR)
	{
		if(m_sheduledBGJoin <= diff)
		{
			SendToBg();
			m_sheduledBGJoin = DAY*HOUR;
		}
		else
			m_sheduledBGJoin -= diff;
	}

	// CombatHandler for all classes
	if(HasDecidedToFight())
	{
		if(!bot->GetSelection())
		{
			m_decideToFight = false;
			return;
		}

		if(Player* plr = sObjectMgr.GetPlayer(bot->GetSelection()))
			if(plr->isAlive())
			{
				m_decideToFight = false;
				return;
			}

		switch(bot->getClass())
		{
			case CLASS_WARRIOR:
				HandleWarriorCombat();
				break;
			case CLASS_PALADIN:
				HandlePaladinCombat();
				break;
			case CLASS_HUNTER:
				HandleHunterCombat();
				break;
			case CLASS_ROGUE:
				HandleRogueCombat();
				break;
			case CLASS_PRIEST:
				HandlePriestCombat();
				break;
			case CLASS_DEATH_KNIGHT:
				HandleDKCombat();
				break;
			case CLASS_SHAMAN:
				HandleShamanCombat();
				break;
			case CLASS_MAGE:
				HandleMageCombat();
				break;
			case CLASS_WARLOCK:
				HandleWarlockCombat();
				break;
			case CLASS_DRUID:
				HandleDruidCombat();
				break;
		}
	}
	else
	{
		if(choice_Timer <= diff && !bot->InBattleGround() && !bot->InBattleGroundQueue())
		{
			ChooseToDoSomething();
		}
		else
			choice_Timer -= diff;
		
		if(react_Timer <= diff)
		{
			switch(m_choice)
			{
				case BCHOICE_PVP:
				{
					JoinBGQueueIfNotIn();

					if(BattleGround* bg = bot->GetBattleGround())
					{
						switch(bg->GetTypeID(true))
						{
							case BATTLEGROUND_WS:
								HandleWarsong(diff);
								break;
							case BATTLEGROUND_AB:
								HandleArathi(diff);
								break;
							case BATTLEGROUND_AV:
								HandleAlterac(diff);
								break;
							case BATTLEGROUND_EY:
								HandleEyeOfTheStorm(diff);
								break;
						}
					}
					break;
				}
				case BCHOICE_AUCTION:
					HandleAuction();
					break;
				case BCHOICE_BANK:
					HandleBank();
					break;
				case BCHOICE_MAIL:
					HandleMail();
					break;
				case BCHOICE_AFK:
				default:
					break;
			}
			react_Timer = 1200;
		}
		else
			react_Timer -= diff;
	}
}

void PlayerBot::ChooseToDoSomething()
{
	float randAct = float(urand(1,1000));

	choice_Timer = urand(600000,3600000);
	bot->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK);

	if(randAct < 200) // 20%
		m_choice = BCHOICE_PVP;
	else if(randAct < 300) // 10%
		m_choice = BCHOICE_FARM_MOBS;
	else if(randAct < 400) // 10%
		m_choice = BCHOICE_GO_ZONE;
	else if(randAct < 430) // 3%
		m_choice = BCHOICE_QUEST;
	else if(randAct < 460) // 3%
		m_choice = BCHOICE_EXPLORE;
	else if(randAct < 560) // 10%
	{
		m_choice = BCHOICE_FARM_MINERALS;
		m_choice = BCHOICE_FARM_HERBS;
		m_choice = BCHOICE_FARM_LEATHER;
		m_choice = BCHOICE_FARM_CLOTH;
	}
	else if(randAct < 590) // 3%
		m_choice = BCHOICE_LEARN_SPELLS;
	else if(randAct < 710) // 12%
	{
		m_choice = BCHOICE_AUCTION;
		chosen_point = 0;
		choice_Timer = urand(60000,1200000);
	}
	else if(randAct < 830) // 12%
	{
		m_choice = BCHOICE_BANK;
		chosen_point = 0;
		choice_Timer = urand(60000,1200000);
	}
	else if(randAct < 950) // 12%
	{
		m_choice = BCHOICE_MAIL;
		chosen_point = 0;
		choice_Timer = urand(60000,1200000);
	}
	else if(randAct < 1000) // 5%
	{
		m_choice = BCHOICE_AFK;
		if(!bot->isAFK())
			bot->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK);
		choice_Timer = urand(60000,3600000);
	}
}

float bank_coords[16][5] = {
	{0,0,-8926.86,607.1f,99.55f}, // st
	{0,0,-8934.9f,622.5f,99.55f}, // st
	{0,0,-4880.33f,-986.4f,504.1f}, // if
	{0,0,-4897.9f,-1001.5f,504.1f}, // if
	{0,571,5978.61f,601.09f,650.63f}, // dala
	{0,571,5984.12f,615.81f,650.63f}, // dala
	{0,571,5629.49f,701.356f,651.99f}, // dala
	{0,571,5623.01f,687.31f,651.99f}, // dala
	{1,1,1622.76f,-4373.21f,12.1f}, // og
	{1,1,1629.96f,-4381.31f,20.1f}, // og
	{1,0,1588.73f,240.5f,-52.2f}, // uc
	{1,0,1602.88f,240.7f,-52.2f}, // uc
	{1,571,5978.61f,601.09f,650.63f}, // dala
	{1,571,5984.12f,615.81f,650.63f}, // dala
	{1,571,5629.49f,701.356f,651.99f}, // dala
	{1,571,5623.01f,687.31f,651.99f}, // dala
};

float ah_coords[8][5] = {
	{0,0,-8818.96f,661.1f,96.5f}, // st
	{0,0,-8820.6f,667.750f,96.5f}, // st
	{0,0,-4963.1f,-914.4f,504.9f}, // if
	{0,0,-4952.31f,-904.352f,504.9f}, // if
	{1,1,1689.611f,-4455.31f,20.1f}, // og
	{1,1,1668.98f,-4458.94f,20.1f}, // og
	{1,1,1682.99f,-4461.18f,20.1f}, // og
	{1,0,1646.63f,221.36f,-55.881f}, // uc

};

bool PlayerBot::isInHostileZoneWithoutLevel()
{
	uint32 zId = bot->GetZoneId();
	
	if(bot->getLevel() < 75)
	{
		switch(bot->getRace())
		{
			case RACE_HUMAN:
			case RACE_DWARF:
			case RACE_GNOME:
			case RACE_NIGHTELF:
			case RACE_DRAENEI:
			case RACE_WORGEN:
				// Capitals
				if(zId == 1637 || zId == 3487 || zId == 1638 || zId == 1497)
					return true;
				break;
			case RACE_ORC:
			case RACE_TROLL:
			case RACE_TAUREN:
			case RACE_BLOODELF:
			case RACE_UNDEAD_PLAYER:
			case RACE_GOBLIN:
				// Capitals
				if(zId == 1657 || zId == 3557 || zId == 1519 || zId == 1537)
					return true;
				break;
		}
	}

	return false;
}
void PlayerBot::HandleBank()
{
	if(!chosen_point)
		chosen_point = urand(1,8);

	switch(bot->getRace())
	{
		case RACE_HUMAN:
		case RACE_DWARF:
		case RACE_GNOME:
		case RACE_NIGHTELF:
		case RACE_DRAENEI:
		case RACE_WORGEN:
			if(bot->GetMapId() != bank_coords[(chosen_point-1)][1] && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = urand(1,8);
				return;
			}
			if(bot->isMoving())
				return;

			if(bot->GetDistance(bank_coords[chosen_point-1][2],bank_coords[chosen_point-1][3],bank_coords[chosen_point-1][4]) >= 2.0f && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				bot->GetMotionMaster()->MovePoint(0,bank_coords[chosen_point-1][2]+urand(0,100)/50,bank_coords[chosen_point-1][3]+urand(0,100)/100,bank_coords[chosen_point-1][4]);
			}
			break;
		case RACE_ORC:
		case RACE_TROLL:
		case RACE_TAUREN:
		case RACE_BLOODELF:
		case RACE_UNDEAD_PLAYER:
		case RACE_GOBLIN:
			if(bot->GetMapId() != bank_coords[(chosen_point+7)][1] && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = urand(1,8);
				return;
			}
			if(bot->isMoving())
				return;
			if(bot->GetDistance(bank_coords[chosen_point+7][2],bank_coords[chosen_point+7][3],bank_coords[chosen_point+7][4]) >= 2.0f && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				bot->GetMotionMaster()->MovePoint(0,bank_coords[chosen_point+7][2]+urand(0,100)/50,bank_coords[chosen_point+7][3]+urand(0,100)/50,bank_coords[chosen_point+7][4]);
			}
			break;
	}
}

void PlayerBot::HandleAuction()
{
	if(!chosen_point)
		chosen_point = urand(1,4);

	switch(bot->getRace())
	{
		case RACE_HUMAN:
		case RACE_DWARF:
		case RACE_GNOME:
		case RACE_NIGHTELF:
		case RACE_DRAENEI:
		case RACE_WORGEN:
			if(bot->GetMapId() != ah_coords[(chosen_point-1)][1] && (bot->GetMapId() == 0 || bot->GetMapId() == 1))
			{
				chosen_point = urand(1,4);
				return;
			}
			if(bot->isMoving())
				return;
			if(bot->GetDistance(ah_coords[chosen_point-1][2],ah_coords[chosen_point-1][3],ah_coords[chosen_point-1][4]) >= 2.0f && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				bot->GetMotionMaster()->MovePoint(0,ah_coords[chosen_point-1][2]+urand(0,100)/50,ah_coords[chosen_point-1][3]+urand(0,100)/50,ah_coords[chosen_point-1][4]);
			}
			break;
		case RACE_ORC:
		case RACE_TROLL:
		case RACE_TAUREN:
		case RACE_BLOODELF:
		case RACE_UNDEAD_PLAYER:
		case RACE_GOBLIN:
			if(bot->GetMapId() != ah_coords[(chosen_point+3)][1] && (bot->GetMapId() == 0 || bot->GetMapId() == 1))
			{
				chosen_point = urand(1,4);
				return;
			}
			if(bot->isMoving())
				return;
			if(bot->GetDistance(ah_coords[chosen_point+3][2],ah_coords[chosen_point+3][3],ah_coords[chosen_point+3][4]) >= 2.0f && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				bot->GetMotionMaster()->MovePoint(0,ah_coords[chosen_point+3][2]+urand(0,100)/50,ah_coords[chosen_point+3][3]+urand(0,100)/50,ah_coords[chosen_point+3][4]);
			}
			break;
	}
}

void PlayerBot::HandleMail()
{
	switch(bot->getRace())
	{
		case RACE_HUMAN:
		case RACE_DWARF:
		case RACE_GNOME:
		case RACE_NIGHTELF:
		case RACE_DRAENEI:
		case RACE_WORGEN:
		{
			if(!chosen_point)
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(ALLIANCE,BCOORD_MAIL);
				return;
			}

			BotCoord* bc = sPlayerBotMgr.GetPoint(ALLIANCE,BCOORD_MAIL,chosen_point);

			if(!bc || bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(ALLIANCE,BCOORD_MAIL);
				return;
			}

			if(bot->isMoving())
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= 2.0f && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				bot->GetMotionMaster()->MovePoint(0,bc->x+urand(0,100)/50,bc->y+urand(0,100)/50,bc->z);
			}
			break;
		}
		case RACE_ORC:
		case RACE_TROLL:
		case RACE_TAUREN:
		case RACE_BLOODELF:
		case RACE_UNDEAD_PLAYER:
		case RACE_GOBLIN:
		{
			if(!chosen_point)
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(HORDE,BCOORD_MAIL);
				return;
			}

			BotCoord* bc = sPlayerBotMgr.GetPoint(HORDE,BCOORD_MAIL,chosen_point);

			if(!bc || bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(HORDE,BCOORD_MAIL);
				return;
			}

			if(bot->isMoving() || !bc)
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= 2.0f && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				bot->GetMotionMaster()->MovePoint(0,bc->x+urand(0,100)/50,bc->y+urand(0,100)/50,bc->z);
			}
			break;
		}
	}
}

void PlayerBot::HandleGoToCorpse()
{
	if(bot->getDeathState() == CORPSE && !bot->GetCorpse()) // need to be before prev condition
	{
		bot->BuildPlayerRepop();
        bot->RepopAtGraveyard();
		bot->GetSession()->HandleMoveWorldportAckOpcode();
		return;
	}

	if(bot->HasAura(8326) && bot->GetCorpse() && bot->GetDistance2d(bot->GetCorpse()) < 5.0f)
	{
		bot->ResurrectPlayer(bot->InBattleGround() ? 1.0f : 0.5f);
		bot->SpawnCorpseBones();
		return;
	}

	if (bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE && bot->GetCorpse())
	{
		bot->GetMotionMaster()->Clear(false);
		bot->GetMotionMaster()->MovePoint(0,bot->GetCorpse()->GetPositionX(),bot->GetCorpse()->GetPositionY(),bot->GetCorpse()->GetPositionZ()+0.1f);
	}
}

void PlayerBot::HandleFearZone()
{
	uint32 zId = bot->GetZoneId();
	
	if(bot->getLevel() < 75)
	{
		switch(bot->getRace())
		{
			case RACE_HUMAN:
			case RACE_DWARF:
			case RACE_GNOME:
			case RACE_NIGHTELF:
			case RACE_DRAENEI:
			case RACE_WORGEN:
				// Capitals
				if(zId == 1637 || zId == 3487 || zId == 1638 || zId == 1497)
					return;
				break;
			case RACE_ORC:
			case RACE_TROLL:
			case RACE_TAUREN:
			case RACE_BLOODELF:
			case RACE_UNDEAD_PLAYER:
			case RACE_GOBLIN:
				// Capitals
				if(zId == 1657 || zId == 3557 || zId == 1519 || zId == 1537)
					return ;
				break;
		}
	}
}

void PlayerBot::HandleRogueCombat()
{
	if(Unit* target = Unit::GetUnit(*bot,bot->GetTargetGUID()))
	{
		GoToCacIfIsnt(target);

		switch(specIdx)
		{
			case 0: // Assass
				break;
			case 1: // Combat
				break;
			case 2: // Finesse
				break;
		}
	}
}

void PlayerBot::HandleShamanCombat()
{
	switch(specIdx)
	{
		case 0: // Heal
			break;
		case 1: // Elem
			break;
		case 2: // Cac
			break;
	}
}

void PlayerBot::HandleDKCombat()
{
	switch(specIdx)
	{
		case 0: // Sang
			break;
		case 1: // Givre
			break;
		case 2: // impie&
			break;
	}
}

void PlayerBot::HandleDruidCombat()
{
	switch(specIdx)
	{
		case 0: // equi
			break;
		case 1: // heal
			break;
		case 2: // cac
			break;
	}
}

void PlayerBot::HandleHunterCombat()
{
	switch(specIdx)
	{
		case 0: // survie
			break;
		case 1: // préci
			break;
		case 2: // ??
			break;
	}
}

void PlayerBot::HandleMageCombat()
{
	switch(specIdx)
	{
		case 0: // feu
			break;
		case 1: // givre
			break;
		case 2: // arcane
			break;
	}
}

void PlayerBot::HandlePaladinCombat()
{
	switch(specIdx)
	{
		case 0: // vindicte
			break;
		case 1: // heal
			break;
		case 2: // proto
			break;
	}
}

void PlayerBot::HandlePriestCombat()
{
	switch(specIdx)
	{
		case 0: // heal
			break;
		case 1: // disci
			break;
		case 2: // shadow
			break;
	}
}

#define POSTURE_DEF			5301
#define POSTURE_ARM			2457
#define POSTURE_FURY		2458
#define SPELL_FRAPPE_HERO	47450
#define SPELL_SANGUINAIRE	2687
#define SPELL_EXEC			47471
#define SPELL_BERSERK		18499
#define SPELL_ENRAGE		2687
#define SPELL_TOURBILLON	1680

void PlayerBot::HandleWarriorCombat()
{
	if(bot->GetPower(POWER_RAGE) < 60)
	{
		bot->CastSpell(bot,SPELL_BERSERK);
		bot->CastSpell(bot,SPELL_ENRAGE);
	}

	if(Unit* target = Unit::GetUnit(*bot,bot->GetSelection()))
	{
		GoToCacIfIsnt(target);

		switch(specIdx)
		{
			case 0: // arme
				if(!bot->HasAura(POSTURE_ARM))
					bot->CastSpell(bot,POSTURE_ARM);
				break;
			case 1: // furie
				if(!bot->HasAura(POSTURE_FURY))
					bot->CastSpell(bot,POSTURE_FURY);

				if(target->GetHealth() * 100.0f / target->GetMaxHealth() < 15.0f && bot->GetPower(POWER_RAGE) >= 35)
					bot->CastSpell(target,SPELL_EXEC);
				
				bot->CastSpell(target,SPELL_TOURBILLON);
				bot->CastSpell(target,SPELL_SANGUINAIRE);

				if(bot->GetPower(POWER_RAGE) >= 40)
					bot->CastSpell(target,SPELL_FRAPPE_HERO);
				break;
			case 2: // proto
				if(!bot->HasAura(POSTURE_DEF))
					bot->CastSpell(bot,POSTURE_DEF);
				break;
		}
	}
}

void PlayerBot::HandleWarlockCombat()
{
	switch(specIdx)
	{
		case 0: // affli
			break;
		case 1: // demono
			break;
		case 2: // destru
			break;
	}
}

void PlayerBot::GoToRandomBGPoint(BattleGroundTypeId bgTypeId)
{
	switch(bgTypeId)
	{
		case BATTLEGROUND_WS:
			break;
	}
}

Unit* PlayerBot::SearchTargetAroundMe()
{
	Map::PlayerList const &PlayerList = bot->GetMap()->GetPlayers();
	Unit* tmpTarget = NULL;
    for(Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
    {
		Player* pPlayer = itr->getSource();
		if(!pPlayer)
			continue;

		if(pPlayer->GetDistance2d(bot) > 50.0f)
			continue;

		if(!pPlayer->isAlive())
			continue;
		
		if(pPlayer->GetBGTeam() == bot->GetTeam())
			continue;

		if(!tmpTarget || tmpTarget->GetDistance2d(bot) > pPlayer->GetDistance2d(bot))
			tmpTarget = pPlayer;
	}
	return tmpTarget;
}

void PlayerBot::HandleWarsong(uint32 diff)
{
	BattleGroundWS* bg = (BattleGroundWS*)bot->GetBattleGround();
	BattleGroundTeamId bgTeamId = BattleGroundTeamId(bot->GetBGTeam());

	if(mode_Timer <= diff)
	{
		m_mode = BotMode(urand(0,2));
		mode_Timer = urand(60000,300000);
	}
	else
		mode_Timer -= diff;

	if(bg->GetStatus() != STATUS_IN_PROGRESS)
		return;

	switch(m_mode)
	{
		case MODE_ATTACKER:
		{
			Player* flagOwner = NULL;
			if(bgTeamId == BG_TEAM_ALLIANCE)
				flagOwner = sObjectMgr.GetPlayer(bg->GetHordeFlagPickerGUID());
			else
				flagOwner = sObjectMgr.GetPlayer(bg->GetAllianceFlagPickerGUID());
			if(flagOwner)
			{
				if(!bot->GetSelection() != flagOwner->GetGUID())
					bot->SetSelection(flagOwner->GetGUID());

				if(bot->GetDistance2d(flagOwner) > 40.0f)
				{
					bot->GetMotionMaster()->MoveChase(flagOwner);
					return;
				}
			}
			else
			{
				Unit* seekTarget = SearchTargetAroundMe();
				if(!seekTarget)
				{
					GoToRandomBGPoint(BATTLEGROUND_WS);
					return;
				}
				bot->SetSelection(seekTarget->GetGUID());
				m_decideToFight = true;
			}
			break;
		}
		case MODE_DEFENDER:
		{
			if(act_Timer <= diff)
			{
				if(bot->GetDistance(925.0f,1435.0f,324.618f) > 50.0f)
					bot->GetMotionMaster()->MovePoint(0,urand(923,942),urand(1424,1447),330.618f);
				if(!bot->isMovingOrTurning())
					if(Unit* seekTarget = SearchTargetAroundMe())
						bot->SetSelection(seekTarget->GetGUID());
				act_Timer = urand(750,1500);
			}
			else
				act_Timer -= diff;
			break;
		}
		case MODE_OBJECTIVE:
			break;
	}
}

void PlayerBot::HandleArathi(uint32 diff)
{
	switch(m_mode)
	{
		case MODE_ATTACKER:
			break;
		case MODE_DEFENDER:
			break;
		case MODE_OBJECTIVE:
			break;
	}
}

void PlayerBot::HandleEyeOfTheStorm(uint32 diff)
{
}

void PlayerBot::HandleAlterac(uint32 diff)
{
	if(mode_Timer <= diff)
	{
		m_mode = BotMode(urand(0,2));
		mode_Timer = 300000;
	}
	else
		mode_Timer -= diff;
}