#include <Policies/SingletonImp.h>

#include "Spell.h"
#include "SpellMgr.h"

#include "ClassSpellHandler.h"

#include "DeathknightSpellHandler.h"
#include "DruidSpellHandler.h"
#include "HunterSpellHandler.h"
#include "MageSpellHandler.h"
#include "PaladinSpellHandler.h"
#include "PriestSpellHandler.h"
#include "RogueSpellHandler.h"
#include "ShamanSpellHandler.h"
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
		case SPELLFAMILY_MAGE:
			sMageSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_PALADIN:
			sPaladinSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_PRIEST:
			sPriestSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_ROGUE:
			sRogueSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_SHAMAN:
			sShamanSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_WARRIOR:
			sWarriorSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
		case SPELLFAMILY_WARLOCK:
			sWarlockSpellHandler.HandleEffectWeaponDamage(spell,spell_bonus,weaponDmgMod,totalDmgPctMod);
			break;
	}
}

void ClassSpellHandler::HandleDummyAuraProc(Unit *u, Spell *dummy, uint32 &trig_sp_id)
{
	if(!dummy || !u)
		return;

	switch(dummy->m_spellInfo->SpellFamilyName)
	{
		/*case SPELLFAMILY_DEATHKNIGHT:
			sDeathknightSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_DRUID:
			sDruidSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_HUNTER:
			sHunterSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_MAGE:
			sMageSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_PALADIN:
			sPaladinSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_PRIEST:
			sPriestSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_ROGUE:
			sRogueSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_SHAMAN:
			sShamanSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_WARRIOR:
			sWarriorSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_WARLOCK:
			sWarlockSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;*/
	}
}

bool ClassSpellHandler::HandleEffectDummy(Spell* spell)
{
	if(!spell)
		return false;

	switch(spell->m_spellInfo->SpellFamilyName)
	{
		case SPELLFAMILY_DEATHKNIGHT:
			return sDeathknightSpellHandler.HandleEffectDummy(spell);
			break;
		/*case SPELLFAMILY_DRUID:
			sDruidSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_HUNTER:
			sHunterSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_MAGE:
			sMageSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_PALADIN:
			sPaladinSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_PRIEST:
			sPriestSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_ROGUE:
			sRogueSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_SHAMAN:
			sShamanSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_WARRIOR:
			sWarriorSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;
		case SPELLFAMILY_WARLOCK:
			sWarlockSpellHandler.HandleDummyAuraProc(u, dummy, trig_sp_id);
			break;*/
	}

	return true;
}