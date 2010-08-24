/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_CREATURE_H
#define SC_CREATURE_H

#include "CreatureAI.h"
#include "Creature.h"
#include "Map.h"
#include "Util.h"
#include "Vehicle.h"
#include "sc_instance.h"

//Spell targets used by SelectSpell
enum SelectTarget
{
    SELECT_TARGET_DONTCARE = 0,                             //All target types allowed

    SELECT_TARGET_SELF,                                     //Only Self casting

    SELECT_TARGET_SINGLE_ENEMY,                             //Only Single Enemy
    SELECT_TARGET_AOE_ENEMY,                                //Only AoE Enemy
    SELECT_TARGET_ANY_ENEMY,                                //AoE or Single Enemy

    SELECT_TARGET_SINGLE_FRIEND,                            //Only Single Friend
    SELECT_TARGET_AOE_FRIEND,                               //Only AoE Friend
    SELECT_TARGET_ANY_FRIEND,                               //AoE or Single Friend
};

//Spell Effects used by SelectSpell
enum SelectEffect
{
    SELECT_EFFECT_DONTCARE = 0,                             //All spell effects allowed
    SELECT_EFFECT_DAMAGE,                                   //Spell does damage
    SELECT_EFFECT_HEALING,                                  //Spell does healing
    SELECT_EFFECT_AURA,                                     //Spell applies an aura
};

//Selection method used by SelectTarget
enum SelectAggroTarget
{
    SELECT_TARGET_RANDOM = 0,                               //Just selects a random target
    SELECT_TARGET_TOPAGGRO,                                 //Selects targes from top aggro to bottom
    SELECT_TARGET_BOTTOMAGGRO,                              //Selects targets from bottom aggro to top
};

enum SCEquip
{
    EQUIP_NO_CHANGE = -1,
    EQUIP_UNEQUIP   = 0
};

struct MANGOS_DLL_DECL ScriptedAI : public CreatureAI
{
    explicit ScriptedAI(Creature* pCreature);
    ~ScriptedAI() {}

    //*************
    //CreatureAI Functions
    //*************

    //Called if IsVisible(Unit *who) is true at each *who move
    void MoveInLineOfSight(Unit*);

    //Called at each attack of me by any victim
    void AttackStart(Unit*);

    // Called for reaction at enter to combat if not in combat yet (enemy can be NULL)
    void EnterCombat(Unit*);

    //Called at stoping attack by any attacker
    void EnterEvadeMode();

    //Called at any heal cast/item used
    void HealBy(Unit* pHealer, uint32& uiAmountHealed) {}

    // Called at any Damage to any victim (before damage apply)
    void DamageDeal(Unit* pDoneTo, uint32& uiDamage) {}

    // Called at any Damage from any attacker (before damage apply)
    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) {}

    //Is unit visible for MoveInLineOfSight
    bool IsVisible(Unit *who) const;

    //Called at World update tick
    void UpdateAI(const uint32);

    //Called at creature death
    void JustDied(Unit*) {}

    //Called at creature killing another unit
    void KilledUnit(Unit*) {}

    // Called when the creature summon successfully other creature
    void JustSummoned(Creature*) {}

    // Called when a summoned creature is despawned
    void SummonedCreatureDespawn(Creature*) {}

    // Called when hit by a spell
    void SpellHit(Unit*, const SpellEntry*) {}

	// Call when hit with a spell
	void SpellHitTarget(Unit*, const SpellEntry*) {}

    // Called when creature is spawned or respawned (for reseting variables)
    void JustRespawned();

    //Called at waypoint reached or PointMovement end
    void MovementInform(uint32, uint32) {}

    //*************
    // Variables
    //*************

    //*************
    //Pure virtual functions
    //*************

    //Called at creature reset either by death or evade
    virtual void Reset() = 0;

    //Called at creature EnterCombat
    virtual void Aggro(Unit*);

    //*************
    //AI Helper Functions
    //*************

    //Start movement toward victim
    void DoStartMovement(Unit* pVictim, float fDistance = 0, float fAngle = 0);

    //Start no movement on victim
    void DoStartNoMovement(Unit* pVictim);

    //Do melee swing of current victim if in rnage and ready and not casting
    void DoMeleeAttackIfReady();

    //Stop attack of current victim
    void DoStopAttack();

    //Cast spell by Id
    void DoCast(Unit* pVictim, uint32 uiSpellId, bool bTriggered = false);
	void DoCastVictim(uint32 uiSpellId, bool bTriggered = false);
	Unit* DoCastRandom(uint32 uiSpellId, bool bTriggered = false, bool InFront = true);
	void DoCastMe(uint32 uiSpellId, bool bTriggered = false);
	void DoCastLowHP(uint32 uiSpellId, bool bTriggered = false);
	void DoCastNear(uint32 uiSpellId, bool bTriggered = false);
	void DoCastHasMana(uint32 uiSpellId, bool bTriggered = false, bool InFront = true);
	void DoCastPlayer(uint32 uiSpellId, bool bTriggered = false, bool InFront = true);

    //Cast spell by spell info
    void DoCastSpell(Unit* pwho, SpellEntry const* pSpellInfo, bool bTriggered = false);

    //Plays a sound to all nearby players
    void DoPlaySoundToSet(WorldObject* pSource, uint32 uiSoundId);

    //Drops all threat to 0%. Does not remove players from the threat list
    void DoResetThreat();

    //Teleports a player without dropping threat (only teleports to same map)
    void DoTeleportPlayer(Unit* pUnit, float fX, float fY, float fZ, float fO);

    //Returns friendly unit with the most amount of hp missing from max hp
    Unit* DoSelectLowestHpFriendly(float fRange, uint32 uiMinHPDiff = 1);

    //Returns a list of friendly CC'd units within range
    std::list<Creature*> DoFindFriendlyCC(float fRange);

    //Returns a list of all friendly units missing a specific buff within range
    std::list<Creature*> DoFindFriendlyMissingBuff(float fRange, uint32 uiSpellId);

    //Return a player with at least minimumRange from me
    Player* GetPlayerAtMinimumRange(float fMinimumRange);

    //Spawns a creature relative to me
    Creature* DoSpawnCreature(uint32 uiId, float fX, float fY, float fZ, float fAngle, uint32 uiType, uint32 uiDespawntime);

    //Selects a unit from the creature's current aggro list
    Unit* SelectUnit(SelectAggroTarget target, uint32 uiPosition);

    //Returns spells that meet the specified criteria from the creatures spell list
    SpellEntry const* SelectSpell(Unit* pTarget, int32 uiSchool, int32 uiMechanic, SelectTarget selectTargets, uint32 uiPowerCostMin, uint32 uiPowerCostMax, float fRangeMin, float fRangeMax, SelectEffect selectEffect);

    //Checks if you can cast the specified spell
    bool CanCast(Unit* pTarget, SpellEntry const* pSpell, bool bTriggered = false);

    void SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand = EQUIP_NO_CHANGE, int32 uiOffHand = EQUIP_NO_CHANGE, int32 uiRanged = EQUIP_NO_CHANGE);

    //Generally used to control if MoveChase() is to be used or not in AttackStart(). Some creatures does not chase victims
    void SetCombatMovement(bool bCombatMove);
    bool IsCombatMovement() { return m_bCombatMovement; }

	void DoCompleteQuest(uint32 entry, Player* player);

    bool EnterEvadeIfOutOfCombatArea(const uint32 diff);

	// LibDevFS Implementation
	void AggroAllPlayers(float maxdist);
	void GiveEmblemsToGroup(uint32 type, uint8 nb = 1, bool group5 = false);
	void GiveEmblems(uint32 type, Player* pPlayer, uint8 nb = 1, bool group5 = false);
	bool CanDoSomething() { return (!me || !me->SelectHostileTarget() || !me->getVictim()) ? false : true; }
	void Kill(Unit* toKill);
	bool CheckPercentLife(uint32 percent) { return (GetPercentLife() <= percent) ? true : false; }
	uint32 GetPercentLife() { return (me->GetHealth() * 100 / me->GetMaxHealth()); }
	void FreezeMob(bool freeze = true, Creature* tmpCr = NULL, bool OOC = false);
	void Speak(uint8 type, uint32 soundid, std::string text, Creature* spkCr = NULL);
	void DoSpeakEmote(Unit* who = NULL);
	void ModifyAuraStack(uint32 spell, int32 stacks = 1, Unit* target = NULL, Unit* caster = NULL);
	void SetFlying(bool fly, Creature* who = NULL);
	void Relocate(float x, float y, float z, bool fly = false, float Time = 0);
	void Relocate(Unit* unitpos) { Relocate(unitpos->GetPositionX(),unitpos->GetPositionY(), unitpos->GetPositionZ()); } ;

	void Yell(uint32 soundid, std::string text, Creature* spkCr = NULL) { Speak(CHAT_TYPE_YELL, soundid, text, spkCr); }
	void Say(uint32 soundid, std::string text, Creature* spkCr = NULL) { Speak(CHAT_TYPE_SAY, soundid, text, spkCr); }
	void BossEmote(uint32 soundid, std::string text, Creature* spkCr = NULL) { Speak(CHAT_TYPE_BOSS_EMOTE, soundid, text, spkCr); }
	Creature* GetInstanceCreature(uint32 data) { return ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(data) : 0)); }
	Unit* GetGuidUnit(uint64 guid) { return Unit::GetUnit(*me, guid); }
	Creature* GetGuidCreature(uint64 guid) { return ((Creature*)GetGuidUnit(guid)); }
	void InitInstance() 
	{
		if(me->GetInstanceData())
			pInstance = (ScriptedInstance*)me->GetInstanceData();
		else
			pInstance = NULL;
		m_difficulty = me->GetMap()->GetDifficulty();
	}

	protected:
		ScriptedInstance* pInstance;
		Difficulty m_difficulty;
    private:
        bool   m_bCombatMovement;
        uint32 m_uiEvadeCheckCooldown;
};

struct MANGOS_DLL_DECL Scripted_NoMovementAI : public ScriptedAI
{
    Scripted_NoMovementAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    //Called at each attack of me by any victim
    void AttackStart(Unit*);
};

enum SpellCastTarget
{
	TARGET_MAIN = 0,
	TARGET_RANDOM = 1,
	TARGET_ME = 2,
	TARGET_LOW_HP = 3,
	TARGET_NEAR = 4,
	TARGET_HAS_MANA = 5,
	PLAYER_RANDOM = 6,
	HEAL_MY_FRIEND = 7,
	NO_TANK = 8,
	TARGET_RANDOMXYZ = 9,
	TARGET_OTHER = 10,
};

struct EventSh
{
	uint32	SpellId;
	uint32	TextId;
	uint32	Timer;
	uint32	NormTimer;
	uint32	Diff;
	uint8	phase;
	uint16	Repeat;
	SpellCastTarget targ;
	bool MaxPriority;
	bool RequireFront;
};

struct TextSh
{
	uint32 SoundId;
	std::string Text;
	uint8 type;
	uint32 Timer;
	uint32 NormTimer;
	uint8 phase;
};

enum ZoneInvoc
{
	ON_ME		=	0,
	NEAR_7M		=	1,
	NEAR_15M	=	2,
	NEAR_30M	=	3,
	NEAR_45M	=	4,
	NEAR_60M	=	5,
	PREC_COORDS	=	6,
};

enum Comportement
{
	AGGRESSIVE_MAIN			=	0,
	AGGRESSIVE_RANDOM		=	1,
	VERY_AGGRESSIVE_MAIN	=	2,
	VERY_AGGRESSIVE_RANDOM	=	3,
	GO_TO_CREATOR			=	4,
	NOTHING					=	5,
};

struct EventSummon
{
	uint32 entry;
	uint32 Timer;
	uint32 NormTimer;
	uint32 diff;
	uint32 phase;
	uint32 Repeat;
	uint32 despawnTime;
	uint32 TextId;
	ZoneInvoc WhereS;
	Comportement Compo;
};

enum Emblem_Types
{
	HEROISME	= 45624, // relace pve
	VAILLANCE	= 45624,
	CONQUETE	= 45624,
	TRIOMPHE	= 47241,
	GIVRE		= 49426,	
};

enum MobConstantes
{
	MAX_ADDS	=	1500,
	THREE_MINS	=	180000,
	TEN_MINS	=	600000,
};

typedef std::vector<EventSh> SpellEvents;
typedef std::vector<EventSummon> SummonEvents;
typedef std::vector<TextSh> TextEvents;

class MANGOS_DLL_SPEC MobEventTasks
{
	public:
		explicit MobEventTasks(){};
		~MobEventTasks() {};

		void AddEvent(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			SpellCastTarget targ = TARGET_RANDOM, uint8 phase = 0, uint32 TextId = 0,
			bool MaxPriority = false, uint16 Repeat = 1, bool front = true);

		void AddNoTankEvent(uint32 SpellId, uint32 Timer) { AddEvent(SpellId,Timer,Timer,0,NO_TANK); }

		void SetObjects(ScriptedAI* tAI,Creature* me)
		{
			thisAI = tAI;
			thisCr = me;
			EventShVect.clear();
			EventSummonVect.clear();
		}

		 void AddSummonEvent(uint32 entry, uint32 Timer, uint32 NormTimer, uint32 phase = 0, uint32 Diff = 0,
			uint32 nb_spawn = 1, uint32 Despawn = TEN_MINS, ZoneInvoc WhereZone = ON_ME,
			Comportement Compo = AGGRESSIVE_RANDOM, uint32 TextId = 0); 

		void UpdateEvent(uint32 diff, uint32 phase = 0);

		Creature* CallCreature(uint32 entry, uint32 Despawn = TEN_MINS,
			ZoneInvoc WhereZone = ON_ME, Comportement Compo = AGGRESSIVE_RANDOM,
			float x = 0,float y = 0, float z = 0, bool force = false);

		void CleanMyAdds();

		void GetNewTargetForMyAdds(Unit* target);

	private:
		ScriptedAI* thisAI;
		Creature* thisCr;
		SpellEvents EventShVect;
		SummonEvents EventSummonVect;
		std::vector<uint64> MyAdds;

};

class MANGOS_DLL_SPEC LibDevFSAI : public ScriptedAI
{
	public:
		explicit LibDevFSAI(Creature* pCreature) : ScriptedAI(pCreature) 
		{
			ManualMoveEnable = false;
			CanMove = true;
			TimedDownEnable = false;
			CheckDistanceTimer = 1000;
		}
		~LibDevFSAI() {};

		Creature* CallCreature(uint32 entry, uint32 Despawn = TEN_MINS,
			ZoneInvoc WhereZone = ON_ME, Comportement Compo = AGGRESSIVE_RANDOM,
			float x = 0,float y = 0, float z = 0, bool force = false);
	protected:
		void AddEvent(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			SpellCastTarget targ = TARGET_RANDOM, uint8 phase = 0, uint32 TextId = 0,
			bool MaxPriority = false, uint16 Repeat = 1, bool front = true);

		void AddMaxPrioEvent(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			SpellCastTarget targ = TARGET_RANDOM, uint8 phase = 0, uint32 TextId = 0,
			uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId, Timer, NormTimer, Diff, targ, phase, TextId, true, Repeat, front); }
			
		void AddNoTankEvent(uint32 SpellId, uint32 Timer) { AddEvent(SpellId,Timer,Timer,0,NO_TANK); }

		void AddEventOnMe(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			uint8 phase = 0, uint32 TextId = 0,	bool MaxPriority = false, uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId,Timer,NormTimer,Diff,TARGET_ME,phase,TextId,MaxPriority,Repeat,front); }
			
		void AddEventMaxPrioOnMe(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			uint8 phase = 0, uint32 TextId = 0,	uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId,Timer,NormTimer,Diff,TARGET_ME,phase,TextId, true, Repeat,front); }	
			
		void AddEventOnTank(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			uint8 phase = 0, uint32 TextId = 0,	bool MaxPriority = false, uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId,Timer,NormTimer,Diff,TARGET_MAIN,phase,TextId,MaxPriority,Repeat,front); }
			
		void AddEventMaxPrioOnTank(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			uint8 phase = 0, uint32 TextId = 0,	uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId, Timer, NormTimer, Diff, TARGET_MAIN, phase, TextId, true, Repeat, front); }	
			
		void AddSummonEvent(uint32 entry, uint32 Timer, uint32 NormTimer, uint32 phase = 0, uint32 Diff = 0,
			uint32 nb_spawn = 1, uint32 Despawn = TEN_MINS, ZoneInvoc WhereZone = ON_ME,
			Comportement Compo = AGGRESSIVE_RANDOM, uint32 TextId = 0);

		void AddNear7mSummonEvent(uint32 entry, uint32 Timer, uint32 NormTimer, uint32 phase = 0, uint32 Diff = 0,
			uint32 nb_spawn = 1, uint32 Despawn = TEN_MINS,	Comportement Compo = AGGRESSIVE_RANDOM, uint32 TextId = 0)
		{	AddSummonEvent(entry, Timer, NormTimer, phase, Diff, nb_spawn, Despawn, NEAR_7M, Compo, TextId); }

		void AddNear15mSummonEvent(uint32 entry, uint32 Timer, uint32 NormTimer, uint32 phase = 0, uint32 Diff = 0,
			uint32 nb_spawn = 1, uint32 Despawn = TEN_MINS,	Comportement Compo = AGGRESSIVE_RANDOM, uint32 TextId = 0)
		{	AddSummonEvent(entry, Timer, NormTimer, phase, Diff, nb_spawn, Despawn, NEAR_15M, Compo, TextId); }

		void AddHealEvent(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			uint8 phase = 0, uint32 TextId = 0,	bool MaxPriority = false, uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId,Timer,NormTimer,Diff,HEAL_MY_FRIEND,phase,TextId,MaxPriority,Repeat,front); }

		void AddHealEventMaxPrio(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			uint8 phase = 0, uint32 TextId = 0,	uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId, Timer, NormTimer, Diff, HEAL_MY_FRIEND, phase, TextId, true, Repeat, front); }

		void AddPhase1Event(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			SpellCastTarget targ = TARGET_RANDOM, uint32 TextId = 0, bool MaxPriority = false,
			uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId, Timer, NormTimer, Diff, targ, 1, TextId, MaxPriority, Repeat, front); }

		void AddPhase2Event(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff = 0,
			SpellCastTarget targ = TARGET_RANDOM, uint32 TextId = 0, bool MaxPriority = false,
			uint16 Repeat = 1, bool front = true)
			{ AddEvent(SpellId, Timer, NormTimer, Diff, targ, 2, TextId, MaxPriority, Repeat, front); }

		void AddEnrageTimer(uint32 Timer) { AddEventMaxPrioOnMe(26662,Timer,60000); }

		void AddTextEvent(uint32 soundId, std::string text, uint32 Timer, uint32 NormTimer, uint8 type = CHAT_TYPE_YELL, uint8 phase = 0);

		void UpdateEvent(uint32 diff, uint32 phase = 0);
			
		Creature* CallAggressiveCreature(uint32 entry, uint32 Despawn = TEN_MINS,
			ZoneInvoc WhereZone = ON_ME, float x = 0,float y = 0, float z = 0, bool force = false)
			{ return CallCreature(entry, Despawn, WhereZone, AGGRESSIVE_RANDOM, x, y, z, force); }

		void CleanMyAdds();

		void GetNewTargetForMyAdds(Unit* target);

		void Jump(float x, float y, float z, float speedXY, float speedZ) { me->GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ); }

		void InitIA()
		{
			EventShVect.clear();
			EventSummonVect.clear();
			EventTextVect.clear();
			Reset();
		}

		void InitInstance()
		{
			ScriptedAI::InitInstance();
			InitIA();
		}
		
		void ResetTimers();
		
		void ClearTimers() 
		{
			EventShVect.clear();
			EventSummonVect.clear();
			SavedEventSh.clear();
			SavedEventSummon.clear();
			EventTextVect.clear();
			SavedEventTexts.clear();
		}

		void SetInstanceData(uint32 type, uint32 data) { if(pInstance) pInstance->SetData(type,data); }

		void CanBeTaunt(bool taunt=true) { me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, taunt); }

		void AddPercentLife(Unit* u,uint8 percent);
		void DealDamage(Unit* target, uint32 damage);
		void DealPercentDamage(Unit* target, float percent);
		void RemoveFromThreatList(Unit* u);

		Unit* GetRandomUnit(uint8 pos = 0) { return SelectUnit(SELECT_TARGET_RANDOM,pos); }

		void ActivateManualMoveSystem() { ManualMoveEnable = true; }
		void SetMovePhase(bool yes = true) { CanMove = yes; }

		void SetDespawnTimer(uint32 Timer) { me->ForcedDespawn(Timer); }

		void ActivateTimeDown(uint32 Time) { TimedDownEnable = true; AchTimedDownTimer = Time; }
		bool TimeDownSucceed() { return TimedDownEnable; }

		void MakeInvisibleStalker();
		void MakeHostileInvisibleStalker();
	
	private:
		
		SpellEvents EventShVect;
		SummonEvents EventSummonVect;
		SpellEvents SavedEventSh;
		SummonEvents SavedEventSummon;
		TextEvents EventTextVect;
		TextEvents SavedEventTexts;
		std::vector<uint64> MyAdds;
		// Manual Move
		bool ManualMoveEnable;
		uint32 CheckDistanceTimer;
		bool CanMove;
		// Down timer on HF
		bool TimedDownEnable;
		uint32 AchTimedDownTimer;
};
#endif
