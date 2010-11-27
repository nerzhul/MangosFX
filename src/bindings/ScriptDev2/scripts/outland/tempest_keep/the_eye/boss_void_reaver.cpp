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
SDName: Boss_Void_Reaver
SD%Complete: 90
SDComment: Should reset if raid are out of room.
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "precompiled.h"
#include "the_eye.h"

#define SAY_AGGRO                   -1550000
#define SAY_SLAY1                   -1550001
#define SAY_SLAY2                   -1550002
#define SAY_SLAY3                   -1550003
#define SAY_DEATH                   -1550004
#define SAY_POUNDING1               -1550005
#define SAY_POUNDING2               -1550006

#define SPELL_POUNDING              34162
#define SPELL_ARCANE_ORB_MISSILE    34172
#define SPELL_KNOCK_AWAY            25778
#define SPELL_BERSERK               26662

//Unknown function. If target not found, this will be created and used as dummy target instead?
//#define CREATURE_ORB_TARGET         19577

struct MANGOS_DLL_DECL boss_void_reaverAI : public ScriptedAI
{
    boss_void_reaverAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        Reset();
    }

    uint32 Pounding_Timer;
    uint32 ArcaneOrb_Timer;
    uint32 KnockAway_Timer;
    uint32 Berserk_Timer;

    void Reset()
    {
        Pounding_Timer = 15000;
        ArcaneOrb_Timer = 3000;
        KnockAway_Timer = 30000;
        Berserk_Timer = 600000;

        if (pInstance && me->isAlive())
            pInstance->SetData(TYPE_VOIDREAVER, NOT_STARTED);
    }

    void KilledUnit(Unit *victim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, me); break;
            case 1: DoScriptText(SAY_SLAY2, me); break;
            case 2: DoScriptText(SAY_SLAY3, me); break;
        }
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_VOIDREAVER, DONE);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
        me->SetInCombatWithZone();

        if (pInstance)
            pInstance->SetData(TYPE_VOIDREAVER, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        // Pounding
        if (Pounding_Timer < diff)
        {
            DoCastVictim(SPELL_POUNDING);
            DoScriptText(urand(0, 1) ? SAY_POUNDING1 : SAY_POUNDING2, me);

            Pounding_Timer = 15000;                         //cast time(3000) + cooldown time(12000)
        }else Pounding_Timer -= diff;

        // Arcane Orb
        if (ArcaneOrb_Timer < diff)
        {
            Unit *target = NULL;
            std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
            std::vector<Unit *> target_list;
            for(std::list<HostileReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
                target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                // exclude pets & totems
                if (target->GetTypeId() != TYPEID_PLAYER)
                    continue;

                //18 yard radius minimum
                if (target && !target->IsWithinDist(me, 18.0f, false))
                    target_list.push_back(target);

                target = NULL;
            }

            if (target_list.size())
                target = *(target_list.begin()+rand()%target_list.size());
            else
                target = me->getVictim();

            if (target)
                DoCast(target, SPELL_ARCANE_ORB_MISSILE);

            ArcaneOrb_Timer = 3000;
        }else ArcaneOrb_Timer -= diff;

        // Single Target knock back, reduces aggro
        if (KnockAway_Timer < diff)
        {
            DoCastVictim(SPELL_KNOCK_AWAY);

            //Drop 25% aggro
            if (me->getThreatManager().getThreat(me->getVictim()))
                me->getThreatManager().modifyThreatPercent(me->getVictim(),-25);

            KnockAway_Timer = 30000;
        }else KnockAway_Timer -= diff;

        //Berserk
        if (Berserk_Timer < diff)
        {
            if (me->IsNonMeleeSpellCasted(false))
                me->InterruptNonMeleeSpells(false);

            DoCastMe(SPELL_BERSERK);
            Berserk_Timer = 600000;
        }else Berserk_Timer -= diff;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);
    }
};

CreatureAI* GetAI_boss_void_reaver(Creature* pCreature)
{
    return new boss_void_reaverAI(pCreature);
}

void AddSC_boss_void_reaver()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_void_reaver";
    newscript->GetAI = &GetAI_boss_void_reaver;
    newscript->RegisterSelf();
}
