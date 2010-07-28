#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "DruidSpellHandler.h"


INSTANTIATE_SINGLETON_1(DruidSpellHandler);

#define FLAGS_SHRED		0x40000

void DruidSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Rend and Tear ( on Maul / Shred )
    if (spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000008800))
    {
        if(spell->getUnitTarget() && spell->getUnitTarget()->HasAuraState(AURA_STATE_BLEEDING))
        {
            Unit::AuraList const& aura = spell->GetCaster()->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator itr = aura.begin(); itr != aura.end(); ++itr)
            {
                if ((*itr)->GetSpellProto()->SpellIconID == 2859 && (*itr)->GetEffIndex() == 0)
                {
                    totalDmgPctMod += (totalDmgPctMod * (*itr)->GetModifier()->m_amount) / 100;
                    break;
                }
            }
        }
    }
    // Shred
    if(spell->m_spellInfo->SpellFamilyFlags2 & FLAGS_SHRED)
    {
            weaponDmgMod = true;
            spell_bonus += spell->m_spellInfo->EffectBasePoints[0];
    }
}