#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_SABER_LASH     		= 69055,
    SPELL_COLDFLAME      		= 69146,
    SPELL_BONE_SPIKE     		= 69057,
    SPELL_BONE_STRIKE_IMPALE	= 69065,
    SPELL_BONE_STORM     		= 69076,
    
    SPELL_BERSERK               = 47008,
    
    NPC_COLDFLAME        		= 36672, // 3 sec of spawn in normal but 8 in heroic
    NPC_BONE_SPIKE              = 38711,
};

enum
{
	//common
	
	//yells
	//summons
	
	//Abilities
	SPELL_COLD_FLAME_0          = 69145,
};

struct MANGOS_DLL_DECL boss_marrowgarAI : public LibDevFSAI
{
    boss_marrowgarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        switch(m_difficulty)
        {
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				FlameDespawn = 8000;
				break;
			default:
				FlameDespawn = 3000;
				break;
		}
    }
	
	uint8 phase;
	uint32 FlameDespawn;
	uint32 Flame_Timer;

    void Reset()
    {
		ResetTimers();
		Flame_Timer = 12000;
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

	void CallColdFlames()
	{
		Unit* flameTarget = GetRandomUnit();
		if(!flameTarget)
			return;
		
		float x_vect = flameTarget->GetPositionX() - me->GetPositionX();
		float y_vect = flameTarget->GetPositionY() - me->GetPositionY();
		for(uint8 i=0;i<5;i++)
			CallCreature(NPC_COLDFLAME,FlameDespawn,PREC_COORDS,NOTHING,me->GetPositionX() + x_vect*(3*i),me->GetPositionY() + x_vect*(3*i),me->GetPositionZ() + 1.0f,true);
	}
	
    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
            
        if(Flame_Timer <= diff)
        {
			CallColdFlames();
			Flame_Timer = 3000;
		}
		else
			Flame_Timer -= diff;

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

struct MANGOS_DLL_DECL bonespike_marrowgarAI : public LibDevFSAI
{
    bonespike_marrowgarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
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

CreatureAI* GetAI_bonespike_marrowgar(Creature* pCreature)
{
    return new bonespike_marrowgarAI(pCreature);
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
    
    NewScript = new Script;
    NewScript->Name = "bonespike_marrowgar";
    NewScript->GetAI = &GetAI_bonespike_marrowgar;
    NewScript->RegisterSelf();
}
