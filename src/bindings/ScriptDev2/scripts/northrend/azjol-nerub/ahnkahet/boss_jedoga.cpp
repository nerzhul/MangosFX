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
SDName: Boss_Jedoga
SD%Complete: 20%
SDComment:
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                           = -1619017,
    SAY_CALL_SACRIFICE_1                = -1619018,
    SAY_CALL_SACRIFICE_2                = -1619019,
    SAY_SACRIFICE_1                     = -1619020,
    SAY_SACRIFICE_2                     = -1619021,
    SAY_SLAY_1                          = -1619022,
    SAY_SLAY_2                          = -1619023,
    SAY_SLAY_3                          = -1619024,
    SAY_DEATH                           = -1619025,
    SAY_PREACHING_1                     = -1619026,
    SAY_PREACHING_2                     = -1619027,
    SAY_PREACHING_3                     = -1619028,
    SAY_PREACHING_4                     = -1619029,
    SAY_PREACHING_5                     = -1619030,

    SAY_VOLUNTEER_1                     = -1619031,         //said by the volunteer image
    SAY_VOLUNTEER_2                     = -1619032,
	id_add								= 30385,
	id_rune								= 181600,
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations add_position[]=
{
	{355.619, -722.752, -16.1815},
    {353.475, -719.255, -16.1815},
    {359.545, -726.266, -16.2},
    {364.964, -727.793, -16.1794},
	{370.439, -727.648, -16.1794},
	{375.411, -725.74, -16.1794},
	{379.78, -721.884, -16.1794},
	{382.989, -717.354, -16.1794},
	{385.859, -712.888, -16.1794},
	{388.51, -709.105, -16.1794},
	{391.23, -705.225, -16.1794},
	{393.95, -701.344, -16.1794},
	{394.855, -696.682, -16.1794},
	{392.013, -691.481, -16.1794},
	{387.928, -687.193, -16.1794},
	{383.729, -683.682, -16.1794},

};

#define X			 368.383
#define Y            -704.09
#define Z            -16.179

/*######
## boss_jedoga
######*/

struct MANGOS_DLL_DECL boss_jedogaAI : public ScriptedAI
{
	Creature* add_tab[16];
	uint8 i;
	Creature* add_choice;
	uint32 phase_2_timer;
	uint32 phase;
	MobEventTasks Tasks;

    boss_jedogaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		for(uint8 i = 0; i < 16; ++i)
			add_tab[i] = NULL;
		phase_2_timer = 25000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
		Decor();
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

	void Decor()
	{
		Creature* add = NULL;
		for(uint8 i = 0; i < 16; ++i)
		{
			add = me->SummonCreature(id_add, add_position[i].x ,add_position[i].y, add_position[i].z, 2.57, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
			if(add)
			{
				add->setFaction(35);
				add_tab[i] = add;
			}
		}
	}

	void phase_2()
	{
		
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->RemoveAllAttackers();
		me->DeleteThreatList();
		me->SendMonsterMove(371.21, -701.72, -5.85, SPLINETYPE_NORMAL, SPLINEFLAG_NO_SPLINE, 0); //m_creaure bouge quand méme
		DoCastMe( 66830);
	
		if(add_choice = add_tab[rand() % 16])
		{
			if(add_choice->isAlive())
			{
				add_choice->setFaction(14);
				add_choice->StopMoving();
				add_choice->GetMotionMaster()->Clear();
				add_choice->GetMotionMaster()->MoveIdle();
				add_choice->SendMonsterMove(me->GetPositionX(), me->GetPositionY(), Z,SPLINETYPE_NORMAL, SPLINEFLAG_NONE, 15000); //mob ne suis pas cette direction si il a une aggro, il se stop car je l'ai bloqué dans son script.

				switch(urand(0, 1))
				{
					case 0: DoScriptText(SAY_SACRIFICE_1, add_choice); break;
					case 1: DoScriptText(SAY_SACRIFICE_2, add_choice); break;
		           
				}
				
				switch(urand(0, 1))
				{
					case 0: DoScriptText(SAY_CALL_SACRIFICE_1 , me); break;
					case 1: DoScriptText(SAY_CALL_SACRIFICE_2 , me); break;
				}
			}
		}
	}


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        DoMeleeAttackIfReady();
		if (phase_2_timer < diff )
		{
			phase_2();
			phase_2_timer = 50000;
			phase = 2;
		
		}
		else
			phase_2_timer -= diff;

		if(phase == 2)
		{
			me->AttackStop(true);
			
			if(add_choice)
				if ((add_choice->GetPositionX() == me->GetPositionX()) && (add_choice->GetPositionY() == me->GetPositionY())) //marche pas
					Kill(add_choice);
		}

		Tasks.UpdateEvent(diff);
	}
		
		
};


struct MANGOS_DLL_DECL add_jedoga : public ScriptedAI
{
   add_jedoga(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
    }
	 
		
	void UpdateAI(const uint32 diff) // Début Script add
	{	
		SetCombatMovement(false);
		me->DeleteThreatList();
	}
};


CreatureAI* GetAI_boss_jedoga(Creature* pCreature)
{
    return new boss_jedogaAI(pCreature);
}

CreatureAI* GetAI_add_jedoga(Creature* pCreature)
{
    return new add_jedoga(pCreature);
} 
void AddSC_boss_jedoga()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_jedoga";
    newscript->GetAI = &GetAI_boss_jedoga;
    newscript->RegisterSelf();
}
void AddSC_add_jedoga()
{
	Script *newscript;

	newscript = new Script;
    newscript->Name = "add_jedoga";
    newscript->GetAI = &GetAI_add_jedoga;
    newscript->RegisterSelf(); 
}
