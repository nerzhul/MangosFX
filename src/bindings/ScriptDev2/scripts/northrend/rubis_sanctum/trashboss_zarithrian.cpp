#include "precompiled.h"
#include "rubis_sanctum.h"

enum
{

};

struct MANGOS_DLL_DECL trashboss_zarithrianAI : public LibDevFSAI
{	
	trashboss_zarithrianAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {	
        InitInstance();
    }

    void Reset()
	{
		ResetTimers();
	}

	void JustDied(Unit* pWho)
	{
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

		UpdateEvent(diff);
	
		DoMeleeAttackIfReady();

	}
};

CreatureAI* GetAI_trashboss_zarithrian(Creature* pCreature)
{
    return new trashboss_zarithrianAI(pCreature);
} 

void AddSC_trashboss_zarithrian()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "trashboss_zarithrian";
    newscript->GetAI = &GetAI_trashboss_zarithrian;
    newscript->RegisterSelf();
}

