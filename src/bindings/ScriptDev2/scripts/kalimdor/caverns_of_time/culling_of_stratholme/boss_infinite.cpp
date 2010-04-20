/*
* Copyright (C) 2008-2009 Trinity <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "precompiled.h"
#include "def_culling_of_stratholme.h"

enum Spells
{
    SPELL_CORRUPTING_BLIGHT           = 60588,
    SPELL_VOID_STRIKE                 = 60590
};

struct MANGOS_DLL_DECL boss_infiniteAI : public ScriptedAI
{
	uint32 blight_Timer;
	uint32 strike_Timer;
	bool m_bIsHeroic;
	MobEventTasks Tasks;

    boss_infiniteAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
    }
    
    ScriptedInstance* pInstance;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		strike_Timer = 8000;
		blight_Timer = 6000;
        /*if (pInstance)
            pInstance->SetData(DATA_INFINITE_EVENT, NOT_STARTED);*/
    }
    
    void EnterCombat(Unit* who)
    {
		/*if (pInstance)
            pInstance->SetData(DATA_INFINITE_EVENT, IN_PROGRESS);*/
    }

    void AttackStart(Unit* who) {}
    void MoveInLineOfSight(Unit* who) {}
    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

		if(strike_Timer <= uiDiff)
		{
			DoCast(SelectUnit(SELECT_TARGET_TOPAGGRO,0),SPELL_VOID_STRIKE);
			strike_Timer = urand(6500,10000);
		}
		else
			strike_Timer -= uiDiff;

		if(blight_Timer <= uiDiff)
		{
			DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_CORRUPTING_BLIGHT);
			blight_Timer = urand(5000,7000);
		}
		else
			blight_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
    
    void JustDied(Unit* killer)
    {
        /*if (pInstance)
            pInstance->SetData(DATA_INFINITE_EVENT, DONE);*/
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
};

CreatureAI* GetAI_boss_infinite(Creature* pCreature)
{
    return new boss_infiniteAI (pCreature);
}

void AddSC_infinite_epoch()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_infinite";
    newscript->GetAI = &GetAI_boss_infinite;
    newscript->RegisterSelf();
}