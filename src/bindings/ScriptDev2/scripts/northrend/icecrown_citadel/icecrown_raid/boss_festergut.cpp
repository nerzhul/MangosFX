#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_GASEOUS_BLIGHT_0   = 70138,
    SPELL_GASEOUS_BLIGHT_1   = 69161,
    SPELL_GASEOUS_BLIGHT_2   = 70468,
    SPELL_INHALE_BLIGHT      = 69165,
    SPELL_INHALED_BLIGHT     = 71912,
    SPELL_PUNGENT_BLIGHT     = 69195,
    SPELL_PUNGENT_BLIGHT_1   = 69157,
    SPELL_PUNGENT_BLIGHT_2   = 69126,
    SPELL_GAS_SPORE          = 69278,
    SPELL_INOCULATE          = 72103,
    SPELL_GASTRIC_BLOAT      = 72219,
    SPELL_GASTRIC_EXPLOSION  = 72227,
    SPELL_VILE_GAS           = 72272,
};

struct MANGOS_DLL_DECL boss_festergutAI : public LibDevFSAI
{
    boss_festergutAI(Creature* pCreature) : LibDevFSAI(pCreature)
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
            pInstance->SetData(TYPE_FESTERGUT, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_FESTERGUT, DONE);

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
            pInstance->SetData(TYPE_FESTERGUT, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_festergut(Creature* pCreature)
{
    return new boss_festergutAI(pCreature);
}

void AddSC_ICC_Festergut()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_festergut";
    NewScript->GetAI = &GetAI_boss_festergut;
    NewScript->RegisterSelf();
}
