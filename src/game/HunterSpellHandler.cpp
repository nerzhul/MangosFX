#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "HunterSpellHandler.h"


INSTANTIATE_SINGLETON_1(HunterSpellHandler);

#define	FLAGS_KILL_SHOT			UI64LIT(0x80000000000000)
#define FLAG_STEADY_SHOT		UI64LIT(0x00000100000000)
#define FLAG_AIMED_SHOT			UI64LIT(0x00000000020000)
#define FLAG_VOLLEY				UI64LIT(0x00000000002000)
#define FLAG_MULTI_SHOT			UI64LIT(0x00000000001000)
#define FLAG_ARCANE_SHOT		UI64LIT(0x00000000000800)

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

void HunterSpellHandler::HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i)
{
	Unit* m_caster = spell->GetCaster();
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	//Gore
    if (m_spellInfo->SpellIconID == 1578)
    {
        if (m_caster->HasAura(57627))           // Charge 6 sec post-affect
          damage *= 2;
    }
    // Mongoose Bite
    else if ((m_spellInfo->SpellFamilyFlags & UI64LIT(0x000000002)) && m_spellInfo->SpellVisual[0]==342)
    {
        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.2f);
    }
    // Counterattack
    else if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x0008000000000000))
    {
        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.2f);
    }
    // Arcane Shot
    else if ((m_spellInfo->SpellFamilyFlags & FLAG_ARCANE_SHOT) && m_spellInfo->maxLevel > 0)
    {
        damage += int32(m_caster->GetTotalAttackPowerValue(RANGED_ATTACK)*0.15f);
    }
	// Volley
	else if (m_spellInfo->SpellFamilyFlags & FLAG_VOLLEY)
	{
		damage += int32(m_caster->GetTotalAttackPowerValue(RANGED_ATTACK)*0.084f);
	}
    // Steady Shot
    else if (m_spellInfo->SpellFamilyFlags & FLAG_STEADY_SHOT)
    {
        int32 base = irand((int32)m_caster->GetWeaponDamageRange(RANGED_ATTACK, MINDAMAGE),(int32)m_caster->GetWeaponDamageRange(RANGED_ATTACK, MAXDAMAGE));
        damage += int32(float(base)/m_caster->GetAttackTime(RANGED_ATTACK)*2800 + m_caster->GetTotalAttackPowerValue(RANGED_ATTACK)*0.1f);
		if(m_caster->GetTypeId() == TYPEID_PLAYER)
			damage += ((Player*)m_caster)->GetAmmoDPS() * ((Player*)m_caster)->GetAPMultiplier(RANGED_ATTACK,true);
		// Steady Shot glyph
		if(m_caster->HasAura(56826) && (spell->getUnitTarget()->HasAura(49001) || spell->getUnitTarget()->HasAura(49000)  || spell->getUnitTarget()->HasAura(27016)))
			damage *= 1.1f;
    }
    // Explosive Trap Effect
    else if (m_spellInfo->SpellFamilyFlags & UI64LIT(0x00000004))
    {
        damage += int32(m_caster->GetTotalAttackPowerValue(RANGED_ATTACK)*0.1f);
    }
}