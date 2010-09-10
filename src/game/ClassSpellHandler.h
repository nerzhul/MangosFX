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
	PRIEST_TWIN_DISCIPLINES		= 0,
	PRIEST_HOLY_FIRE			= 1,

	MAX_AURA_NAMES				= 2
};

class MANGOS_DLL_SPEC ClassSpellHandler
{
	public:
		ClassSpellHandler() {};
		~ClassSpellHandler() {};
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
		void HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i);
		bool HandleEffectDummy(Spell* spell);
		void PeriodicDummyTick(Aura* aura);

		Aura* GetAuraByName(Unit* u,AuraName aName, uint64 casterGUID = 0);
};

#define sClassSpellHandler MaNGOS::Singleton<ClassSpellHandler>::Instance()
#endif