#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "PriestSpellHandler.h"

INSTANTIATE_SINGLETON_1(PriestSpellHandler);

void PriestSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
}

void PriestSpellHandler::HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i)
{
	SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->GetUnitTarget();
	
	// Shadow Word: Death - deals damage equal to damage done to caster
	if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000200000000))
	{
		int32 back_damage = m_caster->SpellDamageBonus(unitTarget, m_spellInfo, (uint32)damage, SPELL_DIRECT_DAMAGE);
		
		if(back_damage < unitTarget->GetHealth())
			m_caster->CastCustomSpell(m_caster, 32409, &back_damage, 0, 0, true);
	}
	// Improved Mind Blast (Mind Blast in shadow form bonus)
	else if (m_caster->m_form == FORM_SHADOW && (m_spellInfo->SpellFamilyFlags & UI64LIT(0x00002000)))
	{
		Unit::AuraList const& ImprMindBlast = m_caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
		for(Unit::AuraList::const_iterator i = ImprMindBlast.begin(); i != ImprMindBlast.end(); ++i)
		{
			if ((*i)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_PRIEST &&
				((*i)->GetSpellProto()->SpellIconID == 95))
			{
				int chance = (*i)->GetSpellProto()->CalculateSimpleValue(1);
				if (roll_chance_i(chance))
					// Mind Trauma
					m_caster->CastSpell(unitTarget, 48301, true);
				break;
			}
		}
	}
}