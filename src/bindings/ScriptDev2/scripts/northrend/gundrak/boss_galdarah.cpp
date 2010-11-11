
/* ScriptData
SDName: Boss_Galdarah
SD%Complete: 80%
SDComment: Bug skill rhino, skill tourbi l'empéche de retaper en mélée aprés.
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"

enum
{
	SAY_AGGRO                       = -1604019,
	SAY_TRANSFORM_1                 = -1604020,
	SAY_TRANSFORM_2                 = -1604021,
	SAY_SUMMON_1                    = -1604022,
	SAY_SUMMON_2                    = -1604023,
	SAY_SUMMON_3                    = -1604024,
	SAY_SLAY_1                      = -1604025,
	SAY_SLAY_2                      = -1604026,
	SAY_SLAY_3                      = -1604027,
	SAY_DEATH                       = -1604028,
	spell_rhino						= 55218,
	spell_tourbi					= 55250,
	spell_enrage_normal				= 55285,
	spell_enrage_hero				= 59828,
	spell_percer				= 55276,
	spell_percer_hero				= 59826,
	spell_pietinement		= 59829,
	spell_pietinement_hero			= 55292,
	spell_empaler			= 54956,
	spell_empaler_hero				= 59827,
	morph							= 26265,
	init							= 27061,
};

/*######
## boss_galdarah
######*/

struct MANGOS_DLL_DECL boss_galdarahAI : public ScriptedAI
{

	uint32 spell_rhino_timer;
	uint32 spell_tourbi_timer;
	uint32 spell_enrage_timer;
	uint32 morph_timer;
	uint8 phase_morph;
	uint8 phase_end;
	Unit* target;


	boss_galdarahAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
	}

	bool m_bIsHeroic;
	MobEventTasks Tasks;

	void Reset()
	{
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(spell_percer_hero,29000,25000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_empaler_hero,50000,15000,0,TARGET_MAIN,1);
			Tasks.AddEvent(spell_pietinement_hero,48000,20000,0,TARGET_MAIN,1);
		}
		else
		{
			Tasks.AddEvent(spell_percer,29000,25000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_empaler,50000,15000,0,TARGET_MAIN,1);
			Tasks.AddEvent(spell_pietinement,48000,20000,0,TARGET_MAIN,1);
		}
		spell_rhino_timer = 7000;
		spell_tourbi_timer = 15000;
		spell_enrage_timer = 45000;
		morph_timer = 40000;
		phase_morph = 0;
		phase_end = 0;
		me->SetDisplayId(me->GetNativeDisplayId());
	}

	void Aggro(Unit* pWho)
	{
		DoScriptText(SAY_AGGRO, me);
	}

	void KilledUnit(Unit* pVictim)
	{
		switch(urand(0,2))
		{
		case 0: DoScriptText(SAY_SLAY_1, me); break;
		case 1: DoScriptText(SAY_SLAY_2, me); break;
		case 2: DoScriptText(SAY_SLAY_3, me); break;
		}
	}

	void JustDied(Unit* pKiller)
	{
		DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
		GiveRandomReward();
	}

	void UpdateAI(const uint32 diff)
	{
		if (!CanDoSomething())
			return;

		if (spell_tourbi_timer < diff && phase_morph == 0)
		{
			DoCastVictim(spell_tourbi);
			spell_tourbi_timer = 5000000; // Valeur Fausse mais elle est reset en fin de P2
		}
		else
			spell_tourbi_timer -= diff;

		// Script Phase 2

		if ((morph_timer <= diff) )
		{			
			DoScriptText(SAY_TRANSFORM_1, me);
			me->SetDisplayId(morph);
			phase_morph = 1;
			phase_end = 0;
			morph_timer = 40000;
		}
		else
			morph_timer -= diff;

		if (phase_morph == 1)
		{
			if (morph_timer <= diff)
			{			
				DoScriptText(SAY_TRANSFORM_2, me);
				me->SetDisplayId(me->GetNativeDisplayId());
				phase_morph = 0;
				morph_timer = 40000;
				phase_end = 1;
			}

			if (spell_enrage_timer <= diff)
			{
				DoCastMe(m_bIsHeroic ? spell_enrage_hero : spell_enrage_normal);
				spell_enrage_timer = morph_timer - 8000;
			}
			else
				spell_enrage_timer -= diff;

		}

		if (phase_morph == 0 && phase_end == 1)
		{
			spell_enrage_timer = 45000;
			phase_end = 0;
		}

		if (phase_morph == 1 && phase_end == 0)
			spell_tourbi_timer = 15000;

		Tasks.UpdateEvent(diff);
		Tasks.UpdateEvent(diff,phase_morph);
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_galdarah(Creature* pCreature)
{
	return new boss_galdarahAI(pCreature);
}

void AddSC_boss_galdarah()
{
	Script *newscript;

	newscript = new Script;
	newscript->Name = "boss_galdarah";
	newscript->GetAI = &GetAI_boss_galdarah;
	newscript->RegisterSelf();
}
