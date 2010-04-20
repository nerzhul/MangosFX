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

struct MANGOS_DLL_DECL boss_vezaxAI : public ScriptedAI
{
    boss_vezaxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;
	uint32 saronite_timer;
	uint8 nbsaro;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_ENRAGE,600000,60000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_MARK_OF_FACELESS,75000,60000);
		Tasks.AddEvent(SPELL_SEARING_FLAME,15000,12000,2000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_SURGE_OF_DARKNESS,70000,60000,10000,TARGET_ME,0,0,true);
		Tasks.AddEvent(SPELL_SHADOW_CRASH,8000,12000,2000);
		Tasks.AddSummonEvent(NPC_VAPOR_SARONITE,45000,45000,0,0,1,TEN_MINS,NEAR_30M,NOTHING);
		nbsaro = 0;
		saronite_timer = 45000;
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEZAX, DONE);
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE,2);
    }

    void Aggro(Unit* pWho)
    {
//        DoScriptText(SAY_AGGRO, me);
        //me->SetInCombatWithZone();
		DoCastMe(SPELL_AURA_OF_DESPAIR);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VEZAX, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanDoSomething())
            return;

		if(saronite_timer <= diff)
		{
			nbsaro++;
			if(nbsaro == 6)
			{
				Tasks.CallCreature(NPC_SARONITE_ANIMUS,TEN_MINS,NEAR_30M);
				DoCastMe(SPELL_VOID_BARRER);
				nbsaro = 0;
			}
			saronite_timer = 45000;
		}
		else
			saronite_timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

        //EnterEvadeIfOutOfCombatArea(diff);

    }

};

CreatureAI* GetAI_boss_vezax(Creature* pCreature)
{
    return new boss_vezaxAI(pCreature);
}

struct MANGOS_DLL_DECL add_vezaxAI : public ScriptedAI
{
    add_vezaxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(63323,200,8000,0,TARGET_ME);
    }

	void UpdateAI(const uint32 diff)
    {
        if (CanDoSomething())
            return;
		
		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_add_vezax(Creature* pCreature)
{
    return new add_vezaxAI(pCreature);
}

struct MANGOS_DLL_DECL add2_vezaxAI : public ScriptedAI
{
    add2_vezaxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(63420,7000,7000,1000,TARGET_MAIN);
    }

	void JustDied(Unit* pWho)
	{
		if (Creature* Vezax = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_VEZAX) : 0)))
		{
			Vezax->RemoveAurasDueToSpell(SPELL_VOID_BARRER);
		}

	}
	void UpdateAI(const uint32 diff)
    {
        if (CanDoSomething())
            return;
		
		Tasks.UpdateEvent(diff);

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
