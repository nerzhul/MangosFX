#include <Policies/SingletonImp.h>
#include "Spell.h"
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
	SpellEntry* m_spellInfo = spell->m_spellInfo;
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
		case SPELL_WATER_ELEMENTAL:
		{
			if (m_caster->HasAura(70937))           // Glyph of Eternal Water (permanent limited by known spells version)
				m_caster->CastSpell(m_caster, 70908, true);
			else                                    // temporary version
				m_caster->CastSpell(m_caster, 70907, true);
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
		unitTarget->CastSpell(unitTarget, m_spellInfo->CalculateSimpleValue(i), true, m_CastItem);
		return false;
	}
	
	return true;
}