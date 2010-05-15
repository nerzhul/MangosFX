/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{
	SPELL_CHILLING_WAVE			=	68778,
	SPELL_DEEP_FREEZE			=	70381,
	SPELL_FROST_MACE			=	68785,
	SPELL_FROST_BLADE			=	68774,
	SPELL_PERMAFROST			=	70326,
	SPELL_SARONITE				=	68788,
	SPELL_STOMP					=	68771
};

static const float Weapon_Locations[2][3] = {
	{722.994f,	-233.293f,	527.111f },
	{642.430f,	-204.899f,	528.931f }
};

struct MANGOS_DLL_DECL boss_garfrostAI : public LibDevFSAI
{
    boss_garfrostAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_CHILLING_WAVE,16000,14000,2000);
		AddEvent(SPELL_DEEP_FREEZE,13000,13000,3500);
		AddEvent(SPELL_SARONITE,25000,30000);
		AddEventOnTank(SPELL_STOMP,10000,10000,2500);
    }

	uint8 phase;

    void Reset()
    {
		ResetTimers();
		phase = 0;
    }

	void EnterCombat(Unit* who)
	{
		DoCastMe(SPELL_PERMAFROST);
	}


    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(66) && phase == 0)
		{
			phase++;
			Jump(Weapon_Locations[0][0],Weapon_Locations[0][1],Weapon_Locations[0][2],2.0f,2.0f);
			DoCastMe(SPELL_FROST_BLADE);
		}
		else if(CheckPercentLife(33) && phase == 1)
		{
			phase++;
			Jump(Weapon_Locations[1][0],Weapon_Locations[1][1],Weapon_Locations[1][2],2.0f,2.0f);
			DoCastMe(SPELL_FROST_MACE);
		}
      
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_garfrost(Creature* pCreature)
{
    return new boss_garfrostAI (pCreature);
}

void AddSC_boss_garfrost()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_garfrost";
    newscript->GetAI = &GetAI_boss_garfrost;
    newscript->RegisterSelf();
}
