#ifndef MANGOS_WARLOCKSPELLHANDLER_H
#define MANGOS_WARLOCKSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC WarlockSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
};

#define sWarlockSpellHandler MaNGOS::Singleton<WarlockSpellHandler>::Instance()
#endif