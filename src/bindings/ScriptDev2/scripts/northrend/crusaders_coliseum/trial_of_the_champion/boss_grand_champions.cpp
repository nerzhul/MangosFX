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
SDName: boss_grand_champions
SD%Complete: 92%
SDComment: missing yells. hunter AI sucks. no pvp diminuishing returns(is it DB related?)
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

enum
{
	//yells

	//warrior
	SPELL_MORTAL_STRIKE			= 68783,
	SPELL_MORTAL_STRIKE_H		= 68784,
	SPELL_BLADESTORM			= 63784,
	SPELL_INTERCEPT				= 67540,
	SPELL_ROLLING_THROW			= 47115, //need core support for spell 67546, using 47115 instead
	//mage
	SPELL_FIREBALL				= 66042,
	SPELL_FIREBALL_H			= 68310,
	SPELL_BLAST_WAVE			= 66044,
	SPELL_BLAST_WAVE_H			= 68312,
	SPELL_HASTE					= 66045,
	SPELL_POLYMORPH				= 66043,
	SPELL_POLYMORPH_H			= 68311,
	//shaman
	SPELL_CHAIN_LIGHTNING		= 67529,
	SPELL_CHAIN_LIGHTNING_H		= 68319,
	SPELL_EARTH_SHIELD			= 67530,
	SPELL_HEALING_WAVE			= 67528,
	SPELL_HEALING_WAVE_H		= 68318,
	SPELL_HEX_OF_MENDING		= 67534,
	//hunter
	SPELL_DISENGAGE				= 68340,
	SPELL_LIGHTNING_ARROWS		= 66083,
	SPELL_MULTI_SHOT			= 66081,
	SPELL_SHOOT					= 66079,
	//rogue
	SPELL_EVISCERATE			= 67709,
	SPELL_EVISCERATE_H			= 68317,
	SPELL_FAN_OF_KNIVES			= 67706,
	SPELL_POISON_BOTTLE			= 67701
};

// Warrior
struct MANGOS_DLL_DECL mob_toc5_warriorAI : public ScriptedAI
{
    mob_toc5_warriorAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	MobEventTasks Tasks;

	uint32 Bladestorm_Timer;
	uint32 Intercept_Cooldown;
	uint32 intercept_check;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_MORTAL_STRIKE_H : SPELL_MORTAL_STRIKE,6000,6000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_ROLLING_THROW,30000,30000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_BLADESTORM,20000,90000,TARGET_ME);
		me->SetRespawnDelay(DAY*3600);
		Intercept_Cooldown = 0;
		intercept_check = 1000;
    }

	void EnterEvadeMode()
	{
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
			me->ForcedDespawn();
		else
		{
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
		}
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		if (Creature* pTemp0 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
                if (!pTemp0->isAlive())
					if (Creature* pTemp1 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
						if (!pTemp1->isAlive())
							if (Creature* pTemp2 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
								if (!pTemp2->isAlive())
								{
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData(DATA_TOC5_ANNOUNCER))))
										pTemp->SetVisibility(VISIBILITY_ON);
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
										pTemp->ForcedDespawn();
									m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);
									GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0);
								}
	}

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		if (intercept_check < diff)
		{
			if (!me->IsWithinDistInMap(me->getVictim(), 8) && me->IsWithinDistInMap(me->getVictim(), 25) && Intercept_Cooldown < diff)
			{
				DoCast(me->getVictim(), SPELL_INTERCEPT);
				Intercept_Cooldown = 15000;
			}
			intercept_check = 1000;
		}
		else 
		{
			intercept_check -= diff;
			Intercept_Cooldown -= diff;
		}
		
		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_toc5_warrior(Creature* pCreature)
{
    return new mob_toc5_warriorAI(pCreature);
}

// Mage
struct MANGOS_DLL_DECL mob_toc5_mageAI : public ScriptedAI
{
    mob_toc5_mageAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_FIREBALL_H : SPELL_FIREBALL,50,3000,0,TARGET_MAIN);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_BLAST_WAVE_H : SPELL_BLAST_WAVE,20000,20000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_HASTE,9000,10000,0,TARGET_ME);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_POLYMORPH_H : SPELL_POLYMORPH,15000,15000);

		me->SetRespawnDelay(DAY*3600);
    }

	void EnterEvadeMode()
	{
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
			me->ForcedDespawn();
		else
		{
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
		}
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		if (Creature* pTemp0 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
                if (!pTemp0->isAlive())
					if (Creature* pTemp1 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
						if (!pTemp1->isAlive())
							if (Creature* pTemp2 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
								if (!pTemp2->isAlive())
								{
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData(DATA_TOC5_ANNOUNCER))))
										pTemp->SetVisibility(VISIBILITY_ON);
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
										pTemp->ForcedDespawn();
									m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);
									GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0);
								}
	}

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		Tasks.UpdateEvent(diff);
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_toc5_mage(Creature* pCreature)
{
    return new mob_toc5_mageAI(pCreature);
}

// Shaman
struct MANGOS_DLL_DECL mob_toc5_shamanAI : public ScriptedAI
{
    mob_toc5_shamanAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	MobEventTasks Tasks;

	uint32 Chain_Lightning_Timer;
	uint32 Earth_Shield_Timer;
	uint32 Healing_Wave_Timer;
	uint32 Hex_Timer;

	float mob1_health;
	float mob2_health;
	float mob3_health;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		me->SetRespawnDelay(999999999);
		Chain_Lightning_Timer = 1000;
		Earth_Shield_Timer = 5000;
		Healing_Wave_Timer = 13000;
		Hex_Timer = 10000;
    }

	void EnterEvadeMode()
	{
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
			me->ForcedDespawn();
		else
		{
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
		}
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		if (Creature* pTemp0 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
                if (!pTemp0->isAlive())
					if (Creature* pTemp1 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
						if (!pTemp1->isAlive())
							if (Creature* pTemp2 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
								if (!pTemp2->isAlive())
								{
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData(DATA_TOC5_ANNOUNCER))))
										pTemp->SetVisibility(VISIBILITY_ON);
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
										pTemp->ForcedDespawn();
									m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);
									GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0);
								}
	}

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		if (Chain_Lightning_Timer < diff)
        {
			DoCast(me->getVictim(), m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);
            Chain_Lightning_Timer = 10000;
        }else Chain_Lightning_Timer -= diff;  

		if (Hex_Timer < diff)
        {
			DoCast(me->getVictim(), SPELL_HEX_OF_MENDING);
            Hex_Timer = 20000;
        }else Hex_Timer -= diff;

		if (Healing_Wave_Timer < diff)
        {
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
				if (pTemp->isAlive())
					mob1_health = pTemp->GetHealth()*100 / pTemp->GetMaxHealth();
				else
					mob1_health = 100;
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
				if (pTemp->isAlive())
					mob2_health = pTemp->GetHealth()*100 / pTemp->GetMaxHealth();
				else
					mob2_health = 100;
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
				if (pTemp->isAlive())
					mob3_health = pTemp->GetHealth()*100 / pTemp->GetMaxHealth();
				else
					mob3_health = 100;
			if (mob1_health < mob2_health && mob1_health < mob3_health && mob1_health < 70)
				if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
					DoCast(pTemp, m_bIsRegularMode ? SPELL_HEALING_WAVE : SPELL_HEALING_WAVE_H);
			if (mob1_health > mob2_health && mob2_health < mob3_health && mob2_health < 70)
				if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
					DoCast(pTemp, m_bIsRegularMode ? SPELL_HEALING_WAVE : SPELL_HEALING_WAVE_H);
			if (mob3_health < mob2_health && mob1_health > mob3_health && mob3_health < 70)
				if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
					DoCast(pTemp, m_bIsRegularMode ? SPELL_HEALING_WAVE : SPELL_HEALING_WAVE_H);
            Healing_Wave_Timer = 8000;
        }else Healing_Wave_Timer -= diff;

		if (Earth_Shield_Timer < diff)
        {
			switch(urand(0, 2))
            {
                case 0:
					if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
						if (pTemp->isAlive())
							DoCast(pTemp, SPELL_EARTH_SHIELD);
						else
							DoCast(me, SPELL_EARTH_SHIELD);
                break;
				case 1:
					if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
						if (pTemp->isAlive())
							DoCast(pTemp, SPELL_EARTH_SHIELD);
						else
							DoCast(me, SPELL_EARTH_SHIELD);
                break;
				case 2:
					if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
						if (pTemp->isAlive())
							DoCast(pTemp, SPELL_EARTH_SHIELD);
						else
							DoCast(me, SPELL_EARTH_SHIELD);
                break;
			}
            Earth_Shield_Timer = 25000;
        }else Earth_Shield_Timer -= diff;
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_toc5_shaman(Creature* pCreature)
{
    return new mob_toc5_shamanAI(pCreature);
}

// Hunter
struct MANGOS_DLL_DECL mob_toc5_hunterAI : public ScriptedAI
{
    mob_toc5_hunterAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	MobEventTasks Tasks;

	uint32 Shoot_Timer;
	uint32 Lightning_Arrows_Timer;
	uint32 Multi_Shot_Timer;
	uint32 Disengage_Cooldown;
	uint32 enemy_check;
	uint32 disengage_check;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		me->SetRespawnDelay(DAY*3600);
		Shoot_Timer = 0;
		Lightning_Arrows_Timer = 13000;
		Multi_Shot_Timer = 10000;
		Disengage_Cooldown = 0;
		enemy_check = 1000;
		disengage_check;
    }

	void EnterEvadeMode()
	{
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
			me->ForcedDespawn();
		else
		{
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
		}
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		if (Creature* pTemp0 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
                if (!pTemp0->isAlive())
					if (Creature* pTemp1 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
						if (!pTemp1->isAlive())
							if (Creature* pTemp2 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
								if (!pTemp2->isAlive())
								{
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData(DATA_TOC5_ANNOUNCER))))
										pTemp->SetVisibility(VISIBILITY_ON);
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
										pTemp->ForcedDespawn();
									m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);
									GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0);
								}
	}

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		if (enemy_check < diff)
		{
			if (!me->IsWithinDistInMap(me->getVictim(), 8) && me->IsWithinDistInMap(me->getVictim(), 30))
			{
				me->SetSpeedRate(MOVE_RUN, 0.0001);
			}
			else
			{
				me->SetSpeedRate(MOVE_RUN, 1);
			}
			enemy_check = 100;
		}else enemy_check -= diff;

		if (Disengage_Cooldown>0)
			Disengage_Cooldown -= diff;

		if (Shoot_Timer < diff)
        {
			DoCast(me->getVictim(), SPELL_SHOOT);
            Shoot_Timer = 3000;
        }else Shoot_Timer -= diff;  

		if (Multi_Shot_Timer < diff)
        {
			me->CastStop(SPELL_SHOOT);
			DoCast(me->getVictim(), SPELL_MULTI_SHOT);
            Multi_Shot_Timer = 10000;
        }else Multi_Shot_Timer -= diff;

		if (Lightning_Arrows_Timer < diff)
        {
			me->CastStop(SPELL_SHOOT);
			DoCast(me, SPELL_LIGHTNING_ARROWS);
            Lightning_Arrows_Timer = 25000;
        }else Lightning_Arrows_Timer -= diff;

		if (disengage_check < diff)
		{
			if (me->IsWithinDistInMap(me->getVictim(), 5) && Disengage_Cooldown == 0)
			{
				DoCast(me, SPELL_DISENGAGE);
				Disengage_Cooldown = 15000;
			}
			disengage_check = 1000;
		}else disengage_check -= diff;
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_toc5_hunter(Creature* pCreature)
{
    return new mob_toc5_hunterAI(pCreature);
}

// Rogue
struct MANGOS_DLL_DECL mob_toc5_rogueAI : public ScriptedAI
{
    mob_toc5_rogueAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	MobEventTasks Tasks;

	uint32 Eviscerate_Timer;
	uint32 FoK_Timer;
	uint32 Poison_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		me->SetRespawnDelay(999999999);
		Eviscerate_Timer = 15000;
		FoK_Timer = 10000;
		Poison_Timer = 7000;
    }

	void EnterEvadeMode()
	{
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(738.665771, 661.031433, 412.394623, 4.698702, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 738.665771, 661.031433, 412.394623, 4.698702);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 746.864441, 660.918762, 411.695465, 4.698700);
			}
		if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
			if (!pTemp->isAlive())
			{
				pTemp->Respawn();
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
			else
			{
				pTemp->RemoveArenaAuras(true);
				pTemp->SetHealth(pTemp->GetMaxHealth());
				pTemp->SendMonsterMove(754.360779, 660.816162, 412.395996, 4.698700, pTemp->GetMonsterMoveFlags(), 1);
				pTemp->GetMap()->CreatureRelocation(pTemp, 754.360779, 660.816162, 412.395996, 4.698700);
			}
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
			me->ForcedDespawn();
		else
		{
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
		}
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		if (Creature* pTemp0 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
                if (!pTemp0->isAlive())
					if (Creature* pTemp1 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
						if (!pTemp1->isAlive())
							if (Creature* pTemp2 = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
								if (!pTemp2->isAlive())
								{
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData(DATA_TOC5_ANNOUNCER))))
										pTemp->SetVisibility(VISIBILITY_ON);
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_1))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_2))))
										pTemp->ForcedDespawn();
									if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_CHAMPION_3))))
										pTemp->ForcedDespawn();
									m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);
									GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0);
								}
	}

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		if (Eviscerate_Timer < diff)
        {
			DoCast(me->getVictim(), m_bIsRegularMode ? SPELL_EVISCERATE : SPELL_EVISCERATE_H);
            Eviscerate_Timer = 10000;
        }else Eviscerate_Timer -= diff;  

		if (FoK_Timer < diff)
        {
			DoCast(me->getVictim(), SPELL_FAN_OF_KNIVES);
			FoK_Timer = 7000;
        }else FoK_Timer -= diff;

		if (Poison_Timer < diff)
        {
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
				DoCast(me, SPELL_POISON_BOTTLE);
            Poison_Timer = 6000;
        }else Poison_Timer -= diff;
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_toc5_rogue(Creature* pCreature)
{
    return new mob_toc5_rogueAI(pCreature);
}

void AddSC_boss_grand_champions()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "mob_toc5_warrior";
	NewScript->GetAI = &GetAI_mob_toc5_warrior;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "mob_toc5_mage";
	NewScript->GetAI = &GetAI_mob_toc5_mage;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "mob_toc5_shaman";
	NewScript->GetAI = &GetAI_mob_toc5_shaman;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "mob_toc5_hunter";
	NewScript->GetAI = &GetAI_mob_toc5_hunter;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "mob_toc5_rogue";
	NewScript->GetAI = &GetAI_mob_toc5_rogue;
    NewScript->RegisterSelf();
}