#include "PlayerBot.h"

PlayerBot::PlayerBot(WorldSession* session): Player(session)
{
}

PlayerBot::~PlayerBot()
{
}

void PlayerBot::Update(uint32 diff)
{
	

	// CombatHandler for all classes
	if(isInCombat())
	{
		switch(getClass())
		{
			case CLASS_WARRIOR:
				break;
			case CLASS_PALADIN:
				break;
			case CLASS_HUNTER:
				break;
			case CLASS_ROGUE:
				break;
			case CLASS_PRIEST:
				break;
			case CLASS_DEATH_KNIGHT:
				break;
			case CLASS_SHAMAN:
				break;
			case CLASS_MAGE:
				break;
			case CLASS_WARLOCK:
				break;
			case CLASS_DRUID:
				break;
		}
	}
	// 
}