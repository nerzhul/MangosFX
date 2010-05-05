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
SDName: Boss_Highlord_Omokk
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"

enum Spells
{
	SPELL_WARSTOMP         = 24375,
	SPELL_CLEAVE           = 15579,
	SPELL_STRIKE           = 18368,
	SPELL_REND             = 18106,
	SPELL_SUNDERARMOR      = 24317,
	SPELL_KNOCKAWAY        = 20686,
	SPELL_SLOW             = 22356
};

struct MANGOS_DLL_DECL boss_highlordomokkAI : public LibDevFSAI
{
    boss_highlordomokkAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_WARSTOMP,15000,14000);
		AddEventOnTank(SPELL_CLEAVE,6000,8000);
		AddEventOnTank(SPELL_STRIKE,10000,10000);
		AddEventOnTank(SPELL_REND,14000,18000);
		AddEventOnTank(SPELL_SUNDERARMOR,2000,25000);
		AddEventOnTank(SPELL_KNOCKAWAY,18000,12000);
		AddEventOnTank(SPELL_SLOW,24000,18000);
	}

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_highlordomokk(Creature* pCreature)
{
    return new boss_highlordomokkAI(pCreature);
}

void AddSC_boss_highlordomokk()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_highlord_omokk";
    newscript->GetAI = &GetAI_boss_highlordomokk;
    newscript->RegisterSelf();
}
