/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "azjol-nerub.h"

enum
{
    SAY_INTRO                       = -1601014,
    SAY_AGGRO                       = -1601015,
    SAY_KILL_1                      = -1601016,
    SAY_KILL_2                      = -1601017,
    SAY_KILL_3                      = -1601018,
    SAY_SUBMERGE_1                  = -1601019,
    SAY_SUBMERGE_2                  = -1601020,
    SAY_LOCUST_1                    = -1601021,
    SAY_LOCUST_2                    = -1601022,
    SAY_LOCUST_3                    = -1601023,
    SAY_DEATH                       = -1601024,
	SPELL_EMPALER_N					= 53454,
	SPELL_EMPALER_H					= 59446,
	SPELL_ESSAIM_N					= 53467,
	SPELL_ESSAIM_H					= 59430,
	SPELL_MARTELER_N				= 53472,
	SPELL_MARTELER_H				= 59433,
	SPELL_NECROPHORES				= 53520,
	SPELL_SUBMERGE					= 65981,
	SPELL_LOCUST_N					= 54022,
	SPELL_LOCUST_H					= 28786,
};

/*######
## boss_anubarak
######*/

struct MANGOS_DLL_DECL boss_anubarakAI : public ScriptedAI
{
	Unit* target;
	uint32 Empaler_Timer;
	uint32 Marteler_Timer;
	uint32 Necrophores_Timer;
	uint32 Submerge_Timer;

    boss_anubarakAI(Creature* pCreature) : ScriptedAI(pCreature)
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
			Tasks.AddEvent(SPELL_LOCUST_H,10000,10000,3000);
			Tasks.AddEvent(SPELL_ESSAIM_H,13000,10000,3000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_LOCUST_N,13000,10000,3000);
			Tasks.AddEvent(SPELL_ESSAIM_N,10000,10000,3000,TARGET_MAIN);
		}
		Marteler_Timer = 15000;
		Necrophores_Timer = 3000;
		Empaler_Timer = 17000;
		Submerge_Timer = 20500;
		me->RemoveAllAuras();
		me->SetVisibility(VISIBILITY_ON);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
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

		if(me->HasAura(SPELL_SUBMERGE,0))
		{
			if(Necrophores_Timer <= uiDiff)
			{
				DoCastMe(SPELL_NECROPHORES);
				Necrophores_Timer = -2500;
			}
			else
				Necrophores_Timer -= uiDiff;

			if(Empaler_Timer <= uiDiff)
			{
				me->CastStop();
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
				DoCastRandom(m_bIsHeroic ? SPELL_EMPALER_H : SPELL_EMPALER_N);
				
				Empaler_Timer = -2500;
			}
			else
				Empaler_Timer -= uiDiff;
		}
		else
		{
			if(Marteler_Timer <= uiDiff)
			{
				DoCastVictim(m_bIsHeroic ? SPELL_MARTELER_H : SPELL_MARTELER_N);
				Marteler_Timer = urand(25000,30000);
			}
			else
				Marteler_Timer -= uiDiff;

			if(Submerge_Timer <= uiDiff)
			{
				me->CastStop();
				DoCastMe(SPELL_SUBMERGE);

				switch(urand(0, 1))
				{
					case 0: DoScriptText(SAY_SUBMERGE_1, me); break;
					case 1: DoScriptText(SAY_SUBMERGE_2, me); break;
				}

				Submerge_Timer = Marteler_Timer + 3500;
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				Tasks.CallCreature(29217,TEN_MINS,NEAR_15M);
				for(short i=0;i<8;i++)
					Tasks.CallCreature(29209,TEN_MINS,NEAR_15M);
					
				Empaler_Timer = 15000;
			}
			else
				Submerge_Timer -= uiDiff;

			Tasks.UpdateEvent(uiDiff);

			DoMeleeAttackIfReady();
		}
	}
};

CreatureAI* GetAI_boss_anubarak(Creature* pCreature)
{
    return new boss_anubarakAI(pCreature);
}

void AddSC_boss_anubarak()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_anubarak";
    newscript->GetAI = &GetAI_boss_anubarak;
    newscript->RegisterSelf();
}
