#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "DruidSpellHandler.h"
#include "ClassSpellHandler.h"

INSTANTIATE_SINGLETON_1(DruidSpellHandler);

#define FLAG_WRATH		UI64LIT(0x0000000000000001)
#define FLAG_STARFIRE	UI64LIT(0x0000000000000004)
#define FLAG_RAKE		UI64LIT(0x0000000000001000)
#define FLAG_SHRED		UI64LIT(0x0000000000008000)
#define FLAG_RIP		UI64LIT(0x0000000000800000)
#define FLAG_MANGLE_CAT	UI64LIT(0x0000040000000000)
#define FLAG_SWIPE		UI64LIT(0x0010000000000000)

void DruidSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	Unit* unitTarget = spell->getUnitTarget();
	// Rend and Tear ( on Maul / Shred )
    if (spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000008800))
    {
        if(spell->getUnitTarget() && spell->getUnitTarget()->HasAuraState(AURA_STATE_BLEEDING))
        {
            Unit::AuraList const& aura = spell->GetCaster()->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator itr = aura.begin(); itr != aura.end(); ++itr)
            {
                if ((*itr)->GetSpellProto()->SpellIconID == 2859 && (*itr)->GetEffIndex() == 0)
                {
                    totalDmgPctMod += (totalDmgPctMod * (*itr)->GetModifier()->m_amount) / 100;
                    break;
                }
            }
        }
    }
    // Shred
    if(spell->m_spellInfo->SpellFamilyFlags & FLAG_SHRED)
    {
		weaponDmgMod = true;
		spell_bonus += spell->m_spellInfo->EffectBasePoints[0];
    }
	
	// Mangle (Cat): CP
    if (spell->m_spellInfo->SpellFamilyFlags & FLAG_MANGLE_CAT)
    {
        if(spell->GetCaster()->GetTypeId()==TYPEID_PLAYER)
            ((Player*)spell->GetCaster())->AddComboPoints(unitTarget, 1);
    }
	
	if(spell->GetCaster()->GetTypeId() == TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags & FLAG_SHRED) && spell->GetCaster()->HasAura(54815))
	{
		Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
        for(Unit::AuraList::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
        {
			if ((*itr)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID && ((*itr)->GetSpellProto()->SpellFamilyFlags & FLAG_RIP)
				&& (*itr)->GetCaster() == spell->GetCaster())
			{
				if((*itr)->GetAuraMaxDuration() < 18000)
				{
					(*itr)->SetAuraMaxDuration((*itr)->GetAuraMaxDuration() + 2000);
					(*itr)->SetAuraDuration((*itr)->GetAuraDuration() + 2000);
					(*itr)->SendAuraUpdate(false);
				}
			}
		}
	}
	
}

void DruidSpellHandler::HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i)
{
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();

	if (m_caster->GetTypeId()==TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags & UI64LIT(0x000800000)) && spell->m_spellInfo->SpellVisual[0]==6587)
    {
        // converts up to 30 points of energy into ($f1+$AP/410) additional damage
        float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
        float multiple = ap / 410 + spell->m_spellInfo->DmgMultiplier[i];
        damage += int32(((Player*)m_caster)->GetComboPoints() * ap * 7 / 100);
        uint32 energy = m_caster->GetPower(POWER_ENERGY);
        uint32 used_energy = energy > 30 ? 30 : energy;
        damage += int32(used_energy * multiple);
        m_caster->SetPower(POWER_ENERGY,energy-used_energy);
    }
    else if (spell->m_spellInfo->SpellFamilyFlags & FLAG_RAKE && spell->m_spellInfo->Effect[2]==SPELL_EFFECT_ADD_COMBO_POINTS)
    {
        // $AP*0.01 bonus
        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
		// Glyoh of rake
		if(m_caster->HasAura(54821))
			m_caster->CastSpell(unitTarget,54820,true);
    }
    else if (spell->m_spellInfo->SpellFamilyFlags & FLAG_SWIPE)
    {
        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)*0.08f);
    }
	// Glyph of starfire
	else if(m_caster->GetTypeId() == TYPEID_PLAYER && (spell->m_spellInfo->SpellFamilyFlags & FLAG_STARFIRE) && m_caster->HasAura(54845))
	{
		Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
        for(Unit::AuraList::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
        {
			if ((*itr)->GetSpellProto()->SpellFamilyName == SPELLFAMILY_DRUID && (*itr)->GetSpellProto()->SpellIconID == 225
				&& (*itr)->GetCaster() == m_caster)
			{
				if((*itr)->GetAuraMaxDuration() < 24000)
				{
					(*itr)->SetAuraMaxDuration((*itr)->GetAuraMaxDuration() + 3000);
					(*itr)->SetAuraDuration((*itr)->GetAuraDuration() + 3000);
					(*itr)->SendAuraUpdate(false);
				}
			}
		}
	}
}
void DruidSpellHandler::SpellDamageBonusDone(SpellEntry* spellProto, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod)
{
	// Improved Insect Swarm (Wrath part)
	if (spellProto->SpellFamilyFlags & FLAG_WRATH)
	{
		// if Insect Swarm on target
		if(sClassSpellHandler.GetAuraByName(pVictim,DRUID_INSECT_SWARM,caster->GetGUID()))
		{
			Unit::AuraList const& improvedSwarm = caster->GetAurasByType(SPELL_AURA_DUMMY);
			for(Unit::AuraList::const_iterator iter = improvedSwarm.begin(); iter != improvedSwarm.end(); ++iter)
			{
				if ((*iter)->GetSpellProto()->SpellIconID == 1771)
				{
					DoneTotalMod *= ((*iter)->GetModifier()->m_amount+100.0f) / 100.0f;
					break;
				}
			}
		}
	}
}
