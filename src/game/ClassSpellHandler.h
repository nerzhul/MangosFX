#ifndef MANGOS_CLASSSPELLHANDLER_H
#define MANGOS_CLASSSPELLHANDLER_H

#include <Policies/Singleton.h>

struct AuraCarac
{
	AuraType aType;
	SpellFamilyNames sfName;
	uint64 sFlag;
	uint32 sFlag2;
};
enum AuraName
{
	PRIEST_TWIN_DISCIPLINES				= 0,
	PRIEST_HOLY_FIRE					= 1,
	PRIEST_RENEW						= 2,
	PRIEST_IMPROVED_DEVOURING_PLAGUE	= 3,
	PRIEST_SILENT_RESOLVE				= 4,
	MAGE_ARCANE_SUBTLELY				= 5,
	ROGUE_VILE_POISON					= 6,
	ROGUE_ENVENOM						= 7,
	DK_RIME								= 8,
	DK_IMPROVED_ICY_TOUCH				= 9,
	DK_IMPURITY							= 10,
	PRIEST_SHADOW_WORD_PAIN				= 11,
	DRUID_INSECT_SWARM					= 12,
	ROGUE_DEADLY_POISON					= 13,
	WARLOCK_EVERLASTING_AFFLICTION		= 14,

	MAX_AURA_NAMES						= 15
};

typedef std::list<Aura *> AuraList;

class MANGOS_DLL_SPEC ClassSpellHandler
{
	public:
		ClassSpellHandler() {};
		~ClassSpellHandler() {};
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		bool HandleProcTriggerSpell(Unit* u, const SpellEntry* auraSpellInfo, uint32 &trig_sp_id, int32* basepoints);
		void HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i);
		bool HandleEffectDummy(Spell* spell,int32 &damage,SpellEffectIndex i);
		void PeriodicDummyTick(Aura* aura);
		void SpellDamageBonusDone(SpellEntry* spell, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod);
		bool HandleDummyAuraProc(Unit* u, const SpellEntry* dummySpell, uint32 &triggered_spell_id, int32 triggerAmount, const SpellEntry* procSpell, uint32 procEx, Unit* target, Unit* pVictim, int32 &basepoints0);
		void HandleAuraDummyWithApply(Aura* aura,Unit* caster,Unit* target);

		Aura* GetAuraByName(Unit* u,AuraName aName, uint64 casterGUID = 0);
		Aura* GetSpecialAura(Unit* u, AuraName aName);
};

#define sClassSpellHandler MaNGOS::Singleton<ClassSpellHandler>::Instance()
#endif