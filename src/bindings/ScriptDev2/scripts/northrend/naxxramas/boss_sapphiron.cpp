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
SDName: Boss_Sapphiron
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"

#define EMOTE_BREATH            -1533082
#define EMOTE_ENRAGE            -1533083

enum
{
	SPELL_ICEBOLT          = 28522,
	SPELL_FROST_BREATH     = 29318,
	SPELL_FROST_AURA       = 28531,
	SPELL_FROST_AURA_H	   = 55799,
	SPELL_LIFE_DRAIN       = 28542,
	SPELL_LIFE_DRAIN_H	   = 55665,
	SPELL_BLIZZARD         = 28547,
	SPELL_BLIZZARD_H	   = 55699,
	SPELL_BERSERK          = 26662,
	SPELL_ENCHAINEMENT	   = 19983,
	SPELL_QUEUE			   = 55697,
	SPELL_QUEUE_H		   = 55696,
};

struct MANGOS_DLL_DECL boss_sapphironAI : public ScriptedAI
{
    boss_sapphironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
		m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
    }

	bool m_bIsHeroic;
    uint8 Icebolt_Count;
    uint32 Icebolt_Timer;
    uint32 FrostBreath_Timer;
    uint32 Fly_Timer;
    uint32 phase;
    bool landoff;
    uint32 land_Timer;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(!m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_QUEUE_H,5000,5000,0,TARGET_MAIN,1);
			Tasks.AddEvent(SPELL_FROST_AURA_H,2000,5000,0,TARGET_ME,1);
			Tasks.AddEvent(SPELL_BLIZZARD_H,20000,20000,0,TARGET_RANDOM,1);
			Tasks.AddEvent(SPELL_LIFE_DRAIN_H,24000,24000,0,TARGET_RANDOM,1,0,false,5);
		}
		else
		{
			Tasks.AddEvent(SPELL_QUEUE,5000,5000,0,TARGET_MAIN,1);
			Tasks.AddEvent(SPELL_FROST_AURA,2000,5000,0,TARGET_ME,1);
			Tasks.AddEvent(SPELL_BLIZZARD,20000,20000,0,TARGET_RANDOM,1);
			Tasks.AddEvent(SPELL_LIFE_DRAIN,24000,24000,0,TARGET_RANDOM,1,0,false,2);
		}
		Tasks.AddEvent(SPELL_ENCHAINEMENT,6000,8000,0,TARGET_MAIN,1);
		Tasks.AddEvent(SPELL_BERSERK,900000,60000,0,TARGET_ME,0,EMOTE_ENRAGE);

        FrostBreath_Timer = 2500;
        Fly_Timer = 45000;
        Icebolt_Timer = 4000;
        land_Timer = 2000;
        phase = 1;
        Icebolt_Count = 0;
        landoff = false;

        //me->ApplySpellMod(SPELL_FROST_AURA, SPELLMOD_DURATION, -1);
    }
	void JustDied(Unit *victim)
	{
		Tasks.GiveEmblemsToGroup((!m_bIsHeroic) ? VAILLANCE : HEROISME, 2);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        if (phase == 1)
        {
			Tasks.UpdateEvent(diff,phase);

            if (me->GetHealth()*100 / me->GetMaxHealth() > 10)
            {
                if (Fly_Timer < diff)
                {
                    phase = 2;
                    me->InterruptNonMeleeSpells(false);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveIdle();
                    DoCastMe(11010);
                    me->SetHover(true);
                    DoCastMe(18430);
                    Icebolt_Timer = 4000;
                    Icebolt_Count = 0;
                    landoff = false;
                }
				else
					Fly_Timer -= diff;
            }
        }
		else if (phase == 2)
        {
			if (Icebolt_Timer < diff && Icebolt_Count < ((m_bIsHeroic) ? 3:2))
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
					DoCast(target,SPELL_ICEBOLT);

                ++Icebolt_Count;
                Icebolt_Timer = 4000;
            }
			else
				Icebolt_Timer -= diff;

            if (Icebolt_Count == ((m_bIsHeroic) ? 3:2) && !landoff)
            {
                if (FrostBreath_Timer < diff)
                {
                    DoScriptText(EMOTE_BREATH, me);
                    DoCastVictim(SPELL_FROST_BREATH);
                    land_Timer = 2000;
                    landoff = true;
					Icebolt_Count = 0;
                    FrostBreath_Timer = 6000;
                }
				else
					FrostBreath_Timer -= diff;
            }

            if (landoff)
            {
                if (land_Timer < diff)
                {
                    phase = 1;
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                    me->SetHover(false);
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveChase(me->getVictim());
                    Fly_Timer = 67000;
                }
				else
					land_Timer -= diff;
            }
        }

		Tasks.UpdateEvent(diff);
        
        if (phase!=2)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sapphiron(Creature* pCreature)
{
    return new boss_sapphironAI(pCreature);
}

void AddSC_boss_sapphiron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_sapphiron";
    newscript->GetAI = &GetAI_boss_sapphiron;
    newscript->RegisterSelf();
}
