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

enum ignis_spells
{
	SPELL_FLAME_JETS        =   62680,
	SPELL_FLAME_JETS_H		=	63472,
	SPELL_SCORCH            =   62546,
	SPELL_SCORCH_H			=	63473,
	SPELL_SLAG_POT          =   62717,
	SPELL_SLAG_POT_H		=	63477,
	SPELL_CREATOR_STRENGH	=	64473,
};

enum add_spec_spells
{
	SPELL_FUSION		=	62373,
	SPELL_CHALEUR		=	62343,
	SPELL_ACTIVATE		=	62488,
	SPELL_FRAGILE_10	=	62382,
	SPELL_FRAGILE_25	=	67114,
	SPELL_CONE_10		=	62548,
	SPELL_CONE_25		=	63476,
};

#define NPC_FIRE		33221


const static float Ignis_Coords[2][2] =
{
	{661.214f,	276.659f},
	{517.598f,	276.068f},
};

struct MANGOS_DLL_DECL boss_ignis_AI : public ScriptedAI
{
    boss_ignis_AI(Creature *pCreature) : ScriptedAI(pCreature) {
		m_bIsHeroic = me->GetMap()->GetDifficulty();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}

	ScriptedInstance* m_pInstance;
	uint32 Assemblage_Timer;
	uint32 Fire_Timer;
	Difficulty m_bIsHeroic;
	std::vector<Unit*> IgnisAdds;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		for (std::vector<Unit*>::iterator itr = IgnisAdds.begin(); itr != IgnisAdds.end();++itr)
        {
			if((*itr))
			{
				((Creature*)(*itr))->Respawn();
				Tasks.FreezeMob(true,((Creature*)(*itr)));
			}
		}
		IgnisAdds.clear();

		if(m_bIsHeroic == RAID_DIFFICULTY_25MAN_NORMAL)
		{
			Tasks.AddEvent(SPELL_FLAME_JETS_H,32000,25000,0,TARGET_MAIN,0);
			Tasks.AddEvent(SPELL_SCORCH_H,10000,25000,0,TARGET_MAIN,0);
			Tasks.AddEvent(SPELL_SLAG_POT_H,100,30000,0,TARGET_RANDOM,0);
			Assemblage_Timer = 30000;
		}
		else
		{
			Tasks.AddEvent(SPELL_FLAME_JETS,32000,25000,0,TARGET_MAIN,0);
			Tasks.AddEvent(SPELL_SCORCH,10000,25000,0,TARGET_MAIN,0);
			Tasks.AddEvent(SPELL_SLAG_POT,5000,30000,0,TARGET_RANDOM,0);
			Assemblage_Timer = 40000;
		}
		Fire_Timer = 10000;
    }

	

    void EnterCombat(Unit* who)
    {
        Tasks.Speak(CHAT_TYPE_YELL,15564,"Jeunes insolents ! Les lames qui serviront � reconquérir ce monde seront trempées dans votre sang !");
    }
    void KilledUnit(Unit* victim)
    {
        if(urand(0,1))
			Tasks.Speak(CHAT_TYPE_YELL,15569,"Encore des déchets pour la décharge !");
		else
			Tasks.Speak(CHAT_TYPE_YELL,15570,"Vos ossements serviront de petit bois !");
    }

    void JustDied(Unit *victim)
    {
        Tasks.Speak(CHAT_TYPE_YELL,15572,"J'ai... échoué...");
		for (std::vector<Unit*>::iterator itr = IgnisAdds.begin(); itr != IgnisAdds.end();++itr)
        {
			if((*itr))
			{
				Tasks.Kill((*itr));
			}
		}
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE);
    }

    void MoveInLineOfSight(Unit* who) 
	{
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		me->RemoveAurasDueToSpell(SPELL_CHALEUR);
		if(Assemblage_Timer <= diff)
		{
			if(Unit* tmpcr = Unit::GetUnit(*me, m_pInstance->GetData64(DATA_IGNIS_ADDS)))
			{
				Tasks.Speak(CHAT_TYPE_YELL,15565,"Levez vous soldats du Creuset de fer ! Que la volonté du faiseur s'accomplisse !");
				IgnisAdds.push_back(tmpcr);
				DoCast(tmpcr,SPELL_ACTIVATE);
				Tasks.FreezeMob(false,(Creature*)tmpcr);
				tmpcr->AddThreat(me->getVictim());
				me->CastStop();
				Tasks.SetAuraStack(SPELL_CREATOR_STRENGH,1,me,me);
			}

			if(m_bIsHeroic == RAID_DIFFICULTY_25MAN_NORMAL)
				Assemblage_Timer = 40000;
			else
				Assemblage_Timer = 30000;
		}
		else
			Assemblage_Timer -= diff;

		if(Fire_Timer <= diff)
		{
			if(Creature* fire = me->SummonCreature(NPC_FIRE,me->getVictim()->GetPositionX(),
					me->getVictim()->GetPositionY(),me->getVictim()->GetPositionZ(),0.0f,TEMPSUMMON_TIMED_DESPAWN,25000))
					m_pInstance->SetData64(DATA_IGNIS_FIRE,fire->GetGUID());

			if(urand(0,1))
				Tasks.Speak(CHAT_TYPE_YELL,15567,"Que la fournaise vous consume !");
			else
				Tasks.Speak(CHAT_TYPE_YELL,15568,"Br�leeeeeeeeeeeez !");

			if(m_bIsHeroic == RAID_DIFFICULTY_25MAN_NORMAL)
				Fire_Timer = 25000;
			else
				Fire_Timer = 25000;
		}
		else
			Fire_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL add_ignis_AI : public ScriptedAI
{
    add_ignis_AI(Creature *pCreature) : ScriptedAI(pCreature) {
		Reset();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
	}

	bool m_bIsHeroic;
	uint32 Check_Timer;
	ScriptedInstance* m_pInstance;
	uint16 nb_stack;
	uint32 CheckReset_Timer;
	MobEventTasks Tasks;

	void Reset()
	{
		Tasks.SetObjects(this,me);
		nb_stack = 0;
		Tasks.FreezeMob(true,me,true);
		Check_Timer = 1000;
		CheckReset_Timer = 5000;
	}

	void DamageTaken(Unit* pDoneBy, uint32 &damage)
	{
		
		if(me->HasAura(SPELL_FRAGILE_25) && damage > 3000)
		{
			damage = 0;
			if(Unit* Ignis = Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_IGNIS) : 0))
				if(Ignis->isAlive())
				{
					uint8 stk = (Ignis->GetAura(SPELL_CREATOR_STRENGH,0)->GetStackAmount() > 1) ? Ignis->GetAura(SPELL_CREATOR_STRENGH,0)->GetStackAmount() - 1 : 1;
					Tasks.SetAuraStack(SPELL_CREATOR_STRENGH,stk,Ignis,Ignis,1);
				}

			Tasks.Kill(me);
		}
		else if(me->HasAura(SPELL_FRAGILE_10) && damage > 5000)
		{
			damage = 0;
			if(Unit* Ignis = Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_IGNIS) : 0))
				if(Ignis->isAlive())
				{
					uint8 stk = (Ignis->GetAura(SPELL_CREATOR_STRENGH,0)->GetStackAmount() > 1) ? Ignis->GetAura(SPELL_CREATOR_STRENGH,0)->GetStackAmount() - 1 : 1;
					Tasks.SetAuraStack(SPELL_CREATOR_STRENGH,stk,Ignis,Ignis,1);
				}
			Tasks.Kill(me);
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!CanDoSomething())
		{
			if(CheckReset_Timer <= uiDiff)
			{
				if(!me->HasAura(66830))
					DoCastMe(66830);
				CheckReset_Timer = 5000;
			}
			else
				CheckReset_Timer -= uiDiff;
			return;
		}
            

		if(me->HasAura(SPELL_FUSION,0) && (me->GetDistance2d(Ignis_Coords[0][0],Ignis_Coords[0][1]) < 13.0f) || (me->GetDistance2d(Ignis_Coords[1][0],Ignis_Coords[1][1]) < 15.0f))
		{
			nb_stack = 0;
			me->RemoveAurasDueToSpell(SPELL_FUSION);
			me->RemoveAurasDueToSpell(SPELL_CHALEUR);
			DoCastMe( (m_bIsHeroic) ? SPELL_FRAGILE_25 : SPELL_FRAGILE_10);
		}

		if(me->HasAura(SPELL_FRAGILE_25) || me->HasAura(SPELL_FRAGILE_25))
			me->RemoveAurasDueToSpell(SPELL_CHALEUR);

		if(Check_Timer <= uiDiff)
		{
			if(Unit* fire = Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_IGNIS_FIRE) : 0))
			{
				if(fire->isAlive() && !me->HasAura(SPELL_FUSION,0) && !me->HasAura(SPELL_FRAGILE_10,0)
					&& !me->HasAura(SPELL_FRAGILE_25,0))
				{
					if(me->GetDistance(fire) < 25.0f)
					{
						me->getVictim()->CastSpell(me,SPELL_CHALEUR,true);
						nb_stack++;
						if(nb_stack > 19)
						{
							me->RemoveAurasDueToSpell(SPELL_CHALEUR);
							me->CastSpell(me,SPELL_FUSION,true);
						}
					}
				}
			}
			Check_Timer = 1000;
		}
		else
			Check_Timer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

struct MANGOS_DLL_DECL add_scorchAI : public ScriptedAI
{
    add_scorchAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	uint32 CheckTimer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		SetCombatMovement(false);
		CheckTimer = 10;
    }

	void Fire()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->GetDistance2d(me) < 13.0f)
						me->DealDamage(pPlayer,(m_bIsHeroic ? 3500 : 2200), NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE, NULL, false);
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
      if(CheckTimer <= uiDiff)
	  {
		  if(me->GetDistance2d(Ignis_Coords[0][0],Ignis_Coords[0][1]) < 10.0f || me->GetDistance2d(Ignis_Coords[1][0],Ignis_Coords[1][1]))
			  me->ForcedDespawn(1000);

			if(m_bIsHeroic)
			{
				if(!me->HasAura(SPELL_CONE_25))
					DoCastMe(SPELL_CONE_25);
			}
			else
			{
				if(!me->HasAura(SPELL_CONE_10))
					DoCastMe(SPELL_CONE_10);
			}

			Fire();
			CheckTimer = 1000;
	  }
	  else
		  CheckTimer -= uiDiff;

    }
};

CreatureAI* GetAI_boss_ignis(Creature* pCreature)
{
    return new boss_ignis_AI (pCreature);
}

CreatureAI* GetAI_add_ignis(Creature* pCreature)
{
    return new add_ignis_AI (pCreature);
}

CreatureAI* GetAI_add_scorch(Creature* pCreature)
{
    return new add_scorchAI (pCreature);
}

void AddSC_boss_ignis()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ignis";
    newscript->GetAI = &GetAI_boss_ignis;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_ignis";
    newscript->GetAI = &GetAI_add_ignis;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_ignis_fire";
    newscript->GetAI = &GetAI_add_scorch;
    newscript->RegisterSelf();
}
