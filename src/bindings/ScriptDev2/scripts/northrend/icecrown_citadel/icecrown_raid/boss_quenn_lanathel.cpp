#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
	SPELL_SHROUD_OF_SORROW                  = 72981, // ok
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

enum phases
{
	PHASE_LAND			= 0,
	PHASE_AIR			= 1,
	SUBPHASE_FEAR		= 0,
	SUBPHASE_STORM		= 1,
	SUBPHASE_LANDING	= 2,
};

struct MANGOS_DLL_DECL boss_lanathelAI : public LibDevFSAI
{
    boss_lanathelAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_DELRIOUS_SLASH,3000,20000,0,PHASE_LAND);

		AddEnrageTimer(320000);
		AddTextEvent(16793,"On arrete MAINTENANT !",320000,60000);
    }

	uint8 phase;
	uint32 phase_Timer;
	uint8 subphase;

    void Reset()
    {
		ResetTimers();
		phase = PHASE_LAND;
		phase_Timer = 120000;
		subphase = SUBPHASE_FEAR;
		ModifyAuraStack(SPELL_SHROUD_OF_SORROW);
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_LANATHEL, IN_PROGRESS);
		Yell(16782,"Ce n'est pas une décision... très sage...");
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Say(16791,"Vraiment ? Et c'est tout ?");
		else
			Say(16792,"Quel affreux gachis...");

	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_LANATHEL, DONE);

		Say(16794,"Mais... tout allait si bien... entre ... nous...");
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
		Yell(16789,"Quel dommage ! Hahahahahaha !");
        if (pInstance)
            pInstance->SetData(TYPE_LANATHEL, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase_Timer <= diff)
		{
			if(phase == PHASE_LAND)
			{
				phase = PHASE_AIR;
				subphase = SUBPHASE_FEAR;
				// animation 2sec
				phase_Timer = 2000;
			}
			else if(phase == PHASE_AIR)
			{
				if(subphase == SUBPHASE_FEAR)
				{
					; // Todo : cast fear
					subphase = SUBPHASE_STORM;
					phase_Timer = 3000;
				}
				else if(subphase == SUBPHASE_STORM)
				{
					; // Todo : cast lightning shadows
					phase_Timer = 1000; // Todo : change this
					subphase = SUBPHASE_LANDING;
				}
				else if(subphase == SUBPHASE_LANDING)
				{
					; // TODO : land
					phase = PHASE_LAND;
					phase_Timer = 2000;
				}
			}
		}
		else
			phase_Timer -= diff;

		UpdateEvent(diff);
		if(phase == PHASE_LAND)
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
