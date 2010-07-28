#ifndef MANGOS_CLASSSPELLHANDLER_H
#define MANGOS_CLASSSPELLHANDLER_H

#include <Policies/Singleton.h>

class MANGOS_DLL_SPEC ClassSpellHandler
{
	public:
		ClassSpellHandler() {};
		~ClassSpellHandler() {};
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
};

#define sClassSpellHandler MaNGOS::Singleton<ClassSpellHandler>::Instance()
#endif