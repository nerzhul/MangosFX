#ifndef __PLAYERBOT_H_
#define __PLAYERBOT_H_

#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"

class PlayerBot// : public Player
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

		void HandleWarsong();
		void HandleArathi();
		void HandleEyeOfTheStorm();
		void HandleAlterac();

		bool HasDecidedToFight() { return m_decideToFight; }

		Player* GetPlayer() { return bot; }
		void SetPlayer(Player* plr) { bot = plr; }

		void Update(uint32 diff);

		// Movements
		void Stay();
		void GoToCacIfIsnt(Unit* target);

		// BG Handlers
		void JoinBGQueueIfNotIn();
		void SheduleSendToBG(BattleGround* bg, BattleGroundTypeId btId, GroupQueueInfo* ginfo);
		void SendToBg();
	private:
		uint8 specIdx;
		bool m_decideToFight;
		Player* bot;

		BattleGround* sheduledBG;
		BattleGroundTypeId bgTypeId;
		GroupQueueInfo* m_ginfo;
		uint32 m_sheduledBGJoin;
};

#endif