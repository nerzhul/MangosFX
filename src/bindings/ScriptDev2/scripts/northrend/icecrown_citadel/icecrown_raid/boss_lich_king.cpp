#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	//** phase 1,2,3
	SPELL_INFEST						=	70541,
	//** phase 1
	SPELL_SUMMON_SHAMBLING_HORROR		=	70372,
	SPELL_SUMMON_DRUDGE_GHOULS			=	70358,
	SPELL_NECROTIC_PLAGUE				=	70337,
	SPELL_PLAGUE_SIPHON					=	74074,
	// Heroic only
	SPELL_SUMMON_SHADOW_TRAP			=	73539,
	SPELL_PROC_SHADOW_TRAP				=	73529,
	// Shambling horror
	SPELL_SHOCKWAVE						=	72149,
	SPELL_ENRAGE						=	72143,
	
	//** phase 2,3
	SPELL_SOUL_REAPER					=	69409,
	SPELL_DEFILE						=	72754,
	
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

	SPELL_FROSTMOURNE_WRATH				=	72350,
};

struct MANGOS_DLL_DECL boss_iccraid_lichkingAI : public LibDevFSAI
{
    boss_iccraid_lichkingAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(10000);
		AddTextEvent(17365,"Rencontrez votre tragique fin !",10000,TEN_MINS);
    }

    void Reset()
    {
		ResetTimers();
    }

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_LICHKING, IN_PROGRESS);
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
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_iccraid_lichking(Creature* pCreature)
{
    return new boss_iccraid_lichkingAI(pCreature);
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
						Say(16653,"Voici venu l'ultime combat. Les échos des évènements d'aujourd'hui résonneront dans l'histoire. Quelle qu'en soit l'issue, "
							"le monde saura que nous avons combattu avec honneur, que nous avons combattu pour la liberté et la survie de nos peuples.");
						Ev_Timer = 13000;
						break;
					default:
						// Dont forget to break that after
						if(Creature* LichKing = GetInstanceCreature(TYPE_LICHKING))
							LichKing->CastSpell(LichKing,SPELL_FROSTMOURNE_WRATH,false);
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
    NewScript->Name = "icc_fordring_lk_event";
    NewScript->GetAI = &GetAI_icc_fordring_lk;
	NewScript->pGossipHello = &GossipHello_icc_fordring_lk;
    NewScript->pGossipSelect = &GossipSelect_icc_fordring_lk;
    NewScript->RegisterSelf();
}
