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

void CreatureAI::DoCastSpell(Unit* pTarget, SpellEntry const* pSpellInfo, bool bTriggered)
{
    if (!pTarget || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, pSpellInfo, bTriggered);
}