#ifndef MANGOS_WARLOCKSPELLHANDLER_H
#define MANGOS_WARLOCKSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC WarlockSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		void HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i);
		void SpellDamageBonusDone(SpellEntry* spell, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
};

#define sWarlockSpellHandler MaNGOS::Singleton<WarlockSpellHandler>::Instance()
#endif