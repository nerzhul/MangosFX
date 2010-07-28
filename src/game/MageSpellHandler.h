#ifndef MANGOS_MAGESPELLHANDLER_H
#define MANGOS_MAGESPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC MageSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
};

#define sMageSpellHandler MaNGOS::Singleton<MageSpellHandler>::Instance()
#endif