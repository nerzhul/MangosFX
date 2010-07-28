#ifndef MANGOS_SHAMANSPELLHANDLER_H
#define MANGOS_SHAMANSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC ShamanSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
};

#define sShamanSpellHandler MaNGOS::Singleton<ShamanSpellHandler>::Instance()
#endif