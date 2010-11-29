/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Deathwhisper
SD%Complete: 0
SDComment: Written by K, with some ideas from salja
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

#define ADD_1X -619.006
#define ADD_1Y 2158.104
#define ADD_1Z 50.848

#define ADD_2X -598.697
#define ADD_2Y 2157.767
#define ADD_2Z 50.848

#define ADD_3X -577.992
#define ADD_3Y 2156.989
#define ADD_3Z 50.848

#define ADD_4X -618.748
#define ADD_4Y 2266.648
#define ADD_4Z 50.849

#define ADD_5X -598.573
#define ADD_5Y 2266.870
#define ADD_5Z 50.849

#define ADD_6X -578.360
#define ADD_6Y 2267.210
#define ADD_6Z 50.849

enum
{
    SPELL_MANA_BARRIER         = 70842,
    SPELL_SHADOW_BOLT          = 71254,
    SPELL_DEATH_AND_DECAY      = 71001,
    SPELL_DARK_EMPOWERMENT     = 70901,
    SPELL_FROSTBOLT            = 71420,
    SPELL_INSIGNIFICANCE       = 71204,
    SPELL_FROSTBOLT_VOLLEY     = 72905,
    SPELL_VENGEFUL_SHADE       = 71426,

    NPC_CULT_ADHERENT          = 37949,
    NPC_CULT_FANATIC           = 37890,
    NPC_VENGEFUL_SHADE         = 38222,
	SPELL_VENGEFUL_BLAST       = 71494,
};

enum BossSpells
{
    //Abilities
    SPELL_DOMINATE_MIND                     = 71289,
};

struct MANGOS_DLL_DECL boss_deathwhisperAI : public LibDevFSAI
{
    boss_deathwhisperAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(TEN_MINS);
		AddHealEvent(SPELL_DARK_EMPOWERMENT,20000,60000,2000,1);
		AddPhase1Event(SPELL_SHADOW_BOLT,3000,4000);
		AddMaxPrioEvent(SPELL_DEATH_AND_DECAY,25000,30000);
		AddPhase2Event(SPELL_FROSTBOLT,5000,5000,1000);
		AddPhase2Event(SPELL_FROSTBOLT_VOLLEY,6000,20000,1000);
		AddEventMaxPrioOnTank(SPELL_INSIGNIFICANCE,8000,10000,1000,2);
		AddTextEvent(16872,"Cette supercherie n'a que trop duré !",TEN_MINS,DAY*HOUR);
    }
    uint32 Summon_Cult_Timer;
	uint32 Shade_Timer;
    uint8 Phase;
    bool SpawnLeft;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		ActivateManualMoveSystem();
		SetMovePhase(false);
        Summon_Cult_Timer = 5000;
		Shade_Timer = 12000;
        Phase = 1;
        SpawnLeft = true;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            SetInstanceData(TYPE_DEATHWHISPER, IN_PROGRESS);

		DoCastMe(SPELL_MANA_BARRIER);
		Yell(16868,"Quelle est cette perturbation ? Vous osez profaner cette terre sacrée ? Elle deviendra votre sépulture !");
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Yell(16869,"Saisissez vous maintenant la futilité de vos actes ?");
		else
			Yell(16870,"Acceptez les ténèbres... éternelles !");
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            SetInstanceData(TYPE_DEATHWHISPER, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,2);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,1);
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
		}

		Yell(16871,"Le maître avait tout prévu... votre fin est... inévitable...");
    }

    void JustReachedHome()
    {
        if (pInstance)
            SetInstanceData(TYPE_DEATHWHISPER, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (me->HasAura(SPELL_MANA_BARRIER))
        {
			if(damage >= me->GetPower(POWER_MANA))
				me->SetPower(POWER_MANA,0);
			else
				me->SetPower(POWER_MANA,me->GetPower(POWER_MANA)-damage);
			damage = 0;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (Phase == 1)
        {
            if ((me->GetPower(POWER_MANA)*100 / me->GetMaxPower(POWER_MANA)) < 1)
            {
                Phase = 2;
				me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
				Yell(16877,"Assez ! Je vois qu'il faut que je prenne la situation en main !");
				DoResetThreat();
				SetMovePhase();
                return;
            }
			else if(!me->HasAura(SPELL_MANA_BARRIER))
			{
				DoCastMe(SPELL_MANA_BARRIER);
			}

            if (Summon_Cult_Timer < diff)
            {
				if(m_difficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_difficulty == RAID_DIFFICULTY_10MAN_NORMAL)
				{
					if (SpawnLeft)
					{
						CallCreature(NPC_CULT_FANATIC,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_1X,ADD_1Y,ADD_1Z);
						CallCreature(NPC_CULT_ADHERENT,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_2X,ADD_2Y,ADD_2Z);
						CallCreature(NPC_CULT_FANATIC,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_3X,ADD_3Y,ADD_3Z);
						SpawnLeft = false;
					}
					else
					{
						CallCreature(NPC_CULT_ADHERENT,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_4X,ADD_4Y,ADD_4Z);
						CallCreature(NPC_CULT_FANATIC,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_5X,ADD_5Y,ADD_5Z);
						CallCreature(NPC_CULT_ADHERENT,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_6X,ADD_6Y,ADD_6Z);
						SpawnLeft = true;
					}
				}
				else
				{
					CallCreature(NPC_CULT_FANATIC,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_1X,ADD_1Y,ADD_1Z);
					CallCreature(NPC_CULT_ADHERENT,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_2X,ADD_2Y,ADD_2Z);
					CallCreature(NPC_CULT_FANATIC,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_3X,ADD_3Y,ADD_3Z);
					CallCreature(NPC_CULT_ADHERENT,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_4X,ADD_4Y,ADD_4Z);
					CallCreature(NPC_CULT_FANATIC,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_5X,ADD_5Y,ADD_5Z);
					CallCreature(NPC_CULT_ADHERENT,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_6X,ADD_6Y,ADD_6Z);
					CallCreature(urand(0,1) ? NPC_CULT_ADHERENT : NPC_CULT_FANATIC,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,-547.631f,2212.955f,53.25f);
				}

                Summon_Cult_Timer = 60000;
            }
            else Summon_Cult_Timer -= diff;
        }

        if (Phase == 2)
        {
            if (me->HasAura(SPELL_MANA_BARRIER))
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);

			if(Shade_Timer <= diff)
			{
				if(Unit* target = GetRandomUnit(1))
				{
					if(Creature* shade = CallCreature(NPC_VENGEFUL_SHADE,20000,PREC_COORDS,NOTHING,target->GetPositionX() + irand(-10,10), 
						target->GetPositionY() + irand(-10,10),target->GetPositionZ() + 0.5f))
					{
						shade->AddThreat(target,10000000.0f,true);
					}
				}
				Shade_Timer = 20000;
			}
			else
				Shade_Timer -= diff;

			DoMeleeAttackIfReady();
        }

		UpdateEvent(diff,Phase);
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_deathwhisper(Creature* pCreature)
{
    return new boss_deathwhisperAI(pCreature);
}

struct MANGOS_DLL_DECL icc_dw_shadeAI : public LibDevFSAI
{
    icc_dw_shadeAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		ModifyAuraStack(SPELL_VENGEFUL_BLAST);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_icc_dw_shade(Creature* pCreature)
{
    return new icc_dw_shadeAI(pCreature);
}

struct MANGOS_DLL_DECL icc_dw_cult_adherentAI : public LibDevFSAI
{
    icc_dw_cult_adherentAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(55095,5000,15000,1000);
		AddEventOnMe(70768,1000,30000);
		AddEvent(72005,500,2500,500);
		AddEvent(71237,7000,10000);
		AddEventMaxPrioOnMe(70901,35000,60000,0,1);
		AddEventOnTank(70906,12000,20000,1000,1);
    }

	uint8 Phase;
	uint32 switchPhase_Timer;

    void Reset()
    {
		ResetTimers();
		switchPhase_Timer = urand(20000,80000);
		Phase = 1;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(switchPhase_Timer <= diff)
		{
			if(Phase == 1)
			{
				me->CastStop();
				DoCastMe(70903);
				if(Creature* Dw = GetInstanceCreature(TYPE_DEATHWHISPER))
					Yell(16875,"Lève toi, dans l'exultation de cette nouvelle pureté.",Dw);
				switchPhase_Timer = 5000;
			}
			else if(Phase == 2)
			{
				me->CastStop();
				DoCastMe(71234);
				switchPhase_Timer = DAY*HOUR;
			}
			Phase++;
		}
		else
			switchPhase_Timer -= diff;

		UpdateEvent(diff);
		UpdateEvent(diff,Phase);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_icc_dw_cult_adherent(Creature* pCreature)
{
    return new icc_dw_cult_adherentAI(pCreature);
}

struct MANGOS_DLL_DECL icc_dw_cult_fanaticAI : public LibDevFSAI
{
    icc_dw_cult_fanaticAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(70659,2000,3000);
		AddEventOnMe(70674,24000,60000);
		AddEventOnTank(70670,5000,5000,1000);
		AddEventMaxPrioOnMe(70900,20000,60000,1000);
    }

	uint8 Phase;
	uint32 switchPhase_Timer;

    void Reset()
    {
		ResetTimers();
		switchPhase_Timer = urand(20000,80000);
		Phase = 1;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(switchPhase_Timer <= diff)
		{
			if(Phase == 1)
			{
				me->CastStop();
				DoCastMe(70903);
				if(Creature* Dw = GetInstanceCreature(TYPE_DEATHWHISPER))
					Yell(16874,"Loyal partisan, je te libère de la malédiction de la chair !",Dw);
				switchPhase_Timer = 5000;
			}
			else if(Phase == 2)
			{
				me->CastStop();
				DoCastMe(71235);
				switchPhase_Timer = DAY*HOUR;
			}
			Phase++;
		}
		else
			switchPhase_Timer -= diff;

		UpdateEvent(diff);
		UpdateEvent(diff,Phase);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_icc_dw_cult_fanatic(Creature* pCreature)
{
    return new icc_dw_cult_fanaticAI(pCreature);
}

void AddSC_boss_deathwhisper()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_deathwhisper";
    NewScript->GetAI = &GetAI_boss_deathwhisper;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_dw_shade";
    NewScript->GetAI = &GetAI_icc_dw_shade;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_dw_cult_adherent";
    NewScript->GetAI = &GetAI_icc_dw_cult_adherent;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_dw_cult_fanatic";
    NewScript->GetAI = &GetAI_icc_dw_cult_fanatic;
    NewScript->RegisterSelf();
}
