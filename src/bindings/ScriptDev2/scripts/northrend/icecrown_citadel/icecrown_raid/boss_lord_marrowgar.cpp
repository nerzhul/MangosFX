#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    SPELL_SABER_LASH     = 69055,
    SPELL_COLDFLAME      = 69146,
    SPELL_BONE_SPIKE     = 69057,
    SPELL_BONE_STORM     = 69076,
};

enum
{
	//common
	SPELL_BERSERK               = 47008,
	//yells
	//summons
	NPC_BONE_SPIKE              = 38711,
	NPC_COLDFLAME               = 36672,
	//Abilities
	SPELL_CALL_COLD_FLAME       = 69138,
	SPELL_COLD_FLAME            = 69146,
	SPELL_COLD_FLAME_0          = 69145,
	SPELL_BONE_STRIKE           = 69057,
	SPELL_BONE_STRIKE_IMPALE	= 69065,
	SPELL_BONE_STORM_STRIKE     = 69075,
};

struct MANGOS_DLL_DECL boss_marrowgarAI : public LibDevFSAI
{
    boss_marrowgarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }


    void Reset()
    {
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_MARROWGAR, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_MARROWGAR, DONE);
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_MARROWGAR, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_marrowgar(Creature* pCreature)
{
    return new boss_marrowgarAI(pCreature);
}

void AddSC_boss_marrowgar()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_marrowgar";
    NewScript->GetAI = &GetAI_boss_marrowgar;
    NewScript->RegisterSelf();
}
