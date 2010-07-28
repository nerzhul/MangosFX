#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellMgr.h"
#include "PaladinSpellHandler.h"

INSTANTIATE_SINGLETON_1(PaladinSpellHandler);

#define	FLAGS_JUDGEMENT_COMMAND		UI64LIT(0x00020000000000)

void PaladinSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod)
{
	// Judgement of Command
    if(spell->m_spellInfo->SpellFamilyFlags & FLAGS_JUDGEMENT_COMMAND)
    {
        float ap = spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK);
        int32 holy = spell->GetCaster()->SpellBaseDamageBonus(GetSpellSchoolMask(spell->m_spellInfo)) +
                     spell->GetCaster()->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(spell->m_spellInfo), spell->getUnitTarget());
        spell_bonus += int32(ap * 0.08f) + int32(holy * 13 / 100);
    }
}