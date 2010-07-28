#include <Policies/SingletonImp.h>
#include "MageSpellHandler.h"

INSTANTIATE_SINGLETON_1(MageSpellHandler);

void MageSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
}