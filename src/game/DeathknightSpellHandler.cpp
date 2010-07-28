#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "DeathknightSpellHandler.h"

INSTANTIATE_SINGLETON_1(DeathknightSpellHandler);

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
    if(spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000400000) &&
        spell->GetCaster()->HasAura(59332) &&
        spell->getUnitTarget()->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED))
    {
        totalDmgPctMod *= 1.2f;              // 120% if snared
    }
	// Rune strike
	if(spell->m_spellInfo->SpellIconID == 3007)
	{
		int32 count = spell->CalculateDamage(2, spell->getUnitTarget());
		spell_bonus += int32(count * spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 100.0f);
	}
    // Glyph of Death Strike
    if(spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000000010) &&
        spell->GetCaster()->HasAura(59336))
    {
        int32 rp = spell->GetCaster()->GetPower(POWER_RUNIC_POWER) / 10;
        if(rp > 25)
            rp = 25;
        totalDmgPctMod *= 1.0f + rp / 100.0f;
    }
    // Glyph of Plague Strike
    if(spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000000001) &&
        spell->GetCaster()->HasAura(58657))
    {
        totalDmgPctMod *= 1.2f;
    }
}