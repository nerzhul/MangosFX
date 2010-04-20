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

struct MANGOS_DLL_DECL boss_lord_epochAI : public ScriptedAI
{
   boss_lord_epochAI(Creature *c) : ScriptedAI(c)
   {
        m_bIsHeroic = c->GetMap()->GetDifficulty();
        Reset();
   }

	uint32 Step;
	uint32 Steptim;
	uint32 Intro;
	bool m_bIsHeroic;
	uint32 Spike_Timer;
	uint32 Warp_Timer;
	uint32 Stop_Timer;
	uint32 Course_Timer;
	Unit* target;
	MobEventTasks Tasks;

	void Reset() 
	{
		Tasks.SetObjects(this,me);
		target = NULL;
		Intro = 0;
		Step = 1;
		Steptim = 26000;
		Course_Timer = 9300;
		Stop_Timer = 21300;
		Warp_Timer = 25300;
		Spike_Timer = 5300;
	}
   
	void Aggro(Unit* who)
	{
		DoScriptText(SAY_EPOCH_AGGRO, me);
	}

   void JustDied(Unit *killer)
    {
       DoScriptText(SAY_EPOCH_DEATH, me);
	   GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
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

   void UpdateAI(const uint32 uiDiff)
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

	   if (Steptim <= uiDiff)
	   {
		   ++Step;
		   Steptim = 330000;
	   } 
	   else
			Steptim -= uiDiff;

        if (!CanDoSomething())
            return;


		if (Course_Timer <= uiDiff)
        {
			target = SelectUnit(SELECT_TARGET_RANDOM,0);
            DoCast(target, SPELL_COURSE);

            Course_Timer = urand(8900,9600);
        }
		else 
			Course_Timer -= uiDiff;

		if (Spike_Timer <= uiDiff)
        {
            DoCastVictim(m_bIsHeroic ? SPELL_SPIKE_H : SPELL_SPIKE_N);
            Spike_Timer = urand(4800,5500);
        }
		else 
			Spike_Timer -= uiDiff;

		if (m_bIsHeroic == true)
		{
			if (Stop_Timer <= uiDiff)
			{
				target = SelectUnit(SELECT_TARGET_RANDOM,0);
				DoCast(target, SPELL_TIME_STOP);

				Stop_Timer = urand(21000,22000);
			}
			else 
				Stop_Timer -= uiDiff;
		}

		if (Warp_Timer <= uiDiff)
        {
			target = SelectUnit(SELECT_TARGET_RANDOM,0);
            DoCast(target, SPELL_TIME_WARP);
			switch(urand(0,2))
			{
				case 0: DoScriptText(SAY_EPOCH_WARP01, me); break;
				case 1: DoScriptText(SAY_EPOCH_WARP02, me); break;
				case 2: DoScriptText(SAY_EPOCH_WARP03, me); break;
			}

            Warp_Timer = urand(24500,25500);
        }
		else 
			Warp_Timer -= uiDiff;

		
		DoMeleeAttackIfReady();
  }
};

CreatureAI* GetAI_boss_lord_epoch(Creature *_Creature)
{
    boss_lord_epochAI* lord_epochAI = new boss_lord_epochAI(_Creature);
    return (CreatureAI*)lord_epochAI;
};

void AddSC_boss_lord_epoch()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lord_epoch";
    newscript->GetAI = &GetAI_boss_lord_epoch;
    newscript->RegisterSelf();
}
