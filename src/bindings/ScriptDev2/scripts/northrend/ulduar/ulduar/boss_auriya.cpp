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

//boss_auriaya

enum AuriayaSpell
{
	SPELL_SONIC_SCREECH					= 64422,
	SPELL_SONIC_SCREECH_H				= 64688,
	SPELL_TERRIFYING_SCREECH			= 64386,
	SPELL_ESSAIM						= 64396,
	SPELL_SENTINEL_BLAST				= 64389,
	SPELL_SENTINEL_BLAST_H				= 64678,
	SPELL_SUMMON_GUARDS					= 64397,
	SPELL_ENRAGE						= 47008,

	NPC_SUMMON_FERAL					= 34035,
};

#define SAY_AGGRO                   -2615016
#define SAY_SLAY_1                  -2615017

struct MANGOS_DLL_DECL boss_auriaya_AI : public ScriptedAI
{
    boss_auriaya_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
		Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;
	uint16 first_add_Timer;
	uint32 enrage_Timer;

    void Reset()
    {
		first_add_Timer = 1000;
		Tasks.SetObjects(this,me);

		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_TERRIFYING_SCREECH,9500,30000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_SUMMON_GUARDS,35000,60000);
		if(m_bIsHeroic)
		{
			Tasks.AddSummonEvent(NPC_SUMMON_FERAL,60000,RESPAWN_ONE_DAY,0,0,2);
			Tasks.AddEvent(SPELL_SENTINEL_BLAST_H,12000,30000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_SONIC_SCREECH_H,18000,18000,0,TARGET_MAIN);
		}
		else
		{
			Tasks.AddSummonEvent(NPC_SUMMON_FERAL,60000,RESPAWN_ONE_DAY,0,0,1);
			Tasks.AddEvent(SPELL_SENTINEL_BLAST,12000,15000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_SONIC_SCREECH,30000,28000,0,TARGET_MAIN);
		}
		enrage_Timer = TEN_MINS;
    }

    void EnterCombat(Unit* who)
    {
        Tasks.CallCreature(34014,TEN_MINS,NEAR_7M,AGGRESSIVE_MAIN);
		Tasks.CallCreature(34014,TEN_MINS,NEAR_7M,AGGRESSIVE_MAIN);
		Tasks.Speak(CHAT_TYPE_SAY,15473,"Certaines choses ne doivent pas êtres dérangÃ©es !");
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(SAY_SLAY_1, me);
		if(urand(0,1))
			Tasks.Speak(CHAT_TYPE_YELL,15474,"Le secret meurt avec vous !");
		else 
			Tasks.Speak(CHAT_TYPE_YELL,15475,"Vous n'en rÃ©chapperez pas !");
    }

    void JustDied(Unit *victim)
    {
		Tasks.Speak(CHAT_TYPE_YELL,15476,"Aaaaaaaaaaaaaaaaaaaaaaaaaaaaargh");

        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, DONE);
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(enrage_Timer <= diff)
		{
			DoCastMe(SPELL_ENRAGE);
			Tasks.Speak(CHAT_TYPE_YELL,15477,"Vous me faîtes perdre mon temps.");
			enrage_Timer = 60000;
		}
		else
			enrage_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_auriaya(Creature* pCreature)
{
    return new boss_auriaya_AI (pCreature);
}

enum FeralSpells
{
	SPELL_FERAL_ESSENCE		=	64455,
	SPELL_FERAL_POUNCE_10	=	64478,
	SPELL_FERAL_POUNCE_25	=	64669,
	SPELL_FERAL_RUSH_10		=	64496,
	SPELL_FERAL_RUSH_25		=	64674,
	SPELL_SEEPING_10		=	64459,
	SPELL_SEEPING_25		=	64675,
};

struct MANGOS_DLL_DECL add_feral_defender_AI : public ScriptedAI
{
    add_feral_defender_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
		Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;
	uint8 numb_lives;

	void Reset()
	{
		Tasks.SetObjects(this,me);
		numb_lives = 8;
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_FERAL_RUSH_25,2000,15000,2000);
			Tasks.AddEvent(SPELL_FERAL_POUNCE_25,10000,10000,2000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_FERAL_RUSH_25,2000,15000,2000);
			Tasks.AddEvent(SPELL_FERAL_POUNCE_25,10000,10000,2000,TARGET_MAIN);
		}
		me->RemoveAurasDueToSpell(SPELL_FERAL_ESSENCE);
		Tasks.SetAuraStack(SPELL_FERAL_ESSENCE,numb_lives,me,me);
	}

	void DamageTaken(Unit* done_by, uint32 &damage)
	{
		if(damage >= me->GetHealth())
		{
			damage = 0;
			if(numb_lives > 0)
			{
				numb_lives--;
				me->SetHealth(me->GetMaxHealth());
				if(me->HasAura(SPELL_FERAL_ESSENCE))
					if(me->GetAura(SPELL_FERAL_ESSENCE,0)->GetStackAmount() > 1)
						Tasks.SetAuraStack(SPELL_FERAL_ESSENCE,numb_lives,me,me,1);
					else
						me->RemoveAurasDueToSpell(SPELL_FERAL_ESSENCE);
			}
			else
			{
				if(m_bIsHeroic)
					DoCastVictim(SPELL_SEEPING_25);
				else
					DoCastVictim(SPELL_SEEPING_10);
				Tasks.Kill(me);
			}
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_feral_defender(Creature* pCreature)
{
    return new add_feral_defender_AI (pCreature);
}

enum SentrySpells
{
	SPELL_RIPFLESH_10	=	64375,
	SPELL_RIPFLESH_25	=	64667,
	SPELL_POUNCE_10		=	64374,
	SPELL_POUNCE_25		=	64666,
	SPELL_PACK_STR		=	64381,
};

struct MANGOS_DLL_DECL add_sanctum_sentry_AI : public ScriptedAI
{
    add_sanctum_sentry_AI(Creature *pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
		Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;

	void Reset()
	{
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_RIPFLESH_25,6000,25000,2000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_POUNCE_25,6000,5000,2000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_RIPFLESH_10,6000,25000,2000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_POUNCE_10,6000,5000,2000,TARGET_MAIN);
		}
	}

	void MoveInLineOfSight(Unit* tar)
	{
		Tasks.SetAuraStack(SPELL_PACK_STR,1,me,me);	
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!CanDoSomething())
            return;
		
		Tasks.UpdateEvent(uiDiff);
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_sanctum_sentry(Creature* pCreature)
{
    return new add_sanctum_sentry_AI (pCreature);
}


void AddSC_boss_auriaya()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_auriaya";
    newscript->GetAI = &GetAI_boss_auriaya;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "auriaya_feral_defender";
    newscript->GetAI = &GetAI_feral_defender;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "sanctum_sentry";
    newscript->GetAI = &GetAI_sanctum_sentry;
    newscript->RegisterSelf();
}
