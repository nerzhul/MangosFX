#include "precompiled.h"
#include "vault_of_archavon.h"

enum Npc
{
	NPC_TORAVON_ORB		= 38461,
};

enum Spells
{
	SPELL_FROZEN_MASS			= 71993,
	SPELL_FROZEN_ORB			= 72091,
	SPELL_WHITE_SNOW			= 72034,
	SPELL_FROZEN_FLOOR			= 72090,
	SPELL_STONE					= 63080,

	// Add
	SPELL_AURA_FROZEN_ORB		= 72081,
	SPELL_EXPLODE_FROZEN_ORB	= 71286,
};

struct MANGOS_DLL_DECL boss_toravonAI : public LibDevFSAI
{
    boss_toravonAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
		AddEvent(SPELL_FROZEN_ORB,12000,30000);
		AddEventMaxPrioOnMe(SPELL_WHITE_SNOW,1000,50000);
		AddEvent(SPELL_FROZEN_FLOOR,20000,30000);
    }


	uint32 stone_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		SetInstanceData(DATA_TORAVON, NOT_STARTED);
		stone_Timer = 24000;
    }

	void SpellHitTarget(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == 72091)
		{
			if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
			{
				CallCreature(38461,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,pWho->GetPositionX(),pWho->GetPositionY(),pWho->GetPositionZ());
				CallCreature(38461,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,pWho->GetPositionX(),pWho->GetPositionY(),pWho->GetPositionZ());
			}
		}
	}

    void Aggro(Unit *who)
    {
		SetInstanceData(DATA_TORAVON, IN_PROGRESS);
		ModifyAuraStack(SPELL_FROZEN_MASS);
    }

    void JustDied(Unit *killer)
    {
        SetInstanceData(DATA_TORAVON, DONE);
		GiveEmblemsToGroup(m_difficulty ? GIVRE : TRIOMPHE,3);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(stone_Timer <= diff)
		{
			if(Unit* target = GetRandomUnit(2))
				ModifyAuraStack(SPELL_STONE,1,target);
			stone_Timer = urand(15000,20000);
		}
		else
			stone_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_toravon(Creature* pCreature)
{
    return new boss_toravonAI(pCreature);
}

struct MANGOS_DLL_DECL toravon_frozenOrbAI : public LibDevFSAI
{
    toravon_frozenOrbAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
		me->SetDisplayId(16925);
    }

	bool die;
    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(72067);
		ModifyAuraStack(SPELL_AURA_FROZEN_ORB);
		die = false;
    }

    void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && !die)
		{
			DoCastMe(SPELL_EXPLODE_FROZEN_ORB);
			die = true;
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_toravon_frozenOrb(Creature* pCreature)
{
    return new toravon_frozenOrbAI(pCreature);
}

void AddSC_boss_toravon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_toravon";
    newscript->GetAI = &GetAI_boss_toravon;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "toravon_frozen_orb";
    newscript->GetAI = &GetAI_toravon_frozenOrb;
    newscript->RegisterSelf();
}
