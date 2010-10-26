#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "WarriorSpellHandler.h"

INSTANTIATE_SINGLETON_1(WarriorSpellHandler);

#define FLAG_THUNDER_CLAP		UI64LIT(0x00000000000080)
#define FLAG_REVENGE			UI64LIT(0x00000000000400)
#define FLAG_MORTALSTRIKE		UI64LIT(0x00000002000000)
#define FLAG_HEROIC_THROW		UI64LIT(0x00000100000000)
#define FLAG_DEVASTATE			UI64LIT(0x00004000000000)
#define FLAG_VICTORY_RUSH		UI64LIT(0x00010000000000)
#define FLAG_SHIELD_SLAM		UI64LIT(0x00020000000000)
#define FLAG_BLOODTHIRST		UI64LIT(0x00040000000000)
#define FLAG_SHOCKWAVE			UI64LIT(0x00800000000000)
#define FLAG_SHATTERING_THROW	UI64LIT(0x40000000000000)
#define SPELL_WHIRLWIND 1680
#define SPELL_OVERPOWER 7384
#define SPELL_WHIRLWIND_OFFHAND 44949

void WarriorSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Devastate bonus and sunder armor refresh
    if(spell->m_spellInfo->SpellVisual[0] == 12295 && spell->m_spellInfo->SpellIconID == 1508)
    {
        uint32 stack = 0;
        // Need refresh all Sunder Armor auras from this caster
        Unit::AuraMap& suAuras = spell->getUnitTarget()->GetAuras();
        SpellEntry const *spellInfo;
        for(Unit::AuraMap::iterator itr = suAuras.begin(); itr != suAuras.end(); ++itr)
        {
            spellInfo = (*itr).second->GetSpellProto();
            if( spellInfo->GetSpellFamilyName() == SPELLFAMILY_WARRIOR &&
                (spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000004000)) &&
                (*itr).second->GetCasterGUID() == spell->GetCaster()->GetGUID())
            {
                (*itr).second->RefreshAura();
                stack = (*itr).second->GetStackAmount();
                break;
            }
        }
        if (stack)
            spell_bonus += stack * spell->CalculateDamage(2, spell->getUnitTarget());
        if (!stack || stack < spellInfo->StackAmount)
            // Devastate causing Sunder Armor Effect
            // and no need to cast over max stack amount
            spell->GetCaster()->CastSpell(spell->getUnitTarget(), 58567, true);
		// glyph of devastate
		if(spell->GetCaster()->HasAura(58388))
			spell->GetCaster()->CastSpell(spell->getUnitTarget(), 58567, true);
    }

	if(spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
	{
		if(spell->m_spellInfo->SpellFamilyFlags == FLAG_DEVASTATE)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
		else if(spell->m_spellInfo->SpellFamilyFlags == FLAG_MORTALSTRIKE)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
		else if(spell->m_spellInfo->Id == SPELL_WHIRLWIND || spell->m_spellInfo->Id == SPELL_WHIRLWIND_OFFHAND ||
			spell->m_spellInfo->Id == SPELL_OVERPOWER)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
	}
}

void WarriorSpellHandler::HandleSchoolDmg(Spell* spell, int32 &damage, SpellEffectIndex i)
{
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();
	// Bloodthirst
	if (m_spellInfo->SpellFamilyFlags & FLAG_BLOODTHIRST)
		damage = uint32(damage * (m_caster->GetTotalAttackPowerValue(BASE_ATTACK)) / 100);
	// Shield Slam
	else if ((m_spellInfo->SpellFamilyFlags & FLAG_SHIELD_SLAM) && m_spellInfo->Category==1209)
	{
		damage += int32(m_caster->GetShieldBlockValue());
		// glyph of shield slam
		if (m_caster->HasAura(58375))
			m_caster->CastSpell(m_caster, 58374, true);
	}
	// Victory Rush
	else if (m_spellInfo->SpellFamilyFlags & FLAG_VICTORY_RUSH)
	{
		damage = uint32(damage * m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
		m_caster->ModifyAuraState(AURA_STATE_WARRIOR_VICTORY_RUSH, false);
	}
	// Revenge ${$m1+$AP*0.310} to ${$M1+$AP*0.310}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_REVENGE)
		damage+= uint32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.310f);
	// Heroic Throw ${$m1+$AP*.50}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_HEROIC_THROW)
		damage+= uint32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.5f);
	// Shattering Throw ${$m1+$AP*.50}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_SHATTERING_THROW)
		damage+= uint32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.5f);
	// Shockwave ${$m3/100*$AP}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_SHOCKWAVE)
	{
		int32 pct = m_caster->CalculateSpellDamage(m_spellInfo, 2, m_spellInfo->EffectBasePoints[2], unitTarget);
		if (pct > 0)
			damage+= int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * pct / 100);
	}
	// Thunder Clap
	else if (m_spellInfo->SpellFamilyFlags & FLAG_THUNDER_CLAP)
	{
		damage+=int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 12 / 100);
	}
}