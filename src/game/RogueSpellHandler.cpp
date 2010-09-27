#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "RogueSpellHandler.h"
#include "ClassSpellHandler.h"

INSTANTIATE_SINGLETON_1(RogueSpellHandler);

#define FLAG_GOUGE			UI64LIT(0x0000000000008)
#define FLAG_INSTANT_POISON	UI64LIT(0x0000000002000)
#define FLAG_EVISCERATE		UI64LIT(0x0000000020000)
#define FLAG_SINISTERSTK	UI64LIT(0x0000000800002)
#define FLAG_BACKSTAB		UI64LIT(0x0000000800004)
#define FLAG_AMBUSH			UI64LIT(0x0000000800200)
#define FLAG_HEMORRHAGE		UI64LIT(0x0000002000000)
#define FLAG_WOUND_POISON	UI64LIT(0x0000010000000)
#define	FLAG_MUTILATE		UI64LIT(0x0000600000000)
#define FLAG_ENVENOM		UI64LIT(0x0000800000000)
#define FLAG_FAN_OF_KNIVES	UI64LIT(0x4000000000000)

#define SPELL_SHIV			5938
#define SPELL_PREPARATION	14185

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

void RogueSpellHandler::HandleSchoolDmg(Spell* spell, int32 &damage, SpellEffectIndex i)
{
	Unit* m_caster = spell->GetCaster();
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* unitTarget = spell->getUnitTarget();
	
	if (m_caster->GetTypeId()==TYPEID_PLAYER && (m_spellInfo->SpellFamilyFlags & FLAG_ENVENOM))
	{
		// consume from stack dozes not more that have combo-points
		if(uint32 combo = ((Player*)m_caster)->GetComboPoints())
		{
			Aura *poison = NULL;
			// Lookup for Deadly poison (only attacker applied)
			Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
			for(Unit::AuraList::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
				if( (*itr)->GetSpellProto()->SpellFamilyName==SPELLFAMILY_ROGUE &&
				   ((*itr)->GetSpellProto()->SpellFamilyFlags & UI64LIT(0x10000)) &&
				   (*itr)->GetCasterGUID() == m_caster->GetGUID() )
				{
					poison = *itr;
					break;
				}
			// count consumed deadly poison doses at target
			if (poison)
			{
				uint32 spellId = poison->GetId();
				uint32 doses = poison->GetStackAmount();
				
				if(!m_caster->HasAura(58410) && !(m_caster->HasAura(31227) 
												  && roll_chance_i(66)) && !(m_caster->HasAura(31226) 
																			 && roll_chance_i(33)))
				{
					if (doses > combo)
						doses = combo;
					for (uint8 i=0; i<combo; i++)
						unitTarget->RemoveSingleSpellAurasByCasterSpell(spellId, m_caster->GetGUID());
					
					if(Aura* aur = sClassSpellHandler.GetAuraByName(m_caster,ROGUE_ENVENOM))
					{
						aur->SetAuraMaxDuration((1+combo)*1000);
						aur->RefreshAura();
					}
				}
				damage *= combo;
				damage += int32(((Player*)m_caster)->GetTotalAttackPowerValue(BASE_ATTACK) * 0.09f * doses);
			}
			// Eviscerate and Envenom Bonus Damage (item set effect)
			if(m_caster->GetDummyAura(37169))
				damage += ((Player*)m_caster)->GetComboPoints()*40;
		}
	}
	else if ((m_spellInfo->SpellFamilyFlags & FLAG_EVISCERATE) && m_caster->GetTypeId()==TYPEID_PLAYER)
	{
		if(uint32 combo = ((Player*)m_caster)->GetComboPoints())
		{
			float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
			damage += irand(int32(ap * combo * 0.03f), int32(ap * combo * 0.07f));
			
			// Eviscerate and Envenom Bonus Damage (item set effect)
			if(m_caster->GetDummyAura(37169))
				damage += combo*40;
		}
	}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_GOUGE)
		damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.21f);
	else if (m_spellInfo->SpellFamilyFlags & FLAG_INSTANT_POISON)
		damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.10f);
	else if (m_spellInfo->SpellFamilyFlags & FLAG_WOUND_POISON)
		damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.04f);
}

bool RogueSpellHandler::HandleEffectDummy(Spell* spell, int32 &damage, SpellEffectIndex i)
{
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();
	
	switch(m_spellInfo->Id)
	{
		case SPELL_SHIV:                                  // Shiv
		{
			if (m_caster->GetTypeId() != TYPEID_PLAYER)
				return false;
			
			Player *pCaster = ((Player*)m_caster);
			
			Item *item = pCaster->GetWeaponForAttack(OFF_ATTACK);
			if (!item)
				return false;
			
			// all poison enchantments is temporary
			uint32 enchant_id = item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT);
			if (!enchant_id)
				return false;
			
			SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
			if (!pEnchant)
				return false;
			
			for (int s=0;s<3;s++)
			{
				if (pEnchant->type[s]!=ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
					continue;
				
				SpellEntry const* combatEntry = sSpellStore.LookupEntry(pEnchant->spellid[s]);
				if (!combatEntry || combatEntry->Dispel != DISPEL_POISON)
					continue;
				
				m_caster->CastSpell(unitTarget, combatEntry, true, item);
			}
			
			m_caster->CastSpell(unitTarget, 5940, true);
			return false;
		}
		case SPELL_PREPARATION:                                 // Preparation
		{
			if (m_caster->GetTypeId()!=TYPEID_PLAYER)
				return false;
			
			//immediately finishes the cooldown on certain Rogue abilities
			const SpellCooldowns& cm = ((Player *)m_caster)->GetSpellCooldownMap();
			for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
			{
				SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
				
				// glyph of preparation
				if (spellInfo->SpellFamilyName == SPELLFAMILY_ROGUE && (spellInfo->SpellFamilyFlags & UI64LIT(0x0000024000000860))
					|| ((Player*)m_caster)->HasAura(56819) && (spellInfo->Id == 13877 || spellInfo->Id == 51722 || spellInfo->Id == 1766))
					((Player*)m_caster)->RemoveSpellCooldown((itr++)->first,true);
					else
						++itr;
			}
			return false;
		}
		case 31231:                                 // Cheat Death
		{
			m_caster->CastSpell(m_caster, 45182, true);
			return false;
		}
		case 51662:
		{
			m_caster->CastSpell(m_caster, 63848, true);
			break;
		}
		case 57934:
		{
			if(m_caster != unitTarget)
				m_caster->CastSpell(unitTarget,57933,true);
				break;
		}
	}
	return true;
}