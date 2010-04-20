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
SDName: boss_argent_challenge
SD%Complete: 92%
SDComment: missing yells. radiance is "wrong"
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

enum
{
	//yells

	//eadric
	SPELL_VENGEANCE				= 66889,
	SPELL_RADIANCE				= 66862,
	SPELL_RADIANCE_H			= 67681,
	SPELL_HAMMER_OF_JUSTICE		= 66940,
	SPELL_HAMMER				= 67680,
	//paletress
	SPELL_SMITE					= 66536,
	SPELL_SMITE_H				= 67674,
	SPELL_HOLY_FIRE				= 66538,
	SPELL_HOLY_FIRE_H			= 67676,
	SPELL_RENEW					= 66537,
	SPELL_RENEW_H				= 67675,
	SPELL_HOLY_NOVA				= 66546,
	SPELL_SHIELD				= 66515,
	SPELL_CONFESS				= 66547,
	//memory
	SPELL_FEAR					= 66552,
	SPELL_FEAR_H				= 67677,
	SPELL_SHADOWS				= 66619,
	SPELL_SHADOWS_H				= 67678,
	SPELL_OLD_WOUNDS			= 66620,
	SPELL_OLD_WOUNDS_H			= 67679,
};

// Eadric The Pure
struct MANGOS_DLL_DECL boss_eadricAI : public ScriptedAI
{
    boss_eadricAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

	uint32 Hammer_Timer;
	uint32 Hammer_Dmg_Timer;
	
	uint64 HammerTarget;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_VENGEANCE,1000,12000,0,TARGET_ME);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_RADIANCE_H : SPELL_RADIANCE,15000,20000,0,TARGET_ME);
		me->SetRespawnDelay(DAY*3600);
		Hammer_Timer = 40000;
		Hammer_Dmg_Timer = 45000;
		HammerTarget = 0;
    }

	void EnterEvadeMode()
	{
		Hammer_Timer = 40000;
		Hammer_Dmg_Timer = 45000;
		HammerTarget = 0;
		me->RemoveArenaAuras(true);
		me->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, me->GetMonsterMoveFlags(), 1);
		me->GetMap()->CreatureRelocation(me, 754.360779, 660.816162, 412.395996, 4.698700);
		me->SetHealth(me->GetMaxHealth());
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == DONE)
			me->ForcedDespawn();
		else
			m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		me->ForcedDespawn();
		m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, DONE);
		GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0);
	}

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		if (Hammer_Timer < diff)
        {
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
			{
				DoCast(target, SPELL_HAMMER_OF_JUSTICE);
				HammerTarget = target->GetGUID();
			}
            Hammer_Timer = 50000;
        }else Hammer_Timer -= diff;

		if (Hammer_Dmg_Timer < diff)
		{
			if (Unit* pHammerTarget = Unit::GetUnit(*me, HammerTarget))
				DoCast(pHammerTarget, SPELL_HAMMER);
			Hammer_Dmg_Timer = 50000;
		}
		else Hammer_Dmg_Timer -= diff;

		Tasks.UpdateEvent(diff);
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_eadric(Creature* pCreature)
{
    return new boss_eadricAI(pCreature);
}

// Argent Confessor Paletress
struct MANGOS_DLL_DECL boss_paletressAI : public ScriptedAI
{
    boss_paletressAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

	uint32 Holy_Fire_Timer;
	uint32 Renew_Timer;
	uint32 Shield_Delay;
	uint32 Shield_Check;
	MobEventTasks Tasks;
	
	bool summoned;
	bool shielded;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_SMITE_H : SPELL_SMITE,5000,2000);
		me->SetRespawnDelay(DAY*3600);
		me->RemoveAurasDueToSpell(SPELL_SHIELD);
		Holy_Fire_Timer = 10000;
		Renew_Timer = 7000;
		Shield_Delay = 0;
		Shield_Check = 1000;
		summoned = false;
		shielded = false;
    }

	void EnterEvadeMode()
	{
		me->RemoveAurasDueToSpell(SPELL_SHIELD);
		Holy_Fire_Timer = 10000;
		Renew_Timer = 7000;
		Shield_Delay = 0;
		Shield_Check = 1000;
		summoned = false;
		shielded = false;
		me->RemoveArenaAuras(true);
		me->SendMonsterMove(746.864441, 660.918762, 411.695465, 4.698700, me->GetMonsterMoveFlags(), 1);
		me->GetMap()->CreatureRelocation(me, 754.360779, 660.816162, 412.395996, 4.698700);
		me->SetHealth(me->GetMaxHealth());
	}

	void Aggro(Unit* pWho)
    {
		if (!m_pInstance)
			return;
		if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == DONE)
			me->ForcedDespawn();
		else
			m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
    }

	void JustDied(Unit* pKiller)
    {
		if (!m_pInstance)
			return;
		me->ForcedDespawn();
		m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, DONE);
		GiveEmblemsToGroup(m_bIsRegularMode ? CONQUETE : 0,1,true);
	}

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		if (Holy_Fire_Timer < diff)
        {
			me->CastStop(!m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
			DoCastRandom(m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);
            Holy_Fire_Timer = 10000;
        }
		else 
			Holy_Fire_Timer -= diff;

		if (Renew_Timer < diff)
        {
			me->CastStop(!m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
			me->CastStop(!m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);
			switch(urand(0, 1))
            {
                case 0:
					if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_MEMORY))))
						if (pTemp->isAlive())
							DoCast(pTemp, !m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H);
						else
							DoCast(pTemp, !m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H);
                break;
				case 1:
					DoCastMe(m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H);
                break;
			}
            Renew_Timer = 25000;
        }
		else 
			Renew_Timer -= diff;

		if ((me->GetHealth()*100 / me->GetMaxHealth()) < 35 && !summoned)
		{
			me->CastStop(!m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
			me->CastStop(!m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);
			DoCast(me, SPELL_HOLY_NOVA);
			switch(urand(0, 24))
            {
                case 0:
					if (Creature* pTemp = me->SummonCreature(MEMORY_ALGALON, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 1:
					if (Creature* pTemp = me->SummonCreature(MEMORY_CHROMAGGUS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 2:
					if (Creature* pTemp = me->SummonCreature(MEMORY_CYANIGOSA, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 3:
					if (Creature* pTemp = me->SummonCreature(MEMORY_DELRISSA, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 4:
					if (Creature* pTemp = me->SummonCreature(MEMORY_ECK, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 5:
					if (Creature* pTemp = me->SummonCreature(MEMORY_ENTROPIUS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 6:
					if (Creature* pTemp = me->SummonCreature(MEMORY_GRUUL, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 7:
					if (Creature* pTemp = me->SummonCreature(MEMORY_HAKKAR, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 8:
					if (Creature* pTemp = me->SummonCreature(MEMORY_HEIGAN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 9:
					if (Creature* pTemp = me->SummonCreature(MEMORY_HEROD, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 10:
					if (Creature* pTemp = me->SummonCreature(MEMORY_HOGGER, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 11:
					if (Creature* pTemp = me->SummonCreature(MEMORY_IGNIS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 12:
					if (Creature* pTemp = me->SummonCreature(MEMORY_ILLIDAN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 13:
					if (Creature* pTemp = me->SummonCreature(MEMORY_INGVAR, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 14:
					if (Creature* pTemp = me->SummonCreature(MEMORY_KALITHRESH, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 15:
					if (Creature* pTemp = me->SummonCreature(MEMORY_LUCIFRON, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 16:
					if (Creature* pTemp = me->SummonCreature(MEMORY_MALCHEZAAR, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 17:
					if (Creature* pTemp = me->SummonCreature(MEMORY_MUTANUS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 18:
					if (Creature* pTemp = me->SummonCreature(MEMORY_ONYXIA, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 19:
					if (Creature* pTemp = me->SummonCreature(MEMORY_THUNDERAAN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 20:
					if (Creature* pTemp = me->SummonCreature(MEMORY_VANCLEEF, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 21:
					if (Creature* pTemp = me->SummonCreature(MEMORY_VASHJ, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 22:
					if (Creature* pTemp = me->SummonCreature(MEMORY_VEKNILASH, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 23:
					if (Creature* pTemp = me->SummonCreature(MEMORY_VEZAX, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
				case 24:
					if (Creature* pTemp = me->SummonCreature(MEMORY_ARCHIMONDE, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
					{
						pTemp->AddThreat(me->getVictim(),1000.0f);
						pTemp->AI()->AttackStart(me->getVictim());
					}
                break;
			}
			summoned = true;
			Shield_Delay = 1000;
		}

		if (Shield_Delay < diff && !shielded && summoned)
        {
			me->CastStop(!m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
			me->CastStop(!m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);
			DoCastMe(SPELL_SHIELD);
            shielded = true;
			Shield_Check = 1000;
        }
		else 
			Shield_Delay -= diff;

		if (Shield_Check < diff && shielded)
        {
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_MEMORY))))
				if (!pTemp->isAlive())
				{
					me->RemoveAurasDueToSpell(SPELL_SHIELD);
					shielded = false;
				}
				else Shield_Check = 1000;
        }
		else 
			Shield_Check -= diff;
		
		Tasks.UpdateEvent(diff);
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_paletress(Creature* pCreature)
{
    return new boss_paletressAI(pCreature);
}

// Summoned Memory
struct MANGOS_DLL_DECL mob_toc5_memoryAI : public ScriptedAI
{
    mob_toc5_memoryAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	MobEventTasks Tasks;

	uint32 Shadows_Timer;
	uint32 Fear_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_OLD_WOUNDS_H : SPELL_OLD_WOUNDS,5000,10000);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_FEAR_H : SPELL_FEAR,13000,40000,0,TARGET_ME);
		Tasks.AddEvent(m_bIsRegularMode ? SPELL_SHADOWS_H : SPELL_SHADOWS,8000,10000);
		Shadows_Timer = 8000;
		Fear_Timer = 13000;
    }

	void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		Tasks.UpdateEvent(diff); 
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_toc5_memory(Creature* pCreature)
{
    return new mob_toc5_memoryAI(pCreature);
}

void AddSC_boss_argent_challenge()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_eadric";
	NewScript->GetAI = &GetAI_boss_eadric;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_paletress";
	NewScript->GetAI = &GetAI_boss_paletress;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "mob_toc5_memory";
	NewScript->GetAI = &GetAI_mob_toc5_memory;
    NewScript->RegisterSelf();
}