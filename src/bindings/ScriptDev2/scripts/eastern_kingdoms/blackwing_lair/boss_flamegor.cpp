#include "precompiled.h"

enum
{
    EMOTE_GENERIC_FRENZY        = -1000002,

    SPELL_SHADOWFLAME           = 22539,
    SPELL_WINGBUFFET            = 23339,
    SPELL_FRENZY                = 23342                     //This spell periodically triggers fire nova
};

struct MANGOS_DLL_DECL boss_flamegorAI : public LibDevFSAI
{
    boss_flamegorAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_SHADOWFLAME,21000,15000,6000);
		AddEventOnMe(SPELL_FRENZY,10000,8000,1000,0,EMOTE_GENERIC_FRENZY);
    }

    uint32 ShadowFlame_Timer;
    uint32 WingBuffet_Timer;
    uint32 Frenzy_Timer;

    void Reset()
    {
		ResetTimers();
        ShadowFlame_Timer = 21000;                          //These times are probably wrong
        WingBuffet_Timer = 35000;
        Frenzy_Timer = 10000;
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
CreatureAI* GetAI_boss_flamegor(Creature* pCreature)
{
    return new boss_flamegorAI(pCreature);
}

void AddSC_boss_flamegor()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_flamegor";
    newscript->GetAI = &GetAI_boss_flamegor;
    newscript->RegisterSelf();
}
