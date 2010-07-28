#include <Policies/SingletonImp.h>
#include "PriestSpellHandler.h"

INSTANTIATE_SINGLETON_1(PriestSpellHandler);

void PriestSpellHandler::HandleEffectWeaponDamage(Spell* spell, int32 &spell_bonus, bool &weaponDmgMod, float &totalDmgPctMod)
{
}