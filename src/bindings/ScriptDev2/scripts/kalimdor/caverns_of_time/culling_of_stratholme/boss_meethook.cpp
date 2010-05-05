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

struct MANGOS_DLL_DECL boss_meathookAI : public ScriptedAI
{
   boss_meathookAI(Creature *c) : ScriptedAI(c)
   {
        m_bIsHeroic = c->GetMap()->GetDifficulty();
        Reset();
   }

	bool m_bIsHeroic;
	uint32 phaseArthas;
	uint32 Chain_Timer;
	uint32 Exploded_Timer;
	uint32 Frenzy_Timer;
	MobEventTasks Tasks;

	void Reset() 
	{
		Tasks.SetObjects(this,me);
		phaseArthas = 0;
		Chain_Timer = 6300;
		Exploded_Timer = 9300;
		Frenzy_Timer = 23300;
   }
   
   void Aggro(Unit* who)
{
   DoScriptText(SAY_MEATHOOK_AGGRO, me);
}

   void JustDied(Unit *killer)
    {
       DoScriptText(SAY_MEATHOOK_DEATH, me);
	   GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
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


		if (Chain_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsHeroic ? SPELL_CHAIN_H : SPELL_CHAIN_N);

            Chain_Timer = urand(5800,6300);
        }
		else 
			Chain_Timer -= diff;

		if (Exploded_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsHeroic ? SPELL_EXPLODED_H : SPELL_EXPLODED_N);
            Exploded_Timer = urand(8500,9600);
        }
		else 
			Exploded_Timer -= diff;

		if (Frenzy_Timer <= diff)
        {
            DoCastMe(SPELL_FRENZY);
            Frenzy_Timer = urand(22000,25000);
        }
		else
			Frenzy_Timer -= diff;

		DoMeleeAttackIfReady();
  }
};

CreatureAI* GetAI_boss_meathook(Creature *_Creature)
{
    boss_meathookAI* meathookAI = new boss_meathookAI(_Creature);
    return (CreatureAI*)meathookAI;
};

void AddSC_boss_meathook()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_meathook";
    newscript->GetAI = &GetAI_boss_meathook;
    newscript->RegisterSelf();
}
