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
};

enum BossSpells
{
    //summons
    NPC_REANIMATED_FANATIC                  = 38009,
    NPC_REANIMATED_ADHERENT                 = 38010,
    //Abilities
    SPELL_DOMINATE_MIND                     = 71289,

    SPELL_VENGEFUL_BLAST                    = 71494,
    SPELL_VENGEFUL_BLAST_0                  = 71544,
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
    }
    uint32 Summon_Cult_Timer;
	uint32 Shade_Timer;
    uint8 Phase;
    bool SpawnLeft;

    void Reset()
    {
		CleanMyAdds();
        Summon_Cult_Timer = 5000;
		Shade_Timer = 12000;
        Phase = 1;
        SpawnLeft = true;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_DEATHWHISPER, IN_PROGRESS);

		DoCastMe(SPELL_MANA_BARRIER);
    }

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_DEATHWHISPER, DONE);
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_DEATHWHISPER, FAIL);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    //Mana Barrier is bugged so we override it for now
    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (me->HasAura(SPELL_MANA_BARRIER))
        {
            me->SetHealth(me->GetHealth()+damage);
            me->SetPower(POWER_MANA,me->GetPower(POWER_MANA)-damage);
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
				DoResetThreat();
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

            DoStartNoMovement(me->getVictim());
        }

        if (Phase == 2)
        {
            if (me->HasAura(SPELL_MANA_BARRIER))
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);

			if(Shade_Timer <= diff)
			{
				if(Unit* target = GetRandomUnit(/*2*/0))
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
		SetAuraStack(71494,1,me,me,1);
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
}
