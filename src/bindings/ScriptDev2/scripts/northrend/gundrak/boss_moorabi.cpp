/* ScriptData
SDName: Boss_Moorabi
SD%Complete: 20%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                   = -1604011,
    SAY_QUAKE                   = -1604012,
    SAY_TRANSFORM               = -1604013,
    SAY_SLAY_1                  = -1604014,
    SAY_SLAY_2                  = -1604015,
    SAY_SLAY_3                  = -1604016,
    SAY_DEATH                   = -1604017,
    EMOTE_TRANSFORM             = -1604018,
	spell_corne_normal			= 55102,
	spell_corne_hero			= 59444,
	spell_poignard				= 55104,
	spell_paralisant_normal		= 55106,
	aura_frenesie				= 55163,
	spell_paralisant_hero		= 55100,
	spell_transformation		= 55098,
	spell_tremblement			= 55142,

    SOUND_ID_TRANSFORMED        = 14724,
};

/*######
## boss_moorabi
######*/

struct MANGOS_DLL_DECL boss_moorabiAI : public ScriptedAI
{
	uint32 spell_transformation_timer ;
	uint8 transfo;

    boss_moorabiAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		Tasks.AddEvent(aura_frenesie,1000,180000,0,TARGET_ME);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(spell_corne_hero,18000,5000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_paralisant_hero,7000,15000,0,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(spell_corne_normal,18000,5000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_paralisant_normal,7000,15000,0,TARGET_MAIN);
		}
		Tasks.AddEvent(spell_tremblement,50000,20000,0,TARGET_MAIN);
		transfo						= 0 ;
		spell_transformation_timer	= 10000 ;
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
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!CanDoSomething())
            return;

		if (transfo != 1 && me->HasAura(spell_transformation, 0))
			transfo = 1;
		
		if (spell_transformation_timer < uiDiff && transfo == 0)
		{		
			DoScriptText(SAY_TRANSFORM, me);
			DoScriptText(EMOTE_TRANSFORM, me);
			spell_transformation_timer = 10000 ;
			DoCastMe(spell_transformation);
		}
		else
			spell_transformation_timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_moorabi(Creature* pCreature)
{
    return new boss_moorabiAI(pCreature);
}

void AddSC_boss_moorabi()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_moorabi";
    newscript->GetAI = &GetAI_boss_moorabi;
    newscript->RegisterSelf();
}
