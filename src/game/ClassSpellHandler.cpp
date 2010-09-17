#include <Policies/SingletonImp.h>

#include "Spell.h"
#include "SpellMgr.h"
#include "SpellAuras.h"

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

void ClassSpellHandler::HandleSchoolDmg(Spell* spell, int32 &damage, SpellEffectIndex i)
{
	if(!spell)
		return;

	switch(spell->m_spellInfo->SpellFamilyName)
	{
		/*case SPELLFAMILY_DEATHKNIGHT:
			return sDeathknightSpellHandler.HandleSchoolDmg(spell);
			break;*/
		case SPELLFAMILY_DRUID:
			sDruidSpellHandler.HandleSchoolDmg(spell,damage,i);
			break;
		case SPELLFAMILY_HUNTER:
			sHunterSpellHandler.HandleSchoolDmg(spell,damage,i);
			break;
		/*case SPELLFAMILY_MAGE:
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

void ClassSpellHandler::PeriodicDummyTick(Aura* aura)
{
	if(!aura)
		return;

	switch(aura->GetSpellProto()->SpellFamilyName)
	{
		case SPELLFAMILY_DEATHKNIGHT:
			return sDeathknightSpellHandler.PeriodicDummyTick(aura);
			break;
		/*case SPELLFAMILY_DRUID:
			sDruidSpellHandler.HandleSchoolDmg(spell,damage,i);
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

AuraCarac caracTab[MAX_AURA_NAMES] =
{
	{SPELL_AURA_ADD_PCT_MODIFIER,	SPELLFAMILY_PRIEST,	UI64LIT(0x00400000),0},
	{SPELL_AURA_PERIODIC_DAMAGE,	SPELLFAMILY_PRIEST, UI64LIT(0x00100000),0},
	{SPELL_AURA_PERIODIC_HEAL,		SPELLFAMILY_PRIEST, UI64LIT(0x00000040),0},
	{SPELL_AURA_PERIODIC_LEECH,		SPELLFAMILY_PRIEST, UI64LIT(0x02000000),0},
	{SPELL_AURA_ADD_FLAT_MODIFIER,	SPELLFAMILY_PRIEST, UI64LIT(0x00000000),0},
	{SPELL_AURA_ADD_FLAT_MODIFIER,	SPELLFAMILY_MAGE,	UI64LIT(0x00000000),0},
	{SPELL_AURA_ADD_FLAT_MODIFIER,	SPELLFAMILY_ROGUE,	UI64LIT(0x00000000),0},
};

Aura* ClassSpellHandler::GetSpecialAura(Unit* u, AuraName aName)
{
	AuraList const& aList = u->GetAurasByType(caracTab[aName].aType);
	for(AuraList::const_iterator i = aList.begin(); i != aList.end(); ++i)
	{
		if(!(caracTab[aName].sfName == (*i)->GetSpellProto()->SpellFamilyName))
			continue;

		switch(aName)
		{
			case PRIEST_SILENT_RESOLVE:
				if((*i)->GetSpellProto()->SpellIconID == 338)
					return (*i);
				break;
			case MAGE_ARCANE_SUBTLELY:
				if((*i)->GetSpellProto()->SpellIconID == 74)
					return (*i);
				break;
			case ROGUE_VILE_POISON:
				if((*i)->GetSpellProto()->SpellIconID == 74)
					return (*i);
				break;
		}
	}

	return NULL;
}

Aura* ClassSpellHandler::GetAuraByName(Unit *u, AuraName aName, uint64 casterGUID)
{
	if(!u || aName >= MAX_AURA_NAMES || aName < 0)
		return NULL;

	if(caracTab[aName].sFlag == UI64LIT(0x0000000) && caracTab[aName].sFlag2 == 0)
		if(Aura* aur = sClassSpellHandler.GetSpecialAura(u,aName))
			return aur;

	if(Aura* aur = u->GetAura(caracTab[aName].aType,caracTab[aName].sfName,caracTab[aName].sFlag,caracTab[aName].sFlag2,casterGUID))
		return aur;

	return NULL;
}