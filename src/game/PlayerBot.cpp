#include "PlayerBot.h"


PlayerBot::PlayerBot(WorldSession* session)//: Player(session)
{
	specIdx = 0;
	m_decideToFight = false;
}

PlayerBot::~PlayerBot()
{
	delete bot;
}

void PlayerBot::Stay()
{
	GetMotionMaster()->Clear();
}

void PlayerBot::Update(uint32 diff)
{
	Stay();
	
	if (!bot->InBattleGroundQueue())
	{
		bot->Say("C'est fou! J'adore les BGs !", LANG_UNIVERSAL);
	}

	// CombatHandler for all classes
	if(HasDecidedToFight())
	{
		switch(getClass())
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
}


void PlayerBot::HandleRogueCombat()
{
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

#define POSTURE_DEF 5301
#define POSTURE_ARM 2457
#define POSTURE_FURY 2458
#define SPELL_FRAPPE_HERO 47450

void PlayerBot::HandleWarriorCombat()
{
	if(GetPower(POWER_RAGE) < 60)
	{
		CastSpell(this,SPELL_BERSERK);
		CastSpell(this,SPELL_ENRAGE);
	}

	if(Unit* target = Unit::GetUnit(*this,GetTargetGUID()))
	{
		if(IsNonMeleeSpellCasted(false))
			return;
	
		if(!GetDistance2d(target) >= 4.5f)
		{
			GetMotionMaster()->MoveChase(target,3.0f);
			return;
		}

		switch(specIdx)
		{
			case 0: // arme
				if(!HasAura(POSTURE_ARM))
					CastSpell(this,POSTURE_ARM);
				break;
			case 1: // furie
				if(!HasAura(POSTURE_FURY))
					CastSpell(this,POSTURE_FURY);

				if(target->GetHealth() * 100.0f / target->GetMaxHealth() < 15.0f && GetPower(POWER_RAGE) >= 35)
					CastSpell(target,SPELL_EXEC);
				
				CastSpell(target,SPELL_TOURBILLON);
				CastSpell(target,SPELL_SANGUINAIRE);

				if(GetPower(POWER_RAGE) >= 40)
					CastSpell(target,SPELL_FRAPPE_HERO);
				break;
			case 2: // proto
				if(!HasAura(POSTURE_DEF))
					CastSpell(this,POSTURE_DEF);
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