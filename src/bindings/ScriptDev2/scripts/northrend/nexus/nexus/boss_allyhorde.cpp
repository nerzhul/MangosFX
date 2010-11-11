
#include "precompiled.h"

enum
{
	SPELL_CHARGE		= 60067,
	SPELL_WARCRY		= 31403,
	SPELL_FEAR			= 19134,
	SPELL_TOURBILLON	= 38618,
	SPELL_TOURBILLON_D	= 38619,
};


struct MANGOS_DLL_DECL boss_allyhordeAI : public LibDevFSAI
{
    boss_allyhordeAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
		AddEvent(SPELL_CHARGE,5000,14000,1000);
		AddEventOnTank(SPELL_TOURBILLON,15000,18000,2000);
		AddEventOnTank(SPELL_FEAR,10000,23000,2000);
		AddEventOnMe(SPELL_WARCRY,120000,120000);
    }

    void Reset()
    {
		ResetTimers();
    }

    void Aggro(Unit* pWho)
    {
        //DoScriptText(SAY_AGGRO, me);
		DoCastMe(SPELL_WARCRY);
    }

    void JustDied(Unit* pKiller)
    {
        //DoScriptText(SAY_DEATH, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        /*if (urand(0, 1))
            DoScriptText(SAY_KILL, me);*/
		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_allyhordeAI(Creature* pCreature)
{
    return new boss_allyhordeAI(pCreature);
}


void AddSC_boss_allyhorde()
{
    Script *newscript;

	newscript = new Script;
    newscript->Name = "boss_commander_kolurg";
    newscript->GetAI = &GetAI_boss_allyhordeAI;
    newscript->RegisterSelf();
}