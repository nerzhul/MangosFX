#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "HunterSpellHandler.h"


INSTANTIATE_SINGLETON_1(HunterSpellHandler);

#define	FLAGS_KILL_SHOT	UI64LIT(0x80000000000000)

void HunterSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Kill Shot
	if(spell->m_spellInfo->SpellFamilyFlags & FLAGS_KILL_SHOT)
    {
        // 0.4*RAP added to damage (that is 0.2 if we apply PercentMod (200%) to spell_bonus, too)
        weaponDmgMod = true;
		spell_bonus += int32(0.2f * spell->GetCaster()->GetTotalAttackPowerValue(RANGED_ATTACK) + spell->m_spellInfo->EffectBasePoints[0]);
    }
}