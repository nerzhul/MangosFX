#ifndef MANGOS_WARRIORSPELLHANDLER_H
#define MANGOS_WARRIORSPELLHANDLER_H

#include <Policies/Singleton.h>
#include <Common.h>

class MANGOS_DLL_SPEC WarriorSpellHandler
{
	public:
		void HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod);
		void HandleSchoolDmg(Spell *spell,int32 &damage,SpellEffectIndex i);
		bool HandleProcTriggerSpell(Unit* u, const SpellEntry* auraSpellInfo, uint32 &trig_sp_id, int32* basepoints);
		bool HandleDummyAuraProc(Unit* u, const SpellEntry* dummySpell, uint32 &triggered_spell_id, int32 triggerAmount, const SpellEntry* procSpell, uint32 procEx, Unit* target, Unit* pVictim, int32 &basepoints0);
		void HandleAuraDummyWithApply(Aura* aura,Unit* caster,Unit* target);
};

#define sWarriorSpellHandler MaNGOS::Singleton<WarriorSpellHandler>::Instance()
#endif