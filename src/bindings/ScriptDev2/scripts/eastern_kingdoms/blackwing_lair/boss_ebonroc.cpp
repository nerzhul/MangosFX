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
SDName: Boss_Ebonroc
SD%Complete: 50
SDComment: Shadow of Ebonroc needs core support
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"

enum Spells
{
	SPELL_SHADOWFLAME          = 22539,
	SPELL_WINGBUFFET           = 18500,
	SPELL_SHADOWOFEBONROC      = 23340,
	SPELL_HEAL                 = 41386                   //Thea Heal spell of his Shadow
};

struct MANGOS_DLL_DECL boss_ebonrocAI : public LibDevFSAI
{
    boss_ebonrocAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_SHADOWFLAME,15000,12000,3000);
		AddEventOnTank(SPELL_WINGBUFFET,30000,25000);
		AddEventOnTank(SPELL_SHADOWOFEBONROC,45000,25000,10000);
	}

    uint32 Heal_Timer;

    void Reset()
    {
		ResetTimers();
        Heal_Timer = 1000;
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (me->getVictim()->HasAura(SPELL_SHADOWOFEBONROC,0))
        {
            if (Heal_Timer < diff)
            {
                DoCastMe(SPELL_HEAL);
                Heal_Timer = urand(1000, 3000);
            }else Heal_Timer -= diff;
        }
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_ebonroc(Creature* pCreature)
{
    return new boss_ebonrocAI(pCreature);
}

void AddSC_boss_ebonroc()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_ebonroc";
    newscript->GetAI = &GetAI_boss_ebonroc;
    newscript->RegisterSelf();
}
