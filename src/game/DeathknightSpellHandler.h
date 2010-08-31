#ifndef MANGOS_DKSPELLHANDLER_H
#define MANGOS_DKSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC DeathknightSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
		bool HandleEffectDummy(Spell* spell);
		void PeriodicDummyTick(Aura* aura);
};

#define sDeathknightSpellHandler MaNGOS::Singleton<DeathknightSpellHandler>::Instance()
#endif