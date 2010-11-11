/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "nexus.h"

enum
{
    SAY_AGGRO                   = -1576011,
    SAY_KILL                    = -1576012,
    SAY_REFLECT                 = -1576013,
    SAY_ICESPIKE                = -1576014,
    SAY_DEATH                   = -1576015,

    SPELL_REFLECTION            = 47981,

    SPELL_CRYSTAL_SPIKES        = 47958,
    SPELL_CRYSTAL_SPIKES_H1     = 57082,
    SPELL_CRYSTAL_SPIKES_H2     = 57083,

    SPELL_FRENZY                = 48017,
    SPELL_FRENZY_H              = 57086,

    SPELL_TRAMPLE               = 48016,
    SPELL_TRAMPLE_H             = 57066,

    SPELL_SUMMON_TANGLER_H      = 61564
};

/*######
## boss_ormorok
######*/

struct MANGOS_DLL_DECL boss_ormorokAI : public LibDevFSAI
{
    boss_ormorokAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(SPELL_REFLECTION,7000,12000,6000);
		if(m_difficulty)
		{
			AddEventOnMe(SPELL_TRAMPLE_H,10000,9000,2000);
			AddEventOnMe(SPELL_SUMMON_TANGLER_H,15000,12000,3000);
		}
		else
			AddEventOnMe(SPELL_TRAMPLE,10000,9000,2000);
    }

	uint32 crystalspikes_Timer;

    void Reset()
    {
		ResetTimers();
		SetInstanceData(TYPE_ORMOROK,NOT_STARTED);
		crystalspikes_Timer = 10000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
		SetInstanceData(TYPE_ORMOROK,IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		SetInstanceData(TYPE_ORMOROK,DONE);
		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
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

		if(CheckPercentLife(20))
		{
			if(!m_difficulty && !me->HasAura(SPELL_FRENZY))
				DoCastMe(SPELL_FRENZY);
			else if(m_difficulty && !me->HasAura(SPELL_FRENZY_H))
				DoCastMe(SPELL_FRENZY_H);
		}

		if(crystalspikes_Timer <= diff)
		{
			if(m_difficulty)
				DoCastMe(SPELL_CRYSTAL_SPIKES);
			else
			{
				if(urand(0,1))
					DoCastMe(SPELL_CRYSTAL_SPIKES_H1);
				else
					DoCastMe(SPELL_CRYSTAL_SPIKES_H2);
			}

		}

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ormorok(Creature* pCreature)
{
    return new boss_ormorokAI(pCreature);
}

void AddSC_boss_ormorok()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ormorok";
    newscript->GetAI = &GetAI_boss_ormorok;
    newscript->RegisterSelf();
}
