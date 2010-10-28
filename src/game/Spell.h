/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __SPELL_H
#define __SPELL_H

#include "GridDefines.h"
#include "SharedDefines.h"
#include "DBCEnums.h"
#include "LootMgr.h"
#include "Unit.h"
#include "Player.h"

class WorldSession;
class WorldPacket;
class DynamicObj;
class Item;
class GameObject;
class Group;
class Aura;

enum SpellCastTargetFlags
{
    TARGET_FLAG_SELF            = 0x00000000,
    TARGET_FLAG_UNUSED1         = 0x00000001,               // not used in any spells as of 3.0.3 (can be set dynamically)
    TARGET_FLAG_UNIT            = 0x00000002,               // pguid
    TARGET_FLAG_UNUSED2         = 0x00000004,               // not used in any spells as of 3.0.3 (can be set dynamically)
    TARGET_FLAG_UNUSED3         = 0x00000008,               // not used in any spells as of 3.0.3 (can be set dynamically)
    TARGET_FLAG_ITEM            = 0x00000010,               // pguid
    TARGET_FLAG_SOURCE_LOCATION = 0x00000020,               // 3 float
    TARGET_FLAG_DEST_LOCATION   = 0x00000040,               // 3 float
    TARGET_FLAG_OBJECT_UNK      = 0x00000080,               // used in 7 spells only
    TARGET_FLAG_UNIT_UNK        = 0x00000100,               // looks like self target (480 spells)
    TARGET_FLAG_PVP_CORPSE      = 0x00000200,               // pguid
    TARGET_FLAG_UNIT_CORPSE     = 0x00000400,               // 10 spells (gathering professions)
    TARGET_FLAG_OBJECT          = 0x00000800,               // pguid, 2 spells
    TARGET_FLAG_TRADE_ITEM      = 0x00001000,               // pguid, 0 spells
    TARGET_FLAG_STRING          = 0x00002000,               // string, 0 spells
    TARGET_FLAG_UNK1            = 0x00004000,               // 199 spells, opening object/lock
    TARGET_FLAG_CORPSE          = 0x00008000,               // pguid, resurrection spells
    TARGET_FLAG_UNK2            = 0x00010000,               // pguid, not used in any spells as of 3.0.3 (can be set dynamically)
    TARGET_FLAG_GLYPH           = 0x00020000,               // used in glyph spells
	TARGET_FLAG_UNK3            = 0x00040000,               // 
    TARGET_FLAG_UNK4            = 0x00080000                // uint32, loop { vec3, guid -> if guid == 0 break }
};

enum SpellCastFlags
{
    CAST_FLAG_NONE               = 0x00000000,
    CAST_FLAG_UNKNOWN0           = 0x00000001,              // may be pending spell cast
    CAST_FLAG_UNKNOWN1           = 0x00000002,
    CAST_FLAG_UNKNOWN11          = 0x00000004,
    CAST_FLAG_UNKNOWN12          = 0x00000008,
    CAST_FLAG_UNKNOWN2           = 0x00000010,
    CAST_FLAG_AMMO               = 0x00000020,              // Projectiles visual
    CAST_FLAG_UNKNOWN8           = 0x00000040,
    CAST_FLAG_UNKNOWN9           = 0x00000080,
    CAST_FLAG_UNKNOWN3           = 0x00000100,
    CAST_FLAG_UNKNOWN13          = 0x00000200,
    CAST_FLAG_UNKNOWN14          = 0x00000400,
    CAST_FLAG_UNKNOWN6           = 0x00000800,              // wotlk, trigger rune cooldown
    CAST_FLAG_UNKNOWN15          = 0x00001000,
    CAST_FLAG_UNKNOWN16          = 0x00002000,
    CAST_FLAG_UNKNOWN17          = 0x00004000,
    CAST_FLAG_UNKNOWN18          = 0x00008000,
    CAST_FLAG_UNKNOWN19          = 0x00010000,
    CAST_FLAG_UNKNOWN4           = 0x00020000,              // wotlk
    CAST_FLAG_UNKNOWN10          = 0x00040000,
    CAST_FLAG_UNKNOWN5           = 0x00080000,              // wotlk
    CAST_FLAG_UNKNOWN20          = 0x00100000,
    CAST_FLAG_UNKNOWN7           = 0x00200000,              // wotlk, rune cooldown list
    CAST_FLAG_UNKNOWN21          = 0x04000000
};

enum SpellNotifyPushType
{
    PUSH_IN_FRONT,
    PUSH_IN_FRONT_90,
    PUSH_IN_FRONT_30,
    PUSH_IN_FRONT_15,
    PUSH_IN_BACK,
    PUSH_SELF_CENTER,
    PUSH_DEST_CENTER,
    PUSH_TARGET_CENTER
};

bool IsQuestTameSpell(uint32 spellId);

namespace MaNGOS
{
    struct SpellNotifierPlayer;
    struct SpellNotifierCreatureAndPlayer;
}

class SpellCastTargets
{
    public:
        SpellCastTargets();
        ~SpellCastTargets();

        //bool read ( WorldPacket * data, Unit *caster );
		bool read ( WorldPacket * data, Unit *caster, SpellEntry const* spell = NULL );
        void write ( WorldPacket * data );

        SpellCastTargets& operator=(const SpellCastTargets &target)
        {
            m_unitTarget = target.m_unitTarget;
            m_itemTarget = target.m_itemTarget;
            m_GOTarget   = target.m_GOTarget;

            m_unitTargetGUID   = target.m_unitTargetGUID;
            m_GOTargetGUID     = target.m_GOTargetGUID;
            m_CorpseTargetGUID = target.m_CorpseTargetGUID;
            m_itemTargetGUID   = target.m_itemTargetGUID;

            m_itemTargetEntry  = target.m_itemTargetEntry;

            m_srcX = target.m_srcX;
            m_srcY = target.m_srcY;
            m_srcZ = target.m_srcZ;

            m_destX = target.m_destX;
            m_destY = target.m_destY;
            m_destZ = target.m_destZ;

            m_strTarget = target.m_strTarget;

            m_targetMask = target.m_targetMask;

            return *this;
        }

        uint64 getUnitTargetGUID() const { return m_unitTargetGUID; }
        Unit *getUnitTarget() const { return m_unitTarget; }
        void setUnitTarget(Unit *target);
        void setDestination(float x, float y, float z);
        void setSource(float x, float y, float z);

        uint64 getGOTargetGUID() const { return m_GOTargetGUID; }
        GameObject *getGOTarget() const { return m_GOTarget; }
        void setGOTarget(GameObject *target);

        uint64 getCorpseTargetGUID() const { return m_CorpseTargetGUID; }
        void setCorpseTarget(Corpse* corpse);
        uint64 getItemTargetGUID() const { return m_itemTargetGUID; }
        Item* getItemTarget() const { return m_itemTarget; }
        uint32 getItemTargetEntry() const { return m_itemTargetEntry; }
        void setItemTarget(Item* item);
        void updateTradeSlotItem()
        {
            if(m_itemTarget && (m_targetMask & TARGET_FLAG_TRADE_ITEM))
            {
                m_itemTargetGUID = m_itemTarget->GetGUID();
                m_itemTargetEntry = m_itemTarget->GetEntry();
            }
        }

        bool IsEmpty() const { return m_GOTargetGUID==0 && m_unitTargetGUID==0 && m_itemTarget==0 && m_CorpseTargetGUID==0; }
		bool HasSrc() const { return m_targetMask & TARGET_FLAG_SOURCE_LOCATION; }
        bool HasDst() const { return m_targetMask & TARGET_FLAG_DEST_LOCATION; }

        void Update(Unit* caster);

        float m_srcX, m_srcY, m_srcZ;
        float m_destX, m_destY, m_destZ;
        std::string m_strTarget;

        uint32 m_targetMask;
    private:
        // objects (can be used at spell creating and after Update at casting
        Unit *m_unitTarget;
        GameObject *m_GOTarget;
        Item *m_itemTarget;

        // object GUID/etc, can be used always
        uint64 m_unitTargetGUID;
        uint64 m_GOTargetGUID;
        uint64 m_CorpseTargetGUID;
        uint64 m_itemTargetGUID;
        uint32 m_itemTargetEntry;
};

enum SpellState
{
    SPELL_STATE_NULL      = 0,
    SPELL_STATE_PREPARING = 1,
    SPELL_STATE_CASTING   = 2,
    SPELL_STATE_FINISHED  = 3,
    SPELL_STATE_IDLE      = 4,
    SPELL_STATE_DELAYED   = 5
};

enum SpellTargets
{
    SPELL_TARGETS_HOSTILE,
    SPELL_TARGETS_NOT_FRIENDLY,
    SPELL_TARGETS_NOT_HOSTILE,
    SPELL_TARGETS_FRIENDLY,
    SPELL_TARGETS_AOE_DAMAGE,
    SPELL_TARGETS_ALL
};

#define SPELL_SPELL_CHANNEL_UPDATE_INTERVAL (1*IN_MILLISECONDS)

typedef std::multimap<uint64, uint64> SpellTargetTimeMap;

class Spell
{
    friend struct MaNGOS::SpellNotifierPlayer;
    friend struct MaNGOS::SpellNotifierCreatureAndPlayer;
    friend void Unit::SetCurrentCastedSpell( Spell * pSpell );
    public:
        void EffectEmpty(SpellEffectEntry const* effect);
        void EffectNULL(SpellEffectEntry const* effect);
        void EffectUnused(SpellEffectEntry const* effect);
        void EffectDistract(SpellEffectEntry const* effect);
        void EffectPull(SpellEffectEntry const* effect);
        void EffectSchoolDMG(SpellEffectEntry const* effect);
        void EffectEnvironmentalDMG(SpellEffectEntry const* effect);
        void EffectInstaKill(SpellEffectEntry const* effect);
        void EffectDummy(SpellEffectEntry const* effect);
        void EffectTeleportUnits(SpellEffectEntry const* effect);
        void EffectApplyAura(SpellEffectEntry const* effect);
        void EffectSendEvent(SpellEffectEntry const* effect);
        void EffectPowerBurn(SpellEffectEntry const* effect);
        void EffectPowerDrain(SpellEffectEntry const* effect);
        void EffectHeal(SpellEffectEntry const* effect);
        void EffectBind(SpellEffectEntry const* effect);
        void EffectHealthLeech(SpellEffectEntry const* effect);
        void EffectQuestComplete(SpellEffectEntry const* effect);
        void EffectCreateItem(SpellEffectEntry const* effect);
        void EffectCreateItem2(SpellEffectEntry const* effect);
        void EffectCreateRandomItem(SpellEffectEntry const* effect);
        void EffectPersistentAA(SpellEffectEntry const* effect);
        void EffectEnergize(SpellEffectEntry const* effect);
        void EffectOpenLock(SpellEffectEntry const* effect);
        void EffectSummonChangeItem(SpellEffectEntry const* effect);
        void EffectProficiency(SpellEffectEntry const* effect);
        void EffectApplyAreaAura(SpellEffectEntry const* effect);
        void EffectSummonType(SpellEffectEntry const* effect);
        void EffectLearnSpell(SpellEffectEntry const* effect);
        void EffectDispel(SpellEffectEntry const* effect);
        void EffectDualWield(SpellEffectEntry const* effect);
        void EffectPickPocket(SpellEffectEntry const* effect);
        void EffectAddFarsight(SpellEffectEntry const* effect);
        void EffectHealMechanical(SpellEffectEntry const* effect);
        void EffectJump(SpellEffectEntry const* effect);
        void EffectTeleUnitsFaceCaster(SpellEffectEntry const* effect);
        void EffectLearnSkill(SpellEffectEntry const* effect);
        void EffectAddHonor(SpellEffectEntry const* effect);
        void EffectTradeSkill(SpellEffectEntry const* effect);
        void EffectEnchantItemPerm(SpellEffectEntry const* effect);
        void EffectEnchantItemTmp(SpellEffectEntry const* effect);
        void EffectTameCreature(SpellEffectEntry const* effect);
        void EffectSummonPet(SpellEffectEntry const* effect);
        void EffectLearnPetSpell(SpellEffectEntry const* effect);
        void EffectWeaponDmg(SpellEffectEntry const* effect);
        void EffectForceCast(SpellEffectEntry const* effect);
        void EffectTriggerSpell(SpellEffectEntry const* effect);
        void EffectTriggerMissileSpell(SpellEffectEntry const* effect);
        void EffectThreat(SpellEffectEntry const* effect);
        void EffectRestoreItemCharges(SpellEffectEntry const* effect);
        void EffectHealMaxHealth(SpellEffectEntry const* effect);
        void EffectInterruptCast(SpellEffectEntry const* effect);
        void EffectSummonWild(SpellEffectEntry const* effect,uint32 forcefaction);
		void EffectSummonTotem(SpellEffectEntry const* effect, uint8 slot=0);
		void EffectSummonVehicle(SpellEffectEntry const* effect);
		void EffectSummonWild(SpellEffectEntry const* effect);
		void EffectSummonGuardian(SpellEffectEntry const* effect, uint32 faction=0);
		void EffectSummonCritter(SpellEffectEntry const* effect, uint32 faction=0);
        void EffectScriptEffect(SpellEffectEntry const* effect);
        void EffectSanctuary(SpellEffectEntry const* effect);
        void EffectAddComboPoints(SpellEffectEntry const* effect);
        void EffectDuel(SpellEffectEntry const* effect);
        void EffectStuck(SpellEffectEntry const* effect);
        void EffectSummonPlayer(SpellEffectEntry const* effect);
        void EffectActivateObject(SpellEffectEntry const* effect);
        void EffectApplyGlyph(SpellEffectEntry const* effect);
        void EffectEnchantHeldItem(SpellEffectEntry const* effect);
        void EffectSummonObject(SpellEffectEntry const* effect);
        void EffectResurrect(SpellEffectEntry const* effect);
        void EffectParry(SpellEffectEntry const* effect);
        void EffectBlock(SpellEffectEntry const* effect);
        void EffectLeapForward(SpellEffectEntry const* effect);
        void EffectLeapBack(SpellEffectEntry const* effect);
        void EffectTransmitted(SpellEffectEntry const* effect);
        void EffectDisEnchant(SpellEffectEntry const* effect);
        void EffectInebriate(SpellEffectEntry const* effect);
        void EffectFeedPet(SpellEffectEntry const* effect);
        void EffectDismissPet(SpellEffectEntry const* effect);
        void EffectReputation(SpellEffectEntry const* effect);
        void EffectSelfResurrect(SpellEffectEntry const* effect);
        void EffectSkinning(SpellEffectEntry const* effect);
        void EffectCharge(SpellEffectEntry const* effect);
        void EffectCharge2(SpellEffectEntry const* effect);
        void EffectProspecting(SpellEffectEntry const* effect);
        void EffectRedirectThreat(SpellEffectEntry const* effect);
        void EffectMilling(SpellEffectEntry const* effect);
        void EffectRenamePet(SpellEffectEntry const* effect);
		void EffectWMODamage(SpellEffectEntry const* effect);
		void EffectQuestClear(SpellEffectEntry const* effect);
		void EffectWMORepair(SpellEffectEntry const* effect);
        void EffectSendTaxi(SpellEffectEntry const* effect);
        void EffectKnockBack(SpellEffectEntry const* effect);
        void EffectPlayerPull(SpellEffectEntry const* effect);
        void EffectDispelMechanic(SpellEffectEntry const* effect);
        void EffectSummonDeadPet(SpellEffectEntry const* effect);
        void EffectSummonAllTotems(SpellEffectEntry const* effect);
		void EffectSummon(SpellEffectEntry const* effect);
        void EffectBreakPlayerTargeting (SpellEffectEntry const* effect);
        void EffectDestroyAllTotems(SpellEffectEntry const* effect);
        void EffectDurabilityDamage(SpellEffectEntry const* effect);
        void EffectSkill(SpellEffectEntry const* effect);
        void EffectTaunt(SpellEffectEntry const* effect);
        void EffectDurabilityDamagePCT(SpellEffectEntry const* effect);
        void EffectModifyThreatPercent(SpellEffectEntry const* effect);
        void EffectResurrectNew(SpellEffectEntry const* effect);
        void EffectAddExtraAttacks(SpellEffectEntry const* effect);
        void EffectSpiritHeal(SpellEffectEntry const* effect);
        void EffectSkinPlayerCorpse(SpellEffectEntry const* effect);
        void EffectStealBeneficialBuff(SpellEffectEntry const* effect);
        void EffectUnlearnSpecialization(SpellEffectEntry const* effect);
        void EffectHealPct(SpellEffectEntry const* effect);
        void EffectEnergisePct(SpellEffectEntry const* effect);
        void EffectTriggerSpellWithValue(SpellEffectEntry const* effect);
        void EffectTriggerRitualOfSummoning(SpellEffectEntry const* effect);
        void EffectKillCreditPersonal(SpellEffectEntry const* effect);
        void EffectKillCredit(SpellEffectEntry const* effect);
        void EffectQuestFail(SpellEffectEntry const* effect);
        void EffectActivateRune(SpellEffectEntry const* effect);
        void EffectTeachTaxiNode(SpellEffectEntry const* effect);
        void EffectTitanGrip(SpellEffectEntry const* effect);
        void EffectEnchantItemPrismatic(SpellEffectEntry const* effect);
        void EffectPlayMusic(SpellEffectEntry const* effect);
        void EffectSpecCount(SpellEffectEntry const* effect);
        void EffectActivateSpec(SpellEffectEntry const* effect);

        Spell( Unit* Caster, SpellEntry const *info, bool triggered, uint64 originalCasterGUID = 0, Spell** triggeringContainer = NULL );
        ~Spell();

        void prepare(SpellCastTargets const* targets, Aura* triggeredByAura = NULL);
        void cancel();
        void update(uint32 difftime);
        void cast(bool skipCheck = false);
        void finish(bool ok = true);
        void TakePower();
        void TakeRunePower();
        void TakeReagents();
        void TakeCastItem();

        SpellCastResult CheckCast(bool strict);
        SpellCastResult CheckPetCast(Unit* target);

        // handlers
        void handle_immediate();
        uint64 handle_delayed(uint64 t_offset);
        // handler helpers
        void _handle_immediate_phase();
        void _handle_finish_phase();

        SpellCastResult CheckItems();
        SpellCastResult CheckRange(bool strict);
        SpellCastResult CheckPower();
        SpellCastResult CheckOrTakeRunePower(bool take);
        SpellCastResult CheckCasterAuras() const;

        int32 CalculateDamage(uint8 i, Unit* target) { return m_caster->CalculateSpellDamage(m_spellInfo,i,m_currentBasePoints[i],target); }
        int32 CalculatePowerCost();

        bool HaveTargetsForEffect(uint8 effect) const;
        void Delayed();
        void DelayedChannel();
        uint32 getState() const { return m_spellState; }
        void setState(uint32 state) { m_spellState = state; }

        void DoCreateItem(SpellEffectEntry const* effect, uint32 itemtype);
        void WriteSpellGoTargets( WorldPacket * data );
        void WriteAmmoToPacket( WorldPacket * data );

        typedef std::list<Unit*> UnitList;
        void FillTargetMap();
        void SetTargetMap(uint32 effIndex, uint32 targetMode, UnitList &targetUnitMap);

        void FillAreaTargets(UnitList &targetUnitMap, float x, float y, float radius, SpellNotifyPushType pushType, SpellTargets spellTargets);
        void FillRaidOrPartyTargets(UnitList &targetUnitMap, Unit* member, Unit* center, float radius, bool raid, bool withPets, bool withcaster);
        void FillRaidOrPartyManaPriorityTargets(UnitList &targetUnitMap, Unit* member, Unit* center, float radius, uint32 count, bool raid, bool withPets, bool withcaster);
        void FillRaidOrPartyHealthPriorityTargets(UnitList &targetUnitMap, Unit* member, Unit* center, float radius, uint32 count, bool raid, bool withPets, bool withcaster);

        template<typename T> WorldObject* FindCorpseUsing();

        bool CheckTarget( Unit* target, uint32 eff );
        bool CanAutoCast(Unit* target);

        static void MANGOS_DLL_SPEC SendCastResult(Player* caster, SpellEntry const* spellInfo, uint8 cast_count, SpellCastResult result);
        void SendCastResult(SpellCastResult result);
        void SendSpellStart();
        void SendSpellGo();
        void SendSpellCooldown();
        void SendLogExecute();
        void SendInterrupted(uint8 result);
        void SendChannelUpdate(uint32 time);
        void SendChannelStart(uint32 duration);
        void SendResurrectRequest(Player* target);
        void SendPlaySpellVisual(uint32 SpellID);

        void HandleEffects(Unit *pUnitTarget,Item *pItemTarget,GameObject *pGOTarget,uint32 i, float DamageMultiplier = 1.0);
        void HandleThreatSpells(uint32 spellId);
        //void HandleAddAura(Unit* Target);

        SpellEntry const* m_spellInfo;
		SpellInterruptsEntry const* m_spellInterrupts;
        int32 m_currentBasePoints[3];                       // cache SpellEntry::EffectBasePoints and use for set custom base points
        Item* m_CastItem;
        uint8 m_cast_count;
        uint32 m_glyphIndex;
        SpellCastTargets m_targets;

		Item* GetCastItem() { return m_CastItem; }
        int32 GetCastTime() const { return m_casttime; }
        uint32 GetCastedTime() { return m_timer; }
        bool IsAutoRepeat() const { return m_autoRepeat; }
        void SetAutoRepeat(bool rep) { m_autoRepeat = rep; }
        void ReSetTimer() { m_timer = m_casttime > 0 ? m_casttime : 0; }
        bool IsNextMeleeSwingSpell() const
        {
            return m_spellInfo->Attributes & (SPELL_ATTR_ON_NEXT_SWING_1|SPELL_ATTR_ON_NEXT_SWING_2);
        }
        bool IsRangedSpell() const
        {
            return  m_spellInfo->Attributes & SPELL_ATTR_RANGED;
        }
        bool IsChannelActive() const { return m_caster->GetUInt32Value(UNIT_CHANNEL_SPELL) != 0; }
        bool IsMeleeAttackResetSpell() const { return !m_IsTriggeredSpell && m_spellInterrupts && (m_spellInterrupts->InterruptFlags & SPELL_INTERRUPT_FLAG_AUTOATTACK);  }
        bool IsRangedAttackResetSpell() const { return !m_IsTriggeredSpell && IsRangedSpell() && m_spellInterrupts && (m_spellInterrupts->InterruptFlags & SPELL_INTERRUPT_FLAG_AUTOATTACK); }

        bool IsDeletable() const { return !m_referencedFromCurrentSpell && !m_executedCurrently; }
        void SetReferencedFromCurrent(bool yes) { m_referencedFromCurrentSpell = yes; }
        void SetExecutedCurrently(bool yes) { m_executedCurrently = yes; }
        uint64 GetDelayStart() const { return m_delayStart; }
        void SetDelayStart(uint64 m_time) { m_delayStart = m_time; }
        uint64 GetDelayMoment() const { return m_delayMoment; }

        bool IsNeedSendToClient() const;                    // use for hide spell cast for client in case when cast not have client side affect (animation or log entries)
        bool IsTriggeredSpellWithRedundentData() const;     // use for ignore some spell data for triggered spells like cast time, some triggered spells have redundent copy data from main spell for client use purpose

        CurrentSpellTypes GetCurrentContainer();

        Unit* GetCaster() const { return m_caster; }
        Unit* GetOriginalCaster() const { return m_originalCaster; }
        int32 GetPowerCost() const { return m_powerCost; }

        void UpdatePointers();                              // must be used at call Spell code after time delay (non triggered spell cast/update spell call/etc)

        bool IsAffectedByAura(Aura *aura) const;

        bool CheckTargetCreatureType(Unit* target) const;

        void AddTriggeredSpell(SpellEntry const* spellInfo) { m_TriggerSpells.push_back(spellInfo); }
        void AddPrecastSpell(SpellEntry const* spellInfo) { m_preCastSpells.push_back(spellInfo); }
        void AddTriggeredSpell(uint32 spellId);
        void AddPrecastSpell(uint32 spellId);
        void CastPreCastSpells(Unit* target);
        void CastTriggerSpells();

        void CleanupTargetList();
		void ClearCastItem();
        static void SelectMountByAreaAndSkill(Unit* target, uint32 spellId75, uint32 spellId150, uint32 spellId225, uint32 spellId300, uint32 spellIdSpecial);
		bool IsValidSingleTargetEffect(Unit const* target, Targets type) const;
		bool IsValidSingleTargetSpell(Unit const* target) const;
		bool isCausingAura(AuraType aura);

		Unit* getUnitTarget() { return unitTarget; }
		WeaponAttackType getAttackType() { return m_attackType; }
		int32 getDamage() { return damage; }

	protected:
		
		bool HasGlobalCooldown();
		void TriggerGlobalCooldown();
		void CancelGlobalCooldown();

        void SendLoot(uint64 guid, LootType loottype);
        bool IgnoreItemRequirements() const;                        // some item use spells have unexpected reagent data

        Unit* m_caster;

        uint64 m_originalCasterGUID;                        // real source of cast (aura caster/etc), used for spell targets selection
                                                            // e.g. damage around area spell trigered by victim aura and da,age emeies of aura caster
        Unit* m_originalCaster;                             // cached pointer for m_originalCaster, updated at Spell::UpdatePointers()

        Spell** m_selfContainer;                            // pointer to our spell container (if applicable)
        Spell** m_triggeringContainer;                      // pointer to container with spell that has triggered us

        //Spell data
        SpellSchoolMask m_spellSchoolMask;                  // Spell school (can be overwrite for some spells (wand shoot for example)
        WeaponAttackType m_attackType;                      // For weapon based attack
        int32 m_powerCost;                                  // Calculated spell cost     initialized only in Spell::prepare
        int32 m_casttime;                                   // Calculated spell cast time initialized only in Spell::prepare
        bool m_canReflect;                                  // can reflect this spell?
        bool m_autoRepeat;
        uint8 m_runesState;

        uint8 m_delayAtDamageCount;
        bool isDelayableNoMore()
        {
            if(m_delayAtDamageCount >= 2)
                return true;

            m_delayAtDamageCount++;
            return false;
        }

        // Delayed spells system
        uint64 m_delayStart;                                // time of spell delay start, filled by event handler, zero = just started
        uint64 m_delayMoment;                               // moment of next delay call, used internally
        bool m_immediateHandled;                            // were immediate actions handled? (used by delayed spells only)

        // These vars are used in both delayed spell system and modified immediate spell system
        bool m_referencedFromCurrentSpell;                  // mark as references to prevent deleted and access by dead pointers
        bool m_executedCurrently;                           // mark as executed to prevent deleted and access by dead pointers
        bool m_needSpellLog;                                // need to send spell log?
        uint8 m_applyMultiplierMask;                        // by effect: damage multiplier needed?
        float m_damageMultipliers[3];                       // by effect: damage multiplier

        // Current targets, to be used in SpellEffects (MUST BE USED ONLY IN SPELL EFFECTS)
        Unit* unitTarget;
        Item* itemTarget;
        GameObject* gameObjTarget;
        int32 damage;

        // this is set in Spell Hit, but used in Apply Aura handler
        DiminishingLevels m_diminishLevel;
        DiminishingGroup m_diminishGroup;

        // -------------------------------------------
        GameObject* focusObject;

        // Damage and healing in effects need just calculate
        int32 m_damage;           // Damge   in effects count here
        int32 m_healing;          // Healing in effects count here
        int32 m_healthLeech;      // Health leech in effects for all targets count here

        //******************************************
        // Spell trigger system
        //******************************************
        bool   m_canTrigger;                  // Can start trigger (m_IsTriggeredSpell can`t use for this)
        uint32 m_procAttacker;                // Attacker trigger flags
        uint32 m_procVictim;                  // Victim   trigger flags
        void   prepareDataForTriggerSystem();

        //*****************************************
        // Spell target subsystem
        //*****************************************
        // Targets store structures and data
        struct TargetInfo
        {
            uint64 targetGUID;
            uint64 timeDelay;
            SpellMissInfo missCondition:8;
            SpellMissInfo reflectResult:8;
            uint8  effectMask:8;
            bool   processed:1;
        };
        std::list<TargetInfo> m_UniqueTargetInfo;
        uint8 m_needAliveTargetMask;                        // Mask req. alive targets

        struct GOTargetInfo
        {
            uint64 targetGUID;
            uint64 timeDelay;
            uint8  effectMask:8;
            bool   processed:1;
        };
        std::list<GOTargetInfo> m_UniqueGOTargetInfo;

        struct ItemTargetInfo
        {
            Item  *item;
            uint8 effectMask;
        };
        std::list<ItemTargetInfo> m_UniqueItemInfo;

        void AddUnitTarget(Unit* target, uint32 effIndex);
        void AddUnitTarget(uint64 unitGUID, uint32 effIndex);
        void AddGOTarget(GameObject* target, uint32 effIndex);
        void AddGOTarget(uint64 goGUID, uint32 effIndex);
        void AddItemTarget(Item* target, uint32 effIndex);
        void DoAllEffectOnTarget(TargetInfo *target);
        void DoSpellHitOnUnit(Unit *unit, uint32 effectMask);
        void DoAllEffectOnTarget(GOTargetInfo *target);
        void DoAllEffectOnTarget(ItemTargetInfo *target);
        bool IsAliveUnitPresentInTargetList();
        SpellCastResult CanOpenLock(uint32 effIndex, uint32 lockid, SkillType& skillid, int32& reqSkillValue, int32& skillValue);
        // -------------------------------------------

        //List For Triggered Spells
        typedef std::list<SpellEntry const*> SpellInfoList;
        SpellInfoList m_TriggerSpells;                      // casted by caster to same targets settings in m_targets at success finish of current spell
        SpellInfoList m_preCastSpells;                      // casted by caster to each target at spell hit before spell effects apply

        uint32 m_spellState;
        uint32 m_timer;

        float m_castPositionX;
        float m_castPositionY;
        float m_castPositionZ;
        float m_castOrientation;
        bool m_IsTriggeredSpell;

        // if need this can be replaced by Aura copy
        // we can't store original aura link to prevent access to deleted auras
        // and in same time need aura data and after aura deleting.
        SpellEntry const* m_triggeredByAuraSpell;
};

enum ReplenishType
{
    REPLENISH_UNDEFINED = 0,
    REPLENISH_HEALTH    = 20,
    REPLENISH_MANA      = 21,
    REPLENISH_RAGE      = 22
};

namespace MaNGOS
{
    struct MANGOS_DLL_DECL SpellNotifierPlayer
    {
        std::list<Unit*> &i_data;
        Spell &i_spell;
        const uint32& i_index;
        float i_radius;
        Unit* i_originalCaster;

        SpellNotifierPlayer(Spell &spell, std::list<Unit*> &data, const uint32 &i, float radius)
            : i_data(data), i_spell(spell), i_index(i), i_radius(radius)
        {
            i_originalCaster = i_spell.GetOriginalCaster();
        }

        void Visit(PlayerMapType &m)
        {
            if(!i_originalCaster)
                return;

            for(PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
            {
                Player * pPlayer = itr->getSource();
                if( !pPlayer->isAlive() || pPlayer->isInFlight())
                    continue;

                if( i_originalCaster->IsFriendlyTo(pPlayer) )
                    continue;

                if( pPlayer->IsWithinDist3d(i_spell.m_targets.m_destX, i_spell.m_targets.m_destY, i_spell.m_targets.m_destZ,i_radius))
                    i_data.push_back(pPlayer);
            }
        }
        template<class SKIP> void Visit(GridRefManager<SKIP> &) {}
    };

    struct MANGOS_DLL_DECL SpellNotifierCreatureAndPlayer
    {
        std::list<Unit*> *i_data;
        Spell &i_spell;
        SpellNotifyPushType i_push_type;
        float i_radius;
        SpellTargets i_TargetType;
        Unit* i_originalCaster;

        SpellNotifierCreatureAndPlayer(Spell &spell, std::list<Unit*> &data, float radius, SpellNotifyPushType type,
            SpellTargets TargetType = SPELL_TARGETS_NOT_FRIENDLY)
            : i_data(&data), i_spell(spell), i_push_type(type), i_radius(radius), i_TargetType(TargetType)
        {
            i_originalCaster = spell.GetOriginalCaster();
        }

        template<class T> inline void Visit(GridRefManager<T>  &m)
        {
            ASSERT(i_data);

            if(!i_originalCaster)
                return;

            for(typename GridRefManager<T>::iterator itr = m.begin(); itr != m.end(); ++itr)
            {
                // there are still more spells which can be casted on dead, but
                // they are no AOE and don't have such a nice SPELL_ATTR flag
                if ( (i_TargetType != SPELL_TARGETS_ALL && !itr->getSource()->isTargetableForAttack(i_spell.m_spellInfo->AttributesEx3 & SPELL_ATTR_EX3_CAST_ON_DEAD))
                    // mostly phase check
                    || !itr->getSource()->IsInMap(i_originalCaster))
                    continue;

                switch (i_TargetType)
                {
                    case SPELL_TARGETS_HOSTILE:
                        if (!i_originalCaster->IsHostileTo( itr->getSource() ))
                            continue;
                        break;
                    case SPELL_TARGETS_NOT_FRIENDLY:
                        if (i_originalCaster->IsFriendlyTo( itr->getSource() ))
                            continue;
                        break;
                    case SPELL_TARGETS_NOT_HOSTILE:
                        if (i_originalCaster->IsHostileTo( itr->getSource() ))
                            continue;
                        break;
                    case SPELL_TARGETS_FRIENDLY:
                        if (!i_originalCaster->IsFriendlyTo( itr->getSource() ))
                            continue;
                        break;
                    case SPELL_TARGETS_AOE_DAMAGE:
                    {
                        if(itr->getSource()->GetTypeId()==TYPEID_UNIT && ((Creature*)itr->getSource())->isTotem())
                            continue;

                        Unit* check = i_originalCaster->GetCharmerOrOwnerOrSelf();

                        if( check->GetTypeId()==TYPEID_PLAYER )
                        {
                            if (check->IsFriendlyTo( itr->getSource() ))
                                continue;
                        }
                        else
                        {
                            if (!check->IsHostileTo( itr->getSource() ))
                                continue;
                        }
                    }
                    break;
                    case SPELL_TARGETS_ALL:
                    break;
                    default: continue;
                }

                // we don't need to check InMap here, it's already done some lines above
                switch(i_push_type)
                {
                    case PUSH_IN_FRONT:
                        if(i_spell.GetCaster()->isInFront((Unit*)(itr->getSource()), i_radius, 2*M_PI/3 ))
                            i_data->push_back(itr->getSource());
                        break;
                    case PUSH_IN_FRONT_90:
                        if(i_spell.GetCaster()->isInFront((Unit*)(itr->getSource()), i_radius, M_PI/2 ))
                            i_data->push_back(itr->getSource());
                        break;
                    case PUSH_IN_FRONT_30:
                        if(i_spell.GetCaster()->isInFront((Unit*)(itr->getSource()), i_radius, M_PI/6 ))
                            i_data->push_back(itr->getSource());
                        break;
                    case PUSH_IN_FRONT_15:
                        if(i_spell.GetCaster()->isInFront((Unit*)(itr->getSource()), i_radius, M_PI/12 ))
                            i_data->push_back(itr->getSource());
                        break;
                    case PUSH_IN_BACK:
                        if(i_spell.GetCaster()->isInBack((Unit*)(itr->getSource()), i_radius, 2*M_PI/3 ))
                            i_data->push_back(itr->getSource());
                        break;
                    case PUSH_SELF_CENTER:
                        if(i_spell.GetCaster()->IsWithinDist((Unit*)(itr->getSource()), i_radius))
                            i_data->push_back(itr->getSource());
                        break;
                    case PUSH_DEST_CENTER:
                        if(itr->getSource()->IsWithinDist3d(i_spell.m_targets.m_destX, i_spell.m_targets.m_destY, i_spell.m_targets.m_destZ,i_radius))
                            i_data->push_back(itr->getSource());
                        break;
                    case PUSH_TARGET_CENTER:
                        if(i_spell.m_targets.getUnitTarget()->IsWithinDist((Unit*)(itr->getSource()), i_radius))
                            i_data->push_back(itr->getSource());
                        break;
                }
            }
        }

        #ifdef WIN32
        template<> inline void Visit(CorpseMapType & ) {}
        template<> inline void Visit(GameObjectMapType & ) {}
        template<> inline void Visit(DynamicObjectMapType & ) {}
        #endif
    };

    #ifndef WIN32
    template<> inline void SpellNotifierCreatureAndPlayer::Visit(CorpseMapType& ) {}
    template<> inline void SpellNotifierCreatureAndPlayer::Visit(GameObjectMapType& ) {}
    template<> inline void SpellNotifierCreatureAndPlayer::Visit(DynamicObjectMapType& ) {}
    #endif
}

typedef void(Spell::*pEffect)(SpellEffectEntry const* spellEffect);

class SpellEvent : public BasicEvent
{
    public:
        SpellEvent(Spell* spell);
        virtual ~SpellEvent();

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
        virtual bool IsDeletable() const;
    protected:
        Spell* m_Spell;
};
#endif
