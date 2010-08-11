#include "precompiled.h"

enum
{
	SPELL_LAVA_BURN				=	59594,
	SPELL_LAVA_BURN_H			=	54249,
	SPELL_FLAME_BREATH			=	54282,
	SPELL_FLAME_BREATH_H		=	59469,
	SPELL_FIREBOLT				=	54235,
	SPELL_FIREBOLT_H			=	59468,
	SPELL_CAUTERIZING_H			=	59466,
};
   
/*######
## boss_Lavanthor
######*/

struct MANGOS_DLL_DECL boss_lavanthorAI : public LibDevFSAI
{	
	boss_lavanthorAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
		if(m_difficulty)
		{
			AddEvent(SPELL_LAVA_BURN,13000,20000,40000);
			AddEventOnTank(SPELL_FLAME_BREATH,10000,15000,7000);
			AddEvent(SPELL_FIREBOLT,8000,11000,3000);
		}
		else
		{
			AddEvent(SPELL_LAVA_BURN_H,13000,20000,40000);
			AddEventOnTank(SPELL_FLAME_BREATH_H,10000,15000,7000);
			AddEvent(SPELL_FIREBOLT_H,8000,11000,3000);
			AddEventOnTank(SPELL_CAUTERIZING_H,18000,28000,4000);
		}
    }

    void Reset()
	{
		ResetTimers();
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething() || me->HasAura(66830,0))
            return;

		UpdateEvent(diff);
	
		DoMeleeAttackIfReady();

	}
};

CreatureAI* GetAI_boss_lavanthor(Creature* pCreature)
{
    return new boss_lavanthorAI(pCreature);
} 

void AddSC_boss_lavanthor()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lavanthor";
    newscript->GetAI = &GetAI_boss_lavanthor;
    newscript->RegisterSelf();
}

