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
SDName: Boss_Grizzle
SD%Complete: 100
SDComment:
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"

#define EMOTE_GENERIC_FRENZY_KILL   -1000001

#define SPELL_GROUNDTREMOR          6524
#define SPELL_FRENZY                28371

struct MANGOS_DLL_DECL boss_grizzleAI : public ScriptedAI
{
    boss_grizzleAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Frenzy_Timer;
    MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_GROUNDTREMOR,12000,8000,0,TARGET_MAIN);
        Frenzy_Timer =0;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //Frenzy_Timer
        if (CheckPercentLife(51))
        {
            if (Frenzy_Timer < diff)
            {
                DoCastMe(SPELL_FRENZY);
                DoScriptText(EMOTE_GENERIC_FRENZY_KILL, me);
                Frenzy_Timer = 15000;
            }else Frenzy_Timer -= diff;
        }
        
        Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_grizzle(Creature* pCreature)
{
    return new boss_grizzleAI(pCreature);
}

void AddSC_boss_grizzle()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_grizzle";
    newscript->GetAI = &GetAI_boss_grizzle;
    newscript->RegisterSelf();
}
