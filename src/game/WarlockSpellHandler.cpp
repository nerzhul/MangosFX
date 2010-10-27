#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "WarlockSpellHandler.h"

INSTANTIATE_SINGLETON_1(WarlockSpellHandler);

#define FLAG_IMMOLATE		UI64LIT(0x0000000000004)
#define FLAG_DRAIN_SOUL		UI64LIT(0x0000000004000)
#define FLAG_INCINERATE		UI64LIT(0x0004000000000)
#define FLAG_SHADOWFLAME	UI64LIT(0x1000000000000)

void WarlockSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
}

void WarlockSpellHandler::HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i)
{
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();
	
	// Incinerate Rank 1 & 2
	if ((m_spellInfo->GetSpellFamilyFlags() & FLAG_INCINERATE) && m_spellInfo->SpellIconID==2128)
	{
		// Incinerate does more dmg (dmg*0.25) if the target have Immolate debuff.
		// Check aura state for speed but aura state set not only for Immolate spell
		if(unitTarget->HasAuraState(AURA_STATE_CONFLAGRATE))
		{
			Unit::AuraList const& RejorRegr = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
			for(Unit::AuraList::const_iterator i = RejorRegr.begin(); i != RejorRegr.end(); ++i)
			{
				// Immolate
				if((*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_WARLOCK &&
				   ((*i)->GetSpellProto()->SpellFamilyFlags & FLAG_IMMOLATE))
				{
					damage += damage/4;
					break;
				}
			}
		}
	}
	// Shadowflame
	else if (m_spellInfo->GetSpellFamilyFlags() & FLAG_SHADOWFLAME)
	{
		// Apply DOT part
		switch(m_spellInfo->Id)
		{
			case 47897: m_caster->CastSpell(unitTarget, 47960, true); break;
			case 61290: m_caster->CastSpell(unitTarget, 61291, true); break;
			default:
				sLog.outError("Spell::EffectDummy: Unhandeled Shadowflame spell rank %u",m_spellInfo->Id);
				break;
		}
	}
	// Conflagrate - consumes Immolate or Shadowflame
	else if (m_spellInfo->TargetAuraState == AURA_STATE_CONFLAGRATE)
	{
		Aura const* aura = NULL;                // found req. aura for damage calculation
		
		Unit::AuraList const &mPeriodic = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
		for(Unit::AuraList::const_iterator i = mPeriodic.begin(); i != mPeriodic.end(); ++i)
		{
			// for caster applied auras only
			if ((*i)->GetSpellProto()->GetSpellFamilyName() != SPELLFAMILY_WARLOCK ||
				(*i)->GetCasterGUID()!=m_caster->GetGUID())
				continue;
			
			// Shadowflame
			if ((*i)->GetSpellProto()->SpellFamilyFlags2 & 0x00000002)
				aura = *i;                      // remember but wait possible Immolate as primary priority
			
			// Immolate
			if ((*i)->GetSpellProto()->SpellFamilyFlags & FLAG_IMMOLATE)
				aura = *i;                      // it selected always if exist
		}
		
		// found Immolate or Shadowflame
		if (aura)
		{
			// DoT not have applied spell bonuses in m_amount
			int32 damagetick = m_caster->SpellDamageBonus(unitTarget, aura->GetSpellProto(), aura->GetModifier()->m_amount, DOT);
			// Save value of further damage
			spell->m_currentBasePoints[1] = damagetick * 2 / 3;
			damage += damagetick * 2;
			
			// Glyph of Conflagrate
			if (!m_caster->HasAura(56235))
				unitTarget->RemoveAurasByCasterSpell(aura->GetId(), m_caster->GetGUID());
		}
	}
}

void WarlockSpellHandler::SpellDamageBonusDone(SpellEntry* spellProto, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod)
{
	// Drain Soul
    if (spellProto->SpellFamilyFlags & FLAG_DRAIN_SOUL)
    {
        if (pVictim->GetHealth() * 100 / pVictim->GetMaxHealth() <= 25)
          DoneTotalMod *= 4;
    }
}
