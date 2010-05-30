#include "precompiled.h"
#include "def_culling_of_stratholme.h"

enum
{
    SPELL_COURSE                = 52772,
    SPELL_TIME_STOP             = 58848,
    SPELL_TIME_WARP				= 52766,
    SPELL_SPIKE_N               = 52771,
    SPELL_SPIKE_H               = 58830,

    SAY_EPOCH_INTRO				= -1594116,
    SAY_EPOCH_AGGRO				= -1594118,   
    SAY_EPOCH_DEATH				= -1594119,  
    SAY_EPOCH_SLAY01			= -1594120, 
    SAY_EPOCH_SLAY02			= -1594121, 
    SAY_EPOCH_SLAY03			= -1594122,
    SAY_EPOCH_WARP01			= -1594123, 
    SAY_EPOCH_WARP02			= -1594124, 
    SAY_EPOCH_WARP03			= -1594125
};

struct MANGOS_DLL_DECL boss_lord_epochAI : public LibDevFSAI
{
   boss_lord_epochAI(Creature *c) : LibDevFSAI(c)
   {
        InitInstance();
		AddEvent(SPELL_COURSE,9300,8900,700);
		AddEventOnTank(m_difficulty ? SPELL_SPIKE_H : SPELL_SPIKE_N,5300,4800,700);
		if(m_difficulty)
			AddEvent(SPELL_TIME_STOP,21300,21000,1000);
   }

	uint32 Step;
	uint32 Steptim;
	uint32 Intro;
	uint32 Warp_Timer;

	void Reset() 
	{
		Intro = 0;
		Step = 1;
		Steptim = 26000;
		Warp_Timer = 25300;
	}
   
	void Aggro(Unit* who)
	{
		DoScriptText(SAY_EPOCH_AGGRO, me);
	}

	void JustDied(Unit *killer)
    {
       DoScriptText(SAY_EPOCH_DEATH, me);
	   GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0,2))
        {
            case 0: DoScriptText(SAY_EPOCH_SLAY01, me); break;
            case 1: DoScriptText(SAY_EPOCH_SLAY02, me); break;
            case 2: DoScriptText(SAY_EPOCH_SLAY03, me); break;
        }
    }

	void UpdateAI(const uint32 diff)
	{
	   if(Intro == 0)
	   {
			 switch(Step)
			 {
				 case 1:
					  DoScriptText(SAY_EPOCH_INTRO, me);
					  ++Step;
					  Steptim = 26000;
					  break;
				 case 3:
					  me->setFaction(14);
					  ++Step;
					  Steptim = 1000;
					  break;
			  }
		}
		else 
			return;

	   if (Steptim <= diff)
	   {
		   ++Step;
		   Steptim = 330000;
	   } 
	   else
			Steptim -= diff;

		if (!CanDoSomething())
			return;

		if (Warp_Timer <= diff)
		{
			DoCastRandom(SPELL_TIME_WARP);
			switch(urand(0,2))
			{
				case 0: DoScriptText(SAY_EPOCH_WARP01, me); break;
				case 1: DoScriptText(SAY_EPOCH_WARP02, me); break;
				case 2: DoScriptText(SAY_EPOCH_WARP03, me); break;
			}
			Warp_Timer = urand(24500,25500);
		}
		else 
			Warp_Timer -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_lord_epoch(Creature *_Creature)
{
    return new boss_lord_epochAI(_Creature);
};

void AddSC_boss_lord_epoch()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lord_epoch";
    newscript->GetAI = &GetAI_boss_lord_epoch;
    newscript->RegisterSelf();
}
