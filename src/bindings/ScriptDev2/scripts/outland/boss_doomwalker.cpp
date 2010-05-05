#include "precompiled.h"

enum Texts
{
	SAY_AGGRO                  = -1000159,
	SAY_EARTHQUAKE_1           = -1000160,
	SAY_EARTHQUAKE_2           = -1000161,
	SAY_OVERRUN_1              = -1000162,
	SAY_OVERRUN_2              = -1000163,
	SAY_SLAY_1                 = -1000164,
	SAY_SLAY_2                 = -1000165,
	SAY_SLAY_3                 = -1000166,
	SAY_DEATH                  = -1000167
};

enum Spells
{
	SPELL_EARTHQUAKE           = 32686,
	SPELL_SUNDER_ARMOR         = 33661,
	SPELL_CHAIN_LIGHTNING      = 33665,
	SPELL_OVERRUN              = 32636,
	SPELL_ENRAGE               = 33653,
	SPELL_MARK_DEATH           = 37128,
	SPELL_AURA_DEATH           = 37131
};
struct MANGOS_DLL_DECL boss_doomwalkerAI : public LibDevFSAI
{
    boss_doomwalkerAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEvent(SPELL_CHAIN_LIGHTNING,urand(10000,30000),7000,20000);
		AddEventOnTank(SPELL_SUNDER_ARMOR,urand(5000,13000),10000,15000);
    }

	uint32 Quake_Timer;
    uint32 Enrage_Timer;
    uint32 Overrun_Timer;

    bool InEnrage;

    void Reset()
    {
        Enrage_Timer    = 0;
        Quake_Timer     = urand(25000, 35000);
        Overrun_Timer   = urand(30000, 45000);

        InEnrage = false;
    }

    void KilledUnit(Unit* Victim)
    {

        Victim->CastSpell(Victim,SPELL_MARK_DEATH,0);

        if (urand(0, 4))
            return;

        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SLAY_3, me); break;
        }

    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_AGGRO, me);
    }


    void MoveInLineOfSight(Unit *who)
    {
        if (who && who->GetTypeId() == TYPEID_PLAYER && me->IsHostileTo(who))
        {
            if (who->HasAura(SPELL_MARK_DEATH,0))
            {
            who->CastSpell(who,SPELL_AURA_DEATH,1);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        //Spell Enrage, when hp <= 20% gain enrage
        if (CheckPercentLife(20))
        {
            if (Enrage_Timer < diff)
            {
                DoCastMe(SPELL_ENRAGE);
                Enrage_Timer = 6000;
                InEnrage = true;
            }else Enrage_Timer -= diff;
        }

        //Spell Overrun
        if (Overrun_Timer < diff)
        {
            DoScriptText(urand(0, 1) ? SAY_OVERRUN_1 : SAY_OVERRUN_2, me);
            DoCastVictim(SPELL_OVERRUN);
            Overrun_Timer = urand(25000, 40000);
        }else Overrun_Timer -= diff;

        //Spell Earthquake
        if (Quake_Timer < diff)
        {
            if (urand(0, 1))
                return;

            DoScriptText(urand(0, 1) ? SAY_EARTHQUAKE_1 : SAY_EARTHQUAKE_2, me);

            //remove enrage before casting earthquake because enrage + earthquake = 16000dmg over 8sec and all dead
            if (InEnrage)
                me->RemoveAura(SPELL_ENRAGE, 0);

            DoCastMe(SPELL_EARTHQUAKE);
            Quake_Timer = urand(30000, 55000);
        }else Quake_Timer -= diff;
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_doomwalker(Creature* pCreature)
{
    return new boss_doomwalkerAI(pCreature);
}

void AddSC_boss_doomwalker()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_doomwalker";
    newscript->GetAI = &GetAI_boss_doomwalker;
    newscript->RegisterSelf();
}
