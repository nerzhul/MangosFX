#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "WarriorSpellHandler.h"

INSTANTIATE_SINGLETON_1(WarriorSpellHandler);

#define FLAG_DEVASTATE UI64LIT(0x4000000000)
#define FLAG_MORTALSTRIKE UI64LIT(0x2000000)
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
            if( spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR &&
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