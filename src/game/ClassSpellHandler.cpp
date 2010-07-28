#include <Policies/SingletonImp.h>

#include "Spell.h"
#include "SpellMgr.h"

#include "ClassSpellHandler.h"

#include "DeathknightSpellHandler.h"
#include "DruidSpellHandler.h"
#include "HunterSpellHandler.h"
#include "PaladinSpellHandler.h"
#include "WarlockSpellHandler.h"
#include "WarriorSpellHandler.h"

INSTANTIATE_SINGLETON_1(ClassSpellHandler);

void ClassSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	if(!spell || !spell->m_spellInfo)
		return;

	switch(spell->m_spellInfo->SpellFamilyName)
	{
		case SPELLFAMILY_DEATHKNIGHT:
			sDeathknightSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_DRUID:
			sDruidSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_HUNTER:
			sHunterSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_PALADIN:
			sPaladinSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_ROGUE:
			sRogueSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_WARRIOR:
			sWarriorSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_WARLOCK:
			sWarlockSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
	}
}