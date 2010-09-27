#ifndef MANGOS_DKSPELLHANDLER_H
#define MANGOS_DKSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC DeathknightSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
		bool HandleEffectDummy(Spell* spell,int32 &damage, SpellEffectIndex i);
		void PeriodicDummyTick(Aura* aura);
};

#define sDeathknightSpellHandler MaNGOS::Singleton<DeathknightSpellHandler>::Instance()
#endif