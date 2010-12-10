#include "precompiled.h"
#include "vault_of_archavon.h"

enum Npc
{
	NPC_TORAVON_ORB		= 38461;
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
		AddEventOnTank(SPELL_FROZEN_MASS,5000,10000);
		AddEvent(SPELL_FROZEN_ORB,12000,30000);
		AddEventOnMe(SPELL_WHITE_SNOW,1000,50000);
		AddEvent(SPELL_FROZEN_FLOOR,20000,30000);
		AddEvent(SPELL_STONE,24000,15000);
    }


    void Reset()
    {
		ResetTimers();
		SetInstanceData(DATA_TORAVON, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
		SetInstanceData(DATA_TORAVON, IN_PROGRESS);
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
		MakeHostileStalker();
    }

	bool die;
    void Reset()
    {
		ResetTimers();
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

        DoMeleeAttackIfReady();
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
