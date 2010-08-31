#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "DeathknightSpellHandler.h"

INSTANTIATE_SINGLETON_1(DeathknightSpellHandler);

#define FLAG_PLAGUE_STRIKE		UI64LIT(0x0000000000000001)
#define FLAG_DEATH_STRIKE		UI64LIT(0x0000000000000010)
#define FLAG_DEATH_AND_DECAY	UI64LIT(0x0000000000000020)
#define FLAG_DEATH_COIL			UI64LIT(0x0000000000002000)
#define FLAG_BLOOD_STRIKE		UI64LIT(0x0000000000400000)
#define FLAG_HEART_STRIKE		UI64LIT(0x0000000001000000)
#define FLAG_CHAIN_OF_ICE		UI64LIT(0x0000400000000000)
#define FLAG_HUNGERING_COLD		UI64LIT(0x0000900000000000)
#define FLAG_OBLITERATE			UI64LIT(0x0002000000000000)

bool DeathknightSpellHandler::HandleEffectDummy(Spell* spell)
{
	 // Death Coil
    if(spell->m_spellInfo->SpellFamilyFlags & FLAG_DEATH_COIL)
    {
        if (spell->GetCaster()->IsFriendlyTo(spell->getUnitTarget()))
        {
            if (!spell->getUnitTarget() || spell->getUnitTarget()->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                return false;

            int32 bp = spell->getDamage() * 1.5f;
            spell->GetCaster()->CastCustomSpell(spell->getUnitTarget(), 47633, &bp, NULL, NULL, true);
        }
        else
        {
            int32 bp = spell->getDamage();
            spell->GetCaster()->CastCustomSpell(spell->getUnitTarget(), 47632, &bp, NULL, NULL, true);
        }
        return false;
    }
    // Hungering Cold
    else if (spell->m_spellInfo->SpellFamilyFlags & FLAG_HUNGERING_COLD)
    {
        spell->GetCaster()->CastSpell(spell->GetCaster(), 51209, true);
        return false;
    }
    // Death Strike
    else if (spell->m_spellInfo->SpellFamilyFlags & FLAG_DEATH_STRIKE)
    {
        uint32 count = 0;
        Unit::AuraMap const& auras = spell->getUnitTarget()->GetAuras();
        for(Unit::AuraMap::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
        {
            if (itr->second->GetSpellProto()->Dispel == DISPEL_DISEASE &&
                itr->second->GetCasterGUID() == spell->GetCaster()->GetGUID() &&
                IsSpellLastAuraEffect(itr->second->GetSpellProto(), itr->second->GetEffIndex()))
            {
                ++count;
                // max. 15%
                if (count == 3)
                    break;
            }
        }

        int32 bp = count * spell->GetCaster()->GetMaxHealth() * spell->m_spellInfo->DmgMultiplier[0] / 100;

		 // Improved Death Strike
        Unit::AuraList const& auraMod = spell->GetCaster()->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
        for(Unit::AuraList::const_iterator iter = auraMod.begin(); iter != auraMod.end(); ++iter)
        {
            if ((*iter)->GetSpellProto()->SpellIconID == 2751 && (*iter)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT)
            {
                bp += (*iter)->GetSpellProto()->CalculateSimpleValue(2) * bp / 100;
                break;
            }
        }
        spell->GetCaster()->CastCustomSpell(spell->GetCaster(), 45470, &bp, NULL, NULL, true);
        return false;
    }
	// Death Grip
    else if (spell->m_spellInfo->Id == 49576)
    {
        if (!spell->getUnitTarget())
            return false;

        spell->GetCaster()->CastSpell(spell->getUnitTarget(), 49560, true);
        return false;
    }
    else if (spell->m_spellInfo->Id == 49560)
    {
        if (!spell->getUnitTarget())
            return false;

        uint32 spellId = spell->m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
        spell->getUnitTarget()->CastSpell(spell->GetCaster()->GetPositionX(), spell->GetCaster()->GetPositionY(), spell->GetCaster()->GetPositionZ()+1, spellId, true);
        return false;
    }

	return true;
}

void DeathknightSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Blood Strike, Heart Strike, Obliterate
    // Blood-Caked Strike
    if (spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x0002000001400000) ||
        spell->m_spellInfo->SpellIconID == 1736)
    {
        uint32 count = 0;
        Unit::AuraMap const& auras = spell->getUnitTarget()->GetAuras();
        for(Unit::AuraMap::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
        {
            if(itr->second->GetSpellProto()->Dispel == DISPEL_DISEASE &&
				itr->second->GetCasterGUID() == spell->GetCaster()->GetGUID() &&
                IsSpellLastAuraEffect(itr->second->GetSpellProto(), itr->second->GetEffIndex()))
                ++count;
        }

        if (count)
        {
            // Effect 1(for Blood-Caked Strike)/3(other) damage is bonus
            float bonus = count * spell->CalculateDamage(spell->m_spellInfo->SpellIconID == 1736 ? 0 : 2, spell->getUnitTarget()) / 100.0f;
            // Blood Strike, Blood-Caked Strike and Obliterate store bonus*2
            if (spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x0002000000400000) ||
                spell->m_spellInfo->SpellIconID == 1736)
                bonus /= 2.0f;

            totalDmgPctMod *= 1.0f + bonus;
        }
    }
    // Glyph of Blood Strike
    if(spell->m_spellInfo->SpellFamilyFlags & FLAG_BLOOD_STRIKE)
	{
        if(spell->GetCaster()->HasAura(59332) && spell->getUnitTarget()->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED))
			totalDmgPctMod *= 1.2f;              // 120% if snared

		if(spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
	}
	// Rune strike
	if(spell->m_spellInfo->SpellIconID == 3007)
	{
		int32 count = spell->CalculateDamage(2, spell->getUnitTarget());
		spell_bonus += int32(count * spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 100.0f);
	}
    // Glyph of Death Strike
    else if(spell->m_spellInfo->SpellFamilyFlags & FLAG_DEATH_STRIKE &&
        spell->GetCaster()->HasAura(59336))
    {
        int32 rp = spell->GetCaster()->GetPower(POWER_RUNIC_POWER) / 10;
        if(rp > 25)
            rp = 25;
        totalDmgPctMod *= 1.0f + rp / 100.0f;
    }
    // Glyph of Plague Strike
    else if(spell->m_spellInfo->SpellFamilyFlags & FLAG_PLAGUE_STRIKE &&
        spell->GetCaster()->HasAura(58657))
    {
        totalDmgPctMod *= 1.2f;
    }
	else if(spell->m_spellInfo->SpellFamilyFlags == FLAG_OBLITERATE)
	{
		if(spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
	}
	else if(spell->m_spellInfo->SpellFamilyFlags & FLAG_HEART_STRIKE)
	{
		if(spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
	}
}

void DeathknightSpellHandler::PeriodicDummyTick(Aura* aura)
{
	SpellEntry const* spell = aura->GetSpellProto();
	Unit* m_target = aura->GetTarget();
	// Death and Decay
    if (spell->SpellFamilyFlags & FLAG_DEATH_AND_DECAY)
    {
        if (Unit *caster = aura->GetCaster())
			caster->CastCustomSpell(aura->GetTarget(), 52212, &(aura->GetModifier()->m_amount), NULL, NULL, true, NULL, aura);
        return;
    }
    // Chains of Ice
    else if (spell->SpellFamilyFlags & FLAG_CHAIN_OF_ICE)
    {
        // Get 0 effect aura
        Aura *slow = m_target->GetAura(aura->GetId(), 0);
        if (slow)
        {
            slow->ApplyModifier(false, true);
            Modifier *mod = slow->GetModifier();
            mod->m_amount+= aura->GetModifier()->m_amount;
            if (mod->m_amount > 0) mod->m_amount = 0;
            slow->ApplyModifier(true, true);
        }
        return;
    }
    // Bladed Armor
    if (spell->SpellIconID == 2653)
    {
        // Increases your attack power by $s1 for every $s2 armor value you have.
        // Calculate AP bonus (from 1 efect of this spell)
        int32 apBonus = aura->GetModifier()->m_amount * m_target->GetArmor() / m_target->CalculateSpellDamage(spell, 1, spell->EffectBasePoints[1], m_target);
        m_target->CastCustomSpell(m_target, 61217, &apBonus, &apBonus, NULL, true, NULL, aura);
        return;
    }
	// Death Rune Mastery
	if (spell->SpellIconID == 2622)
	{
		if (m_target->GetTypeId() != TYPEID_PLAYER)
			return;
		
		Player *player = (Player*)m_target;
		for (uint32 i = 0; i < MAX_RUNES; ++i)
		{
			if (!player->GetRuneCooldown(i))
			{
				RuneType type = player->GetBaseRune(i);
				if (player->GetCurrentRune(i) == RUNE_DEATH && (type == RUNE_FROST || type == RUNE_UNHOLY) && player->IsRuneConvertedBy(i, spell->Id))
				{
					player->ConvertRune(i, type);
					player->ClearConvertedBy(i);
				}
			}
		}
	}
	// Blood of the North and Reaping
	if (spell->SpellIconID == 3041 || spell->SpellIconID == 22)
	{
		if (m_target->GetTypeId() != TYPEID_PLAYER)
			return;
		
		Player *player = (Player*)m_target;
		for (uint32 i = 0; i < MAX_RUNES; ++i)
		{
			if (!player->GetRuneCooldown(i) && player->IsRuneConvertedBy(i, spell->Id))
			{
				RuneType type = player->GetBaseRune(i);
				if (player->GetCurrentRune(i) == RUNE_DEATH && type == RUNE_BLOOD)
				{
					player->ConvertRune(i, type);
					player->ClearConvertedBy(i);
				}
			}
		}
		return;
	}
	// Hysteria dot fix
	if(spell->Id == 49016)
	{
		if(m_target->isAlive())
			m_target->ModifyHealth(- int32(m_target->GetMaxHealth() * 1 / 100));
		else
			m_target->RemoveAurasDueToSpell(49016);
		return;
	}
}