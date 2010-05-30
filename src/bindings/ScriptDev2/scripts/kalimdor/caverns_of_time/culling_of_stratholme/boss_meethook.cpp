#include "precompiled.h"
#include "def_culling_of_stratholme.h"

enum
{
    SPELL_CHAIN_N           = 52696,
    SPELL_CHAIN_H           = 58823,
    SPELL_EXPLODED_N		= 52666,
    SPELL_EXPLODED_H        = 58821,
    SPELL_FRENZY            = 58841,

    SAY_MEATHOOK_SPAWN      = -1594110,
    SAY_MEATHOOK_AGGRO      = -1594111,   
    SAY_MEATHOOK_DEATH      = -1594112,  
    SAY_MEATHOOK_SLAY01     = -1594113, 
    SAY_MEATHOOK_SLAY02     = -1594114, 
    SAY_MEATHOOK_SLAY03     = -1594115
};

struct MANGOS_DLL_DECL boss_meathookAI : public LibDevFSAI
{
   boss_meathookAI(Creature *c) : LibDevFSAI(c)
   {
        InitInstance();
		AddEvent(m_difficulty ? SPELL_CHAIN_H : SPELL_CHAIN_N,6300,5800,500);
		AddEvent(m_difficulty ? SPELL_EXPLODED_H : SPELL_EXPLODED_N,9300,8500,600);
		AddEventOnMe(SPELL_FRENZY,23300,22000,3000);
   }

	void Reset() 
	{
		ResetTimers();
	}
   
	void Aggro(Unit* who)
	{
	   DoScriptText(SAY_MEATHOOK_AGGRO, me);
	}

	void JustDied(Unit *killer)
    {
       DoScriptText(SAY_MEATHOOK_DEATH, me);
	   GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0,2))
        {
            case 0: DoScriptText(SAY_MEATHOOK_SLAY01, me); break;
            case 1: DoScriptText(SAY_MEATHOOK_SLAY02, me); break;
            case 2: DoScriptText(SAY_MEATHOOK_SLAY03, me); break;
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

CreatureAI* GetAI_boss_meathook(Creature *_Creature)
{
    return new boss_meathookAI(_Creature);
};

void AddSC_boss_meathook()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_meathook";
    newscript->GetAI = &GetAI_boss_meathook;
    newscript->RegisterSelf();
}
