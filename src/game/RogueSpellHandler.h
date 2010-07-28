#ifndef MANGOS_ROGUESPELLHANDLER_H
#define MANGOS_ROGUESPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC RogueSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
};

#define sRogueSpellHandler MaNGOS::Singleton<RogueSpellHandler>::Instance()
#endif