#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{
	// boss
	SPELL_CLEAVE_ARMOR			= 74367,
	SPELL_INTRIMIDATING_ROAR	= 74384,
	// adds
	NPC_ONYX_FLAME_CALLER		= 39814,
	SPELL_BLAST_NOVA			= 74392,
	SPELL_LAVA_GOUT				= 74394,
};

struct MANGOS_DLL_DECL trashboss_zarithrianAI : public LibDevFSAI
{	
	trashboss_zarithrianAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
		AddEventOnTank(SPELL_CLEAVE_ARMOR,5000,15000);
		AddEventOnTank(SPELL_INTRIMIDATING_ROAR,20000,20000);
    }

	bool north;
	uint32 spawn_Timer;

    void Reset()
	{
		ResetTimers();
		CleanMyAdds();
		spawn_Timer = 15000;
		north = true;
		SetInstanceData(TYPE_ZARITHRIAN,NOT_STARTED);
	}

	void Aggro(Unit* pWho)
	{
		SetInstanceData(TYPE_ZARITHRIAN,IN_PROGRESS);
		Say(17512,"Alexstrasza a choisi des alliés valeureux. Dommage que je doive vous anéantir...");
	}

	void KilledUnit(Unit* pWho)
	{
		if(urand(0,1))
			Say(17514,"C'est mieux ainsi.");
		else
			Say(17513,"Vous pensiez avoir une chance ?");
	}

	void JustDied(Unit* pWho)
	{
		Yell(17515,"Halion... je... arrgh...");
		SetInstanceData(TYPE_ZARITHRIAN,DONE);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				break;
		}
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething())
            return;

		if(spawn_Timer <= diff)
		{
			Yell(17516,"Serviteurs ! Réduisez les en cendre !");
			for(uint8 i=0;i<3;i++)
			{
				if(north)
					CallCreature(NPC_ONYX_FLAME_CALLER,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,3012.721f,500.426f,89.93f);
				else
					CallCreature(NPC_ONYX_FLAME_CALLER,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,3016.04f,566.03f,88.57f);
			}
			north = !north;
			spawn_Timer = 60000;
		}
		else
			spawn_Timer -= diff;

		UpdateEvent(diff);
	
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_trashboss_zarithrian(Creature* pCreature)
{
    return new trashboss_zarithrianAI(pCreature);
}

struct MANGOS_DLL_DECL zarithrian_addAI : public LibDevFSAI
{	
	zarithrian_addAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
		AddEvent(SPELL_LAVA_GOUT,3000,12000,3000);
		AddEventOnTank(SPELL_BLAST_NOVA,urand(8000,12000),12000,3000);
    }

    void Reset()
	{
		ResetTimers();
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_zarithrian_add(Creature* pCreature)
{
    return new zarithrian_addAI(pCreature);
}

void AddSC_trashboss_zarithrian()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "trashboss_zarithrian";
    newscript->GetAI = &GetAI_trashboss_zarithrian;
    newscript->RegisterSelf();
}

