#include "precompiled.h"

enum
{
    SAY_INTRO                       = -1000147,
    SAY_AGGRO1                      = -1000148,
    SAY_AGGRO2                      = -1000149,
    SAY_SURPREME1                   = -1000150,
    SAY_SURPREME2                   = -1000151,
    SAY_KILL1                       = -1000152,
    SAY_KILL2                       = -1000153,
    SAY_KILL3                       = -1000154,
    SAY_DEATH                       = -1000155,
    EMOTE_GENERIC_FRENZY            = -1000002,
    SAY_RAND1                       = -1000157,
    SAY_RAND2                       = -1000158,

    SPELL_SHADOWVOLLEY              = 32963,
    SPELL_CLEAVE                    = 31779,
    SPELL_THUNDERCLAP               = 36706,
    SPELL_VOIDBOLT                  = 39329,
    SPELL_MARKOFKAZZAK              = 32960,
    SPELL_ENRAGE                    = 32964,
    SPELL_CAPTURESOUL               = 32966,
    SPELL_TWISTEDREFLECTION         = 21063
};

struct MANGOS_DLL_DECL boss_doomlordkazzakAI : public ScriptedAI
{
    boss_doomlordkazzakAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_SHADOWVOLLEY,urand(6000,10000),4000,2000);
		AddEventOnTank(SPELL_CLEAVE,7000,8000,4000);
		AddEventOnTank(SPELL_THUNDERCLAP,urand(14000,18000),10000,4000);
		AddEventOnTank(SPELL_VOIDBOLT,30000,15000,3000);
		AddEvent(SPELL_MARKOFKAZZAK,25000,20000,0,TARGET_HAS_MANA);
		AddEvent(SPELL_ENRAGE,60000,30000,0,TARGET_ME,0,EMOTE_GENERIC_FRENZY);
		AddEvent(SPELL_TWISTEDREFLECTION,33000,15000);
    }

    void Reset()
    {
        ResetTimers();
    }

    void JustRespawned()
    {
        DoScriptText(SAY_INTRO, me);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(urand(0, 1) ? SAY_AGGRO1 : SAY_AGGRO2, me);
    }

    void KilledUnit(Unit* victim)
    {
        // When Kazzak kills a player (not pets/totems), he regens some health
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoCastMe(SPELL_CAPTURESOUL);

        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL1, me); break;
            case 1: DoScriptText(SAY_KILL2, me); break;
            case 2: DoScriptText(SAY_KILL3, me); break;
        }
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_doomlordkazzak(Creature* pCreature)
{
    return new boss_doomlordkazzakAI(pCreature);
}

void AddSC_boss_doomlordkazzak()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_doomlord_kazzak";
    newscript->GetAI = &GetAI_boss_doomlordkazzak;
    newscript->RegisterSelf();
}
