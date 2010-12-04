#ifndef __PLAYERBOT_H_
#define __PLAYERBOT_H_

#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"

enum BotMode
{
	MODE_ATTACKER	=	0,
	MODE_DEFENDER	=	1,
	MODE_OBJECTIVE	=	2,
};
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

		void HandleWarsong(uint32 diff);
		void HandleArathi(uint32 diff);
		void HandleEyeOfTheStorm(uint32 diff);
		void HandleAlterac(uint32 diff);

		bool HasDecidedToFight() { return m_decideToFight; }

		Player* GetPlayer() { return bot; }
		void SetPlayer(Player* plr) { bot = plr; }

		void Update(uint32 diff);

		// Units
		Unit* SearchTargetAroundMe();
		// Movements
		void Stay();
		void GoToCacIfIsnt(Unit* target);
		void GoToRandomBGPoint(BattleGroundTypeId bgTypeId);

		// BG Handlers
		void JoinBGQueueIfNotIn();
		void SheduleSendToBG(BattleGround* bg, BattleGroundTypeId btId, GroupQueueInfo* ginfo);
		void SendToBg();
	private:
		uint8 specIdx;
		bool m_decideToFight;
		Player* bot;
		BotMode m_mode;
		
		// Timers
		uint32 mode_Timer;
		uint32 act_Timer;

		BattleGround* sheduledBG;
		BattleGroundTypeId bgTypeId;
		GroupQueueInfo* m_ginfo;
		uint32 m_sheduledBGJoin;
};

#endif