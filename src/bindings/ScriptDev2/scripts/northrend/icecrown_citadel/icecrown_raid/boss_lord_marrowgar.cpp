#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_SABER_LASH     		= 69055, // ok
    SPELL_COLDFLAME      		= 69146, // ok
    SPELL_BONE_SPIKE     		= 69057, 
    SPELL_BONE_SPIKE_IMPALE		= 69065, 
    SPELL_BONE_STORM     		= 69076, // ok
    
    SPELL_BERSERK               = 47008,
    
    NPC_COLDFLAME        		= 36672,
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
		AddEventOnTank(SPELL_SABER_LASH,1000,1000);
    }
	
	uint8 phase;
	uint32 FlameDespawn;
	uint32 Flame_Timer;
	uint32 Storm_Timer;
	uint32 StormTarget_Timer;

    void Reset()
    {
		ResetTimers();
		Flame_Timer = 12000;
		Storm_Timer = 27000;
		StormTarget_Timer = 15000;
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
		
		float dist = me->GetDistance2d(flameTarget);
		uint8 nbFlame = ceil((double)dist/3.0f) + 1;

		for(uint8 i=0;i<(nbFlame+1);i++)
		{
			float x_add = i*3*dist/(flameTarget->GetPositionX() - me->GetPositionX());
			float y_add = i*3*dist/(flameTarget->GetPositionY() - me->GetPositionY());
			CallCreature(NPC_COLDFLAME,FlameDespawn,PREC_COORDS,NOTHING,me->GetPositionX() + x_add, me->GetPositionY() + y_add, me->GetPositionZ() + 1.0f,true);
		}
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
				
        if(phase == 0)
        {
			if(Storm_Timer <= diff)
			{
				phase = 1;
				me->CastStop();
				DoCastMe(SPELL_BONE_STORM);
				BossEmote(0,"Gargamoelle commence a incanter une tempete d'os");
				Storm_Timer = 20000;
				StormTarget_Timer = 5000;
			}
			else
				Storm_Timer -= diff;
			
			UpdateEvent(diff);
			DoMeleeAttackIfReady();
		}
		else if(phase == 1)
		{
			if(StormTarget_Timer <= diff)
			{
				DoResetThreat();
				Unit* StormTarget = GetRandomUnit();
				if(StormTarget)
					me->AddThreat(StormTarget,100000.0f);
				StormTarget_Timer = 5500;
			}
			else
				StormTarget_Timer -= diff;
				
			if(Storm_Timer <= diff)
			{
				phase = 0;
				DoResetThreat();
				Storm_Timer = 40000;
			}
			else
				Storm_Timer -= diff;
		}
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
		me->setFaction(14);
		MakeInvisibleStalker();
    }
	
    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		AggroAllPlayers(150.0f);
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
		AggroAllPlayers(150.0f);
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
