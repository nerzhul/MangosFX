#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
	SPELL_SHROUD_OF_SORROW                  = 72981,
	SPELL_DELRIOUS_SLASH                    = 71623,
	SPELL_BLOOD_MIRROR_1                    = 70821,
	SPELL_BLOOD_MIRROR_2                    = 71510,
	SPELL_VAMPIRIC_BITE                     = 71726,
	SPELL_ESSENCE_OF_BLOOD_QWEEN            = 70867,
	SPELL_FRENZIED_BLOODTHIRST              = 70877,
	SPELL_UNCONTROLLABLE_FRENZY             = 70923,
	SPELL_PACT_OF_DARKFALLEN                = 71340,
	SPELL_SWARMING_SHADOWS                  = 71265,
	SPELL_TWILIGHT_BLOODBOLT                = 71446,
	SPELL_BLOODBOLT_WHIRL                   = 71772,
	SPELL_PRESENCE_OF_DARKFALLEN            = 71952,
};

struct MANGOS_DLL_DECL boss_lanathelAI : public LibDevFSAI
{
    boss_lanathelAI(Creature* pCreature) : LibDevFSAI(pCreature)
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
            pInstance->SetData(TYPE_LANATHEL, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_LANATHEL, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				GiveEmblemsToGroup(GIVRE,1);
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
            pInstance->SetData(TYPE_LANATHEL, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lanathel(Creature* pCreature)
{
    return new boss_lanathelAI(pCreature);
}

void AddSC_ICC_Lanathel()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_lanathel";
    NewScript->GetAI = &GetAI_boss_lanathel;
    NewScript->RegisterSelf();
}
