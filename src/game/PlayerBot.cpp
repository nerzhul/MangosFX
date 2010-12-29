#include "PlayerBot.h"
#include <Policies/SingletonImp.h>
#include "ObjectMgr.h"
#include "GameObject.h"
#include "BattleGroundAB.h"
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

float PlayerBotMgr::GetChance(BotChoice bc)
{
	BotChance::iterator itr = m_choiceChances.find(uint32(bc));
	if(itr != m_choiceChances.end())
		return itr->second;

	return 0.0f;
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

	for(BotCoords::iterator itr = bank_a.begin(); itr != bank_a.end();)
	{
		BotCoords::iterator next = ++itr;
		--itr;
		delete itr->second;
		bank_a.erase(itr);
		itr = next;
	}

	for(BotCoords::iterator itr = bank_h.begin(); itr != bank_h.end();)
	{
		BotCoords::iterator next = ++itr;
		--itr;
		delete itr->second;
		bank_h.erase(itr);
		itr = next;
	}

	for(BotCoords::iterator itr = warsong.begin(); itr != warsong.end();)
	{
		BotCoords::iterator next = ++itr;
		--itr;
		delete itr->second;
		warsong.erase(itr);
		itr = next;
	}
}

void PlayerBotMgr::LoadBotCoordinates()
{
	uint32 count = 0;
	CleanCoordinates();
	if(QueryResult* result = WorldDatabase.PQuery("SELECT `type`,`faction`,`map`,`x`,`y`,`z`,`id`,`range`,`maxdist` FROM playerbot_coordinates"))
	{
		do
		{
			Field* fields = result->Fetch();
			BotCoord* bc = new BotCoord();
			bc->mapId = fields[2].GetUInt32();
			bc->x = fields[3].GetFloat();
			bc->y = fields[4].GetFloat();
			bc->z = fields[5].GetFloat();
			bc->range = fields[7].GetFloat();
			bc->maxdist = fields[8].GetFloat();
			switch(BotCoordType(fields[0].GetUInt32()))
			{
				case BCOORD_MAIL:
				{
					if(fields[1].GetUInt32() == 0)
						mail_a[fields[6].GetUInt32()] = bc;
					else
						mail_h[fields[6].GetUInt32()] = bc;
					break;
				}
				case BCOORD_BANK:
				{
					if(fields[1].GetUInt32() == 0)
						bank_a[fields[6].GetUInt32()] = bc;
					else
						bank_h[fields[6].GetUInt32()] = bc;
					break;
				}
				case BCOORD_RANDOM:
				{
					if(fields[1].GetUInt32() == 0)
						random_a[fields[6].GetUInt32()] = bc;
					else
						random_h[fields[6].GetUInt32()] = bc;
					break;
				}
				case BCOORD_WARSONG:
				{
					warsong[fields[6].GetUInt32()] = bc;
					break;
				}
				case BCOORD_ARATHI:
				{
					arathi[fields[6].GetUInt32()] = bc;
					break;
				}
				case BCOORD_CYCLONE:
				{
					eyeofthestorm[fields[6].GetUInt32()] = bc;
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
				uint32 pos = 0;
				uint32 idxchosen = urand(0,mail_a.size()-1);
				for(BotCoords::const_iterator itr = mail_a.begin(); itr != mail_a.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
			case BCOORD_BANK:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,bank_a.size()-1);
				for(BotCoords::const_iterator itr = bank_a.begin(); itr != bank_a.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
			case BCOORD_RANDOM:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,random_a.size()-1);
				for(BotCoords::const_iterator itr = random_a.begin(); itr != random_a.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
		}
	}
	else if(faction == HORDE)
	{
		switch(bcType)
		{
			case BCOORD_MAIL:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,mail_h.size()-1);
				for(BotCoords::const_iterator itr = mail_h.begin(); itr != mail_h.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
			case BCOORD_BANK:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,bank_h.size()-1);
				for(BotCoords::const_iterator itr = bank_h.begin(); itr != bank_h.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
			case BCOORD_RANDOM:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,random_h.size()-1);
				for(BotCoords::const_iterator itr = random_h.begin(); itr != random_h.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
		}
	}
	else
	{
		switch(bcType)
		{
			case BCOORD_WARSONG:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,warsong.size()-1);
				for(BotCoords::const_iterator itr = warsong.begin(); itr != warsong.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
			case BCOORD_ARATHI:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,arathi.size()-1);
				for(BotCoords::const_iterator itr = arathi.begin(); itr != arathi.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
			}
			case BCOORD_CYCLONE:
			{
				uint32 pos = 0;
				uint32 idxchosen = urand(0,eyeofthestorm.size()-1);
				for(BotCoords::const_iterator itr = eyeofthestorm.begin(); itr != eyeofthestorm.end(); ++itr)
				{
					if(pos == idxchosen)
						return itr->first;
					++pos;
				}
				break;
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
				break;
			}
			case BCOORD_BANK:
			{
				BotCoords::iterator itr = bank_a.find(idx);
				if(itr != bank_a.end())
					return itr->second;
				break;
			}
			case BCOORD_RANDOM:
			{
				BotCoords::iterator itr = random_a.find(idx);
				if(itr != random_a.end())
					return itr->second;
				break;
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
				break;
			}
			case BCOORD_BANK:
			{
				BotCoords::iterator itr = bank_h.find(idx);
				if(itr != bank_h.end())
					return itr->second;
				break;
			}
			case BCOORD_RANDOM:
			{
				BotCoords::iterator itr = random_h.find(idx);
				if(itr != random_h.end())
					return itr->second;
				break;
			}
		}
	}
	else
	{
		switch(bcType)
		{
			case BCOORD_WARSONG:
			{
				BotCoords::iterator itr = warsong.find(idx);
				if(itr != warsong.end())
					return itr->second;
				break;
			}
			case BCOORD_ARATHI:
			{
				BotCoords::iterator itr = arathi.find(idx);
				if(itr != arathi.end())
					return itr->second;
				break;
			}
			case BCOORD_CYCLONE:
			{
				BotCoords::iterator itr = eyeofthestorm.find(idx);
				if(itr != eyeofthestorm.end())
					return itr->second;
				break;
			}
		}
	}

	return NULL;
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

	if(bot->GetDistance2d(target) >= 2.0f)
		GoPoint(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ());
	else
		bot->SetFacingToObject(target);
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
		if(bot->isDead() && bot->GetBattleGround())
			Stay();

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
			if(plr->isAlive() || plr->GetDistance2d(bot) > 75.0f)
			{
				if(Unit* target = SearchTargetAroundMe())
					bot->SetSelection(target->GetGUID());
				else
				{
					m_decideToFight = false;
					bot->SetSelection(0);
					return;
				}
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
		return;
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
				case BCHOICE_GO_ZONE:
					HandleGoZone();
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

void PlayerBot::GoPoint(BotCoord* bc)
{
	float x = bc->x+irand(-bc->range,bc->range)/100;
	float y = bc->y+irand(-bc->range,bc->range)/100;
	float z = bot->GetMap()->GetHeight(x,y,bc->z,100.0f);

	Stay();
	GoPoint(x,y,z);
}
void PlayerBot::ChooseToDoSomething()
{
	float randAct = float(urand(1,1000));
	float maxCh = 0.0f;
	choice_Timer = urand(600000,1800000);
	bot->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK);

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_PVP)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_PVP;
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_FARM_MOBS)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_FARM_MOBS;
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_GO_ZONE)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_GO_ZONE;
		chosen_point = 0;
		choice_Timer = urand(30000,540000);
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_QUEST)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_QUEST;
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_EXPLORE)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_EXPLORE;
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_FARM_MINERALS)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_FARM_MINERALS;
		m_choice = BCHOICE_FARM_HERBS;
		m_choice = BCHOICE_FARM_LEATHER;
		m_choice = BCHOICE_FARM_CLOTH;
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_LEARN_SPELLS)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_LEARN_SPELLS;
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_AUCTION)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_AUCTION;
		chosen_point = 0;
		choice_Timer = urand(60000,540000);
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_BANK)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_BANK;
		chosen_point = 0;
		choice_Timer = urand(30000,400000);
		return;
	}

	maxCh += sPlayerBotMgr.GetChance(BCHOICE_MAIL)*10.0f;

	if(randAct < maxCh)
	{
		m_choice = BCHOICE_MAIL;
		chosen_point = 0;
		choice_Timer = urand(40000,400000);
		return;
	}

	m_choice = BCHOICE_AFK;
	if(!bot->isAFK())
		bot->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK);
	choice_Timer = urand(60000,3600000);
}

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
				chosen_point = sPlayerBotMgr.GetRandomPoint(ALLIANCE,BCOORD_BANK);
				return;
			}

			BotCoord* bc = sPlayerBotMgr.GetPoint(ALLIANCE,BCOORD_BANK,chosen_point);

			if(!bc || bot->GetDistance(bc->x,bc->y,bc->z) > 5000.0f ||  bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 530 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(ALLIANCE,BCOORD_BANK);
				return;
			}

			if(bot->isMoving())
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= bc->maxdist && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				GoPoint(bc);
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
				chosen_point = sPlayerBotMgr.GetRandomPoint(HORDE,BCOORD_BANK);
				return;
			}

			BotCoord* bc = sPlayerBotMgr.GetPoint(HORDE,BCOORD_BANK,chosen_point);

			if(!bc || bot->GetDistance(bc->x,bc->y,bc->z) > 5000.0f || bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 530 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(HORDE,BCOORD_BANK);
				return;
			}

			if(bot->isMoving() || !bc)
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= bc->maxdist && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				GoPoint(bc);
			}
			break;
		}
	}
}

void PlayerBot::HandleGoZone()
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
				chosen_point = sPlayerBotMgr.GetRandomPoint(ALLIANCE,BCOORD_RANDOM);
				return;
			}

			BotCoord* bc = sPlayerBotMgr.GetPoint(ALLIANCE,BCOORD_RANDOM,chosen_point);

			if(!bc || bot->GetDistance(bc->x,bc->y,bc->z) > 5000.0f || bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 530 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(ALLIANCE,BCOORD_RANDOM);
				return;
			}

			if(bot->isMoving())
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= bc->maxdist && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				GoPoint(bc);
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
				chosen_point = sPlayerBotMgr.GetRandomPoint(HORDE,BCOORD_RANDOM);
				return;
			}

			BotCoord* bc = sPlayerBotMgr.GetPoint(HORDE,BCOORD_RANDOM,chosen_point);

			if(!bc || bot->GetDistance(bc->x,bc->y,bc->z) > 5000.0f || bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 530 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(HORDE,BCOORD_RANDOM);
				return;
			}

			if(bot->isMoving() || !bc)
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= bc->maxdist && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				GoPoint(bc);
			}
			break;
		}
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

			if(!bc || bot->GetDistance(bc->x,bc->y,bc->z) > 5000.0f || bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(ALLIANCE,BCOORD_MAIL);
				return;
			}

			if(bot->isMoving())
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= bc->maxdist && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				GoPoint(bc);
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

			if(!bc || bot->GetDistance(bc->x,bc->y,bc->z) > 5000.0f || bot->GetMapId() != bc->mapId && (bot->GetMapId() == 0 || bot->GetMapId() == 1 || bot->GetMapId() == 571 && bot->GetZoneId() == 4395))
			{
				chosen_point = sPlayerBotMgr.GetRandomPoint(HORDE,BCOORD_MAIL);
				return;
			}

			if(bot->isMoving())
				return;

			if(bot->GetDistance(bc->x,bc->y,bc->z) >= bc->maxdist && bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				bot->GetMotionMaster()->Clear(false);
				GoPoint(bc);
			}
			break;
		}
	}
}

void PlayerBot::HandleGoToCorpse()
{
	if(!bot->GetCorpse()) // need to be before prev condition
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
			if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,urand(154,169)))
				GoPoint(bc);
			break;
		case BATTLEGROUND_AB:
			if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_ARATHI,urand(216,231)))
				GoPoint(bc);
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

		if(pPlayer->GetDistance2d(bot) > 44.0f)
			continue;

		if(!pPlayer->isAlive())
			continue;
		
		if(pPlayer->GetTeam() == bot->GetTeam())
			continue;

		if(pPlayer->isGameMaster())
			continue;

		if(!tmpTarget || tmpTarget->GetDistance2d(bot) > pPlayer->GetDistance2d(bot))
			tmpTarget = pPlayer;
	}
	bot->CastSpell(tmpTarget,6603);
	return tmpTarget;
}

void PlayerBot::UseGameObject(uint64 guid)
{
	GameObject *obj = bot->GetMap()->GetGameObject(guid);

    if(!obj)
        return;

    obj->Use(bot);
}

void PlayerBot::GoToRandomBGStartingPoint(BattleGroundTypeId bgTypeId,uint32 diff)
{
	if(act_Timer <= diff && isStaying())
	{
		switch(bgTypeId)
		{
			case BATTLEGROUND_WS:
				if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,bot->GetTeam() == ALLIANCE ? 135 : 134))
					GoPoint(bc);
				break;
			case BATTLEGROUND_AB:
				if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,bot->GetTeam() == ALLIANCE ? 195 : 194))
					GoPoint(bc);
				break;
		}
		act_Timer = urand(3000,12000);
	}
	else
		act_Timer -= diff;
}

void PlayerBot::HandleWarsong(uint32 diff)
{
	BattleGroundWS* bg = (BattleGroundWS*)bot->GetBattleGround();
	BattleGroundTeamId bgTeamId = BattleGroundTeamId(bot->GetBGTeam());

	if(mode_Timer <= diff)
	{
		m_mode = BotMode(urand(0,2));
		mode_Timer = urand(30000,60000);
	}
	else
		mode_Timer -= diff;

	if(bg->GetStatus() != STATUS_IN_PROGRESS)
	{
		GoToRandomBGStartingPoint(BATTLEGROUND_WS,diff);
		return;
	}

	switch(m_mode)
	{
		case MODE_ATTACKER:
		{
			if(act_Timer <= diff || isStaying())
			{
				Player* flagOwner = NULL;
				if(bot->GetTeam() == ALLIANCE)
					flagOwner = sObjectMgr.GetPlayer(bg->GetHordeFlagPickerGUID());
				else
					flagOwner = sObjectMgr.GetPlayer(bg->GetAllianceFlagPickerGUID());
				if(flagOwner && flagOwner->GetDistance2d(bot) < 45.0f)
				{
					if(!bot->GetSelection() != flagOwner->GetGUID())
						bot->SetSelection(flagOwner->GetGUID());

					if(bot->GetDistance2d(flagOwner) > 40.0f || 
						(bot->getClass() == CLASS_DEATH_KNIGHT || bot->getClass() == CLASS_WARRIOR || bot->getClass() == CLASS_ROGUE) && bot->GetDistance2d(flagOwner) > 3.0f)
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
						act_Timer = urand(4000,5000);
						return;
					}
					if(bot->GetSelection() != seekTarget->GetGUID())
					{
						bot->SetSelection(seekTarget->GetGUID());
						m_decideToFight = true;
					}
					act_Timer = urand(10000,30000);
				}
			}
			else
				act_Timer -= diff;
			break;
		}
		case MODE_DEFENDER:
		{
			if(act_Timer <= diff || bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				if(!HasDecidedToFight())
				{
					if(bot->GetTeam() == HORDE)
					{
						if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,134))
							if(bot->GetDistance2d(bc->x,bc->y) > 50.0f)
								GoPoint(bc);
					}
					else
					{
						if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,135))
							if(bot->GetDistance2d(bc->x,bc->y) > 50.0f)
								GoPoint(bc);
					}
					
					if(Unit* seekTarget = SearchTargetAroundMe())
					{
						Stay();
						bot->SetSelection(seekTarget->GetGUID());
						m_decideToFight = true;
					}
				}
				act_Timer = urand(750,1500);
			}
			else
				act_Timer -= diff;
			break;
		}
		case MODE_OBJECTIVE:
		{
			if(act_Timer <= diff)
			{
				if(bot->GetTeam() == HORDE)
				{
					if(Player* flagowner = sObjectMgr.GetPlayer(bg->GetHordeFlagPickerGUID()))
					{
						if(flagowner->GetDistance2d(bot) > 25.0f)
							GoPoint(flagowner->GetPositionX(),flagowner->GetPositionY(),flagowner->GetPositionZ()+0.1f);
						else
						{
							if(bot->GetSelection() != flagowner->GetGUID())
							{
								bot->SetSelection(flagowner->GetGUID());
								m_decideToFight = true;
							}
						}
					}
					else
					{
						if(Player* flagowner = sObjectMgr.GetPlayer(bg->GetAllianceFlagPickerGUID()))
						{
							if(flagowner == bot)
							{
								if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,136))
								{
									if(bot->GetDistance2d(bc->x,bc->y) > 2.0f)
										GoPoint(bc);
									else
										bg->HandleAreaTrigger(bot,3647);
								}
							}
							else if(flagowner->GetDistance2d(bot) > 25.0f)
								GoPoint(flagowner->GetPositionX(),flagowner->GetPositionY(),flagowner->GetPositionZ()+0.1f);
							else
							{
								if(Unit* seekTarget = SearchTargetAroundMe())
								{
									Stay();
									if(bot->GetSelection() != seekTarget->GetGUID())
									{
										bot->SetSelection(seekTarget->GetGUID());
										m_decideToFight = true;
									}
								}
							}
						}
						else
						{
							if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,137))
							{
								if(bot->GetDistance2d(bc->x,bc->y) > 5.0f)
									GoPoint(bc);
								else if(GameObject* go = bot->GetClosestGameObjectWithEntry(179830,10.0f))
									UseGameObject(go->GetGUID());
							}
						}
					}
				}
				else if(bot->GetTeam() == ALLIANCE)
				{
					if(Player* flagowner = sObjectMgr.GetPlayer(bg->GetAllianceFlagPickerGUID()))
					{
						if(flagowner->GetDistance2d(bot) > 25.0f)
							GoPoint(flagowner->GetPositionX(),flagowner->GetPositionY(),flagowner->GetPositionZ()+0.1f);
						else
						{
							if(bot->GetSelection() != flagowner->GetGUID())
							{
								bot->SetSelection(flagowner->GetGUID());
								m_decideToFight = true;
							}
						}
					}
					else
					{
						if(Player* flagowner = sObjectMgr.GetPlayer(bg->GetHordeFlagPickerGUID()))
						{
							if(flagowner == bot)
							{
								if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,137))
								{
									if(bot->GetDistance2d(bc->x,bc->y) > 2.0f)
										GoPoint(bc);
									else
										bg->HandleAreaTrigger(bot,3646);
								}
							}
							else if(flagowner->GetDistance2d(bot) > 25.0f)
								GoPoint(flagowner->GetPositionX(),flagowner->GetPositionY(),flagowner->GetPositionZ()+0.1f);
							else
							{
								if(Unit* seekTarget = SearchTargetAroundMe())
								{
									Stay();
									if(bot->GetSelection() != flagowner->GetGUID())
									{
										bot->SetSelection(seekTarget->GetGUID());
										m_decideToFight = true;
									}
								}
							}
						}
						else
						{
							if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_WARSONG,136))
							{
								if(bot->GetDistance2d(bc->x,bc->y) > 5.0f)
									GoPoint(bc);
								else if(GameObject* go = bot->GetClosestGameObjectWithEntry(179831,10.0f))
									UseGameObject(go->GetGUID());
							}
						}
					}
				}

				act_Timer = urand(1000,3000);
			}
			else
				act_Timer -= diff;
			break;
		}
	}
}

void PlayerBot::HandleArathi(uint32 diff)
{
	BattleGroundAB* bg = (BattleGroundAB*)bot->GetBattleGround();
	BattleGroundTeamId bgTeamId = BattleGroundTeamId(bot->GetBGTeam());

	if(mode_Timer <= diff)
	{
		m_mode = BotMode(urand(0,2));
		mode_Timer = urand(30000,60000);
	}
	else
		mode_Timer -= diff;

	if(bg->GetStatus() != STATUS_IN_PROGRESS)
	{
		GoToRandomBGStartingPoint(BATTLEGROUND_AB,diff);
		return;
	}

	switch(m_mode)
	{
		case MODE_ATTACKER:
		{
			if(act_Timer <= diff || isStaying())
			{
				Unit* seekTarget = SearchTargetAroundMe();
				if(!seekTarget)
				{
					GoToRandomBGPoint(BATTLEGROUND_AB);
					act_Timer = urand(4000,15000);
					return;
				}
				if(bot->GetSelection() != seekTarget->GetGUID())
				{
					bot->SetSelection(seekTarget->GetGUID());
					m_decideToFight = true;
				}
				act_Timer = urand(10000,30000);
			}
			else
				act_Timer -= diff;
			break;
		}
		case MODE_DEFENDER:
		{
			if(act_Timer <= diff)
			{
				std::vector<uint8> goodNodes;
				goodNodes.clear();
				if(bot->GetTeam() == ALLIANCE)
				{
					if(bg->getNodePoint(0) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(0) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // st
						goodNodes.push_back(0);
					if(bg->getNodePoint(1) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(1) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // bs
						goodNodes.push_back(1);
					if(bg->getNodePoint(2) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(2) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // farm
						goodNodes.push_back(2);
					if(bg->getNodePoint(3) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(3) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // sci
						goodNodes.push_back(3);
					if(bg->getNodePoint(4) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(4) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // mine
						goodNodes.push_back(4);
				}
				else
					{
					if(bg->getNodePoint(0) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(0) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // st
						goodNodes.push_back(0);
					if(bg->getNodePoint(1) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(1) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // bs
						goodNodes.push_back(1);
					if(bg->getNodePoint(2) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(2) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // farm
						goodNodes.push_back(2);
					if(bg->getNodePoint(3) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(3) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // sci
						goodNodes.push_back(3);
					if(bg->getNodePoint(4) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(4) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // mine
						goodNodes.push_back(4);
				}
				if(goodNodes.empty())
				{
					m_mode = MODE_DEFENDER;
					return;
				}
				uint32 pId = 0;
				switch(urand(0,goodNodes.size()-1))
				{
					case 0:	pId = 196;	break;
					case 1:	pId = 198;	break;
					case 2:	pId = 200;	break;
					case 3:	pId = 197;	break;
					case 4:	pId = 199;	break;
					
				}
				if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_ARATHI,pId))
				{
					if(bot->GetDistance2d(bc->x,bc->y) > 5.0f)
						GoPoint(bc);
					else
					{
					}
				}
				act_Timer = urand(10000,30000);
			}
			else
				act_Timer -= diff;
			break;
		}
		case MODE_OBJECTIVE:
		{
			if(act_Timer <= diff)
			{
				std::vector<uint8> goodNodes;
				goodNodes.clear();
				if(bot->GetTeam() == ALLIANCE)
				{
					if(bg->getNodePoint(0) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(0) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // st
						goodNodes.push_back(0);
					if(bg->getNodePoint(1) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(1) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // bs
						goodNodes.push_back(1);
					if(bg->getNodePoint(2) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(2) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // farm
						goodNodes.push_back(2);
					if(bg->getNodePoint(3) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(3) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // sci
						goodNodes.push_back(3);
					if(bg->getNodePoint(4) != BG_AB_NODE_STATUS_ALLY_CONTESTED && bg->getNodePoint(4) != BG_AB_NODE_STATUS_ALLY_OCCUPIED) // mine
						goodNodes.push_back(4);
				}
				else
					{
					if(bg->getNodePoint(0) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(0) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // st
						goodNodes.push_back(0);
					if(bg->getNodePoint(1) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(1) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // bs
						goodNodes.push_back(1);
					if(bg->getNodePoint(2) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(2) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // farm
						goodNodes.push_back(2);
					if(bg->getNodePoint(3) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(3) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // sci
						goodNodes.push_back(3);
					if(bg->getNodePoint(4) != BG_AB_NODE_STATUS_HORDE_CONTESTED && bg->getNodePoint(4) != BG_AB_NODE_STATUS_HORDE_OCCUPIED) // mine
						goodNodes.push_back(4);
				}
				if(goodNodes.empty())
				{
					m_mode = MODE_DEFENDER;
					return;
				}
				uint32 pId = 0;
				switch(urand(0,goodNodes.size()-1))
				{
					case 0:	pId = 196;	break;
					case 1:	pId = 198;	break;
					case 2:	pId = 200;	break;
					case 3:	pId = 197;	break;
					case 4:	pId = 199;	break;
					
				}
				if(BotCoord* bc = sPlayerBotMgr.GetPoint(0,BCOORD_ARATHI,pId))
				{
					if(bot->GetDistance2d(bc->x,bc->y) > 5.0f)
						GoPoint(bc);
					else
					{
					}
				}
				act_Timer = urand(10000,30000);
			}
			else
				act_Timer -= diff;
			break;
		}
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