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
SDName: Boss_Sjonnir
SD%Complete: 20%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                   = -1599000,
    SAY_SLAY_1                  = -1599001,
    SAY_SLAY_2                  = -1599002,
    SAY_SLAY_3                  = -1599003,
    SAY_DEATH                   = -1599004,
    EMOTE_GENERIC_FRENZY        = -1000002,
	SPELL_CHAINLIGHTNING_N		= 50830,
	SPELL_CHAINLIGHTNING_H		= 59844,
	SPELL_FRENZY				= 28747,
	SPELL_LIGHTNINGRING_N		= 51849,
	SPELL_LIGHTNINGRING_H		= 59861,
	SPELL_LIGHTNINGSHIELD_N		= 50831,
	SPELL_LIGHTNINGSHIELD_H		= 59845,
	SPELL_STATICCHARGE_N		= 50834,
	SPELL_STATICCHARGE_H		= 59846,
};

/*######
## boss_sjonnir
######*/

struct MANGOS_DLL_DECL boss_sjonnirAI : public ScriptedAI
{
	uint32 enrage_Timer;
	uint32 Chain_Timer;
	uint32 Ring_Timer;
	uint32 Shield_Timer;
	uint32 Charge_Timer;

	Unit* target;

    boss_sjonnirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
		enrage_Timer = 180000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SLAY_3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

		if(enrage_Timer <= uiDiff)
		{
			DoCastMe(SPELL_FRENZY);
			enrage_Timer = 240000;
		}
		else
			enrage_Timer -= uiDiff;

		if(Chain_Timer <= uiDiff)
		{
			target = SelectUnit(SELECT_TARGET_RANDOM, 0);
			DoCast(target, m_bIsHeroic ? SPELL_CHAINLIGHTNING_H : SPELL_CHAINLIGHTNING_N);
			Chain_Timer = urand(8000,10000);
		}
		else
			Chain_Timer -= uiDiff;

		if(Ring_Timer <= uiDiff)
		{
			target = SelectUnit(SELECT_TARGET_TOPAGGRO, 0);
			DoCast(target, m_bIsHeroic ? SPELL_LIGHTNINGRING_H : SPELL_LIGHTNINGRING_N);
			Ring_Timer = urand(13000,17000);
		}
		else
			Ring_Timer -= uiDiff;
		
		if(Shield_Timer <= uiDiff)
		{
			DoCastMe( m_bIsHeroic ? SPELL_LIGHTNINGSHIELD_H : SPELL_LIGHTNINGSHIELD_N);
			Shield_Timer = urand(28000,30000);
		}
		else
			Shield_Timer -= uiDiff;
		
		if(Charge_Timer <= uiDiff)
		{
			target = SelectUnit(SELECT_TARGET_RANDOM, 0);
			DoCast(target, m_bIsHeroic ? SPELL_STATICCHARGE_H : SPELL_STATICCHARGE_N);
			Charge_Timer = urand(12000,14000);
		}
		else
			Charge_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sjonnir(Creature* pCreature)
{
    return new boss_sjonnirAI(pCreature);
}

void AddSC_boss_sjonnir()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sjonnir";
    newscript->GetAI = &GetAI_boss_sjonnir;
    newscript->RegisterSelf();
}
