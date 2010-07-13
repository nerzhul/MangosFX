#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	// Dreamwalker
	SPELL_NIGHTMARE_PORTAL		=	72482,
	SPELL_EMERALD_VIGOR			=	70873,
	SPELL_DREAMWALKER_RAGE		=	71189,

	// adds
	// Archmages
	SPELL_FROSTBOLT_VOLLEY		=	70759,
	SPELL_MANA_VOID				=	71179,
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
    }

    void Reset()
    {
		ResetTimers();
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_DREAMWALKER, IN_PROGRESS);
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
			pInstance->SetData(TYPE_DREAMWALKER, DONE);
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

void AddSC_ICC_DreamWalker()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_dreamwalker";
    NewScript->GetAI = &GetAI_boss_dreamwalker;
    NewScript->RegisterSelf();
}
