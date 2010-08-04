#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_OOZE_FLOOD				= 69783, // use this
    SPELL_SLIME_SPRAY				= 69508, // ok
    SPELL_MUTATED_INFECTION_AURA	= 69674,
    SPELL_MUTATED_INFECTION			= 70003, // 12sec intervalle

	// Little
    SPELL_STICKY_OOZE				= 69774, // ok
    SPELL_STICKY_AURA				= 69776, // ok
    SPELL_MERGE_OOZE				= 69889,
    SPELL_RADIATING_OOZE			= 69750, // ok

	// Big
    SPELL_UNSTABLE_OOZE				= 69644,
    SPELL_UNSTABLE_OOZE_AURA		= 69558,
    SPELL_OOZE_EXPLODE				= 69839, // ok
	SPELL_BIG_RADIATING_OOZE		= 69760, // ok

    NPC_BIG_OOZE					= 36899,
    NPC_SMALL_OOZE					= 36897,
    NPC_STICKY_OOZE					= 37006,
    NPC_OOZE_SPRAY_STALKER			= 37986,
    NPC_OOZE_STALKER				= 37013,
    NPC_OOZE_EXPLODE_STALKER		= 38107,

    MAX_INFECTION_TARGETS			= 5,
};

static float SpawnLoc[8][3]=
{
	{4466.14f,	3095.25f,	360.39f}, // 1st
	{4486.6f,	3115.12f,	360.39f},
	{4488.458f,	3158.43f,	360.39f}, // 2nd
	{4467.59f,	3178.11f,	360.39f},
	{4425.33f,	3178.61f,	360.39f}, // 3rd
	{4405.00f,	3158.76f,	360.39f},
	{4404.26f,	3115.89f,	360.39f}, // 4the
	{4423.86f,	3096.41f,	360.39f}
};

struct MANGOS_DLL_DECL boss_rotfaceAI : public LibDevFSAI
{
    boss_rotfaceAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_SLIME_SPRAY,10000,15000,2000); // TODO : review that
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

struct MANGOS_DLL_DECL big_limonAI : public LibDevFSAI
{
    big_limonAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_STICKY_OOZE,8000,12000,1500);
    }

    void Reset()
    {
		ResetTimers();
		DoCastMe(SPELL_BIG_RADIATING_OOZE);
    }

	void SpellHit(Unit* pWho, const SpellEntry* sp)
	{
		if(!sp)
			return;

		if(sp->Id == SPELL_STICKY_AURA)
			if(Aura* aur = me->GetAura(SPELL_STICKY_AURA))
			{
				if(aur->GetStackAmount() == 5)
				{
					DoCastMe(SPELL_OOZE_EXPLODE);
					me->ForcedDespawn(5000);
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

CreatureAI* GetAI_big_limon(Creature* pCreature)
{
    return new big_limonAI(pCreature);
}

struct MANGOS_DLL_DECL small_limonAI : public LibDevFSAI
{
    small_limonAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_STICKY_OOZE,8000,12000,1500);
    }

    void Reset()
    {
		ResetTimers();
		DoCastMe(SPELL_RADIATING_OOZE);
		CanBeTaunt(false);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_small_limon(Creature* pCreature)
{
    return new small_limonAI(pCreature);
}

struct MANGOS_DLL_DECL sticky_oozeAI : public LibDevFSAI
{
    sticky_oozeAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		MakeHostileInvisibleStalker();
		DoCastMe(SPELL_STICKY_AURA);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_sticky_ooze(Creature* pCreature)
{
    return new sticky_oozeAI(pCreature);
}

void AddSC_ICC_Rotface()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_rotface";
    NewScript->GetAI = &GetAI_boss_rotface;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_small_ooze";
    NewScript->GetAI = &GetAI_small_limon;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_big_ooze";
    NewScript->GetAI = &GetAI_big_limon;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "rotface_sticky_ooze";
    NewScript->GetAI = &GetAI_sticky_ooze;
    NewScript->RegisterSelf();
}
