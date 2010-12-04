
#include "PlayerBot.h"
#include "BattleGroundWS.h"

PlayerBot::PlayerBot(WorldSession* session)//: Player(session)
{
	specIdx = 0;
	m_decideToFight = false;
	m_sheduledBGJoin = DAY*HOUR;
	bgTypeId = BATTLEGROUND_TYPE_NONE;
	m_ginfo = 0;
	sheduledBG = NULL;
	mode_Timer = 5000;
	act_Timer = 1000;
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
	
	JoinBGQueueIfNotIn();

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