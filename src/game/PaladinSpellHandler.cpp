#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "PaladinSpellHandler.h"

INSTANTIATE_SINGLETON_1(PaladinSpellHandler);

#define FLAG_AVENGER_SHIELD				UI64LIT(0x00000000004000)
#define FLAG_HAMMER_OF_WRATH			UI64LIT(0x00008000000000)
#define	FLAG_JUDGEMENT_COMMAND			UI64LIT(0x00020000000000)
#define FLAG_HAMMER_OF_THE_RIGHTEOUS	UI64LIT(0x04000000000000)
#define FLAG_SHIELD_OF_RIGHTEOUSNESS	UI64LIT(0x10000000000000)

#define SPELL_CRUSADER_STRIKE				35395
#define SPELL_JUDGEMENT_OF_RIGHTEOUSNESS	20187
#define SPELL_JUDGEMENT						54158

void PaladinSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Judgement of Command
    if(spell->m_spellInfo->SpellFamilyFlags & FLAG_JUDGEMENT_COMMAND)
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

void PaladinSpellHandler::HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i)
{
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();
	
	// Judgement of Righteousness - receive benefit from Spell Damage and Attack power
	if (m_spellInfo->Id == SPELL_JUDGEMENT_OF_RIGHTEOUSNESS)
	{
		float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
		int32 holy = m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo)) +
		m_caster->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(m_spellInfo), unitTarget);
		damage += int32(ap * 0.2f) + int32(holy * 32 / 100);
	}
	// Judgement of Vengeance/Corruption ${1+0.22*$SPH+0.14*$AP} + 10% for each application of Holy Vengeance/Blood Corruption on the target
	else if ((m_spellInfo->SpellFamilyFlags & UI64LIT(0x800000000)) && m_spellInfo->SpellIconID==2292)
	{
		uint32 debuf_id;
		switch(m_spellInfo->Id)
		{
			case 53733: debuf_id = 53742; break;// Judgement of Corruption -> Blood Corruption
			case 31804: debuf_id = 31803; break;// Judgement of Vengeance -> Holy Vengeance
			default: return;
		}
		
		float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
		int32 holy = m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo)) +
		m_caster->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(m_spellInfo), unitTarget);
		damage+=int32(ap * 0.14f) + int32(holy * 22 / 100);
		// Get stack of Holy Vengeance on the target added by caster
		uint32 stacks = 0;
		Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
		for(Unit::AuraList::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
		{
			if( ((*itr)->GetId() == debuf_id) && (*itr)->GetCasterGUID()==m_caster->GetGUID())
			{
				stacks = (*itr)->GetStackAmount();
				break;
			}
		}
		// + 10% for each application of Holy Vengeance on the target
		if(stacks)
			damage += damage * stacks * 10 /100;
	}
	// Avenger's Shield ($m1+0.07*$SPH+0.07*$AP) - ranged sdb for future
	else if (m_spellInfo->SpellFamilyFlags & FLAG_AVENGER_SHIELD)
	{
		float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
		int32 holy = m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo)) +
		m_caster->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(m_spellInfo), unitTarget);
		damage += int32(ap * 0.07f) + int32(holy * 7 / 100);
	}
	// Hammer of Wrath ($m1+0.15*$SPH+0.15*$AP) - ranged type sdb future fix
	else if (m_spellInfo->SpellFamilyFlags & FLAG_HAMMER_OF_WRATH)
	{
		float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
		int32 holy = m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo)) +
		m_caster->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(m_spellInfo), unitTarget);
		damage += int32(ap * 0.15f) + int32(holy * 15 / 100);
	}
	// Hammer of the Righteous
	else if (m_spellInfo->SpellFamilyFlags & FLAG_HAMMER_OF_THE_RIGHTEOUS)
	{
		// Add main hand dps * effect[2] amount
		float average = (m_caster->GetFloatValue(UNIT_FIELD_MINDAMAGE) + m_caster->GetFloatValue(UNIT_FIELD_MAXDAMAGE)) / 2;
		int32 count = m_caster->CalculateSpellDamage(m_spellInfo, 2, m_spellInfo->EffectBasePoints[2], unitTarget);
		damage += count * int32(average * IN_MILLISECONDS) / m_caster->GetAttackTime(BASE_ATTACK);
	}
	// Shield of Righteousness
	else if (m_spellInfo->SpellFamilyFlags & FLAG_SHIELD_OF_RIGHTEOUSNESS)
	{
		damage+=int32(m_caster->GetShieldBlockValue());
	}
	// Judgement
	else if (m_spellInfo->Id == SPELL_JUDGEMENT)
	{
		// [1 + 0.25 * SPH + 0.16 * AP]
		float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
		int32 holy = m_caster->SpellBaseDamageBonus(GetSpellSchoolMask(m_spellInfo)) +
		m_caster->SpellBaseDamageBonusForVictim(GetSpellSchoolMask(m_spellInfo), unitTarget);
		damage += int32(ap * 0.16f) + int32(holy * 25 / 100);
	}
}