#ifndef __PLAYERBOT_H_
#define __PLAYERBOT_H_

#include "Player.h"
#include "WorldSession.h"

class PlayerBot: public Player
{
	public:
		explicit PlayerBot (WorldSession *session);
		~PlayerBot();

		void HandleWarriorCombat();
		void HandlePaladinCombat();
		void HandleHunterCombat();
		void HandleRogueCombat();
		void HandlePriestCombat();
		void HandleDKCombat();
		void HandleShamanCombat();
		void HandleMageCombat();
		void HandleWarlockCombat();
		void HandleDruidCombat();

		bool HasDecidedToFight() { return m_decideToFight; }

		void Update(uint32 diff);
		void Stay();
	private:
		uint8 specIdx;
		bool m_decideToFight;
};
#endif