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
SDName: Boss_Gorosh_the_Dervish
SD%Complete: 100
SDComment:
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"

#define SPELL_WHIRLWIND             15589
#define SPELL_MORTALSTRIKE          24573

struct MANGOS_DLL_DECL boss_gorosh_the_dervishAI : public ScriptedAI
{
    boss_gorosh_the_dervishAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_WHIRLWIND,12000,15000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_MORTALSTRIKE,22000,15000,0,TARGET_MAIN);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;
        
        Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_gorosh_the_dervish(Creature* pCreature)
{
    return new boss_gorosh_the_dervishAI(pCreature);
}

void AddSC_boss_gorosh_the_dervish()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gorosh_the_dervish";
    newscript->GetAI = &GetAI_boss_gorosh_the_dervish;
    newscript->RegisterSelf();
}
