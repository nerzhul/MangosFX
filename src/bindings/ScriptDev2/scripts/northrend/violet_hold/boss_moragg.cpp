#include "precompiled.h"

enum
{
	SPELL_OPTIC_LINK		=	54396,
	SPELL_POISON			=	54527,

};
                                 
struct MANGOS_DLL_DECL boss_moraggAI : public ScriptedAI
{	
	MobEventTasks Tasks;

    void Reset()
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_OPTIC_LINK,24000,27000,5000);
		Tasks.AddEvent(SPELL_POISON,5000,5500,2000,TARGET_MAIN);
	}
	 
	boss_moraggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

	bool m_bIsHeroic;
	    
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething() || me->HasAura(66830,0))
            return;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_moragg(Creature* pCreature)
{
    return new boss_moraggAI(pCreature);
} 

void AddSC_boss_moragg()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_moragg";
    newscript->GetAI = &GetAI_boss_moragg;
    newscript->RegisterSelf();
}

