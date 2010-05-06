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
SDName: Boss_Razorgore
SD%Complete: 50
SDComment: Needs additional review. Phase 1 NYI (Grethok the Controller)
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"

//Razorgore Phase 2 Script

#define SAY_EGGS_BROKEN1        -1469022
#define SAY_EGGS_BROKEN2        -1469023
#define SAY_EGGS_BROKEN3        -1469024
#define SAY_DEATH               -1469025

enum Spells
{
	SPELL_CLEAVE           = 22540,
	SPELL_WARSTOMP         = 24375,
	SPELL_FIREBALLVOLLEY   = 22425,
	SPELL_CONFLAGRATION    = 23023
};

struct MANGOS_DLL_DECL boss_razorgoreAI : public LibDevFSAI
{
    boss_razorgoreAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_CLEAVE,15000,7000,6000);
		AddEventOnTank(SPELL_WARSTOMP,35000,15000,10000);
		AddEventOnTank(SPELL_FIREBALLVOLLEY,7000,12000,3000);
    }

    uint32 Cleave_Timer;
    uint32 WarStomp_Timer;
    uint32 FireballVolley_Timer;
    uint32 Conflagration_Timer;

    void Reset()
    {
        ResetTimers();
        Conflagration_Timer = 12000;
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        //Conflagration_Timer
        if (Conflagration_Timer < diff)
        {
            DoCastVictim(SPELL_CONFLAGRATION);

            if (me->getThreatManager().getThreat(me->getVictim()))
				me->getThreatManager().modifyThreatPercent(me->getVictim(),-50);

            Conflagration_Timer = 12000;
        }else Conflagration_Timer -= diff;

        // Aura Check. If the gamer is affected by confliguration we attack a random gamer.
        if (me->getVictim()->HasAura(SPELL_CONFLAGRATION,0))
        {
            Unit* target = NULL;
            target = SelectUnit(SELECT_TARGET_RANDOM,1);
            if (target)
                me->TauntApply(target);
        }
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_razorgore(Creature* pCreature)
{
    return new boss_razorgoreAI(pCreature);
}

void AddSC_boss_razorgore()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_razorgore";
    newscript->GetAI = &GetAI_boss_razorgore;
    newscript->RegisterSelf();
}
