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
SDName: Boss_Faerlina
SD%Complete: 50
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SAY_GREET                   -1533009
#define SAY_AGGRO1                  -1533010
#define SAY_AGGRO2                  -1533011
#define SAY_AGGRO3                  -1533012
#define SAY_AGGRO4                  -1533013
#define SAY_SLAY1                   -1533014
#define SAY_SLAY2                   -1533015
#define SAY_DEATH                   -1533016

//#define SOUND_RANDOM_AGGRO  8955                            //soundId containing the 4 aggro sounds, we not using this

#define SPELL_POSIONBOLT_VOLLEY     28796
#define H_SPELL_POSIONBOLT_VOLLEY   54098
#define SPELL_ENRAGE                28798
#define H_SPELL_ENRAGE              54100

#define SPELL_FIREBALL              54095
#define SPELL_FIREBALL_H            54096
#define SPELL_WIDOWS_EMBRACE        28732

#define SPELL_RAINOFFIRE            28794                   //Not sure if targeted AoEs work if casted directly upon a pPlayer
#define SPELL_RAINOFFIRE_H			58936

struct MANGOS_DLL_DECL boss_faerlinaAI : public ScriptedAI
{
    boss_faerlinaAI(Creature* pCreature) : ScriptedAI(pCreature) {
		Reset();
		m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
	}

    bool HasTaunted;
	bool m_bIsHeroic;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(!m_bIsHeroic)
		{
			Tasks.AddEvent(H_SPELL_POSIONBOLT_VOLLEY,8000,11000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_RAINOFFIRE_H,16000,16000,0,TARGET_RANDOM);
			Tasks.AddEvent(H_SPELL_ENRAGE,60000,61000,0,TARGET_ME);
		}
		else
		{
			Tasks.AddEvent(SPELL_POSIONBOLT_VOLLEY,8000,11000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_RAINOFFIRE,16000,16000,0,TARGET_RANDOM);
			Tasks.AddEvent(SPELL_ENRAGE,60000,61000,0,TARGET_ME);
		}
        HasTaunted = false;
    }

    void Aggro(Unit *who)
    {
        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_AGGRO1, me); break;
            case 1: DoScriptText(SAY_AGGRO2, me); break;
            case 2: DoScriptText(SAY_AGGRO3, me); break;
            case 3: DoScriptText(SAY_AGGRO4, me); break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!HasTaunted && me->IsWithinDistInMap(who, 60.0f))
        {
            DoScriptText(SAY_GREET, me);
            HasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit* victim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup((!m_bIsHeroic) ? VAILLANCE : HEROISME);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};


struct MANGOS_DLL_DECL mob_worshippersAI : public ScriptedAI
{
    mob_worshippersAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
    bool m_bIsDead;

    uint32 m_uiFireball_Timer;
    uint32 m_uiDeathDelay_Timer;

    void Reset()
    {
        m_bIsDead = false;
		m_pInstance->SetData(TYPE_FAERLINA,NOT_STARTED);
        m_uiFireball_Timer = 0;
        m_uiDeathDelay_Timer = 0;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bIsDead)
        {
            uiDamage = 0;
        }

        if (uiDamage > me->GetHealth())
        {
            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            me->RemoveAllAuras();
            me->AttackStop();

			m_pInstance->SetData(TYPE_ENR_FAERLINA,0);
            DoCastMe( SPELL_WIDOWS_EMBRACE);

            m_bIsDead = true;
            m_uiDeathDelay_Timer = 500;

            uiDamage = 0;
        }
    }

	void JustDied(Unit* Killer)
	{
		m_pInstance->SetData(TYPE_ENR_FAERLINA,0);
		m_pInstance->SetData(TYPE_FAERLINA,DONE);

	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiDeathDelay_Timer != 0)
            if (m_uiDeathDelay_Timer <= uiDiff)
            {
				m_pInstance->SetData(TYPE_ENR_FAERLINA,0);
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_uiDeathDelay_Timer = 0;
            }
            else 
				m_uiDeathDelay_Timer -= uiDiff;

        if (CanDoSomething() || m_bIsDead)
            return;


        if (m_uiFireball_Timer < uiDiff)
        {
			m_pInstance->SetData(TYPE_FAERLINA,IN_PROGRESS);
            DoCastVictim( m_bIsHeroic ? SPELL_FIREBALL_H : SPELL_FIREBALL);
            m_uiFireball_Timer = 7000 + rand()%4000;
        }
        else m_uiFireball_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_worshippers(Creature* pCreature)
{
    return new mob_worshippersAI(pCreature);
}

CreatureAI* GetAI_boss_faerlina(Creature* pCreature)
{
    return new boss_faerlinaAI(pCreature);
}

void AddSC_boss_faerlina()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_faerlina";
    newscript->GetAI = &GetAI_boss_faerlina;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_worshippers";
    newscript->GetAI = &GetAI_mob_worshippers;
    newscript->RegisterSelf();
}
