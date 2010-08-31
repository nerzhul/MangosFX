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

#include "PetAI.h"
#include "Errors.h"
#include "Pet.h"
#include "Player.h"
#include "DBCStores.h"
#include "Spell.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "World.h"
#include "Util.h"

int PetAI::Permissible(const Creature *creature)
{
    if( creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

PetAI::PetAI(Creature *c) : CreatureAI(c), i_tracker(TIME_INTERVAL_LOOK), inCombat(false)
{
    m_AllySet.clear();
    UpdateAllies();
}

void PetAI::MoveInLineOfSight(Unit *u)
{
    if (me->getVictim())
        return;

    if (me->isPet() && ((Pet*)me)->GetModeFlags() & PET_MODE_DISABLE_ACTIONS)
        return;

    if (!me->GetCharmInfo() || !me->GetCharmInfo()->HasReactState(REACT_AGGRESSIVE))
        return;

    if (u->isTargetableForAttack() && me->IsHostileTo( u ) &&
        u->isInAccessablePlaceFor(me))
    {
        float attackRadius = me->GetAttackDistance(u);
        if(me->IsWithinDistInMap(u, attackRadius) && me->GetDistanceZ(u) <= CREATURE_Z_ATTACK_RANGE)
        {
            if(me->IsWithinLOSInMap(u))
            {
                AttackStart(u);
                u->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            }
        }
    }
}

void PetAI::AttackStart(Unit *u)
{
    if(!u || (me->isPet() && ((Pet*)me)->getPetType() == MINI_PET))
        return;

    if(me->Attack(u,true))
    {
        // TMGs call CreatureRelocation which via MoveInLineOfSight can call this function
        // thus with the following clear the original TMG gets invalidated and crash, doh
        // hope it doesn't start to leak memory without this :-/
        //i_pet->Clear();
        me->GetMotionMaster()->MoveChase(u);
        inCombat = true;
    }
}

void PetAI::EnterEvadeMode()
{
}

bool PetAI::IsVisible(Unit *pl) const
{
    return _isVisible(pl);
}

bool PetAI::_needToStop() const
{
    // This is needed for charmed creatures, as once their target was reset other effects can trigger threat
    if(me->isCharmed() && me->getVictim() == me->GetCharmer())
        return true;

    return !me->getVictim()->isTargetableForAttack();
}

void PetAI::_stopAttack()
{
    inCombat = false;
    if( !me->isAlive() )
    {
        DEBUG_LOG("PetAI (guid = %u) stopped attack, he is dead.", me->GetGUIDLow());
        me->StopMoving();
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->CombatStop();
        me->getHostileRefManager().deleteReferences();

        return;
    }

    Unit* owner = me->GetCharmerOrOwner();

    if(owner && me->GetCharmInfo() && me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
    {
        me->GetMotionMaster()->MoveFollow(owner,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
    }
    else
    {
        me->GetMotionMaster()->Clear(false);
        me->GetMotionMaster()->MoveIdle();
    }
    me->AttackStop();
}

void PetAI::UpdateAI(const uint32 diff)
{
    if (!me->isAlive())
        return;

    Unit* owner = me->GetCharmerOrOwner();

	// chained, use original owner instead
    if (owner && owner->GetTypeId() == TYPEID_UNIT && ((Creature*)owner)->GetEntry() == me->GetEntry())
        if (Unit *creator = me->GetCreator())
            owner = creator;

    if(m_updateAlliesTimer <= diff)
        // UpdateAllies self set update timer
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    if (inCombat && (!me->getVictim() || me->isPet() && ((Pet*)me)->GetModeFlags() & PET_MODE_DISABLE_ACTIONS))
        _stopAttack();

    // i_pet.getVictim() can't be used for check in case stop fighting, i_pet.getVictim() clear at Unit death etc.
    if (me->getVictim())
    {
        if (_needToStop())
        {
            DEBUG_LOG("PetAI (guid = %u) is stopping attack.", me->GetGUIDLow());
            _stopAttack();
            return;
        }
        else if (me->IsStopped() || me->IsWithinDistInMap(me->getVictim(), ATTACK_DISTANCE))
        {
            // required to be stopped cases
            if (me->IsStopped() && me->IsNonMeleeSpellCasted(false))
            {
                if (me->hasUnitState(UNIT_STAT_FOLLOW_MOVE))
                    me->InterruptNonMeleeSpells(false);
                else
                    return;
            }
            // not required to be stopped case
            else if (me->isAttackReady() && me->canReachWithAttack(me->getVictim()))
            {
                me->AttackerStateUpdate(me->getVictim());

                me->resetAttackTimer();

                if (!me->getVictim())
                    return;

                //if pet misses its target, it will also be the first in threat list
                me->getVictim()->AddThreat(me);

                if( _needToStop() )
                    _stopAttack();
            }
        }
    }
    else if (owner && me->GetCharmInfo())
    {
        if (owner->isInCombat() && !(me->GetCharmInfo()->HasReactState(REACT_PASSIVE) || me->GetCharmInfo()->HasCommandState(COMMAND_STAY)))
        {
            AttackStart(owner->getAttackerForHelper());
        }
        else if(me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
        {
            if (!me->hasUnitState(UNIT_STAT_FOLLOW) )
            {
                me->GetMotionMaster()->MoveFollow(owner,PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
            }
        }
    }

    // Autocast (casted only in combat or persistent spells in any state)
    if (me->GetGlobalCooldown() == 0 && !me->IsNonMeleeSpellCasted(false))
    {
        typedef std::vector<std::pair<Unit*, Spell*> > TargetSpellList;
        TargetSpellList targetSpellStore;

		if(me->isVehicle())
			return;
			
        for (uint8 i = 0; i < me->GetPetAutoSpellSize(); ++i)
        {
            uint32 spellID = me->GetPetAutoSpellOnPos(i);
            if (!spellID)
                continue;

            SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellID);
            if (!spellInfo)
                continue;

            // ignore some combinations of combat state and combat/noncombat spells
            if (!inCombat)
            {
                // ignore attacking spells, and allow only self/around spells
                if (!IsPositiveSpell(spellInfo->Id))
                    continue;

                // non combat spells allowed
                // only pet spells have IsNonCombatSpell and not fit this reqs:
                // Consume Shadows, Lesser Invisibility, so ignore checks for its
                if (!IsNonCombatSpell(spellInfo))
                {
                    // allow only spell without spell cost or with spell cost but not duration limit
                    int32 duration = GetSpellDuration(spellInfo);
                    if ((spellInfo->manaCost || spellInfo->ManaCostPercentage || spellInfo->manaPerSecond) && duration > 0)
                        continue;

                    // allow only spell without cooldown > duration
                    int32 cooldown = GetSpellRecoveryTime(spellInfo);
                    if (cooldown >= 0 && duration >= 0 && cooldown > duration)
                        continue;
                }
            }
            else
            {
                // just ignore non-combat spells
                if (IsNonCombatSpell(spellInfo))
                    continue;
            }

            Spell *spell = new Spell(me, spellInfo, false, 0);

            if (inCombat && !me->hasUnitState(UNIT_STAT_FOLLOW) && spell->CanAutoCast(me->getVictim()))
            {
                targetSpellStore.push_back(std::make_pair<Unit*, Spell*>(me->getVictim(), spell));
                continue;
            }
            else
            {
                bool spellUsed = false;
                for(std::set<uint64>::const_iterator tar = m_AllySet.begin(); tar != m_AllySet.end(); ++tar)
                {
                    Unit* Target = ObjectAccessor::GetUnit(*me,*tar);

                    //only buff targets that are in combat, unless the spell can only be cast while out of combat
                    if(!Target)
                        continue;

                    if(spell->CanAutoCast(Target))
                    {
                        targetSpellStore.push_back(std::make_pair<Unit*, Spell*>(Target, spell));
                        spellUsed = true;
                        break;
                    }
                }
                if (!spellUsed)
                    delete spell;
            }
        }

        //found units to cast on to
        if (!targetSpellStore.empty())
        {
            uint32 index = urand(0, targetSpellStore.size() - 1);

            Spell* spell  = targetSpellStore[index].second;
            Unit*  target = targetSpellStore[index].first;

            targetSpellStore.erase(targetSpellStore.begin() + index);

            SpellCastTargets targets;
            targets.setUnitTarget( target );

            if (!me->HasInArc(M_PI, target))
            {
                me->SetInFront(target);
                if (target->GetTypeId() == TYPEID_PLAYER)
                    me->SendCreateUpdateToPlayer((Player*)target);

                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    me->SendCreateUpdateToPlayer( (Player*)owner );
            }

            me->AddCreatureSpellCooldown(spell->m_spellInfo->Id);

            spell->prepare(&targets);
        }

        // deleted cached Spell objects
        for(TargetSpellList::const_iterator itr = targetSpellStore.begin(); itr != targetSpellStore.end(); ++itr)
            delete itr->second;
    }
}

bool PetAI::_isVisible(Unit *u) const
{
    return me->IsWithinDist(u,sWorld.getConfig(CONFIG_SIGHT_GUARDER))
        && u->isVisibleForOrDetect(me,me,true);
}

void PetAI::UpdateAllies()
{
    Unit* owner = me->GetCharmerOrOwner();
    Group *pGroup = NULL;

    m_updateAlliesTimer = 10*IN_MILLISECONDS;                //update friendly targets every 10 seconds, lesser checks increase performance

    if(!owner)
        return;
    else if(owner->GetTypeId() == TYPEID_PLAYER)
        pGroup = ((Player*)owner)->GetGroup();

    //only pet and owner/not in group->ok
    if(m_AllySet.size() == 2 && !pGroup)
        return;
    //owner is in group; group members filled in already (no raid -> subgroupcount = whole count)
    if(pGroup && !pGroup->isRaidGroup() && m_AllySet.size() == (pGroup->GetMembersCount() + 2))
        return;

    m_AllySet.clear();
    m_AllySet.insert(me->GetGUID());
    if(pGroup)                                              //add group
    {
        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();
            if(!Target || !pGroup->SameSubGroup((Player*)owner, Target))
                continue;

            if(Target->GetGUID() == owner->GetGUID())
                continue;

            m_AllySet.insert(Target->GetGUID());
        }
    }
    else                                                    //remove group
        m_AllySet.insert(owner->GetGUID());
}

void PetAI::AttackedBy(Unit *attacker)
{
    //when attacked, fight back in case 1)no victim already AND 2)not set to passive AND 3)not set to stay, unless can it can reach attacker with melee attack anyway
    if(!me->getVictim() && me->GetCharmInfo() && !me->GetCharmInfo()->HasReactState(REACT_PASSIVE) &&
        (!me->GetCharmInfo()->HasCommandState(COMMAND_STAY) || me->canReachWithAttack(attacker)))
        AttackStart(attacker);
}
