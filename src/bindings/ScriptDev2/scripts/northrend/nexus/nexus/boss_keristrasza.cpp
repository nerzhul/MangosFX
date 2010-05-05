/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum
{
    SAY_AGGRO                   = -1576016,
    SAY_CRYSTAL_NOVA            = -1576017,
    SAY_ENRAGE                  = -1576018,
    SAY_KILL                    = -1576019,
    SAY_DEATH                   = -1576020,
    SPELL_CRYSTALFIRE_BREATH    = 48096,
    SPELL_CRYSTALFIRE_BREATH_H  = 57091,
    SPELL_CRYSTALLIZE           = 48179,
    SPELL_CRYSTAL_CHAINS        = 50997,
    SPELL_CRYSTAL_CHAINS_H      = 57050,
    SPELL_TAIL_SWEEP            = 50155,
    SPELL_INTENSE_COLD          = 48094,
    SPELL_ENRAGE                = 8599
};

/*######
## boss_keristrasza
######*/

struct MANGOS_DLL_DECL boss_keristraszaAI : public ScriptedAI
{
	float percent;

    boss_keristraszaAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		Tasks.AddEvent(SPELL_TAIL_SWEEP,7000,16000,2000,TARGET_ME);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_CRYSTAL_CHAINS_H,3000,10000,1000);
			Tasks.AddEvent(SPELL_CRYSTALFIRE_BREATH_H,5000,10000,2000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_CRYSTALLIZE,15000,14000,2000);
		}
		else
		{
			Tasks.AddEvent(SPELL_CRYSTAL_CHAINS,3000,10000,1000);
			Tasks.AddEvent(SPELL_CRYSTALFIRE_BREATH,5000,10000,2000,TARGET_MAIN);
		}
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
		DoCastMe(SPELL_INTENSE_COLD);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(CheckPercentLife(25) && !me->HasAura(8599,0))
			DoCastMe(SPELL_ENRAGE);

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_keristrasza(Creature* pCreature)
{
    return new boss_keristraszaAI(pCreature);
}

void AddSC_boss_keristrasza()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_keristrasza";
    newscript->GetAI = &GetAI_boss_keristrasza;
    newscript->RegisterSelf();
}
