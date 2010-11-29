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

#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "CreatureAI.h"
#include "Creature.h"
#include "DBCStores.h"
#include "WorldPacket.h"
#include "Player.h"
#include "Vehicle.h"

CreatureAI::~CreatureAI()
{
}

void CreatureAI::InitInstance()
{
	if(me->GetInstanceData())
		pInstance = me->GetInstanceData();
	else
		pInstance = NULL;
	m_difficulty = me->GetMap()->GetDifficulty();
}

void CreatureAI::AttackedBy( Unit* attacker )
{
	if(!me->getVictim())
        AttackStart(attacker);
}

CanCastResult CreatureAI::CanCastSpell(Unit* pTarget, const SpellEntry *pSpell, bool isTriggered)
{
    // If not triggered, we check
    if (!isTriggered)
    {
        // State does not allow
        if (me->hasUnitState(UNIT_STAT_CAN_NOT_REACT))
            return CAST_FAIL_STATE;

        if (pSpell->PreventionType == SPELL_PREVENTION_TYPE_SILENCE && me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            return CAST_FAIL_STATE;

        if (pSpell->PreventionType == SPELL_PREVENTION_TYPE_PACIFY && me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
            return CAST_FAIL_STATE;

        // Check for power (also done by Spell::CheckCast())
        if (me->GetPower((Powers)pSpell->powerType) < pSpell->manaCost)
            return CAST_FAIL_POWER;
    }

    if (const SpellRangeEntry *pSpellRange = sSpellRangeStore.LookupEntry(pSpell->rangeIndex))
    {
        if (pTarget != me)
        {
            // pTarget is out of range of this spell (also done by Spell::CheckCast())
            float fDistance = me->GetCombatDistance(pTarget);

            if (fDistance > (me->IsHostileTo(pTarget) ? pSpellRange->maxRange : pSpellRange->maxRangeFriendly))
                return CAST_FAIL_TOO_FAR;

            float fMinRange = me->IsHostileTo(pTarget) ? pSpellRange->minRange : pSpellRange->minRangeFriendly;

            if (fMinRange && fDistance < fMinRange)
                return CAST_FAIL_TOO_CLOSE;
        }

        return CAST_OK;
    }
    else
        return CAST_FAIL_OTHER;
}

CanCastResult CreatureAI::DoCastSpellIfCan(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags, uint64 uiOriginalCasterGUID)
{
    Unit* pCaster = me;

    if (uiCastFlags & CAST_FORCE_TARGET_SELF)
        pCaster = pTarget;

    // Allowed to cast only if not casting (unless we interrupt ourself) or if spell is triggered
    if (!pCaster->IsNonMeleeSpellCasted(false) || (uiCastFlags & (CAST_TRIGGERED | CAST_INTURRUPT_PREVIOUS)))
    {
        if (const SpellEntry* pSpell = sSpellStore.LookupEntry(uiSpell))
        {
            // If cast flag CAST_AURA_NOT_PRESENT is active, check if target already has aura on them
            if (uiCastFlags & CAST_AURA_NOT_PRESENT)
            {
                if (pTarget->HasAura(uiSpell))
                    return CAST_FAIL_TARGET_AURA;
            }

            // Check if cannot cast spell
            if (!(uiCastFlags & (CAST_FORCE_TARGET_SELF | CAST_FORCE_CAST)))
            {
                CanCastResult castResult = CanCastSpell(pTarget, pSpell, uiCastFlags & CAST_TRIGGERED);

                if (castResult != CAST_OK)
                    return castResult;
            }

            // Interrupt any previous spell
            if (uiCastFlags & CAST_INTURRUPT_PREVIOUS && pCaster->IsNonMeleeSpellCasted(false))
                pCaster->InterruptNonMeleeSpells(false);

            pCaster->CastSpell(pTarget, pSpell, uiCastFlags & CAST_TRIGGERED, NULL, NULL, uiOriginalCasterGUID);
            return CAST_OK;
        }
        else
        {
            sLog.outErrorDb("DoCastSpellIfCan by creature entry %u attempt to cast spell %u but spell does not exist.", me->GetEntry(), uiSpell);
            return CAST_FAIL_OTHER;
        }
    }
    else
        return CAST_FAIL_IS_CASTING;
}


void CreatureAI::SendPortrait(Unit* u,bool activate,uint32 nb)
{
	if(!u)
		u = me;
	
	Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
	if (!lPlayers.isEmpty())
		for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			if (Player* pPlayer = itr->getSource())
			{
				WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE);
				data << uint32(0x0);
				data.appendPackGUID(u->GetGUID());
				data << uint8(activate?0:1);
				pPlayer->GetSession()->SendPacket(&data);
			}
}

void CreatureAI::ResetTimers()
{
	EventShVect.clear();
	EventSummonVect.clear();
	EventTextVect.clear();
	for(SpellEvents::iterator itr = SavedEventSh.begin(); itr!= SavedEventSh.end(); ++itr)
		EventShVect.push_back(*itr);

	for(SummonEvents::iterator itr = SavedEventSummon.begin(); itr!= SavedEventSummon.end(); ++itr)
		EventSummonVect.push_back(*itr);

	for(TextEvents::iterator itr = SavedEventTexts.begin(); itr!= SavedEventTexts.end(); ++itr)
		EventTextVect.push_back(*itr);
}

void CreatureAI::GiveEmblemsToGroup(uint32 type, uint8 nb, bool group5)
{
	if(type == 0)
		return;

    Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
	if (!lPlayers.isEmpty())
		for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			if (Player* pPlayer = itr->getSource())
				if(pPlayer->GetDistance2d(me) < 120.0f)
				{
					GiveEmblems(type,pPlayer,nb, group5);
					pPlayer->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, type, nb);
				}
}

void CreatureAI::GiveEmblems(uint32 type, Player* pPlayer, uint8 nb, bool group5)
{
	ItemPosCountVec dest;
	uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, type, nb, false);
	if (msg == EQUIP_ERR_OK)
		if(Item* pItem = pPlayer->StoreNewItem(dest, type, nb, true))
		{
			pPlayer->SendNewItem(pItem, nb, true, false);
		}

	// Wintergrasp Aura
	if(group5 && pPlayer->HasAura(57940))
	{
		ItemPosCountVec dest;
	        uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 43228, 4, false);
	        if (msg == EQUIP_ERR_OK)
                	if(Item* pItem = pPlayer->StoreNewItem(dest, 43228, 4, true))
					{
        				pPlayer->SendNewItem(pItem, 4, true, false);
					}
	}
}

Unit* CreatureAI::SelectUnit(SelectAggroTarget target, uint32 uiPosition)
{
    //ThreatList m_threatlist;
   ThreatList const& threatlist = me->getThreatManager().getThreatList();
   ThreatList::const_iterator itr = threatlist.begin();
   ThreatList::const_reverse_iterator ritr = threatlist.rbegin();

    if (uiPosition >= threatlist.size() || !threatlist.size())
        return NULL;

    switch (target)
    {
    case SELECT_TARGET_RANDOM:
    {
		Unit* target;
		uint16 occur_ = 0;
		bool right_target = true;
		do
		{
			occur_ ++;
			right_target = true;
	       	advance(itr, uiPosition +  (rand() % (threatlist.size() - uiPosition)));
			target = Unit::GetUnit((*me),(*itr)->getUnitGuid());
			if(!target || (target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->isTotem()) || abs(target->GetPositionZ() - me->GetPositionZ()) > 55.0f)
			{
				itr = threatlist.begin();
				right_target = false;
			}
			if(occur_ > 300)
				right_target = true;
		}
		while(!right_target);
		return target;
        break;
    }
    case SELECT_TARGET_TOPAGGRO:
        advance(itr, uiPosition);
        return Unit::GetUnit((*me),(*itr)->getUnitGuid());
        break;

    case SELECT_TARGET_BOTTOMAGGRO:
        advance(ritr, uiPosition);
        return Unit::GetUnit((*me),(*ritr)->getUnitGuid());
        break;
    }

    return NULL;
}

void CreatureAI::DoCastSpell(Unit* pTarget, SpellEntry const* pSpellInfo, bool bTriggered)
{
    if (!pTarget || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, pSpellInfo, bTriggered);
}

void CreatureAI::DoCast(Unit* pTarget, uint32 uiSpellId, bool bTriggered)
{
    if (!pTarget || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, uiSpellId, bTriggered);
}

void CreatureAI::DoCastXYZ(float x, float y, float z, uint32 uiSpellId, bool bTriggered)
{
	me->StopMoving();
	me->CastSpell(x,y,z,uiSpellId,bTriggered);
}

void CreatureAI::DoCastMe(uint32 uiSpellId, bool bTriggered)
{
    if (!me->isAlive() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(me, uiSpellId, bTriggered);
}

void CreatureAI::DoCastVictim(uint32 uiSpellId, bool bTriggered)
{
    if (!me->getVictim() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(me->getVictim(), uiSpellId, bTriggered);
}

Unit* CreatureAI::DoCastRandom(uint32 uiSpellId, bool bTriggered, bool InFront)
{
    Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    if (!pTarget || me->IsNonMeleeSpellCasted(false) || !pTarget->isAlive())
        return NULL;

    me->StopMoving();
	if(InFront)
		me->SetInFront(pTarget);
    me->CastSpell(pTarget, uiSpellId, bTriggered);
    return pTarget;
}

void CreatureAI::DoCastHasMana(uint32 uiSpellId, bool bTriggered, bool InFront)
{
	Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    if (!pTarget || me->IsNonMeleeSpellCasted(false) || !pTarget->isAlive())
        return;

	if(pTarget->getPowerType() != POWER_MANA)
	{
		int search=0;
		do
		{
			pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
			search++;
		}
		while(pTarget && pTarget->getPowerType() != POWER_MANA && search < 30);
	}

	if(!pTarget)
		return;

	me->StopMoving();
	if(InFront)
		me->SetInFront(pTarget);
	me->CastSpell(pTarget, uiSpellId, bTriggered);
}

void CreatureAI::DoCastPlayer(uint32 uiSpellId, bool bTriggered, bool InFront)
{
	Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
	if (!lPlayers.isEmpty())
	{
		uint32 _rand = urand(0,lPlayers.getSize());
		uint16 i=0;
		for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
		{
			if(i == _rand)
			{
				if(Player* pPlayer = itr->getSource())
				{
					if(InFront)
						me->SetInFront(pPlayer);
					me->CastSpell(pPlayer, uiSpellId, bTriggered);
					return;
				}
			}
			i++;
		}
	}
}

Unit* CreatureAI::DoSelectLowestHpFriendly(float fRange, uint32 uiMinHPDiff)
{
    CellPair p(MaNGOS::ComputeCellPair(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Unit* pUnit = NULL;

    MaNGOS::MostHPMissingInRange u_check(me, fRange, uiMinHPDiff);
    MaNGOS::UnitLastSearcher<MaNGOS::MostHPMissingInRange> searcher(me, pUnit, u_check);

    /*
    typedef TYPELIST_4(GameObject, Creature*except pets*, DynamicObject, Corpse*Bones*) AllGridObjectTypes;
    This means that if we only search grid then we cannot possibly return pets or players so this is safe
    */
    TypeContainerVisitor<MaNGOS::UnitLastSearcher<MaNGOS::MostHPMissingInRange>, GridTypeMapContainer >  grid_unit_searcher(searcher);

    cell.Visit(p, grid_unit_searcher, *(me->GetMap()));

    return pUnit;
}

void CreatureAI::DoCastLowHP(uint32 uiSpellId, bool bTriggered)
{
	Unit *pTarget = DoSelectLowestHpFriendly(60);

    if (!pTarget ||!pTarget->isAlive() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, uiSpellId, bTriggered);
}

void CreatureAI::DoCastNear(uint32 uiSpellId, bool bTriggered)
{
	Unit *pTarget = NULL;
    float tmpDist = 100.0f;
    ThreatList::const_iterator i;
    for (i = me->getThreatManager().getThreatList().begin();i != me->getThreatManager().getThreatList().end(); ++i)
    {
        Unit* pUnit = NULL;
        pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
        if (!pUnit)
            continue;
        if(pUnit->GetDistance2d(me) < tmpDist)
		{
			tmpDist = pUnit->GetDistance2d(me);
			pTarget = pUnit;
		}
    }

    if (!pTarget ||!pTarget->isAlive() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, uiSpellId, bTriggered);
}
