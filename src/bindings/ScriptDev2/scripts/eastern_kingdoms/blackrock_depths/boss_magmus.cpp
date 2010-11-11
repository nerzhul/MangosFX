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
SDName: Boss_Magmus
SD%Complete: 80
SDComment: Missing pre-event to open doors
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "blackrock_depths.h"

enum
{
    SPELL_FIERYBURST        = 13900,
    SPELL_WARSTOMP          = 24375
};

struct MANGOS_DLL_DECL boss_magmusAI : public ScriptedAI
{
    boss_magmusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;

    uint32 m_uiWarStomp_Timer;
    
    MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_FIERYBURST,5000,6000,0,TARGET_MAIN);
        m_uiWarStomp_Timer = 0;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_IRON_HALL, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_IRON_HALL, FAIL);
    }

    void JustDied(Unit* pVictim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_IRON_HALL, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //WarStomp_Timer
        if (CheckPercentLife(51))
        {
            if (m_uiWarStomp_Timer < diff)
            {
                DoCastVictim(SPELL_WARSTOMP);
                m_uiWarStomp_Timer = 8000;
            }
            else
                m_uiWarStomp_Timer -= diff;
        }
        
        Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_magmus(Creature* pCreature)
{
    return new boss_magmusAI(pCreature);
}

void AddSC_boss_magmus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_magmus";
    newscript->GetAI = &GetAI_boss_magmus;
    newscript->RegisterSelf();
}
