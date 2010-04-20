/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "azjol-nerub.h"

enum
{
    SAY_AGGRO                       = -1601000,
    SAY_KILL_1                      = -1601001,
    SAY_KILL_2                      = -1601002,
    SAY_KILL_3                      = -1601003,
    SAY_SEND_GROUP_1                = -1601004,
    SAY_SEND_GROUP_2                = -1601005,
    SAY_SEND_GROUP_3                = -1601006,
    SAY_PREFIGHT_1                  = -1601007,
    SAY_PREFIGHT_2                  = -1601008,
    SAY_PREFIGHT_3                  = -1601009,
    SAY_SWARM_1                     = -1601010,
    SAY_SWARM_2                     = -1601011,
    SAY_DEATH                       = -1601012,
    EMOTE_BOSS_GENERIC_FRENZY       = -1000005,
	SPELL_FOUET_N					= 52586,
	SPELL_FOUET_H					= 59367,
	SPELL_ENRAGE					= 28747,
	SPELL_MALE_N					= 52592,
	SPELL_MALE_H					= 59368,

	// adds 
	SPELL_TOILE						= 52086,
	SPELL_SILTHIK_POISON_N			= 52493,
	SPELL_SILTHIK_POISON_H			= 59366,
	SPELL_MORS_N					= 52469,
	SPELL_MORS_H					= 59364,
	SPELL_GASHRA_ENRAGE				= 52470,
	SPELL_NARJIL_RETS_N				= 52524,
	SPELL_NARJIL_RETS_H				= 59365,

};

/*######
## boss_krikthir
######*/

struct MANGOS_DLL_DECL boss_krikthirAI : public ScriptedAI
{
    boss_krikthirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

	MobEventTasks Tasks;
    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_FOUET_H,5000,5000,3000);
			Tasks.AddEvent(SPELL_MALE_H,12000,8000,5000);
		}
		else
		{
			Tasks.AddEvent(SPELL_FOUET_N,5000,5000,3000);
			Tasks.AddEvent(SPELL_MALE_N,12000,8000,5000);
		}
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, me); break;
            case 1: DoScriptText(SAY_KILL_2, me); break;
            case 2: DoScriptText(SAY_KILL_3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);

		if(Tasks.CheckPercentLife(30) && !me->HasAura(SPELL_ENRAGE,0))
			DoCastMe(SPELL_ENRAGE);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_krikthir(Creature* pCreature)
{
    return new boss_krikthirAI(pCreature);
}

struct MANGOS_DLL_DECL boss_silthikAI : public ScriptedAI
{
	MobEventTasks Tasks;

    boss_silthikAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_SILTHIK_POISON_H,7000,5000,3000);
			Tasks.AddEvent(SPELL_MORS_H,10000,8000,5000);
		}
		else
		{
			Tasks.AddEvent(SPELL_SILTHIK_POISON_N,7000,5000,3000);
			Tasks.AddEvent(SPELL_MORS_N,10000,8000,5000);
		}
		Tasks.AddEvent(SPELL_TOILE,5000,13000,5000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_silthik(Creature* pCreature)
{
    return new boss_silthikAI(pCreature);
}

struct MANGOS_DLL_DECL boss_gashraAI : public ScriptedAI
{
	float percent;

	MobEventTasks Tasks;

    boss_gashraAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_TOILE,13000,13000,5000);
		if(m_bIsHeroic)
			Tasks.AddEvent(SPELL_MORS_H,10000,8000,3000);
		else
			Tasks.AddEvent(SPELL_MORS_N,10000,8000,3000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;
		
		Tasks.UpdateEvent(uiDiff);

		percent = ((float)me->GetHealth() * 100/(float)me->GetMaxHealth());
		if(percent < 40 && !me->HasAura(SPELL_GASHRA_ENRAGE,0))
			DoCastMe(SPELL_GASHRA_ENRAGE);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gashra(Creature* pCreature)
{
    return new boss_gashraAI(pCreature);
}


struct MANGOS_DLL_DECL boss_narjilAI : public ScriptedAI
{
    boss_narjilAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_TOILE,10000,13000,5000);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_NARJIL_RETS_H,10000,5000,5000);
			Tasks.AddEvent(SPELL_MORS_H,7000,5000,3000);
		}
		else
		{
			Tasks.AddEvent(SPELL_NARJIL_RETS_N,10000,5000,5000);
			Tasks.AddEvent(SPELL_MORS_N,7000,5000,3000);
		}
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_narjil(Creature* pCreature)
{
    return new boss_narjilAI(pCreature);
}

void AddSC_boss_krikthir()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_krikthir";
    newscript->GetAI = &GetAI_boss_krikthir;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_narjil";
    newscript->GetAI = &GetAI_boss_narjil;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_gashra";
    newscript->GetAI = &GetAI_boss_gashra;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_silthik";
    newscript->GetAI = &GetAI_boss_silthik;
    newscript->RegisterSelf();
}
