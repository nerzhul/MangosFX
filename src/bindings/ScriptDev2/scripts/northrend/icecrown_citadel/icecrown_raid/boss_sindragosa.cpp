#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	// phase 1 & 3
	SPELL_FROST_AURA		=	70084,
	SPELL_CLEAVE			=	19983,
	SPELL_SOUL_FEAST		=	71203,
	SPELL_BLISTERING_COLD	=	70123,
	SPELL_FROST_BREATH		=	69649,
	SPELL_FROST_BREATH2		=	73061,
	SPELL_ICY_GRIP			=	70117, // teleport all aggroed players
	SPELL_PERMEATING_CHILL	=	70109,
	SPELL_TAIL_SMASH		=	71077,
	SPELL_UNCHAINED_MAGIC	=	69762,
	
	// phase 3 only
	SPELL_MYSTIC_BUFFER		=	70128,
	
	// phase 2 only
	SPELL_ICE_TOMB			=	69712,
	SPELL_FROST_BEACON		=	70126,
	SPELL_FROST_BOMB		=	71053,
	SPELL_ASPHYXIATION		=	71665,
	
	// others
	SPELL_ENRAGE			=	26662,
};

struct MANGOS_DLL_DECL boss_sindragosaAI : public LibDevFSAI
{
    boss_sindragosaAI(Creature* pCreature) : LibDevFSAI(pCreature)
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
            pInstance->SetData(TYPE_SINDRAGOSA, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_SINDRAGOSA, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,1);
				GiveEmblemsToGroup(GIVRE,2);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
		}
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_SINDRAGOSA, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

void AddSC_ICC_Sindragosa()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_sindragosa";
    NewScript->GetAI = &GetAI_boss_sindragosa;
    NewScript->RegisterSelf();
}
