/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
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

#include "ByteBuffer.h"
#include "ReactorAI.h"
#include "Errors.h"
#include "Creature.h"
#include "Log.h"
#include "ObjectAccessor.h"

#define REACTOR_VISIBLE_RANGE (26.46f)

int
ReactorAI::Permissible(const Creature *creature)
{
    if( creature->isCivilian() || creature->IsNeutralToAll() )
        return PERMIT_BASE_REACTIVE;

    return PERMIT_BASE_NO;
}

void
ReactorAI::MoveInLineOfSight(Unit *)
{
}

void
ReactorAI::AttackStart(Unit *p)
{
    if(!p)
        return;

    if(me->Attack(p,true))
    {
        DEBUG_LOG("Tag unit GUID: %u (TypeId: %u) as a victim", p->GetGUIDLow(), p->GetTypeId());
        i_victimGuid = p->GetGUID();
        me->AddThreat(p);

        me->SetInCombatWith(p);
        p->SetInCombatWith(me);

        me->GetMotionMaster()->MoveChase(p);
    }
}

bool
ReactorAI::IsVisible(Unit *) const
{
    return false;
}

void
ReactorAI::UpdateAI(const uint32 /*time_diff*/)
{
    // update i_victimGuid if i_creature.getVictim() !=0 and changed
    if(!me->SelectHostileTarget() || !me->getVictim())
        return;

    i_victimGuid = me->getVictim()->GetGUID();

    if( me->isAttackReady() )
    {
        if( me->IsWithinDistInMap(me->getVictim(), ATTACK_DISTANCE))
        {
            me->AttackerStateUpdate(me->getVictim());
            me->resetAttackTimer();
        }
    }
}

void
ReactorAI::EnterEvadeMode()
{
    if (!me->isAlive())
    {
        DEBUG_LOG("Creature stopped attacking, he is dead [guid=%u]", me->GetGUIDLow());
        me->GetMotionMaster()->MovementExpired();
        me->GetMotionMaster()->MoveIdle();
        i_victimGuid = 0;
        me->CombatStop(true);
        me->DeleteThreatList();
        return;
    }

    Unit* victim = ObjectAccessor::GetUnit(*me, i_victimGuid );

    if (!victim)
    {
        DEBUG_LOG("Creature stopped attacking, no victim [guid=%u]", me->GetGUIDLow());
    }
    else if (victim->HasStealthAura())
    {
        DEBUG_LOG("Creature stopped attacking, victim is in stealth [guid=%u]", me->GetGUIDLow());
    }
    else if (victim->isInFlight())
    {
        DEBUG_LOG("Creature stopped attacking, victim is in flight [guid=%u]", me->GetGUIDLow());
    }
    else
    {
        DEBUG_LOG("Creature stopped attacking, victim %s [guid=%u]", victim->isAlive() ? "out run him" : "is dead", me->GetGUIDLow());
    }

    me->RemoveAllAuras();
    me->DeleteThreatList();
    i_victimGuid = 0;
    me->CombatStop(true);
    me->SetLootRecipient(NULL);

    // Remove ChaseMovementGenerator from MotionMaster stack list, and add HomeMovementGenerator instead
    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
        me->GetMotionMaster()->MoveTargetedHome();
}
