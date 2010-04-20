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
SDName: Boss_Colossus
SD%Complete: 20%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"

enum
{
	EMOTE_SURGE                 = -1604008,
	EMOTE_SEEP                  = -1604009,
	EMOTE_GLOW                  = -1604010,
	spell_coup					= 54719,
	spell_emerger				= 54850,
	spell_afflux				= 54801,
	spell_fusionement			= 54878,
	ADD							= 29573,
};

/*######
## boss_colossus
######*/

struct MANGOS_DLL_DECL boss_colossusAI : public ScriptedAI
{

	uint8 phase;
	Creature *add;


	boss_colossusAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		phase = 1 ;
		Tasks.FreezeMob(false,me);
		add = NULL;
		Tasks.AddEvent(spell_coup,1500,10000,0,TARGET_MAIN);
	}

	void phase2()
	{
		Tasks.FreezeMob(true,me);
		add = Tasks.CallCreature(ADD,TEN_MINS,ON_ME);
	}

	void JustDied(Unit* pWho)
	{
		DoScriptText(EMOTE_SURGE, me);
		if(add)
			Tasks.FreezeMob(false,add);
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!CanDoSomething())
			return;

		if (Tasks.CheckPercentLife(50) && phase == 1)
		{	 
			DoCastMe(spell_emerger);
			DoScriptText(EMOTE_SURGE, me);
			phase = 2 ;
			phase2();
		}

		if (add)
		{
			if (phase == 2 && add->isAlive() && (add->GetHealth() * 100 / add->GetMaxHealth()) <= 50 )
			{
				Tasks.FreezeMob(true,add);
				Tasks.FreezeMob(false,me);
				phase = 3;
			}
		}

		if (phase == 1 || phase == 3)
			DoMeleeAttackIfReady();
	}

};


struct MANGOS_DLL_DECL add_drakkari : public ScriptedAI // Add Drakkarie
{	
	void Reset()
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(spell_afflux,1500,10000,0,TARGET_MAIN);
	}

	add_drakkari(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();

	}

	MobEventTasks Tasks;

	void UpdateAI(const uint32 uiDiff) // Début Script add
	{		
		if (Tasks.GetPercentLife() < 50 && Tasks.GetPercentLife() > 40)
			me->SetHealth((me->GetMaxHealth()/2));
		else if (Tasks.CheckPercentLife(10)) 
			Tasks.Kill(me);

		DoMeleeAttackIfReady();

		Tasks.UpdateEvent(uiDiff);
	}
};

CreatureAI* GetAI_add_drakkari(Creature* pCreature)
{
	return new add_drakkari(pCreature);
} 

void AddSC_add_drakkari()
{
	Script *newscript;
	newscript = new Script;
	newscript->Name = "add_drakkari";
	newscript->GetAI = &GetAI_add_drakkari;
	newscript->RegisterSelf(); 
}


CreatureAI* GetAI_boss_colossus(Creature* pCreature)
{
	return new boss_colossusAI(pCreature);
}

void AddSC_boss_colossus()
{
	Script *newscript;
	newscript = new Script;
	newscript->Name = "boss_colossus";
	newscript->GetAI = &GetAI_boss_colossus;
	newscript->RegisterSelf();
}
