#ifndef MANGOS_PALADINSPELLHANDLER_H
#define MANGOS_PALADINSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC PaladinSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod);
};

#define sPaladinSpellHandler MaNGOS::Singleton<PaladinSpellHandler>::Instance()
#endif