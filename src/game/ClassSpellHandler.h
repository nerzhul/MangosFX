#ifndef MANGOS_CLASSSPELLHANDLER_H
#define MANGOS_CLASSSPELLHANDLER_H

#include <Policies/Singleton.h>

class MANGOS_DLL_SPEC ClassSpellHandler
{
	public:
		ClassSpellHandler() {};
		~ClassSpellHandler() {};
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
		void HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i);
		bool HandleEffectDummy(Spell* spell);
};

#define sClassSpellHandler MaNGOS::Singleton<ClassSpellHandler>::Instance()
#endif