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
	SPELL_PACT_OF_DARKFALLEN                = 71340, // ok: TODO : distance damages
	SPELL_SWARMING_SHADOWS                  = 71264, // ok: TODO : spawn event and casting region
	SPELL_FEAR								= 73070, // ok
	SPELL_TWILIGHT_BLOODBOLT                = 71446,
	SPELL_BLOODBOLT_WHIRL                   = 71772,
	SPELL_PRESENCE_OF_DARKFALLEN            = 71952,
};

enum phases
{
	PHASE_LAND			= 1,
	PHASE_AIR			= 2,
	SUBPHASE_MOVE		= 0,
	SUBPHASE_FEAR		= 1,
	SUBPHASE_STORM		= 2,
	SUBPHASE_LANDING	= 3,
};

struct MANGOS_DLL_DECL boss_lanathelAI : public LibDevFSAI
{
    boss_lanathelAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_DELRIOUS_SLASH,3000,20000,0,PHASE_LAND);
		AddEvent(SPELL_SWARMING_SHADOWS,35000,30000,0,TARGET_RANDOM,PHASE_LAND);
		AddEnrageTimer(320000);
		AddTextEvent(16793,"On arrete MAINTENANT !",320000,60000);
		pactTargets = 0;
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_10MAN_HEROIC:
				pactTargets = 2;
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				pactTargets = 3;
				break;
		}
    }

	uint8 phase;
	uint32 phase_Timer;
	uint8 subphase;
	uint32 pact_Timer;
	uint8 pactTargets;

    void Reset()
    {
		ResetTimers();
		phase = PHASE_LAND;
		phase_Timer = 120000;
		subphase = SUBPHASE_MOVE;
		ModifyAuraStack(SPELL_SHROUD_OF_SORROW);
		pact_Timer = 20000;
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
				if(subphase == SUBPHASE_MOVE)
				{
					me->GetMotionMaster()->MovePoint(0,4595.9f,2769.32f,400.14f);
					subphase = SUBPHASE_FEAR;
					phase_Timer = 2000;
				}
				else if(subphase == SUBPHASE_FEAR)
				{
					DoCastVictim(SPELL_FEAR);					
					subphase = SUBPHASE_STORM;
					phase_Timer = 2000;
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
		UpdateEvent(diff,phase);
		if(phase == PHASE_LAND)
		{
			if(pact_Timer <= diff)
			{
				uint8 breaker = 0;
				for(int8 i=0;i<pactTargets;i++)
				{
					if(Unit* u = GetRandomUnit())
					{
						if(u->HasAura(SPELL_PACT_OF_DARKFALLEN))
							i--;
						else
							DoCast(u,SPELL_PACT_OF_DARKFALLEN,true);
					}
					breaker++;
					if(breaker >= 50)
						break;
				}
				pact_Timer = 30000;
			}
			else
				pact_Timer -= diff;

			DoMeleeAttackIfReady();
		}
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
