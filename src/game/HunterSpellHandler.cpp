#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "HunterSpellHandler.h"


INSTANTIATE_SINGLETON_1(HunterSpellHandler);

#define	FLAGS_KILL_SHOT	UI64LIT(0x80000000000000)
#define FLAG_AIMED_SHOT UI64LIT(0x20000)
#define FLAG_MULTI_SHOT UI64LIT(0x1000)

void HunterSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Kill Shot
	if(spell->m_spellInfo->SpellFamilyFlags & FLAGS_KILL_SHOT)
    {
        // 0.4*RAP added to damage (that is 0.2 if we apply PercentMod (200%) to spell_bonus, too)
        weaponDmgMod = true;
		spell_bonus += int32(0.2f * spell->GetCaster()->GetTotalAttackPowerValue(RANGED_ATTACK) + spell->m_spellInfo->EffectBasePoints[0]);
    }

	if(spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
	{
		if(spell->m_spellInfo->SpellIconID == 3412 && spell->m_spellInfo->SpellVisual[0] == 11725)
		{
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.8);
		}
		else if(spell->m_spellInfo->SpellFamilyFlags == FLAG_AIMED_SHOT)
		{
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.8);
		}
		else if(spell->m_spellInfo->SpellFamilyFlags == FLAG_MULTI_SHOT)
		{
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.8);
		}
	}
}