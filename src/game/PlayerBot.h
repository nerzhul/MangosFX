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

enum BotChoice
{
	BCHOICE_PVP			=	0,
	BCHOICE_FARM_MOBS,
	BCHOICE_GO_ZONE,
	BCHOICE_QUEST,
	BCHOICE_EXPLORE,
	BCHOICE_FARM_MINERALS,
	BCHOICE_FARM_HERBS,
	BCHOICE_FARM_LEATHER,
	BCHOICE_FARM_CLOTH,
	BCHOICE_LEARN_SPELLS,
	BCHOICE_AUCTION,
	BCHOICE_AFK,
	BCHOICE_BANK,
	BCHOICE_ATTACK_CAP,
	BCHOICE_MAIL,
	MAX_BCHOICE			=	15,
};

enum BotCoordType
{
	BCOORD_MAIL		= 0,
	BCOORD_BANK		= 1,
	BCOORD_AH		= 2,
	BCOORD_RANDOM	= 3,
	BCOORD_WARSONG	= 4,
	BCOORD_ARATHI	= 5,
	BCOORD_CYCLONE	= 6,
};

#define MAX_SUPPORTED_BG	3
struct BotCoord
{
	float x,y,z,range,maxdist;
	uint32 mapId;
};

struct MountObj
{
	uint32 mountId;
	bool flying;
	uint8 reqrace;
};

typedef std::map<uint32,float> BotChance;
typedef std::vector<MountObj*> MountList;
typedef std::map<uint32,BotCoord*> BotCoords;
class PlayerBotMgr
{
	public:
		explicit PlayerBotMgr();
		~PlayerBotMgr();

		void LoadBotChoiceChances();
		void LoadBotCoordinates();
		void LoadBotMounts();
		
		// Chances
		float GetChance(BotChoice bc);
		// Coordinates
		void CleanCoordinates();
		uint32 GetRandomPoint(uint32 faction,BotCoordType bcType);
		BotCoord* GetPoint(uint32 faction, BotCoordType bcType, uint32 idx);
		// Mounts
		MountList GetMountList(uint32 faction) { return (faction == ALLIANCE) ? mounts_a : mounts_h; }
	private:
		BotChance m_choiceChances;
		BotCoords mail_h;
		BotCoords mail_a;
		BotCoords bank_h;
		BotCoords bank_a;
		BotCoords random_h;
		BotCoords random_a;
		BotCoords warsong;
		BotCoords arathi;
		BotCoords eyeofthestorm;
		MountList mounts_a;
		MountList mounts_h;

};

#define sPlayerBotMgr MaNGOS::Singleton<PlayerBotMgr>::Instance()

class PlayerBot// : public Player
{
	public:
		explicit PlayerBot (WorldSession *session);
		~PlayerBot();

		void HandleWarriorCombat(uint32 diff);
		void HandlePaladinCombat(uint32 diff);
		void HandleHunterCombat();
		void HandleRogueCombat(uint32 diff);
		void HandlePriestCombat();
		void HandleDKCombat();
		void HandleShamanCombat();
		void HandleMageCombat();
		void HandleWarlockCombat(uint32 diff);
		void HandleDruidCombat();

		void HandleGoToCorpse();

		void HandleWarsong(uint32 diff);
		void HandleArathi(uint32 diff);
		void HandleEyeOfTheStorm(uint32 diff);
		void HandleAlterac(uint32 diff);

		void HandleAuction();
		void HandleBank();
		void HandleMail();
		void HandleGoZone();

		void HandleFearZone();

		bool HasDecidedToFight() { return m_decideToFight; }
		bool isInHostileZoneWithoutLevel();

		void ChooseToDoSomething();

		Player* GetPlayer() { return bot; }
		void SetPlayer(Player* plr) { bot = plr; }

		void Update(uint32 diff);

		// Use Go
		void UseGameObject(uint64 go);
		// Units
		Unit* SearchTargetAroundMe();
		// Movements
		void TakeAppropriateMount();
		void Stay();
		void GoToCacIfIsnt(Unit* target);
		void GoToRandomBGPoint(BattleGroundTypeId bgTypeId);
		void GoPoint(float x,float y,float z) { bot->GetMotionMaster()->MovePoint(0,x,y,z); }
		void GoPoint(BotCoord* bc, bool ignoreMount=false);
		bool isStaying() { return (bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE); }
		void GoToRandomBGStartingPoint(BattleGroundTypeId bgTypeId, uint32 diff);

		// BG Handlers
		void JoinBGQueueIfNotIn();
		void SheduleSendToBG(BattleGround* bg, BattleGroundTypeId btId, GroupQueueInfo* ginfo);
		void SendToBg();
	private:
		uint8 specIdx;
		bool m_decideToFight;
		Player* bot;
		BotMode m_mode;
		
		BotChoice m_choice;
		uint32 choice_Timer;

		uint16 chosen_point;

		// Timers
		uint32 mode_Timer;
		uint32 act_Timer;
		uint32 combat_Timer;
		uint32 react_Timer;

		BattleGround* sheduledBG;
		BattleGroundTypeId bgTypeId;
		GroupQueueInfo* m_ginfo;

		// Differed
		uint32 m_differedAction;
		BotChoice differedAct;
		BotCoord* registered_bc;
};

#endif