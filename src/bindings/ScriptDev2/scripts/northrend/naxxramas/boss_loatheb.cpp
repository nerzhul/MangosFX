/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Loatheb
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"

#define SPELL_POISON_AURA           29865
#define SPELL_POISON_AURA_H         55053
#define SPELL_INEVITABLE_DOOM       29204
#define SPELL_INEVITABLE_DOOM_H     55052
#define SPELL_REMOVE_CURSE          30281
#define SPELL_NECROTIC_AURA         55593

#define ADD_1X 2957.040
#define ADD_1Y -3997.590
#define ADD_1Z 274.280

#define ADD_2X 2909.130
#define ADD_2Y -4042.970
#define ADD_2Z 274.280

#define ADD_3X 2861.102
#define ADD_3Y -3997.901
#define ADD_3Z 274.280

struct MANGOS_DLL_DECL boss_loathebAI : public ScriptedAI
{
    boss_loathebAI(Creature* pCreature) : ScriptedAI(pCreature) {
		Reset();
		m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
	}

	bool m_bIsHeroic;
    uint32 Corrupturse_Timer;
    uint32 Summon_Timer;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_NECROTIC_AURA,5000,20000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_REMOVE_CURSE,30000,30000,0,TARGET_ME);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_POISON_AURA_H,2500,60000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_INEVITABLE_DOOM_H,120000,120000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_INEVITABLE_DOOM_H,300000,15000,0,TARGET_MAIN); // celle des 5 min
		}
		else
		{
			Tasks.AddEvent(SPELL_POISON_AURA,2500,60000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_INEVITABLE_DOOM,120000,120000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_INEVITABLE_DOOM,300000,15000,0,TARGET_MAIN);
		}
        Summon_Timer = 8000;
    }

	void JustDied(Unit* victim)
	{
		GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		Tasks.UpdateEvent(diff);

        //Summon_Timer
        if (Summon_Timer < diff)
        {
            Unit* target = NULL;
            Unit* SummonedSpores = NULL;

            SummonedSpores = me->SummonCreature(16286,ADD_1X,ADD_1Y,ADD_1Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
            SummonedSpores = me->SummonCreature(16286,ADD_2X,ADD_2Y,ADD_2Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
            SummonedSpores = me->SummonCreature(16286,ADD_3X,ADD_3Y,ADD_3Z,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,80000);
            if (SummonedSpores)
            {
                target = SelectUnit(SELECT_TARGET_RANDOM,0);
                if (target)
                    SummonedSpores->AddThreat(target,1.0f);
            }

            Summon_Timer = 28000;
        } 
		else 
			Summon_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_loatheb(Creature* pCreature)
{
    return new boss_loathebAI(pCreature);
}

void AddSC_boss_loatheb()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_loatheb";
    newscript->GetAI = &GetAI_boss_loatheb;
    newscript->RegisterSelf();
}
