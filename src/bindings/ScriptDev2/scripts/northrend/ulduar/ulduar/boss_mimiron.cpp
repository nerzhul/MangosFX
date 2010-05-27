#include "precompiled.h"
#include "ulduar.h"

enum spells
{
	// mkII
	SPELL_NAPALM_SHELL			=	63666,
	SPELL_NAPALM_SHELL_H		=	65026,
	SPELL_PLASMA_BLAST			=	62997,
	SPELL_PLASMA_BLAST_H		=	64529,
	SPELL_SHOCK_BLAST			=	63631,
	// HM
	SPELL_EMERGENCY				=	64582,
	SPELL_FLAME_SUPPRESSANT		=	64570,
	// VX001
	SPELL_RAPID_BURST			=	63387,
	SPELL_RAPID_BURST_H			=	64531, 
	SPELL_LASER_BARAGE_INIT		=	63414,
	SPELL_LASER_BARAGE_PACIFY	=	66490,
	SPELL_LASER_BARAGE			=	63274,
	SPELL_ROCKET_STRIKE			=	63041,
	SPELL_HEAT_WAVE				=	63677,
	SPELL_HEAT_WAVE_H			=	64533,
	// phase4
	SPELL_HAND_PULSE			=	64348,
	SPELL_HAND_PULSE_H			=	64536,
	// aerial unit
	SPELL_PLASMA_BALL			=	63689,
	SPELL_PLASMA_BALL_H			=	64535,
	// others
	SPELL_MINE_EXPLOSION		=	66351,
	SPELL_MINE_EXPLOSION_H		=	63009,
	SPELL_AB_MAGNETIC			=	64668,
	// p4 all
	SPELL_SELF_REPAIR			=	64383,
};

enum Npcs
{
	NPC_PROXIM_MINE		=	34362,
	NPC_ASSAULT_BOT		=	34057,
	NPC_BOMB_BOT		=	33836,
	NPC_JUNK_BOT		=	33855
};

const static float bot[3] = {34057,33836,33855};

enum Misc
{
	ITEM_MAGNETIC_CORE	=	46029 // disable air for aerial unit
};

const static float Bot_Coords[3][9] = {
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
};

struct MANGOS_DLL_DECL boss_leviMKIIAI : public LibDevFSAI
{
	// TODO: spawn mines around MKII
    boss_leviMKIIAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEvent(m_difficulty ? SPELL_NAPALM_SHELL_H : SPELL_NAPALM_SHELL,5000,8000,3000);
        AddEventOnTank(m_difficulty ? SPELL_PLASMA_BLAST_H : SPELL_PLASMA_BLAST,15000,30000);
        AddEventMaxPrioOnTank(SPELL_SHOCK_BLAST,25000,30000);
    }

	uint32 mine_Timer;
    void Reset()
    {
		ResetTimers();
		mine_Timer = 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		if(mine_Timer <= diff)
		{
			SpawnMines();
			mine_Timer = 15000;
		}
		else
			mine_Timer -= diff;
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
		// TODO: start next event
    }
    
    void SpawnTimes() 
    {
		// TODO: spawn mines
	}
    
    void ActivateP4()
    {
		ClearTimers();
		AddEventMaxPrioOnTank(SPELL_SHOCK_BLAST,25000,30000);
	}
};

CreatureAI* GetAI_boss_leviMKII(Creature* pCreature)
{
    return new boss_leviMKIIAI (pCreature);
}

struct MANGOS_DLL_DECL boss_leviMKII_mineAI : public LibDevFSAI
{
    boss_leviMKII_mineAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(m_difficulty ? SPELL_MINE_EXPLOSION_H : SPELL_MINE_EXPLOSION, 5000,DAY);
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
    }


    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_leviMKII_mine(Creature* pCreature)
{
    return new boss_leviMKII_mineAI (pCreature);
}

struct MANGOS_DLL_DECL boss_VX001AI : public LibDevFSAI
{
    boss_VX001AI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(m_difficulty ? SPELL_HEAT_WAVE_H : SPELL_HEAT_WAVE,5000,10000,5000);
        // TODO : verify this spell, add rocketbarrage event, add ground target for rocketstrike
        AddEvent(m_difficulty ? SPELL_RAPID_BURST_H : SPELL_RAPID_BURST,3000,5000,3000);
    }

	uint32 rocketStrike_Timer;
    void Reset()
    {
		ResetTimers();
		rocketStrike_Timer = 12000;
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(rocketStrike_Timer <= diff)
		{
			if(Unit* rockTarget = DoCastRandom(SPELL_ROCKET_STRIKE))
			{
				// TODO: marker for rocket
			}
			rocketStrike_Timer = urand(12000,15000);
		}
		else
			rocketStrike_Timer -= diff;
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
		// TODO : start next event
    }
    
    void ActivateP4()
    {
		ClearTimers();
		AddEvent(m_difficulty ? SPELL_HAND_PULSE_H : SPELL_HAND_PULSE,5000,7000,2000);
		if(Unit* veh = GetInstanceCreature(DATA_LEVIMKII))
			EnterVehicle(veh);
	}
};

CreatureAI* GetAI_boss_VX001(Creature* pCreature)
{
    return new boss_VX001AI (pCreature);
}

struct MANGOS_DLL_DECL boss_aerialCommandUnitAI : public LibDevFSAI
{
    boss_aerialCommandUnitAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        // TODO: spawn bots
        AddEvent(m_difficulty ? SPELL_PLASMA_BALL_H : SPELL_PLASMA_BALL,5000,10000,2000);
    }

	uint32 bots_Timer;
	bool phase4;
    void Reset()
    {
		ResetTimers();
		phase4 = false;
		bots_Timer = 6000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
           
        if(!phase4)
        {
			if(bots_Timer <= diff)
			{
				SpawnBots();
				bots_Timer = 15000;
			}
			else
				bots_Timer -= diff;
		}

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
		// TODO : launch event 4
    }
    
    void SpawnBots()
    {
		uint8 maxbots = urand(1,3);
		for(uint8 i=0;i<maxbots;i++)
		{
			uint8 botType = urand(0,2);
			uint8 botSpawn = urand(0,8);
			CallCreature(botType,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,bot[botSpawn][0],
				bot[botSpawn][1],bot[botSpawn][2]);
		}
	}
    
    void ActivateP4()
    {
		phase4 = false;
		if(Unit* veh = GetInstanceCreature(DATA_VX001))
			EnterVehicle(veh);
	}
};

CreatureAI* GetAI_boss_aerialCommandUnit(Creature* pCreature)
{
    return new boss_aerialCommandUnitAI (pCreature);
}

struct MANGOS_DLL_DECL boss_MimironassaultBotAI : public LibDevFSAI
{
    boss_MimironassaultBotAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEvent(SPELL_AB_MAGNETIC,2000,15000);
    }

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
		if(killer->GetTypeId() == TYPEID_PLAYER)
			((Player*)killer)->AddItem(ITEM_MAGNETIC_CORE);
    }
};

CreatureAI* GetAI_boss_MimironassaultBot(Creature* pCreature)
{
    return new boss_MimironassaultBotAI (pCreature);
}

struct MANGOS_DLL_DECL boss_mimironAI : public LibDevFSAI
{
    boss_mimironAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MIMIRON, DONE);
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE);
    }

    void Aggro(Unit* pWho)
    {

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		
        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_mimiron(Creature* pCreature)
{
    return new boss_mimironAI(pCreature);
}

void AddSC_boss_mimiron()
{
    Script *newscript;
    
    newscript = new Script;
    newscript->Name = "boss_mimiron";
    newscript->GetAI = &GetAI_boss_mimiron;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_leviMKII";
    newscript->GetAI = &GetAI_boss_leviMKII;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_leviMKII_mine";
    newscript->GetAI = &GetAI_boss_leviMKII_mine;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_aerialCommandUnit";
    newscript->GetAI = &GetAI_boss_aerialCommandUnit;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_VX001";
    newscript->GetAI = &GetAI_boss_VX001;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "mimiron_assault_bot";
    newscript->GetAI = &GetAI_boss_MimironassaultBot;
    newscript->RegisterSelf();
}
