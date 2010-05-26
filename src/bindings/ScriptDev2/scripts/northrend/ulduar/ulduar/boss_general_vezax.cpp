/* LibDevFS By Frost Sapphire Studios
 */

#include "precompiled.h"
#include "ulduar.h"

enum MiscIds
{
	SPELL_AURA_OF_DESPAIR		=	62692,
	SPELL_ENRAGE				=	26662,
	SPELL_MARK_OF_FACELESS		=	63276,
	SPELL_SARONITE_BANNER_HM	=	63364,
	SPELL_SEARING_FLAME			=	62661,
	SPELL_SHADOW_CRASH			=	62660,
	SPELL_SURGE_OF_DARKNESS		=	62662,
	SPELL_VOID_BARRER			=	63710,

	NPC_SARONITE_ANIMUS			=	33524,
	NPC_VAPOR_SARONITE			=	33488,

	SPELL_SARONITE_VAPOR		=	63322,
};

struct MANGOS_DLL_DECL boss_vezaxAI : public LibDevFSAI
{
    boss_vezaxAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(SPELL_ENRAGE,600000,60000);
		AddEvent(SPELL_MARK_OF_FACELESS,75000,60000);
		AddEventOnTank(SPELL_SEARING_FLAME,15000,12000,2000);
		AddEventMaxPrioOnMe(SPELL_SURGE_OF_DARKNESS,70000,60000,10000);
		AddEvent(SPELL_SHADOW_CRASH,8000,12000,2000);
		AddSummonEvent(NPC_VAPOR_SARONITE,45000,45000,0,0,1,TEN_MINS,NEAR_30M,NOTHING);
    }

	uint32 saronite_timer;
	uint8 nbsaro;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		
		nbsaro = 0;
		saronite_timer = 45000;
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_VEZAX, DONE);
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE,2);
    }

    void Aggro(Unit* pWho)
    {
//        DoScriptText(SAY_AGGRO, me);
        //me->SetInCombatWithZone();
		DoCastMe(SPELL_AURA_OF_DESPAIR);
        if (pInstance)
            pInstance->SetData(TYPE_VEZAX, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(saronite_timer <= diff)
		{
			nbsaro++;
			if(nbsaro == 6)
			{
				CallCreature(NPC_SARONITE_ANIMUS,TEN_MINS,NEAR_30M);
				DoCastMe(SPELL_VOID_BARRER);
				nbsaro = 0;
			}
			saronite_timer = 45000;
		}
		else
			saronite_timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();

        //EnterEvadeIfOutOfCombatArea(diff);

    }

};

CreatureAI* GetAI_boss_vezax(Creature* pCreature)
{
    return new boss_vezaxAI(pCreature);
}

struct MANGOS_DLL_DECL add_vezaxAI : public LibDevFSAI
{
    add_vezaxAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(63323,200,8000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_add_vezax(Creature* pCreature)
{
    return new add_vezaxAI(pCreature);
}

struct MANGOS_DLL_DECL add2_vezaxAI : public LibDevFSAI
{
    add2_vezaxAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnTank(63420,7000,7000,1000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void JustDied(Unit* pWho)
	{
		if (Creature* Vezax = ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(TYPE_VEZAX) : 0)))
		{
			Vezax->RemoveAurasDueToSpell(SPELL_VOID_BARRER);
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

CreatureAI* GetAI_add2_vezax(Creature* pCreature)
{
    return new add2_vezaxAI(pCreature);
}


void AddSC_boss_vezax()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_vezax";
    newscript->GetAI = &GetAI_boss_vezax;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_vezax";
    newscript->GetAI = &GetAI_add_vezax;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add2_vezax";
    newscript->GetAI = &GetAI_add2_vezax;
    newscript->RegisterSelf();

}
