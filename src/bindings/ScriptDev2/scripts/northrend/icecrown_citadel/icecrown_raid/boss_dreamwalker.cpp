#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	// Dreamwalker
	SPELL_NIGHTMARE_PORTAL		=	72482,
	SPELL_EMERALD_VIGOR			=	70873,
	SPELL_DREAMWALKER_RAGE		=	71189, //ok

	// adds
	// Archmages
	SPELL_FROSTBOLT_VOLLEY		=	70759,
	SPELL_MANA_VOID				=	71085,
	NPC_MANA_VOID				=	38068,
	SPELL_COLUMN_OF_FROST		=	70702,
	// Blazing Skeletons
	SPELL_LAY_WASTE				=	69325,
	SPELL_FIREBALL				=	70754,
	// Suppressors
	SPELL_SUPPRESSION			=	70588,
	// Blistering Zombies
	SPELL_CORROSION				=	70751,
	SPELL_ACID_BURST			=	70744,
	// Gluttonous Abominations
	SPELL_GUT_SPRAY				=	71283,
	// Rot
	NPC_ROT_WORM				=	37907,
	SPELL_FLESH_ROT				=	72963,
	
};

const static float SpawnLoc[6][3]=
{
    {4203.470215f, 2484.500000f, 364.872009f},  // 0 Valithria
    {4240.688477f, 2405.794678f, 364.868591f},  // 1 Valithria Room 1
    {4165.112305f, 2405.872559f, 364.872925f},  // 2 Valithria Room 2
    {4166.216797f, 2564.197266f, 364.873047f},  // 3 Valithria Room 3
    {4239.579102f, 2566.753418f, 364.868439f},  // 4 Valithria Room 4
};

struct MANGOS_DLL_DECL boss_dreamwalkerAI : public LibDevFSAI
{
    boss_dreamwalkerAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		SetCombatMovement(false);
    }

    void Reset()
    {
		ResetTimers();
    }

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_DREAMWALKER, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

	void HealBy(Unit* pHealer, uint32 &heal)
	{
		if(CheckPercentLife(100) && pInstance && pInstance->GetData(TYPE_DREAMWALKER) == IN_PROGRESS)
		{
			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					GiveEmblemsToGroup(TRIOMPHE,2);
					GiveEmblemsToGroup(GIVRE,1);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
					GiveEmblemsToGroup(GIVRE,2);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
					GiveEmblemsToGroup(GIVRE,2);
					GiveEmblemsToGroup(TRIOMPHE,1);
					break;
				case RAID_DIFFICULTY_25MAN_HEROIC:
					GiveEmblemsToGroup(GIVRE,3);
					break;
			}
			DoCastMe(SPELL_DREAMWALKER_RAGE);
			SetInstanceData(TYPE_DREAMWALKER, DONE);
		}
            
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_DREAMWALKER, FAIL);
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_DREAMWALKER, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dreamwalker(Creature* pCreature)
{
    return new boss_dreamwalkerAI(pCreature);
}

struct MANGOS_DLL_DECL dw_archmageAI : public LibDevFSAI
{
    dw_archmageAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_FROSTBOLT_VOLLEY,10000,20000,5000);
		AddEvent(SPELL_COLUMN_OF_FROST,5000,12000,2000);
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
};

CreatureAI* GetAI_boss_dw_archmage(Creature* pCreature)
{
    return new dw_archmageAI(pCreature);
}

struct MANGOS_DLL_DECL dw_manavoidAI : public LibDevFSAI
{
    dw_manavoidAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		SetCombatMovement(false);
    }

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(SPELL_MANA_VOID);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_manavoid(Creature* pCreature)
{
    return new dw_manavoidAI(pCreature);
}

struct MANGOS_DLL_DECL dw_blazingskeletonsAI : public LibDevFSAI
{
    dw_blazingskeletonsAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventMaxPrioOnMe(SPELL_LAY_WASTE,5000,17000,5000);
		AddEventOnTank(SPELL_FIREBALL,1000,2000,3000);
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
};

CreatureAI* GetAI_boss_dw_blazingskeletons(Creature* pCreature)
{
    return new dw_blazingskeletonsAI(pCreature);
}

struct MANGOS_DLL_DECL dw_suppressorAI : public LibDevFSAI
{
    dw_suppressorAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_SUPPRESSION,2000,20000);
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
};

CreatureAI* GetAI_boss_dw_suppressor(Creature* pCreature)
{
    return new dw_suppressorAI(pCreature);
}

struct MANGOS_DLL_DECL dw_zombieAI : public LibDevFSAI
{
    dw_zombieAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_CORROSION,3000,5000);
    }

	bool die;
    void Reset()
    {
		ResetTimers();
		die = false;
    }

	void DamageTaken(Unit* pDoneby, uint32 &dmg)
	{
		if(dmg >= me->GetHealth())
		{
			dmg = 0;
			if(!die)
			{
				DoCastMe(SPELL_ACID_BURST);
				die = true;
				me->ForcedDespawn(3000);
			}
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_zombie(Creature* pCreature)
{
    return new dw_zombieAI(pCreature);
}

struct MANGOS_DLL_DECL dw_abominationAI : public LibDevFSAI
{
    dw_abominationAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_GUT_SPRAY,3000,10000,6000);
    }

	bool die;
    void Reset()
    {
		ResetTimers();
		die = false;
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth())
		{
			dmg = 0;
			if(!die)
			{
				CallCreature(NPC_ROT_WORM,THREE_MINS);
				die = true;
			}
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_abomination(Creature* pCreature)
{
    return new dw_abominationAI(pCreature);
}

struct MANGOS_DLL_DECL dw_fleshrotAI : public LibDevFSAI
{
    dw_fleshrotAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(72962);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_fleshrot(Creature* pCreature)
{
    return new dw_fleshrotAI(pCreature);
}

void AddSC_ICC_DreamWalker()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_dreamwalker";
    NewScript->GetAI = &GetAI_boss_dreamwalker;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_archmage";
    NewScript->GetAI = &GetAI_boss_dw_archmage;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_manavoid";
    NewScript->GetAI = &GetAI_boss_dw_manavoid;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_blazingskeleton";
    NewScript->GetAI = &GetAI_boss_dw_blazingskeletons;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_suppressor";
    NewScript->GetAI = &GetAI_boss_dw_suppressor;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_zombie";
    NewScript->GetAI = &GetAI_boss_dw_zombie;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_abomination";
    NewScript->GetAI = &GetAI_boss_dw_abomination;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_fleshrot";
    NewScript->GetAI = &GetAI_boss_dw_fleshrot;
    NewScript->RegisterSelf();
}
