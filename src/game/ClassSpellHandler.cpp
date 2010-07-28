#include <Policies/SingletonImp.h>

#include "Spell.h"
#include "SpellMgr.h"

#include "ClassSpellHandler.h"

#include "HunterSpellHandler.h"
#include "PaladinSpellHandler.h"

INSTANTIATE_SINGLETON_1(ClassSpellHandler);

void ClassSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod)
{
	if(!spell)
		return;

	switch(spell->m_spellInfo->SpellFamilyName)
	{
		case SPELLFAMILY_HUNTER:
			sHunterSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod);
			break;
		case SPELLFAMILY_PALADIN:
			sPaladinSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod);
			break;
	}
}