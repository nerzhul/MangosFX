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
SDName: boss_black_knight
SD%Complete: 92%
SDComment: missing yells. not sure about timers.
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

enum
{
	//yells

	//undead
	SPELL_PLAGUE_STRIKE			= 67724,
	SPELL_PLAGUE_STRIKE_H		= 67884,
	SPELL_ICY_TOUCH				= 67718,
	SPELL_ICY_TOUCH_H			= 67881,
	SPELL_OBLITERATE			= 67725,
	SPELL_OBLITERATE_H			= 67883,
	SPELL_CHOKE					= 68306,
	//skeleton
	SPELL_ARMY					= 42650,			//replacing original one, since that one spawns millions of ghouls!!
	//ghost
	SPELL_DEATH 				= 67808,
	SPELL_DEATH_H				= 67875,
	SPELL_MARK					= 67823,

	//risen ghoul
	SPELL_CLAW					= 67879,
	SPELL_EXPLODE				= 67729,
	SPELL_EXPLODE_H				= 67886,
	SPELL_LEAP					= 67749,
	SPELL_LEAP_H				= 67880,

	//sword ID
	EQUIP_SWORD					= 40343
};

// Risen Ghoul
struct MANGOS_DLL_DECL mob_toc5_risen_ghoulAI : public ScriptedAI
{
    mob_toc5_risen_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

	uint32 Attack;

    void Reset()
    {
		Attack = 2500;
    }

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if (Attack < diff)
        {
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_BLACK_KNIGHT))))
				if (pTemp->isAlive())
					if ((pTemp->GetHealth()*100 / pTemp->GetMaxHealth()) < 25)
						DoCastMe(!m_bIsRegularMode ? SPELL_EXPLODE : SPELL_EXPLODE_H);
			if (me->IsWithinDistInMap(me->getVictim(), 4))
			{
				DoCast(me->getVictim(), SPELL_CLAW);
				if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1))
					me->AI()->AttackStart(target);
				Attack = 2500;
			}else
			if (me->IsWithinDistInMap(me->getVictim(), 30))
			{
				DoCastVictim(!m_bIsRegularMode ? SPELL_LEAP : SPELL_LEAP_H);
				Attack = 2500;
			}
        }else Attack -= diff;

		if ((me->GetHealth()*100 / me->GetMaxHealth()) < 25)
			DoCastMe(!m_bIsRegularMode ? SPELL_EXPLODE : SPELL_EXPLODE_H);
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_toc5_risen_ghoul(Creature* pCreature)
{
    return new mob_toc5_risen_ghoulAI(pCreature);
}

// The Black Knight
struct MANGOS_DLL_DECL boss_black_knightAI : public ScriptedAI
{
    boss_black_knightAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	MobEventTasks Tasks;

	uint32 Phase_Delay;
	uint32 Summon_Ghoul;

	bool phase1;
	bool phase2;
	bool phase3;
	uint8 phase;
	bool ghoul;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_DEATH_H : SPELL_DEATH,5000,3500,0,TARGET_ME,3);
		Tasks.AddEvent(SPELL_MARK,9000,15000,0,TARGET_RANDOM,3);
		Tasks.AddEvent(SPELL_CHOKE,15000,15000,0,TARGET_RANDOM,1);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_PLAGUE_STRIKE_H : SPELL_PLAGUE_STRIKE,5000,10500,0,TARGET_MAIN,1);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_PLAGUE_STRIKE_H : SPELL_PLAGUE_STRIKE,5000,10500,0,TARGET_MAIN,2);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_ICY_TOUCH_H : SPELL_ICY_TOUCH,10000,10000,0,TARGET_MAIN,1);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_ICY_TOUCH_H : SPELL_ICY_TOUCH,10000,10000,0,TARGET_MAIN,2);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_OBLITERATE_H : SPELL_OBLITERATE,16000,11000,0,TARGET_MAIN,1);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_OBLITERATE_H : SPELL_OBLITERATE,16000,11000,0,TARGET_MAIN,2);
		
		me->SetRespawnDelay(DAY*3600);
		me->SetDisplayId(29837);
		SetEquipmentSlots(false, EQUIP_SWORD, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
		Summon_Ghoul = 4000;
		phase1 = true;
		phase2 = false;
		phase3 = false;
		ghoul = false;
		phase = 1;
    }

	void EnterEvadeMode()
	{
		me->SetDisplayId(29837);
		Summon_Ghoul = 4000;
		phase1 = true;
		phase2 = false;
		phase3 = false;
		ghoul = false;
		me->RemoveArenaAuras(true);
		me->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, me->GetMonsterMoveFlags(), 1);
		me->GetMap()->CreatureRelocation(me, 754.360779, 660.816162, 412.395996, 4.698700);
		me->SetHealth(me->GetMaxHealth());
		phase = 1;
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_BLACK_KNIGHT) == DONE)
			me->ForcedDespawn();
		else
			m_pInstance->SetData(TYPE_BLACK_KNIGHT, IN_PROGRESS);
    }

	void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
		if (uiDamage > me->GetHealth() && !phase3){
			uiDamage = 0;
			if (phase2)
				StartPhase3();
			if (phase1)
				StartPhase2();
		}
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		if (phase3)
		{
			m_pInstance->SetData(TYPE_BLACK_KNIGHT, DONE);
			GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0,1,true);
		}
		if (phase2)
			if (!me->isAlive())
			{
				me->Respawn();
				StartPhase3();
			}
		if (phase1)
			if (!me->isAlive())
			{
				me->Respawn();
				StartPhase2();
			}
	}

	void StartPhase2()
	{
		me->SetHealth(me->GetMaxHealth());
		me->SetDisplayId(27550);
		phase1 = false;
		phase2 = true;
		phase3 = false;
		DoCast(me, SPELL_ARMY);
		phase = 2;
	}

	void StartPhase3()
	{
		me->SetHealth(me->GetMaxHealth());
		me->SetDisplayId(14560);
		SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
		phase1 = false;
		phase2 = false;
		phase3 = true;
		phase = 3;
	}

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if (Summon_Ghoul < diff && phase1 && !ghoul)
        {
			if (m_pInstance->GetData(DATA_TOC5_ANNOUNCER) == m_pInstance->GetData(DATA_JAEREN))
				 me->SummonCreature(NPC_RISEN_JAEREN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
			else
				me->SummonCreature(NPC_RISEN_ARELAS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
			ghoul = true;
        }
		else 
			Summon_Ghoul -= diff;

		Tasks.UpdateEvent(diff,phase);
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_black_knight(Creature* pCreature)
{
    return new boss_black_knightAI(pCreature);
}

void AddSC_boss_black_knight()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "mob_toc5_risen_ghoul";
	NewScript->GetAI = &GetAI_mob_toc5_risen_ghoul;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_black_knight";
	NewScript->GetAI = &GetAI_boss_black_knight;
    NewScript->RegisterSelf();
}