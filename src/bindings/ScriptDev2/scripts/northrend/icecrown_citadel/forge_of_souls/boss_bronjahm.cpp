/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{
	SPELL_CORRUPT_SOUL			= 68839,
	SPELL_FEAR					= 68950,
	SPELL_MAGIC_BANE			= 68793,
	SPELL_MAGIC_BANE_H			= 69050,
	SPELL_SHADOW_BOLT			= 70043,
	SPELL_SOULSTORM				= 68872,
	SPELL_TELEPORT				= 68988,
	SPELL_CONSUME				= 68858,
};

struct MANGOS_DLL_DECL boss_bronjahmAI : public ScriptedAI
{
    boss_bronjahmAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	bool HasTeleported;
	uint32 CorruptedSoulFrag_Timer;
	Creature* frag;
	uint32 CheckDist_Timer;
	uint32 Teleport_Timer;
	uint8 subphase;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_CORRUPT_SOUL,25000,30000,2000);
		Tasks.AddEvent(SPELL_FEAR,10000,12000,1000);
		Tasks.AddEvent((m_bIsHeroic ? SPELL_MAGIC_BANE_H : SPELL_MAGIC_BANE),5000,7000,5000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_SHADOW_BOLT,3000,3000,3000,TARGET_MAIN);
		CorruptedSoulFrag_Timer = 29000;
		CheckDist_Timer = 20000;
		frag = NULL;
		Teleport_Timer = 1500;
		subphase = 0;
    }


    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;
	
		if(CorruptedSoulFrag_Timer <= uiDiff)
		{
			if(Creature* tmp_add = me->SummonCreature(36535,me->GetPositionX() + urand(3,6),
				me->GetPositionY() + urand(3,6), me->GetPositionZ() + 0.5f,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
			{
				tmp_add->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),
					me->GetPositionZ());

				frag = tmp_add;

			}
			CorruptedSoulFrag_Timer = 32000;
		}
		else
			CorruptedSoulFrag_Timer -= uiDiff;

		if(CheckDist_Timer <= uiDiff)
		{	
			if(frag)
				if(frag->isAlive())
					if(frag->GetDistance2d(me) < 1.0f)
					{
						me->CastStop();
						DoCastMe(SPELL_CONSUME);
						frag->ForcedDespawn(500);
					}

			CheckDist_Timer = 1000;
		}
		else
			CheckDist_Timer -= uiDiff;

		if(!HasTeleported && me->GetHealth() * 100 / me->GetMaxHealth() < 30)
		{
			if(Teleport_Timer <= uiDiff)
			{
				if(subphase == 0)
				{
					DoCastMe(SPELL_TELEPORT);
					Teleport_Timer = 2000;
					subphase++;
				}
				else if(subphase == 1)
				{
					Tasks.Relocate(5297.3f,2506.6f,686.1f);
					DoCastMe(SPELL_SOULSTORM);
					Teleport_Timer = DAY;
				}
			}
			else
				Teleport_Timer -= uiDiff;
		}
       
		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
};

CreatureAI* GetAI_boss_bronjahm(Creature* pCreature)
{
    return new boss_bronjahmAI (pCreature);
}

void AddSC_boss_bronjahm()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_bronjahm";
    newscript->GetAI = &GetAI_boss_bronjahm;
    newscript->RegisterSelf();

}
