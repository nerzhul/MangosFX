#ifndef MANGOS_DRUIDSPELLHANDLER_H
#define MANGOS_DRUIDSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC DruidSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		//void HandleDummyAuraProc(Unit* u, Spell* dummy, uint32 &trig_sp_id);
		void SpellDamageBonusDone(SpellEntry* spellProto, Unit* caster, Unit* pVictim, int32 &DoneTotal, float &DoneTotalMod);
		void HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i);
};

#define sDruidSpellHandler MaNGOS::Singleton<DruidSpellHandler>::Instance()
#endif