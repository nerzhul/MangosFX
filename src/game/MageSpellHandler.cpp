#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "MageSpellHandler.h"

INSTANTIATE_SINGLETON_1(MageSpellHandler);

#define SPELL_COLD_SNAP				11958
#define SPELL_WATER_ELEMENTAL		31687
#define SPELL_POLYMORPH				32826

void MageSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
}

bool MageSpellHandler::HandleEffectDummy(Spell* spell, int32 &damage, SpellEffectIndex i)
{
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();
	
	switch(m_spellInfo->Id)
	{
		case SPELL_COLD_SNAP:
		{
			if (m_caster->GetTypeId()!=TYPEID_PLAYER)
				return false;
			
			// immediately finishes the cooldown on Frost spells
			const SpellCooldowns& cm = ((Player *)m_caster)->GetSpellCooldownMap();
			for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
			{
				SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
				
				if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE &&
					(GetSpellSchoolMask(spellInfo) & SPELL_SCHOOL_MASK_FROST) &&
					spellInfo->Id != 11958 && GetSpellRecoveryTime(spellInfo) > 0)
				{
					((Player*)m_caster)->RemoveSpellCooldown((itr++)->first, true);
				}
				else
					++itr;
			}
			
			((Player*)m_caster)->RemoveSpellCooldown(12472,true);
			return false;
		}
		case SPELL_POLYMORPH:                                 // Polymorph Cast Visual
		{
			if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
			{
				//Polymorph Cast Visual Rank 1
				const uint32 spell_list[6] = {
					32813,                          // Squirrel Form
					32816,                          // Giraffe Form
					32817,                          // Serpent Form
					32818,                          // Dragonhawk Form
					32819,                          // Worgen Form
					32820                           // Sheep Form
				};
				unitTarget->CastSpell( unitTarget, spell_list[urand(0, 5)], true);
			}
			return false;
		}
	}
	// Conjure Mana Gem
	if (i == 1 && m_spellInfo->Effect[0] == SPELL_EFFECT_CREATE_ITEM)
	{
		unitTarget->CastSpell(unitTarget, m_spellInfo->CalculateSimpleValue(i), true, spell->GetCastItem());
		return false;
	}
	
	return true;
}

void MageSpellHandler::SpellDamageBonusDone(SpellEntry* spellProto, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod)
{

	Unit* owner = caster->GetOwner();
	// Ice Lance
    if (spellProto->SpellIconID == 186)
    {
		if (pVictim->isFrozen() || caster->isIgnoreUnitState(spellProto))
        {
			float multiplier;
			// Glyph of Ice Lance
            if (owner && owner->HasAura(56377) && pVictim->getLevel() > caster->getLevel())
                multiplier = 4.0f;
            else
				multiplier = 3.0f;

            DoneTotalMod *= multiplier;
        }
    }
    // Torment the weak affected (Arcane Barrage, Arcane Blast, Frostfire Bolt, Arcane Missiles, Fireball)
    if ((spellProto->SpellFamilyFlags & UI64LIT(0x0000900020200021)) &&
        (pVictim->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) || pVictim->HasAuraType(SPELL_AURA_HASTE_ALL)))
    {
        //Search for Torment the weak dummy aura
        Unit::AuraList const& ttw = caster->GetAurasByType(SPELL_AURA_DUMMY);
        for(Unit::AuraList::const_iterator i = ttw.begin(); i != ttw.end(); ++i)
        {
            if ((*i)->GetSpellProto()->SpellIconID == 3263)
            {
                DoneTotalMod *= ((*i)->GetModifier()->m_amount+100.0f) / 100.0f;
                break;
            }
        }
    }

	// Arcane empowerment						
	if(spellProto->SpellFamilyFlags & UI64LIT(0x020000000))
	{
		if(Aura* aur = caster->GetAura(31583))
		{
			if(aur->GetCaster() == caster)
				DoneTotalMod *= (aur->GetModifier()->m_amount+100.0f) / 100.0f;
		}
		else if(Aura* aur = caster->GetAura(31582))
		{
			if(aur->GetCaster() == caster)
				DoneTotalMod *= (aur->GetModifier()->m_amount+100.0f) / 100.0f;
		}
		else if(Aura* aur = caster->GetAura(31579))
		{
			if(aur->GetCaster() == caster)
				DoneTotalMod *= (aur->GetModifier()->m_amount+100.0f) / 100.0f;
		}
	}
	else if(spellProto->SpellFamilyFlags & UI64LIT(0x0800))
	{
		if(Aura* aur = caster->GetAura(31583))
		{
			if(aur->GetCaster() == caster)
				DoneTotalMod *= (aur->GetModifier()->m_amount*5+100.0f) / 100.0f;
		}
		else if(Aura* aur = caster->GetAura(31582))
		{
			if(aur->GetCaster() == caster)
				DoneTotalMod *= (aur->GetModifier()->m_amount*5+100.0f) / 100.0f;
		}
		else if(Aura* aur = caster->GetAura(31579))
		{
			if(aur->GetCaster() == caster)
				DoneTotalMod *= (aur->GetModifier()->m_amount*5+100.0f) / 100.0f;
		}
	}
}