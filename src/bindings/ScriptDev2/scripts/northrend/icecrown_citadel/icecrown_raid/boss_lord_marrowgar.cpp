#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_SABER_LASH     = 69055,
    SPELL_COLDFLAME      = 69146,
    SPELL_BONE_SPIKE     = 69057,
    SPELL_BONE_STORM     = 69076,
    
    NPC_COLDFLAME        = 36672,
};

enum
{
	//common
	SPELL_BERSERK               = 47008,
	//yells
	//summons
	NPC_BONE_SPIKE              = 38711,
	//Abilities
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
	
	uint8 phase;

    void Reset()
    {
		ResetTimers();
		phase = 0;
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

	void CallColdFlames(Unit* victim)
	{
		if(!victim)
			return;
		
		float x_vect = victim->GetPositionX() - me->GetPositionX();
		float y_vect = victim->GetPositionY() - me->GetPositionY();
		for(uint8 i=0;i<5;i++)
			CallCreature(NPC_COLDFLAME,30000,PREC_COORDS,NOTHING,me->GetPositionX() + x_vect*(3*i),me->GetPositionY() + x_vect*(3*i),me->GetPositionZ() + 1.0f,true);
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

struct MANGOS_DLL_DECL flame_marrowgarAI : public LibDevFSAI
{
    flame_marrowgarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(SPELL_COLDFLAME,2000,2000);
    }
	
    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		AggroAllPlayers();
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_flame_marrowgar(Creature* pCreature)
{
    return new flame_marrowgarAI(pCreature);
}

void AddSC_boss_marrowgar()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_marrowgar";
    NewScript->GetAI = &GetAI_boss_marrowgar;
    NewScript->RegisterSelf();
    
    NewScript = new Script;
    NewScript->Name = "flame_marrowgar";
    NewScript->GetAI = &GetAI_flame_marrowgar;
    NewScript->RegisterSelf();
}
