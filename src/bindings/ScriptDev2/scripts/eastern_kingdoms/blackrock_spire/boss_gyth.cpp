#include "precompiled.h"

enum Spells
{
	SPELL_CORROSIVEACID     = 20667,
	SPELL_FREEZE            = 18763,
	SPELL_FLAMEBREATH       = 20712
};

struct MANGOS_DLL_DECL boss_gythAI : public LibDevFSAI
{
    boss_gythAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitIA();
		AddEventOnTank(SPELL_CORROSIVEACID,8000,7000);
		AddEventOnTank(SPELL_FREEZE,11000,16000);
		AddEventOnTank(SPELL_FLAMEBREATH,4000,10500);
	}

    uint32 Aggro_Timer;
    uint32 Dragons_Timer;
    uint32 Orc_Timer;
    uint32 Line1Count;
    uint32 Line2Count;

    bool Event;
    bool SummonedDragons;
    bool SummonedOrcs;
    bool SummonedRend;
    bool bAggro;
    bool RootSelf;
    Creature *SummonedCreature;

    void Reset()
    {
		ResetTimers();
        Dragons_Timer = 3000;
        Orc_Timer = 60000;
        Aggro_Timer = 60000;
        Event = false;
        SummonedDragons = false;
        SummonedOrcs= false;
        SummonedRend = false;
        bAggro = false;
        RootSelf = false;

        // how many times should the two lines of summoned creatures be spawned
        // min 2 x 2, max 7 lines of attack in total
        Line1Count = rand() % 4 + 2;
        if (Line1Count < 5)
            Line2Count = rand() % (5 - Line1Count) + 2;
        else
            Line2Count = 2;

        //Invisible for event start
        me->SetDisplayId(11686);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void SummonCreatureWithRandomTarget(uint32 creatureId)
    {
        Unit* Summoned = me->SummonCreature(creatureId, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240000);
        if (Summoned)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (target)
                Summoned->AddThreat(target, 1.0f);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //char buf[200];

        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (!RootSelf)
        {
            //me->m_canMove = true;
            DoCastMe( 33356);
            RootSelf = true;
        }

        if (!bAggro && Line1Count == 0 && Line2Count == 0)
        {
            if (Aggro_Timer < diff)
            {
                bAggro = true;
                // Visible now!
                me->SetDisplayId(9723);
                me->setFaction(14);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            } else Aggro_Timer -= diff;
        }

        // Summon Dragon pack. 2 Dragons and 3 Whelps
        if (!bAggro && !SummonedRend && Line1Count > 0)
        {
            if (Dragons_Timer < diff)
            {
                SummonCreatureWithRandomTarget(10372);
                SummonCreatureWithRandomTarget(10372);
                SummonCreatureWithRandomTarget(10442);
                SummonCreatureWithRandomTarget(10442);
                SummonCreatureWithRandomTarget(10442);
                Line1Count = Line1Count - 1;
                Dragons_Timer = 60000;
            } else Dragons_Timer -= diff;
        }

        //Summon Orc pack. 1 Orc Handler 1 Elite Dragonkin and 3 Whelps
        if (!bAggro && !SummonedRend && Line1Count == 0 && Line2Count > 0)
        {
            if (Orc_Timer < diff)
            {
                SummonCreatureWithRandomTarget(10447);
                SummonCreatureWithRandomTarget(10317);
                SummonCreatureWithRandomTarget(10442);
                SummonCreatureWithRandomTarget(10442);
                SummonCreatureWithRandomTarget(10442);
                Line2Count = Line2Count - 1;
                Orc_Timer = 60000;
            } else Orc_Timer -= diff;
        }

        // we take part in the fight
        if (bAggro)
        {
            //Summon Rend
            if (!SummonedRend && CheckPercentLife(11)
                && me->GetHealth() > 0)
            {
                //summon Rend and Change model to normal Gyth
                //Inturrupt any spell casting
                me->InterruptNonMeleeSpells(false);
                //Gyth model
                me->SetDisplayId(9806);
                me->SummonCreature(10429, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 900000);
                SummonedRend = true;
            }

			UpdateEvent(diff);

            DoMeleeAttackIfReady();
        }                                                   // end if Aggro
    }
};

CreatureAI* GetAI_boss_gyth(Creature* pCreature)
{
    return new boss_gythAI(pCreature);
}

void AddSC_boss_gyth()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gyth";
    newscript->GetAI = &GetAI_boss_gyth;
    newscript->RegisterSelf();
}
