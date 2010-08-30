#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "DeathknightSpellHandler.h"

INSTANTIATE_SINGLETON_1(DeathknightSpellHandler);

#define FLAG_DEATH_COIL			UI64LIT(0x002000)
#define FLAG_HUNGERING_COLD		UI64LIT(0x0000100000000000)
#define FLAG_DEATH_STRIKE		UI64LIT(0x0000000000000010)
#define FLAG_PLAGUE_STRIKE		UI64LIT(0x0000000000000001)
#define FLAG_BLOOD_STRIKE		UI64LIT(0x0000000000400000)
#define FLAG_OBLITERATE			UI64LIT(0x2000000000000)

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
}