#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "PriestSpellHandler.h"
#include "ClassSpellHandler.h"

INSTANTIATE_SINGLETON_1(PriestSpellHandler);

#define FLAG_MIND_BLAST		UI64LIT(0x000000002000)
#define FLAG_SW_PAIN		UI64LIT(0x000000008000)
#define FLAG_SW_DEATH		UI64LIT(0x000200000000)

void PriestSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
}

void PriestSpellHandler::HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i)
{
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();
	
	// Shadow Word: Death - deals damage equal to damage done to caster
	if (m_spellInfo->SpellFamilyFlags & FLAG_SW_DEATH)
	{
		int32 back_damage = m_caster->SpellDamageBonus(unitTarget, m_spellInfo, (uint32)damage, SPELL_DIRECT_DAMAGE);
		
		if(back_damage < unitTarget->GetHealth())
			m_caster->CastCustomSpell(m_caster, 32409, &back_damage, 0, 0, true);
	}
	// Improved Mind Blast (Mind Blast in shadow form bonus)
	else if (m_caster->m_form == FORM_SHADOW && (m_spellInfo->SpellFamilyFlags & FLAG_MIND_BLAST))
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

void PriestSpellHandler::SpellDamageBonusDone(SpellEntry* spellProto, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod)
{
	// Glyph of Smite
    if (spellProto->SpellFamilyFlags & UI64LIT(0x00000080))
    {
        // Holy Fire
		if (sClassSpellHandler.GetAuraByName(pVictim,PRIEST_HOLY_FIRE))
            if (Aura *aur = caster->GetAura(55692))
                DoneTotalMod *= (aur->GetModifier()->m_amount+100.0f) / 100.0f;
    }

	// Twin Disciplines
	if(spellProto->SpellFamilyName == SPELLFAMILY_PRIEST && (spellProto->SpellFamilyFlags & UI64LIT(200204008000)))
	{
		if(Aura* aur = sClassSpellHandler.GetAuraByName(caster,PRIEST_TWIN_DISCIPLINES))
			DoneTotalMod *= (100.0f + aur->GetModifier()->m_amount) / 100.0f;
	}

	switch(spellProto->Id)
	{
		// Glyph of Shadow Word: Pain
		case 58381:
		{
			if(Aura* glyph = caster->GetAura(55687))
			{
				//search for shadow word: pain on target
				if(sClassSpellHandler.GetAuraByName(pVictim,PRIEST_SHADOW_WORD_PAIN))
					DoneTotalMod += glyph->GetModifier()->m_amount * DoneTotalMod / 100;
			}
			break;
		}
		// Glyph of Shadow Word: Death
		case 32379:
		case 32996:
		case 48157:
		case 48158:
		{
			if(caster->HasAura(55682) && pVictim->GetHealth() * 100 / pVictim->GetMaxHealth() <= 35.0f)
				DoneTotalMod *= 1.1;
			break;
		}

	}
}
