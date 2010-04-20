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

struct MANGOS_DLL_DECL boss_lavanthorAI : public ScriptedAI
{	
	boss_lavanthorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {	
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    void Reset()
	{
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_LAVA_BURN,13000,20000,40000,TARGET_RANDOM);
			Tasks.AddEvent(SPELL_FLAME_BREATH,10000,15000,7000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_FIREBOLT,8000,11000,3000,TARGET_RANDOM);
		}
		else
		{
			Tasks.AddEvent(SPELL_LAVA_BURN_H,13000,20000,40000,TARGET_RANDOM);
			Tasks.AddEvent(SPELL_FLAME_BREATH_H,10000,15000,7000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_FIREBOLT_H,8000,11000,3000,TARGET_RANDOM);
			Tasks.AddEvent(SPELL_CAUTERIZING_H,18000,28000,4000,TARGET_MAIN);
		}
	}
	
	MobEventTasks Tasks;
    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	    
    void UpdateAI(const uint32 uiDiff)
	{	
        if (!Tasks.CanDoSomething() || me->HasAura(66830,0))
            return;

		Tasks.UpdateEvent(uiDiff);
	
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

