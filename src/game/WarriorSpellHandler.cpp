#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "WarriorSpellHandler.h"

INSTANTIATE_SINGLETON_1(WarriorSpellHandler);

#define FLAG_THUNDER_CLAP		UI64LIT(0x00000000000080)
#define FLAG_REVENGE			UI64LIT(0x00000000000400)
#define FLAG_MORTALSTRIKE		UI64LIT(0x00000002000000)
#define FLAG_HEROIC_THROW		UI64LIT(0x00000100000000)
#define FLAG_RETALIATION		UI64LIT(0x00000800000000)
#define FLAG_DEVASTATE			UI64LIT(0x00004000000000)
#define FLAG_VICTORY_RUSH		UI64LIT(0x00010000000000)
#define FLAG_SHIELD_SLAM		UI64LIT(0x00020000000000)
#define FLAG_BLOODTHIRST		UI64LIT(0x00040000000000)
#define FLAG_SHOCKWAVE			UI64LIT(0x00800000000000)
#define FLAG_SHATTERING_THROW	UI64LIT(0x40000000000000)
#define SPELL_WHIRLWIND 1680
#define SPELL_OVERPOWER 7384
#define SPELL_WHIRLWIND_OFFHAND 44949

void WarriorSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
	// Devastate bonus and sunder armor refresh
    if(spell->m_spellInfo->SpellVisual[0] == 12295 && spell->m_spellInfo->SpellIconID == 1508)
    {
        uint32 stack = 0;
        // Need refresh all Sunder Armor auras from this caster
        Unit::AuraMap& suAuras = spell->getUnitTarget()->GetAuras();
        SpellEntry const *spellInfo;
        for(Unit::AuraMap::iterator itr = suAuras.begin(); itr != suAuras.end(); ++itr)
        {
            spellInfo = (*itr).second->GetSpellProto();
            if( spellInfo->SpellFamilyName == SPELLFAMILY_WARRIOR &&
                (spellInfo->SpellFamilyFlags & UI64LIT(0x0000000000004000)) &&
                (*itr).second->GetCasterGUID() == spell->GetCaster()->GetGUID())
            {
                (*itr).second->RefreshAura();
                stack = (*itr).second->GetStackAmount();
                break;
            }
        }
        if (stack)
            spell_bonus += stack * spell->CalculateDamage(2, spell->getUnitTarget());
        if (!stack || stack < spellInfo->StackAmount)
            // Devastate causing Sunder Armor Effect
            // and no need to cast over max stack amount
            spell->GetCaster()->CastSpell(spell->getUnitTarget(), 58567, true);
		// glyph of devastate
		if(spell->GetCaster()->HasAura(58388))
			spell->GetCaster()->CastSpell(spell->getUnitTarget(), 58567, true);
    }

	if(spell->GetCaster()->GetTypeId() == TYPEID_PLAYER)
	{
		if(spell->m_spellInfo->SpellFamilyFlags == FLAG_DEVASTATE)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
		else if(spell->m_spellInfo->SpellFamilyFlags == FLAG_MORTALSTRIKE)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
		else if(spell->m_spellInfo->Id == SPELL_WHIRLWIND || spell->m_spellInfo->Id == SPELL_WHIRLWIND_OFFHAND ||
			spell->m_spellInfo->Id == SPELL_OVERPOWER)
		{
			Item* weapon = ((Player*)spell->GetCaster())->GetWeaponForAttack(spell->getAttackType(),true,true);
			if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 1.7);
			else
				spell_bonus += int32(spell->GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK) / 14 * 2.2);
		}
	}
}

void WarriorSpellHandler::HandleSchoolDmg(Spell* spell, int32 &damage, SpellEffectIndex i)
{
	const SpellEntry* m_spellInfo = spell->m_spellInfo;
	Unit* m_caster = spell->GetCaster();
	Unit* unitTarget = spell->getUnitTarget();
	// Bloodthirst
	if (m_spellInfo->SpellFamilyFlags & FLAG_BLOODTHIRST)
		damage = uint32(damage * (m_caster->GetTotalAttackPowerValue(BASE_ATTACK)) / 100);
	// Shield Slam
	else if ((m_spellInfo->SpellFamilyFlags & FLAG_SHIELD_SLAM) && m_spellInfo->Category==1209)
	{
		damage += int32(m_caster->GetShieldBlockValue());
		// glyph of shield slam
		if (m_caster->HasAura(58375))
			m_caster->CastSpell(m_caster, 58374, true);
	}
	// Victory Rush
	else if (m_spellInfo->SpellFamilyFlags & FLAG_VICTORY_RUSH)
	{
		damage = uint32(damage * m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
		m_caster->ModifyAuraState(AURA_STATE_WARRIOR_VICTORY_RUSH, false);
	}
	// Revenge ${$m1+$AP*0.310} to ${$M1+$AP*0.310}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_REVENGE)
		damage+= uint32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.310f);
	// Heroic Throw ${$m1+$AP*.50}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_HEROIC_THROW)
		damage+= uint32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.5f);
	// Shattering Throw ${$m1+$AP*.50}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_SHATTERING_THROW)
		damage+= uint32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.5f);
	// Shockwave ${$m3/100*$AP}
	else if (m_spellInfo->SpellFamilyFlags & FLAG_SHOCKWAVE)
	{
		int32 pct = m_caster->CalculateSpellDamage(m_spellInfo, 2, m_spellInfo->EffectBasePoints[2], unitTarget);
		if (pct > 0)
			damage+= int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * pct / 100);
	}
	// Thunder Clap
	else if (m_spellInfo->SpellFamilyFlags & FLAG_THUNDER_CLAP)
	{
		damage+=int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 12 / 100);
	}
}

bool WarriorSpellHandler::HandleProcTriggerSpell(Unit *u, const SpellEntry* auraSpellInfo, uint32 &trig_sp_id, int32* basepoints)
{
	// Deep Wounds (replace triggered spells to directly apply DoT), dot spell have finilyflags
    if (auraSpellInfo->SpellFamilyFlags == UI64LIT(0x0) && auraSpellInfo->SpellIconID == 243)
    {
        float weaponDamage;
        // DW should benefit of attack power, damage percent mods etc.
        // TODO: check if using offhand damage is correct and if it should be divided by 2
        if (u->haveOffhandWeapon() && u->getAttackTimer(BASE_ATTACK) > u->getAttackTimer(OFF_ATTACK))
            weaponDamage = (u->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + u->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE))/2;
        else
            weaponDamage = (u->GetFloatValue(UNIT_FIELD_MINDAMAGE) + u->GetFloatValue(UNIT_FIELD_MAXDAMAGE))/2;

        switch (auraSpellInfo->Id)
        {
            case 12834: basepoints[0] = int32(weaponDamage * 16 / 100); break;
            case 12849: basepoints[0] = int32(weaponDamage * 32 / 100); break;
            case 12867: basepoints[0] = int32(weaponDamage * 48 / 100); break;
            // Impossible case
            default:
                sLog.outError("Unit::HandleProcTriggerSpell: DW unknown spell rank %u",auraSpellInfo->Id);
                return false;
        }

        // 1 tick/sec * 6 sec = 6 ticks
        basepoints[0] /= 6;
        trig_sp_id = 12721;
    }
	return true;
}

bool WarriorSpellHandler::HandleDummyAuraProc(Unit* u, const SpellEntry* dummySpell, uint32 &triggered_spell_id, int32 triggerAmount, const SpellEntry* procSpell, uint32 procEx, Unit* target, Unit* pVictim, int32 &basepoints0)
{
	// Retaliation
    if (dummySpell->SpellFamilyFlags == FLAG_RETALIATION)
    {
        // check attack comes not from behind
        if (!u->HasInArc(M_PI, pVictim))
            return false;

        triggered_spell_id = 22858;
        return true;
    }
    // Second Wind
    if (dummySpell->SpellIconID == 1697)
    {
        // only for spells and hit/crit (trigger start always) and not start from self casted spells (5530 Mace Stun Effect for example)
        if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) || u == pVictim)
            return false;
        // Need stun or root mechanic
        if (!(GetAllSpellMechanicMask(procSpell) & IMMUNE_TO_ROOT_AND_STUN_MASK))
            return false;

        switch (dummySpell->Id)
        {
            case 29838: triggered_spell_id=29842; break;
            case 29834: triggered_spell_id=29841; break;
            case 42770: triggered_spell_id=42771; break;
            default:
                sLog.outError("Unit::HandleDummyAuraProc: non handled spell id: %u (SW)",dummySpell->Id);
            return false;
        }

        target = u;
		return true;
    }
    // Damage Shield
    else if (dummySpell->SpellIconID == 3214)
    {
        triggered_spell_id = 59653;
        basepoints0 = u->GetShieldBlockValue() * triggerAmount / 100;
        return true;
    }

    // Sweeping Strikes
    switch (dummySpell->Id)
    {
       case 12328: // Sweeping Strikes
       {
           // prevent chain of triggered spell from same triggered spell
		   if(procSpell && procSpell->Id == 26654)
                return false;

			target = u->SelectNearbyTarget(pVictim);
			if(!target)
				return false;

			triggered_spell_id = 26654;
			break;
	   }
	   case 58375: // Glyph of Blocking
        {
            triggered_spell_id = 58374;
            break;
        }
        case 58388: // Glyph of Devastate
        {
            triggered_spell_id = 58567;
            break;
        }
		case 12311: // Gag Order rank 1 
		case 12958: // Gag Order rank 2 
		{ 
			triggered_spell_id = 18498; 
			break; 
		}
    }

	return true;
}

void WarriorSpellHandler::HandleAuraDummyWithApply(Aura* aura,Unit* caster,Unit* target)
{
	SpellEntry const* m_spellProto = aura->GetSpellProto();
	Unit* m_target = target;
	// Overpower
    if(m_spellProto->SpellFamilyFlags & UI64LIT(0x0000000000000004))
    {
        // Must be casting target
        if (!m_target->IsNonMeleeSpellCasted(false))
            return;

        Unit* caster = aura->GetCaster();
        if (!caster)
            return;

        Unit::AuraList const& modifierAuras = caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
        for(Unit::AuraList::const_iterator itr = modifierAuras.begin(); itr != modifierAuras.end(); ++itr)
        {
            // Unrelenting Assault
            if((*itr)->GetSpellProto()->SpellFamilyName==SPELLFAMILY_WARRIOR && (*itr)->GetSpellProto()->SpellIconID == 2775)
            {
                switch ((*itr)->GetSpellProto()->Id)
                {
                    case 46859:                 // Unrelenting Assault, rank 1
                        m_target->CastSpell(m_target,64849,true,NULL,(*itr));
                        break;
                    case 46860:                 // Unrelenting Assault, rank 2
                        m_target->CastSpell(m_target,64850,true,NULL,(*itr));
                        break;
                    default:
                        break;
                }
                break;
            }
        }
        return;
    }
}