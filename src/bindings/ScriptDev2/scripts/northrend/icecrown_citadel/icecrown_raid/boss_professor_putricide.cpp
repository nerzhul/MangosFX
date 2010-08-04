#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_SLIME_PUDDLE            = 70346,
    SPELL_UNSTABLE_EXPERIMENT     = 71968,
    SPELL_TEAR_GAS                = 71617,
    SPELL_TEAR_GAS_1              = 71618,
    SPELL_CREATE_CONCOCTION       = 71621,
    SPELL_CHOKING_GAS             = 71278,
    SPELL_CHOKING_GAS_EXPLODE     = 71279,
    SPELL_MALLEABLE_GOO           = 72296,
    SPELL_GUZZLE_POTIONS          = 73122,
    SPELL_MUTATED_STRENGTH        = 71603,
    SPELL_MUTATED_PLAGUE          = 72672,
//
    NPC_GAS_CLOUD                 = 37562,
    SPELL_GASEOUS_BLOAT           = 70672,
    SPELL_EXPUNGED_GAS            = 70701,
    SPELL_SOUL_FEAST              = 71203,
//
    NPC_VOLATILE_OOZE             = 37697,
    SPELL_OOZE_ADHESIVE           = 70447,
    SPELL_OOZE_ERUPTION           = 70492,
//
    NPC_MUTATED_ABOMINATION       = 37672,
    SPELL_MUTATED_TRANSFORMATION  = 70311,
    SPELL_EAT_OOZE                = 72527,
    SPELL_REGURGITATED_OOZE       = 70539,
    SPELL_MUTATED_SLASH           = 70542,
    SPELL_MUTATED_AURA            = 70405,
};

enum Phases
{
	PHASE_INIT	= 1,
	PHASE_80	= 2,
	PHASE_35	= 3,
};

struct MANGOS_DLL_DECL boss_putricideAI : public LibDevFSAI
{
    boss_putricideAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	Phases phase;

    void Reset()
    {
		ResetTimers();
		phase = PHASE_INIT;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PUTRICIDE, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PUTRICIDE, DONE);

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
            pInstance->SetData(TYPE_PUTRICIDE, FAIL);
    }

	void StunAndGo()
	{
		// Todo : cast stun, timed text, special flags...
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(80) && phase == PHASE_INIT)
		{
			StunAndGo();
			return;
		}

		if(CheckPercentLife(35) && phase == PHASE_80)
		{
			StunAndGo();
			return;
		}

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_putricide(Creature* pCreature)
{
    return new boss_putricideAI(pCreature);
}

void AddSC_ICC_Putricide()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_putricide";
    NewScript->GetAI = &GetAI_boss_putricide;
    NewScript->RegisterSelf();
}
