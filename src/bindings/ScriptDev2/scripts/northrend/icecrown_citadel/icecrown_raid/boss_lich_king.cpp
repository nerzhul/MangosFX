#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	//** phase 1,2,3
	SPELL_INFEST						=	70541, // ok
	//** phase 1
	SPELL_SUMMON_SHAMBLING_HORROR		=	70372, // ok
	SPELL_SUMMON_DRUDGE_GHOULS			=	70358, // ok
	SPELL_NECROTIC_PLAGUE				=	70337, // ok
	SPELL_PLAGUE_SIPHON					=	74074, // ok

	// Heroic only
	SPELL_SUMMON_SHADOW_TRAP			=	73539,
	SPELL_PROC_SHADOW_TRAP				=	73529,

	// Shambling horror
	SPELL_SHOCKWAVE						=	72149, // ok
	SPELL_ENRAGE_SH						=	72143, // ok
	
	//** phase 2,3
	SPELL_SOUL_REAPER					=	69409,
	SPELL_DEFILE						=	72762, // ok
	SPELL_DEFILE_DOT					=	72743, // ok
	SPELL_DEFILE_SIZE					=	74164, // ok
	NPC_DEFILE							=	38757,
	
	//** phase 2
	SPELL_SUMMON_VALKYR					=	69037,
	NPC_VALKYR							=	36609,
	// Valkyr
	//-> Grab: Main target teleported to her
	SPELL_WINGS_OF_THE_DARMED			=	74352,
	// Heroic only for valkyr
	SPELL_LIFE_SIPHON					=	73783,
	
	//** phase 3
	SPELL_HARVEST_SOUL					=	68980,
	SPELL_VILE_SPIRITS					=	70498,
	// Vile Spirits
	SPELL_SPIRIT_BURST					=	70503,
	
	//** Frostmourne phase
	NPC_TERENAS_MENETHIL				=	36823,
	NPC_SPIRIT_WARDEN					=	36824,
	SPELL_HARVESTED_SOUL				=	74322, // for players

	// Teneras Menethil
	SPELL_LIGHTS_FAVOR					=	69382, // only in normal mode
	SPELL_RESTORE_SOUL					=	72595,
	// Spirit Warden
	SPELL_SOUL_RIP						=	69397,
	SPELL_DARK_HUNGER					=	69383,
	// Wicked spirits : to find
	
	//** Transitional phases
	SPELL_REMORSELESS_WINTER			=	68981,
	SPELL_PAIN_AND_SUFFERING			=	72133,
	SPELL_SUMMON_ICE_SPHERE				=	69103,
	SPELL_RAGING_SPIRIT					=	69200,
	SPELL_QUAKE							=	72262,
	// Ice sphere
	SPELL_ICE_PULSE						=	69099,
	SPELL_ICE_BURST						=	69108,
	// Raging Spirit
	SPELL_SOUl_SHRIEK					=	69242,

	// For Events

	// Wipe Event
	SPELL_FROSTMOURNE_WRATH				=	72350,

	// Intro event
	SPELL_FROZEN_JAIL					=	71321,
};

const static float FrostmourneCoords[3] = {481.0f,-2522.0f,1000.0f};

enum Phase 
{
	PHASE_100	= 1,
	PHASE_TR_1	= 2,
	PHASE_70	= 3,
	PHASE_TR_2	= 4,
	PHASE_40	= 5,
};

struct MANGOS_DLL_DECL boss_iccraid_lichkingAI : public LibDevFSAI
{
    boss_iccraid_lichkingAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(10000);
		AddTextEvent(17365,"Rencontrez votre tragique fin !",10000,TEN_MINS);
		AddEvent(SPELL_SUMMON_SHAMBLING_HORROR,15000,60000,0,TARGET_RANDOM,PHASE_100);
		AddEvent(SPELL_SUMMON_DRUDGE_GHOULS,25000,20000,0,TARGET_RANDOM,PHASE_100);
		AddEvent(SPELL_DEFILE,30000,30000,0,TARGET_RANDOM,PHASE_70);
		AddEvent(SPELL_DEFILE,30000,30000,0,TARGET_RANDOM,PHASE_40);
    }

	Phase phase;
	uint32 necrotic_Timer;
	uint64 necroticTarget;

    void Reset()
    {
		SetCombatMovement(false);
		ActivateManualMoveSystem();
		SetMovePhase();
		ResetTimers();
		necroticTarget = 0;
		necrotic_Timer = 6000;
		phase = PHASE_100;
    }

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_LICHKING, IN_PROGRESS);
		ModifyAuraStack(SPELL_INFEST);
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Yell(17363,"L'espoir faiblit.");
		else
			Yell(17364,"La fin est venue !");
	}

    void JustDied(Unit* pKiller)
    {
        SetInstanceData(TYPE_LICHKING, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,4);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,5);
				break;
		}
    }

    void JustReachedHome()
    {
        SetInstanceData(TYPE_LICHKING, FAIL);
		me->RemoveAurasDueToSpell(SPELL_INFEST);
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    }

	void SwitchPhase()
	{
		switch(phase)
		{
			case PHASE_100:
				phase = PHASE_TR_1;
				break;
			case PHASE_TR_1:
				phase = PHASE_70;
				break;
			case PHASE_70:
				phase = PHASE_TR_2;
				break;
			case PHASE_TR_2:
				phase = PHASE_40;
				break;
			default:
				break;
		}
	}

	Player* getPlayerTarget()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			uint32 _rand = urand(0,lPlayers.getSize());
			uint16 i=0;
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			{
				if(i == _rand)
				{
					if(Player* pPlayer = itr->getSource())
						return pPlayer;
				}
				i++;
			}
		}
		return NULL;
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase == PHASE_100)
		{
			if(necrotic_Timer <= diff)
			{
				if(Unit* u = pInstance->GetUnitInMap(necroticTarget))
				{
					if(!u->isAlive() || !u->HasAura(SPELL_NECROTIC_PLAGUE))
					{
						if(Player* plr = pInstance->GetClosestPlayer(u))
						{
							ModifyAuraStack(SPELL_NECROTIC_PLAGUE,1,plr);
							necroticTarget = plr->GetGUID();
							ModifyAuraStack(SPELL_PLAGUE_SIPHON);
						}
					}
				}
				else
				{
					if(Player* plr = getPlayerTarget())
					{
						ModifyAuraStack(SPELL_NECROTIC_PLAGUE,1,plr);
						necroticTarget = plr->GetGUID();
					}
				}
				necrotic_Timer = 500;
			}
			else
				necrotic_Timer -= diff;
		}

		UpdateEvent(diff);
		UpdateEvent(diff,phase);
		if(phase == PHASE_100 || phase == PHASE_70 || phase == PHASE_40)
			DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_iccraid_lichking(Creature* pCreature)
{
    return new boss_iccraid_lichkingAI(pCreature);
}

struct MANGOS_DLL_DECL lk_shambling_horrorAI : public LibDevFSAI
{
    lk_shambling_horrorAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_SHOCKWAVE,5000,8000,5000);
		AddEventOnMe(SPELL_ENRAGE_SH,16000,30000);
    }

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lk_shambling_horror(Creature* pCreature)
{
    return new lk_shambling_horrorAI(pCreature);
}

struct MANGOS_DLL_DECL lk_defileAI : public LibDevFSAI
{
    lk_defileAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
		ModifyAuraStack(SPELL_DEFILE_DOT);
		me->ForcedDespawn(31000);
    }

	uint8 stack;
    void Reset()
    {
		ResetTimers();
		AggroAllPlayers();
		stack = 1;
    }

	void DamageDeal(Unit* pWho, uint32 &dmg)
	{
		if(pWho->GetDistance2d(me) < 12+stack*5/100*12)
		{
			ModifyAuraStack(SPELL_DEFILE_SIZE);
			stack++;
		}
		else
			dmg = 0;
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_lk_defile(Creature* pCreature)
{
    return new lk_defileAI(pCreature);
}

struct MANGOS_DLL_DECL icc_fordring_lkAI : public LibDevFSAI
{
    icc_fordring_lkAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		IntroEvent = false;
    }

	bool IntroEvent;
	uint16 EvPhase;
	uint32 Ev_Timer;

    void Reset()
    {
		ResetTimers();
		EvPhase = 0;
		Ev_Timer = 1000;
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		dmg = 0;
	}

	void KilledUnit(Unit* who)
	{
	}

	void StartIntroEvent()
	{
		/*if(!(pInstance->GetData(TYPE_SINDRAGOSA) == DONE))
			return;*/

		IntroEvent = true;
		EvPhase = 0;
		Ev_Timer = 500;
	}

	void StopIntroEvent()
	{
		IntroEvent = false;
	}

    void UpdateAI(const uint32 diff)
    {
		if(IntroEvent)
		{
			if(Ev_Timer <= diff)
			{
				switch(EvPhase)
				{
					case 0:
						me->GetMotionMaster()->MovePoint(0,488.602f,-2124.82f,1040.87f);
						// Emote stand attack for tirion
						if(Creature* LichKing = GetInstanceCreature(TYPE_LICHKING))
						{
							LichKing->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
							LichKing->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
						}
						Ev_Timer = 3000;
						break;
					case 1:
						if(Creature* LichKing = GetInstanceCreature(TYPE_LICHKING))
						{
							Yell(17349,"Voici donc qu'arrive la fameuse justice de la Lumière. Dois-je déposer Deuillegivre et me jeter à tes pieds en implorant pitié, Fordring ?",LichKing);
							//LichKing->GetMotionMaster()->MovePoint(0,458.799f,-2124.087f,1040.88f);
						}
						Ev_Timer = 3000;
						break;
					case 2:
						me->HandleEmoteCommand(48);
						Ev_Timer = 10000;
						break;
					case 3:
						Say(17390,"Nous t'accorderons une mort rapide, Arthas. Les milliers d'âme que tu as torturées et anéanties n'y ont pas eu droit.");
						Ev_Timer = 7000;
						break;
					case 4:
						if(Creature* LichKing = GetInstanceCreature(TYPE_LICHKING))
							Yell(17350,"Et vous allez les rejoindre. Quand j'en aurais fini avec vous, vous crierez grâce, et je vous la refuserai. Vos cris d'agonie seront un hymne, à ma puissance, ILLIMITEE !",LichKing);
						Ev_Timer = 21000;
						break;
					case 5:
						Yell(17391,"Tout est dit. Champions, à l'ATTAQUE !");
						Ev_Timer = 4000;
						break;
					case 6:
						if(Creature* LichKing = GetInstanceCreature(TYPE_LICHKING))
						{
							Yell(17351,"Je vais te laisser en vie. Que tu sois le témoin de la fin, Fordring. Je ne voudrais pas priver le plus grand héros de la Lumière du spectacle de ce misérable monde "
							"refaçonné à mon image",LichKing);
							LichKing->HandleEmoteCommand(25);
						}
						me->GetMotionMaster()->MovePoint(0,458.799f,-2124.087f,1040.88f);
						Ev_Timer = 1000;
						break;
					case 7:
						DoCastMe(SPELL_FROZEN_JAIL,true);
						FreezeMob();
						Ev_Timer = 25000;
						break;
					default:
						// Dont forget to break that after
						if(Creature* LichKing = GetInstanceCreature(TYPE_LICHKING))
						{
							//LichKing->GetMotionMaster()->MovePoint(0,458.799f,-2124.087f,1040.88f);
							LichKing->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
							LichKing->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
							((boss_iccraid_lichkingAI*)LichKing->AI())->AggroAllPlayers(250.0f);
						}
						Ev_Timer = 10000;
						break;
				}
				EvPhase++;
			}
			else
				Ev_Timer -= diff;
		}
    }
};

bool GossipHello_icc_fordring_lk(Player* pPlayer, Creature* pCreature)
{
	if(((icc_fordring_lkAI*)pCreature->AI())->IntroEvent && !pPlayer->isGameMaster())
		return false;

	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Je suis pret a affronter Arthas.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

	if(pPlayer->isGameMaster())
	{
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Debug_LK", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stop Intro", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
	}
	pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_icc_fordring_lk(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	pPlayer->CLOSE_GOSSIP_MENU();
    switch(uiAction)
    {
		case GOSSIP_ACTION_INFO_DEF+1:
			((icc_fordring_lkAI*)pCreature->AI())->StartIntroEvent();
			break;
		case GOSSIP_ACTION_INFO_DEF+2:
			// ALL TO DEBUG LK
			break;
		case GOSSIP_ACTION_INFO_DEF+3:
			((icc_fordring_lkAI*)pCreature->AI())->StopIntroEvent();
			break;
    }

    return true;
}

CreatureAI* GetAI_icc_fordring_lk(Creature* pCreature)
{
    return new icc_fordring_lkAI(pCreature);
}

void AddSC_ICC_LichKing()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_iccraid_lichking";
    NewScript->GetAI = &GetAI_boss_iccraid_lichking;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_iccraid_shambling_horror";
    NewScript->GetAI = &GetAI_boss_lk_shambling_horror;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_iccraid_defile";
    NewScript->GetAI = &GetAI_boss_lk_defile;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_fordring_lk_event";
    NewScript->GetAI = &GetAI_icc_fordring_lk;
	NewScript->pGossipHello = &GossipHello_icc_fordring_lk;
    NewScript->pGossipSelect = &GossipSelect_icc_fordring_lk;
    NewScript->RegisterSelf();
}
