
#include "precompiled.h"

enum Spells
{
	SPELL_BLASTWAVE        = 11130,
	SPELL_SHOUT            = 23511,
	SPELL_CLEAVE           = 20691,
	SPELL_KNOCKAWAY        = 20686
};

#define ADD_1X -39.355381
#define ADD_1Y -513.456482
#define ADD_1Z 88.472046
#define ADD_1O 4.679872

#define ADD_2X -49.875881
#define ADD_2Y -511.896942
#define ADD_2Z 88.195160
#define ADD_2O 4.613114

struct MANGOS_DLL_DECL boss_overlordwyrmthalakAI : public LibDevFSAI
{
    boss_overlordwyrmthalakAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_BLASTWAVE,20000,20000);
		AddEventOnTank(SPELL_SHOUT,2000,10000);
		AddEventOnTank(SPELL_CLEAVE,6000,7000);
		AddEventOnTank(SPELL_KNOCKAWAY,12000,14000);
	}

    bool Summoned;

    void Reset()
    {
		ResetTimers();
        Summoned = false;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //Summon two Beserks
        if (!Summoned && CheckPercentLife(51))
        {
			CallAggressiveCreature(9216,300000,PREC_COORDS,ADD_1X,ADD_1Y,ADD_1Z);
			CallAggressiveCreature(9268,300000,PREC_COORDS,ADD_2X,ADD_2Y,ADD_2Z);
            Summoned = true;
        }
        
        UpdateEvent(diff);
        
        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_overlordwyrmthalak(Creature* pCreature)
{
    return new boss_overlordwyrmthalakAI(pCreature);
}

void AddSC_boss_overlordwyrmthalak()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_overlord_wyrmthalak";
    newscript->GetAI = &GetAI_boss_overlordwyrmthalak;
    newscript->RegisterSelf();
}
