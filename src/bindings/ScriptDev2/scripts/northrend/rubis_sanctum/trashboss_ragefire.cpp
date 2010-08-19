#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{
	SPELL_FLAME_BREATH		= 74403,
	SPELL_ENRAGE			= 78722,
	SPELL_CONFLAGRATION		= 74452,
	SPELL_CONFLAG_TRIG		= 74456,
	SPELL_CONFLAG_MARK		= 74453,
};

const static float FlightCoords[3] = {3155.79f,670.621f,99.50f};

struct MANGOS_DLL_DECL trashboss_ragefireAI : public LibDevFSAI
{	
	trashboss_ragefireAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
		AddEventOnTank(SPELL_FLAME_BREATH,5000,12000,5000);
		AddEventOnMe(SPELL_ENRAGE,18000,30000);
		AddEvent(SPELL_CONFLAGRATION,30000,35000);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_10MAN_HEROIC:
				maxtargets = 2;
			case RAID_DIFFICULTY_25MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				maxtargets = 5;
				break;
		}
    }

	uint32 FlightEvent;
	uint32 maxtargets;
	std::vector<uint64> conflag_Targets;
	bool isFlying;

    void Reset()
	{
		ResetTimers();
		SetFlying(false);
		conflag_Targets.clear();
		FlightEvent = 25000;
		SetInstanceData(TYPE_RAGEFIRE,NOT_STARTED);
		isFlying = false;
	}

	void SpellHit(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == 74455 && isFlying)
		{
			Say(17532,"Brulez dans les flammes du maître");
			me->CastStop();
			for(std::vector<uint64>::const_iterator itr = conflag_Targets.begin(); itr != conflag_Targets.end(); ++itr)
				if(Unit* u = GetGuidUnit(*itr))
					DoCast(u,SPELL_CONFLAG_TRIG,true);
			SetFlying(false);
			isFlying = false;
			if(me->getVictim())
				me->GetMotionMaster()->MoveChase(me->getVictim());
		}
	}

	void DamageDeal(Unit* pWho, uint32 &dmg)
	{
		// anti bug abuse
		if(pWho->GetTypeId() == TYPEID_UNIT)
			if(((Creature*)pWho)->getFaction() == 103)
				dmg = 25000000;
	}
	
	void Aggro(Unit* pWho)
	{
		SetInstanceData(TYPE_RAGEFIRE,IN_PROGRESS);
		Yell(17528,"Vous allez souffrir pour cette intrusion.");
	}

	void JustDied(Unit* pWho)
	{
		Say(17531,"Aarrrrrgh !");
		SetInstanceData(TYPE_RAGEFIRE,DONE);
		if(pWho->getFaction() == 103)
			return;
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

	void KilledUnit(Unit* pWho)
	{
		if(urand(0,1))
			Say(17529,"C'est l'ordre des choses.");
		else
			Say(17530,"Halion sera satisfait !");
	}

	void JustReachedHome()
	{
		SetInstanceData(TYPE_RAGEFIRE,FAIL);
	}
	
    void UpdateAI(const uint32 diff)
	{	
        if (!CanDoSomething())
            return;

		if(FlightEvent <= diff)
		{
			conflag_Targets.clear();
			SetFlying(true);
			isFlying = true;
			me->GetMotionMaster()->MovePoint(0,FlightCoords[0],FlightCoords[1],FlightCoords[2]);
			for(int8 i=0;i<maxtargets;i++)
				if(Unit* u = GetRandomUnit())
				{
					DoCast(u,SPELL_CONFLAG_MARK,true);
					conflag_Targets.push_back(u->GetGUID());
				}
			FlightEvent = 35000;
		}
		else
			FlightEvent -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_trashboss_ragefire(Creature* pCreature)
{
    return new trashboss_ragefireAI(pCreature);
} 

void AddSC_trashboss_ragefire()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "trashboss_ragefire";
    newscript->GetAI = &GetAI_trashboss_ragefire;
    newscript->RegisterSelf();
}

