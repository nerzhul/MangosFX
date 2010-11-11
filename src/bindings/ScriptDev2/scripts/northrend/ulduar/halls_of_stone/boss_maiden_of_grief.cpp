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
SDName: Boss_Maiden_of_Grief
SD%Complete: 20%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                   = -1599005,
    SAY_SLAY_1                  = -1599006,
    SAY_SLAY_2                  = -1599007,
    SAY_SLAY_3                  = -1599008,
    SAY_SLAY_4                  = -1599009,
    SAY_STUN                    = -1599010,
    SAY_DEATH                   = -1599011,
	SPELL_PARTINGSORROW_H		= 59723,
	SPELL_PILLAROFWOE_N			= 50761,
	SPELL_PILLAROFWOE_H			= 59727,
	SPELL_SHOCKOFSORROW_N		= 50760,
	SPELL_SHOCKOFSORROW_H		= 59726,
	SPELL_STORMOFGRIEF_N		= 50752,
	SPELL_STORMOFGRIEF_H		= 59772,
};

/*######
## boss_maiden_of_grief
######*/

struct MANGOS_DLL_DECL boss_maiden_of_griefAI : public ScriptedAI
{
	uint32 PartingSorrow_Timer;
	uint32 Pillar_Timer;
	uint32 Shock_Timer;
	uint32 Storm_Timer;

	Unit* target;

    boss_maiden_of_griefAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    bool m_bIsHeroic;

    void Reset()
    {
		PartingSorrow_Timer = 12000;
		Pillar_Timer = 8000;
		Shock_Timer	= 15000;
		Storm_Timer = 18000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SLAY_3, me); break;
            case 3: DoScriptText(SAY_SLAY_4, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;


		if(m_bIsHeroic && PartingSorrow_Timer <= diff)
		{
			DoCastRandom(SPELL_PARTINGSORROW_H);
			PartingSorrow_Timer = urand (12500,14500);
		}
		else
			PartingSorrow_Timer -= diff;

		if(Pillar_Timer <= diff)
		{
			DoCastVictim(m_bIsHeroic ? SPELL_PILLAROFWOE_H : SPELL_PILLAROFWOE_N);
			Pillar_Timer = urand(9000,13000);
		}
		else
			Pillar_Timer -= diff;

		if(Shock_Timer <= diff)
		{
			DoCastVictim(m_bIsHeroic ? SPELL_SHOCKOFSORROW_H : SPELL_SHOCKOFSORROW_N);
			Shock_Timer = urand(10000,12000);
		}
		else
			Shock_Timer -= diff;

		if(Storm_Timer <= diff)
		{
			DoCastVictim(m_bIsHeroic ? SPELL_STORMOFGRIEF_H : SPELL_STORMOFGRIEF_N);
			Storm_Timer = urand(16000,18000);
		}
		else
			Storm_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_maiden_of_grief(Creature* pCreature)
{
    return new boss_maiden_of_griefAI(pCreature);
}

void AddSC_boss_maiden_of_grief()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_maiden_of_grief";
    newscript->GetAI = &GetAI_boss_maiden_of_grief;
    newscript->RegisterSelf();
}
