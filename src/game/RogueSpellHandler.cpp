#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "RogueSpellHandler.h"

INSTANTIATE_SINGLETON_1(RogueSpellHandler);

#define	FLAG_MUTILATE		UI64LIT(0x600000000)
#define FLAG_FAN_OF_KNIVES	UI64LIT(0x0004000000000000)
#define FLAG_HEMORRHAGE		UI64LIT(0x2000000)
#define FLAG_SINISTERSTK	UI64LIT(0x800002)
#define FLAG_AMBUSH			UI64LIT(0x800200)
#define FLAG_BACKSTAB		UI64LIT(0x800004)

void RogueSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Mutilate (for each hand)
    if(spell->m_spellInfo->SpellFamilyFlags & FLAG_MUTILATE)
    {
        bool found = false;
        // fast check
        if(spell->getUnitTarget()->HasAuraState(AURA_STATE_DEADLY_POISON))
            found = true;
        // full aura scan
        else
        {
            Unit::AuraMap const& auras = spell->getUnitTarget()->GetAuras();
            for(Unit::AuraMap::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
            {
                if(itr->second->GetSpellProto()->Dispel == DISPEL_POISON)
                {
                    found = true;
                    break;
                }
            }
        }

        if(found)
            totalDmgPctMod *= 1.2f;          // 120% if poisoned

		// WOTLK formula
		spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7); 
    }
    // Fan of Knives
    else if (spell->GetCaster()->GetTypeId()==TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags & FLAG_FAN_OF_KNIVES))
    {
        Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
        if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
            totalDmgPctMod *= 1.5f;          // 150% to daggers
    }
	// Hemorrhage
	else if (spell->GetCaster()->GetTypeId()==TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags & FLAG_HEMORRHAGE))
	{
		((Player*)spell->GetCaster())->AddComboPoints(spell->getUnitTarget(), 1);
		Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
		if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
			totalDmgPctMod *= 1.45f;         // 145% with dagger
	}
	// Ghostly Strike
	else if (spell->GetCaster()->GetTypeId()==TYPEID_PLAYER && (spell->m_spellInfo->Id == 14278))
	{
		Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
		if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
			totalDmgPctMod *= 1.44f;         // 144% with dagger 
	}
	else if(spell->GetCaster()->GetTypeId()==TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags == FLAG_SINISTERSTK))
	{
		Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
		if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
		else
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
	}
	else if(spell->GetCaster()->GetTypeId()==TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags == FLAG_AMBUSH))
	{
		Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
		if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
	}
	else if(spell->GetCaster()->GetTypeId()==TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags == FLAG_BACKSTAB))
	{
		Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
		if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
	}
}