#ifndef MANGOS_DRUIDSPELLHANDLER_H
#define MANGOS_DRUIDSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC DruidSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
};

#define sDruidSpellHandler MaNGOS::Singleton<DruidSpellHandler>::Instance()
#endif