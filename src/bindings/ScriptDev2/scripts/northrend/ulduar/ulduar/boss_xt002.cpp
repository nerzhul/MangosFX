/*
 * Copyright (C) 2008 - 2009 Trinity <http://www.trinitycore.org/>
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
#include "ulduar.h"

enum spells
{
	SPELL_SEARING_LIGHT_10		=	63018,
	SPELL_SEARING_LIGHT_25		=	65121,
	SPELL_GRAVITY_10			=	63024,
	SPELL_GRAVITY_25			=	64234,
	SPELL_WRATH					=	62776,
	SPELL_HEART_SURCHARGE		=	62789,
	SPELL_SLEEP					=	62777,

	SPELL_HEART_EXPOSED_HARD	=	63849,

	SPELL_VOID_ZONE_HARD_10		=	64203,
	SPELL_VOID_ZONE_HARD_25		=	64235,
	SPELL_LIFE_SPARK_HARD		=	64210,

	SPELL_XT002_ACT_HARD_25		=	64193,
	SPELL_XT002_ACT_HARD_10		=	65737,
};

enum mobs
{
	NPC_HEART_XT002				=	33329,
	NPC_XM024					=	33344,
	NPC_XE321					=	33346,
	NPC_XS013					=	33343,
};

struct Locations
{
	float x,y,z;
};

const Locations spawn_coords[] =
{
	{894.762f,62.515f,410.903f},
	{796.574f,53.496f,410.901f},
	{797.676f,-94.245f,411.2f},
	{890.87f,-88.966f,410.803f},
	{839.05f,-10.028f,410.901f},
};

struct MANGOS_DLL_DECL boss_xt002_AI : public ScriptedAI
{
    boss_xt002_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
		Reset();
    }

    ScriptedInstance* m_pInstance;
	float percent;
	uint8 nbexplode;

	uint32 check_Heart_Timer;
	uint32 wrath_Timer;
	Difficulty m_bIsHeroic;

	uint32 addspawn_Timer;

	bool HARDMODE;
	bool OpenHeart;

	Creature* Heart;
	uint16 Heart_Count;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		nbexplode = 0;
		check_Heart_Timer = 1500;
		percent = 100;
		wrath_Timer = 30000;
		Heart = NULL;
		HARDMODE = false;
		OpenHeart = false;
		addspawn_Timer = 1000;
		Heart_Count = 0;
		Tasks.FreezeMob(false,me);

		Tasks.AddEvent(44779,360000,60000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_LIFE_SPARK_HARD,12000,28000,0,TARGET_RANDOM,1);
		if(m_bIsHeroic == RAID_DIFFICULTY_25MAN_NORMAL)
		{
			Tasks.AddEvent(SPELL_SEARING_LIGHT_25,1000,50000);
			Tasks.AddEvent(SPELL_GRAVITY_25,2000,20000);
			Tasks.AddEvent(SPELL_VOID_ZONE_HARD_25,15000,25000,0,TARGET_RANDOM,1);
		}
		else
		{
			Tasks.AddEvent(SPELL_SEARING_LIGHT_10,1000,50000);
			Tasks.AddEvent(SPELL_GRAVITY_10,2000,20000);
			Tasks.AddEvent(SPELL_VOID_ZONE_HARD_25,15000,25000,0,TARGET_RANDOM,1);
		}
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* who)
    {
        Tasks.Speak(CHAT_TYPE_SAY,15724,"De nouveaux jouets ! Pour moi. Cette fois-ci je ne les casserai pas !");
    }

    void KilledUnit(Unit* victim)
    {
		if(urand(0,1))
			Tasks.Speak(CHAT_TYPE_SAY,15728,"Je crois que jl'ai cassÃ©...");
		else
			Tasks.Speak(CHAT_TYPE_SAY,15729,"Ca devrait pas se plier dans ce sens là...");
    }

    void JustDied(Unit *victim)
    {
        Tasks.Speak(CHAT_TYPE_SAY,15731,"Vous êtes des vilains jouets, bah oui, vilains...");

        if (m_pInstance)
            m_pInstance->SetData(TYPE_XT002, DONE);
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE);
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    }

    void MoveInLineOfSight(Unit* who) 
	{
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
	}

	void DoSpawnAdds()
	{
		uint16 _rand = rand() % 4;

		for(uint8 i=0;i<(m_bIsHeroic ? 3 : 2);i++)
		{
			_rand = rand() % 4;
			switch(urand(0,1))
			{
				case 0:
					Tasks.CallCreature(NPC_XM024,TEN_MINS,PREC_COORDS,GO_TO_CREATOR,spawn_coords[_rand].x,spawn_coords[_rand].y,spawn_coords[_rand].z);
					break;
				case 1:
					for(int i=0;i<2;i++)
						Tasks.CallCreature(NPC_XE321,TEN_MINS,PREC_COORDS,GO_TO_CREATOR,spawn_coords[_rand].x,spawn_coords[_rand].y,spawn_coords[_rand].z);
					for(int i=0;i<5;i++)
						Tasks.CallCreature(NPC_XS013,TEN_MINS,PREC_COORDS,GO_TO_CREATOR,spawn_coords[_rand].x,spawn_coords[_rand].y,spawn_coords[_rand].z);
					break;
				default:
					break;
			}
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		// explosion du coeur 
		percent = ((float)me->GetHealth() * 100/(float)me->GetMaxHealth());
		if(!HARDMODE && ((percent < 75 && nbexplode == 0) || (percent < 50 && nbexplode == 1) ||
			(percent < 25 && nbexplode == 2)))
		{
			nbexplode++;
			me->CastStop();
			DoCastMe(SPELL_SLEEP);
			Tasks.FreezeMob(true,me);
			Tasks.Speak(CHAT_TYPE_SAY,15725,"Ah, tellement fatiguÃ©, je vais me reposer une petite minute.");
			Heart = me->SummonCreature(NPC_HEART_XT002,me->GetPositionX() + 5.0f,me->GetPositionY() + 5.0f,me->GetPositionZ() + 1.0f,-me->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,30000);

			OpenHeart = true;
			
		}
		
		if(me->HasAura(SPELL_WRATH,0) || me->HasAura(SPELL_SLEEP))
			me->Relocate(spawn_coords[4].x,spawn_coords[4].y,spawn_coords[4].z);

		if(OpenHeart && !HARDMODE)
		{
			if(check_Heart_Timer <= diff)
			{
				if(Heart)
				{
					if(!Heart->isAlive())
					{
						if(m_bIsHeroic == RAID_DIFFICULTY_25MAN_NORMAL)
						{
							DoCastMe(SPELL_XT002_ACT_HARD_25);
							me->SetHealth(me->GetMaxHealth() * 1.5);
						}
						else
						{
							DoCastMe(SPELL_XT002_ACT_HARD_10);
							me->SetHealth(me->GetMaxHealth() * 1.6);
						}
						
						HARDMODE = true;
						OpenHeart = false;
						Heart = NULL;
						Tasks.FreezeMob(true,me);
						Heart_Count = 0;
					}
					else
					{
						if(addspawn_Timer <= diff)
						{
							DoSpawnAdds();
							addspawn_Timer = 16000;
						}
						else
							addspawn_Timer -= 1000;
					}

					Heart_Count++;
					if(Heart_Count == 29)
					{
						Heart = NULL;
						OpenHeart = false;	
						Tasks.FreezeMob(false,me);
						Tasks.Speak(CHAT_TYPE_SAY,15726,"Je suis prêt, à jouer !");
						Heart_Count = 0;
					}
				}
				else
					OpenHeart = false;

				check_Heart_Timer = 1000;
			}
			else
				check_Heart_Timer -= diff;
		}	

		if(HARDMODE && !OpenHeart)
			Tasks.UpdateEvent(diff,1);

		if(!OpenHeart)
		{
			if(wrath_Timer <= diff)
			{
				me->CastStop();
				DoCastVictim(SPELL_WRATH);
				me->SetVisibility(VISIBILITY_ON);
				wrath_Timer = 60000;
			}
			else
				wrath_Timer -= diff;
			
			Tasks.UpdateEvent(diff);

			DoMeleeAttackIfReady();
		}
		
    }
};

enum xm_spells
{
	SPELL_SMASH		=	8374,
	SPELL_TRAMPLE	=	5568,
	SPELL_UPPERCUT	=	10966,
};

struct MANGOS_DLL_DECL boss_xm024_AI : public ScriptedAI
{
    boss_xm024_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
    }
	
	uint32 uppercut_Timer;
	MobEventTasks Tasks;

	void Reset() {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_SMASH,1000,2000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_TRAMPLE,2000,4000,3000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_UPPERCUT,2500,3000,2000,TARGET_MAIN);
		uppercut_Timer = 2500;
	}
    ScriptedInstance* m_pInstance;

	void UpdateAI(const uint32 uiDiff)
	{
		if (!me->SelectHostileTarget() || !me->getVictim())
		{
			/*if (Unit* pUnit = Unit::GetUnit(*me, m_pInstance->GetData64(TYPE_XT002)))
				me->GetMotionMaster()->MovePoint(0,pUnit->GetPositionX(),pUnit->GetPositionY(),pUnit->GetPositionZ());*/
		}
		else
		{

			Tasks.UpdateEvent(uiDiff);
				
			DoMeleeAttackIfReady();
		}
	}
};

#define SPELL_BOOM		62834

struct MANGOS_DLL_DECL boss_xe321_AI : public ScriptedAI
{
    boss_xe321_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
    }

	void Reset() {}
    ScriptedInstance* m_pInstance;
	
	void UpdateAI(const uint32 uiDiff)
	{
		if (!me->SelectHostileTarget() || !me->getVictim())
			return;
		
		if(me->getVictim() && me->GetDistance2d(me->getVictim()->GetPositionX(),me->getVictim()->GetPositionY()) < 4.0f)
			DoCastVictim(SPELL_BOOM);
	}
};


struct MANGOS_DLL_DECL boss_xs013_AI : public ScriptedAI
{
    boss_xs013_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
    }

	void Reset()
	{}

    ScriptedInstance* m_pInstance;
	
	void UpdateAI(const uint32 uiDiff)
	{
		me->ClearInCombat();
		if(m_pInstance)
		{
			if (Unit* pUnit = Unit::GetUnit(*me, m_pInstance->GetData64(TYPE_XT002)))
			{
				//me->GetMotionMaster()->MovePoint(0,pUnit->GetPositionX(),pUnit->GetPositionY(),pUnit->GetPositionZ());
				if(me->GetDistance2d(pUnit->GetPositionX(),pUnit->GetPositionY()) < 3.0f)
				{
					pUnit->SetHealth(pUnit->GetHealth() + pUnit->GetMaxHealth() / 5);
					me->DealDamage(me,me->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				}	
			}
		}
	}
};

struct MANGOS_DLL_DECL xt_heart_AI : public ScriptedAI
{
    xt_heart_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
    }

	MobEventTasks Tasks;
	void Reset()
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(63849,500,120000,0,TARGET_ME);
		SetCombatMovement(false);
	}

    ScriptedInstance* m_pInstance;
	
	void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
	{
		if (Unit* pUnit = Unit::GetUnit(*me, m_pInstance->GetData64(TYPE_XT002)))
			if(pUnit->isAlive())
				pUnit->DealDamage(pUnit,uiDamage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

	void UpdateAI(const uint32 diff)
	{
		Tasks.UpdateEvent(diff);
	}
};

CreatureAI* GetAI_xt_heart(Creature* pCreature)
{
    return new xt_heart_AI (pCreature);
}

CreatureAI* GetAI_boss_xm024(Creature* pCreature)
{
    return new boss_xm024_AI (pCreature);
}

CreatureAI* GetAI_boss_xe321(Creature* pCreature)
{
    return new boss_xe321_AI (pCreature);
}

CreatureAI* GetAI_boss_xs013(Creature* pCreature)
{
    return new boss_xs013_AI (pCreature);
}

CreatureAI* GetAI_boss_xt002(Creature* pCreature)
{
    return new boss_xt002_AI (pCreature);
}

void AddSC_boss_xt002()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_xt002";
    newscript->GetAI = &GetAI_boss_xt002;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_xm024";
    newscript->GetAI = &GetAI_boss_xm024;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_xe321";
    newscript->GetAI = &GetAI_boss_xe321;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_xs013";
    newscript->GetAI = &GetAI_boss_xs013;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "xt_heart";
    newscript->GetAI = &GetAI_xt_heart;
    newscript->RegisterSelf();
}
