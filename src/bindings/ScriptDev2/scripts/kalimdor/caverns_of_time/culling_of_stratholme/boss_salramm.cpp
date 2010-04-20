#include "precompiled.h"
#include "def_culling_of_stratholme.h"

enum
{
    SAY_SALRAMM_SPAWN		= -1594129,
    SAY_SALRAMM_AGGRO		= -1594130,   
    SAY_SALRAMM_DEATH       = -1594131,  
    SAY_SALRAMM_SLAY01      = -1594132, 
    SAY_SALRAMM_SLAY02      = -1594133, 
    SAY_SALRAMM_SLAY03      = -1594134,
    SAY_SALRAMM_STEAL01     = -1594135,
    SAY_SALRAMM_STEAL02     = -1594136,
    SAY_SALRAMM_STEAL03     = -1594137,

    SPELL_SB_N              = 57725,
    SPELL_SB_H              = 58827,
    SPELL_FLESH             = 58845,
    SPELL_STEAL             = 52708
};

struct MANGOS_DLL_DECL boss_salrammAI : public ScriptedAI
{
   boss_salrammAI(Creature *c) : ScriptedAI(c)
   {
        m_bIsHeroic = c->GetMap()->GetDifficulty();
        Reset();
   }

	bool m_bIsHeroic;
	uint32 Step;
	uint32 Steptim;
	uint32 Motion;
	uint32 ShadowBolt_Timer;
	uint32 Flesh_Timer;
	uint32 Steal_Timer;
	MobEventTasks Tasks;

	void Reset() 
	{
		Tasks.SetObjects(this,me);
		Step = 1;
		Steptim = 7000;
		Motion = 0;
		ShadowBolt_Timer = 5300;
		Flesh_Timer = 7300;
		Steal_Timer = 17300;
	}
   
	void Aggro(Unit* who)
	{
	   DoScriptText(SAY_SALRAMM_AGGRO, me);
	   me->GetMotionMaster()->Clear(false);
	}

   void JustDied(Unit *killer)
    {
       DoScriptText(SAY_SALRAMM_DEATH, me);
	   GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_SALRAMM_SLAY01, me); break;
            case 1: DoScriptText(SAY_SALRAMM_SLAY02, me); break;
            case 2: DoScriptText(SAY_SALRAMM_SLAY03, me); break;
        }
    }

   void UpdateAI(const uint32 uiDiff)
   {
      if(Motion == 0)
      {
			 switch(Step)
			 {
				 case 1:
						DoScriptText(SAY_SALRAMM_SPAWN, me); 
					    ++Step;
					    Steptim = 7000;
					    break;
				 case 3:
					   me->GetMotionMaster()->MovePoint(0, 2165.073f,1279.338f,133.40f);
					   ++Step;
					   Steptim = 7000;
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

        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		if (ShadowBolt_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsHeroic ? SPELL_SB_H : SPELL_SB_N);

            ShadowBolt_Timer = 5300;
        }
		else 
			ShadowBolt_Timer -= uiDiff;

		if (Flesh_Timer <= uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target,SPELL_FLESH);

            Flesh_Timer = 7300;
        }
		else 
			Flesh_Timer -= uiDiff;

		if (Steal_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target,SPELL_STEAL);
            switch(urand(0,2))
            {
              case 0: DoScriptText(SAY_SALRAMM_STEAL01, me); break;
              case 1: DoScriptText(SAY_SALRAMM_STEAL02, me); break;
              case 2: DoScriptText(SAY_SALRAMM_STEAL03, me); break;
            }

            Steal_Timer = 17300;
        }
		else
			Steal_Timer -= uiDiff;

		DoMeleeAttackIfReady();

  }
};

CreatureAI* GetAI_boss_salramm(Creature *_Creature)
{
    boss_salrammAI* salrammAI = new boss_salrammAI(_Creature);
    return (CreatureAI*)salrammAI;
};

void AddSC_boss_salramm()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_salramm";
    newscript->GetAI = &GetAI_boss_salramm;
    newscript->RegisterSelf();

}