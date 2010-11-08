#ifndef MANGOS_ROGUESPELLHANDLER_H
#define MANGOS_ROGUESPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC RogueSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		void HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i);
		bool HandleEffectDummy(Spell* spell, int32 &damage, SpellEffectIndex i);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
};

#define sRogueSpellHandler MaNGOS::Singleton<RogueSpellHandler>::Instance()
#endif