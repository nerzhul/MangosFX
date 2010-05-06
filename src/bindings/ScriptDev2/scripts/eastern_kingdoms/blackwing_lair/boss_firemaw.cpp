#include "precompiled.h"

enum Spells
{
	SPELL_SHADOWFLAME      = 22539,
	SPELL_WINGBUFFET       = 23339,
	SPELL_FLAMEBUFFET      = 23341
};
struct MANGOS_DLL_DECL boss_firemawAI : public LibDevFSAI
{
    boss_firemawAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_SHADOWFLAME,30000,15000,3000);
		AddEventOnTank(SPELL_FLAMEBUFFET,5000,5000);
    }

    uint32 ShadowFlame_Timer;
    uint32 WingBuffet_Timer;
    uint32 FlameBuffet_Timer;

    void Reset()
    {
		ResetTimers();
        WingBuffet_Timer = 24000;
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        //WingBuffet_Timer
        if (WingBuffet_Timer < diff)
        {
            DoCastVictim(SPELL_WINGBUFFET);
            if (me->getThreatManager().getThreat(me->getVictim()))
                me->getThreatManager().modifyThreatPercent(me->getVictim(),-75);

            WingBuffet_Timer = 25000;
        }else WingBuffet_Timer -= diff;

        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_firemaw(Creature* pCreature)
{
    return new boss_firemawAI(pCreature);
}

void AddSC_boss_firemaw()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_firemaw";
    newscript->GetAI = &GetAI_boss_firemaw;
    newscript->RegisterSelf();
}
