#include "precompiled.h"

enum Spells
{
	SPELL_CROWDPUMMEL     =  10887,
	SPELL_MIGHTYBLOW      =  14099
};

#define ADD_1X                  -169.839203
#define ADD_1Y                  -324.961395
#define ADD_1Z                  64.401443

struct MANGOS_DLL_DECL boss_halyconAI : public LibDevFSAI
{
    boss_halyconAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_CROWDPUMMEL,8000,14000);
		AddEventOnTank(SPELL_MIGHTYBLOW,14000,10000);
	}

    bool Summoned;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
        Summoned = false;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //Summon Gizrul
        if (!Summoned && CheckPercentLife(25))
        {
			CallAggressiveCreature(10268,300000,PREC_COORDS,AGGRESSIVE_RANDOM,ADD_1X,ADD_1Y,ADD_1Z);
            Summoned = true;
        }

		UpdateEvent(diff);
		
        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_halycon(Creature* pCreature)
{
    return new boss_halyconAI(pCreature);
}

void AddSC_boss_halycon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_halycon";
    newscript->GetAI = &GetAI_boss_halycon;
    newscript->RegisterSelf();
}
