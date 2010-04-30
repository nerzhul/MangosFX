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
SDName: Boss_Anubshiah
SD%Complete: 100
SDComment:
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"

#define SPELL_SHADOWBOLT            17228
#define SPELL_CURSEOFTONGUES        15470
#define SPELL_CURSEOFWEAKNESS       17227
#define SPELL_DEMONARMOR            11735
#define SPELL_ENVELOPINGWEB         15471

struct MANGOS_DLL_DECL boss_anubshiahAI : public ScriptedAI
{
    boss_anubshiahAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_SHADOWBOLT,7000,7000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_CURSEOFTONGUES,24000,18000);
		Tasks.AddEvent(SPELL_CURSEOFWEAKNESS,12000,45000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_DEMONARMOR,3000,300000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_ENVELOPINGWEB,16000,12000);
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
CreatureAI* GetAI_boss_anubshiah(Creature* pCreature)
{
    return new boss_anubshiahAI(pCreature);
}

void AddSC_boss_anubshiah()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anubshiah";
    newscript->GetAI = &GetAI_boss_anubshiah;
    newscript->RegisterSelf();
}
