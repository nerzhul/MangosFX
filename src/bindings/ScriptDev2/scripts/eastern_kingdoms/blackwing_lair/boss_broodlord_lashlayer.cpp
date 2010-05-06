#include "precompiled.h"

#define SAY_AGGRO               -1469000
#define SAY_LEASH               -1469001

enum Spells
{
	SPELL_CLEAVE           = 26350,
	SPELL_BLASTWAVE        = 23331,
	SPELL_MORTALSTRIKE     = 24573,
	SPELL_KNOCKBACK        = 25778
};

struct MANGOS_DLL_DECL boss_broodlordAI : public LibDevFSAI
{
    boss_broodlordAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_CLEAVE,8000,7000);
		AddEventOnTank(SPELL_BLASTWAVE,12000,8000,8000);
		AddEventOnTank(SPELL_MORTALSTRIKE,20000,25000,10000);
	}
    uint32 KnockBack_Timer;

    void Reset()
    {
		ResetTimers();
        KnockBack_Timer = 30000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
        me->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if (KnockBack_Timer < diff)
        {
            DoCastVictim(SPELL_KNOCKBACK);
            //Drop 50% aggro
            if (me->getThreatManager().getThreat(me->getVictim()))
                me->getThreatManager().modifyThreatPercent(me->getVictim(),-50);

            KnockBack_Timer = urand(15000, 30000);
        }else KnockBack_Timer -= diff;

		UpdateEvent(diff);
		
        DoMeleeAttackIfReady();

        if (EnterEvadeIfOutOfCombatArea(diff))
            DoScriptText(SAY_LEASH, me);
    }
};
CreatureAI* GetAI_boss_broodlord(Creature* pCreature)
{
    return new boss_broodlordAI(pCreature);
}

void AddSC_boss_broodlord()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_broodlord";
    newscript->GetAI = &GetAI_boss_broodlord;
    newscript->RegisterSelf();
}
