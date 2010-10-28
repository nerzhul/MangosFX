#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "ShamanSpellHandler.h"

INSTANTIATE_SINGLETON_1(ShamanSpellHandler);
#define FLAG_STORMSTRIKE		UI64LIT(0x001000000000)

void ShamanSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Skyshatter Harness item set bonus
    // Stormstrike
    if(spell->m_spellInfo->GetSpellFamilyFlags() & FLAG_STORMSTRIKE)
    {
        Unit::AuraList const& m_OverrideClassScript = spell->GetCaster()->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for(Unit::AuraList::const_iterator citr = m_OverrideClassScript.begin(); citr != m_OverrideClassScript.end(); ++citr)
        {
            // Stormstrike AP Buff
            if ( (*citr)->GetModifier()->m_miscvalue == 5634 )
            {
                spell->GetCaster()->CastSpell(spell->GetCaster(), 38430, true, NULL, *citr);
                break;
            }
        }
    }
}
void ShamanSpellHandler::SpellDamageBonusDone(SpellEntry* spellProto, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod)
{
	// Frozen power
	if((spellProto->GetSpellFamilyFlags() & UI64LIT(0x80100003)) || spellProto->Id == 60103)
	{
		Aura* aur = caster->GetAura(63373);
		if(!aur)
			aur = caster->GetAura(63374);
		if(aur)
			DoneTotalMod *= (100.0f + aur->GetModifier()->m_amount) / 100.0f;
	}
}