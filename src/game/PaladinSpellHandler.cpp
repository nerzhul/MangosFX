#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellMgr.h"
#include "PaladinSpellHandler.h"

INSTANTIATE_SINGLETON_1(PaladinSpellHandler);

#define	FLAGS_JUDGEMENT_COMMAND		UI64LIT(0x00020000000000)
#define SPELL_CRUSADER_STRIKE 35395

void PaladinSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Judgement of Command
    if(spell->m_spellInfo->SpellFamilyFlags & FLAGS_JUDGEMENT_COMMAND)
    {
        float ap = spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK);
        int32 holy = spell->GetCaster()->SpellBaseDamageBonus(GetSpellSchoolMask(spell->m_spellInfo)) +
                     spell->GetCaster()->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(spell->m_spellInfo), spell->getUnitTarget());
        spell_bonus += int32(ap * 0.08f) + int32(holy * 13 / 100);
    }
	else if(spell->m_spellInfo->Id == SPELL_CRUSADER_STRIKE && spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
	{
		Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
		if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
		else
			spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
	}
}