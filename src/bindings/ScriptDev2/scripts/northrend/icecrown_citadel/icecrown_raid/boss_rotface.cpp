#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_OOZE_FLOOD				= 69789,
    SPELL_OOZE_FLOOD_0				= 69788,
    SPELL_OOZE_FLOOD_1				= 69783,
    SPELL_SLIME_SPRAY				= 69508,
    SPELL_MUTATED_INFECTION_AURA	= 69674,
    SPELL_MUTATED_INFECTION			= 70003,
    SPELL_BERSERK					= 47008,

    SPELL_STICKY_OOZE				= 69774,
    SPELL_STICKY_AURA				= 69776,
    SPELL_MERGE_OOZE				= 69889,
    SPELL_RADIATING_OOZE			= 69750,
    SPELL_RADIATING_OOZE_1			= 69760,
    SPELL_UNSTABLE_OOZE				= 69644,
    SPELL_UNSTABLE_OOZE_AURA		= 69558,
    SPELL_OOZE_EXPLODE				= 69839,
    SPELL_OOZE_EXPLODE_AURA			= 69840,

    NPC_BIG_OOZE					= 36899,
    NPC_SMALL_OOZE					= 36897,
    NPC_STICKY_OOZE					= 37006,
    NPC_OOZE_SPRAY_STALKER			= 37986,
    NPC_OOZE_STALKER				= 37013,
    NPC_OOZE_EXPLODE_STALKER		= 38107,

    MAX_INFECTION_TARGETS			= 5,
};

static float SpawnLoc[4][3]=
{
    {4471.821289f, 3162.986084f, 360.38501f}, // 0
    {4471.821289f, 3110.452148f, 360.38501f}, // 1
    {4418.825684f, 3110.452148f, 360.38501f}, // 2
    {4418.825684f, 3162.986084f, 360.38501f}, // 3
};

struct MANGOS_DLL_DECL boss_rotfaceAI : public LibDevFSAI
{
    boss_rotfaceAI(Creature* pCreature) : LibDevFSAI(pCreature)
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
            pInstance->SetData(TYPE_ROTFACE, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_ROTFACE, DONE);

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
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_ROTFACE, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rotface(Creature* pCreature)
{
    return new boss_rotfaceAI(pCreature);
}

void AddSC_ICC_Rotface()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_rotface";
    NewScript->GetAI = &GetAI_boss_rotface;
    NewScript->RegisterSelf();
}
