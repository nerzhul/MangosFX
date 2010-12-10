#include "precompiled.h"
#include "vault_of_archavon.h"

enum Spells
{
	SPELL_FROZEN_MASS		= 71993;
};

struct MANGOS_DLL_DECL boss_toravonAI : public LibDevFSAI
{
    boss_toravonAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
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

void AddSC_boss_toravon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_toravon";
    newscript->GetAI = &GetAI_boss_toravon;
    newscript->RegisterSelf();
}
