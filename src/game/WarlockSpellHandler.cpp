#include <Policies/SingletonImp.h>
#include "Spell.h"
#include "WarlockSpellHandler.h"

INSTANTIATE_SINGLETON_1(WarlockSpellHandler);

void WarlockSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
}