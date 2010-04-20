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
SDName: Boss_Nadox
SD%Complete: 20%
SDComment:
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                       = -1619000,
    SAY_SUMMON_EGG_1                = -1619001,
    SAY_SUMMON_EGG_2                = -1619002,
    SAY_SLAY_1                      = -1619003,
    SAY_SLAY_2                      = -1619004,
    SAY_SLAY_3                      = -1619005,
    SAY_DEATH                       = -1619006,
    EMOTE_HATCH                     = -1619007,
	id_add1_normal					= 30178,
	id_add2_normal					= 30176, // Gardien
	id_add1_hero					= 31447,
	id_add2_hero					= 31441,
	SPELL_BERSERK					= 26662,
	SPELL_RAGE_H					= 59465,
	SPELL_PESTE_N					= 56130,
	SPELL_PESTE_H						= 59467,
	SPELL_INSENSIBLE				= 40733, // Non Défini Aura de Gardien normalement.
};
struct Locations
{
    float x, y, z;
    uint32 id;
};
static Locations add_position[]=
{
	{665.35, -905.06, 26.10},
    {655.89, -930, 25.79},
    {617.60, -923 , 25.86},
    {619.36, -957.56, 25.99}
};


/*######
## boss_nadox
######*/

struct MANGOS_DLL_DECL boss_nadoxAI : public ScriptedAI
{
	uint32 invoc_add2_timer;
	uint32 invoc_add_timer;
	Creature* Add2,*Add;
	Unit* target;
	
	boss_nadoxAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_BERSERK,300000,60000,0,TARGET_ME);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_PESTE_H,15000,32000,3000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_PESTE_N,15000,32000,3000,TARGET_MAIN);
		}
		invoc_add2_timer = 45000;
		invoc_add_timer = 5000;
    }

	void DamageTaken(Unit *done_by, uint32 &damage)
	{
		if(me->HasAura(SPELL_INSENSIBLE))
		{
			damage = 0;
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

	void invoc_add()
	{	
		uint16 i = urand(0,4);
		Tasks.CallCreature(id_add1_normal,8000,PREC_COORDS,AGGRESSIVE_RANDOM,add_position[i].x ,add_position[i].y, add_position[i].z);
	}

	void invoc_add_2()
	{
		uint16 i = urand(0,4);
		Tasks.CallCreature(id_add2_normal,8000,PREC_COORDS,AGGRESSIVE_RANDOM,add_position[i].x ,add_position[i].y, add_position[i].z);
	}

	void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

		if (invoc_add_timer <= uiDiff)
		{	
			DoScriptText(SAY_SUMMON_EGG_1 , me);
			invoc_add();
			invoc_add_timer = urand(5000,6000);
		}
		else
			invoc_add_timer -= uiDiff;

		if (invoc_add2_timer <= uiDiff)
		{	
			DoScriptText(SAY_SUMMON_EGG_2 , me);
			invoc_add_2();
			invoc_add2_timer = urand(44000,46000);
		}
		else
			invoc_add2_timer -= uiDiff;
	

		Tasks.UpdateEvent(uiDiff);

		if(Add2 != NULL && Add2->isAlive() && !me->HasAura(SPELL_INSENSIBLE))
			DoCastMe(SPELL_INSENSIBLE, true);

        DoMeleeAttackIfReady();
    }
};


struct MANGOS_DLL_DECL add_nadox : public ScriptedAI // Add 
{	
	bool m_bIsHeroic;
	MobEventTasks Tasks;
	
	void Reset()
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_RAGE_H,5000,5000,0,TARGET_ME);
    }
    	 
	add_nadox(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        
    }
    
	ScriptedInstance* m_pInstance;
		
	void UpdateAI(const uint32 uiDiff) // Début Script add

	{
		if (CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);
		
		DoMeleeAttackIfReady();

	}

};

CreatureAI* GetAI_boss_nadox(Creature* pCreature)
{
    return new boss_nadoxAI(pCreature);
}

CreatureAI* GetAI_add_nadox(Creature* pCreature)
{
    return new add_nadox(pCreature);
} 



void AddSC_boss_nadox()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_nadox";
    newscript->GetAI = &GetAI_boss_nadox;
    newscript->RegisterSelf();
}
void AddSC_add_nadox()
{
	Script *newscript;

    newscript = new Script;
    newscript->Name = "add_nadox";
    newscript->GetAI = &GetAI_add_nadox;
    newscript->RegisterSelf(); 
}
