#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1000360,
    SAY_SUMMONDRUIDS        = -1000361,

    SPELL_SLEEP             = 24777,
    SPELL_NOXIOUSBREATH     = 24818,
    SPELL_TAILSWEEP         = 15847,
    //SPELL_MARKOFNATURE   = 25040,                         // Not working
    SPELL_LIGHTNINGWAVE     = 24819,
    SPELL_SUMMONDRUIDS      = 24795,

    SPELL_SUMMON_PLAYER     = 24776,

    //druid spells
    SPELL_MOONFIRE          = 21669
};

// Ysondre script
struct MANGOS_DLL_DECL boss_ysondreAI : public LibDevFSAI
{
    boss_ysondreAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEvent(SPELL_SLEEP,urand(15000,20000),8000,8000);
		AddEventOnTank(SPELL_NOXIOUSBREATH,8000,14000,6000);
		AddEventOnMe(SPELL_TAILSWEEP,4000,2000);
		AddEvent(SPELL_LIGHTNINGWAVE,12000,7000,5000);
	}

    //uint32 m_uiMarkOfNature_Timer;
    uint32 m_uiLightningWave_Timer;
    uint32 m_uiSummonDruidModifier;

    void Reset()
    {
		ResetTimers();
        //m_uiMarkOfNature_Timer = 45000;
        m_uiLightningWave_Timer = 12000;
        m_uiSummonDruidModifier = 0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
			return;

        //MarkOfNature_Timer
        //if (m_uiMarkOfNature_Timer < uiDiff)
        //{
        //    DoCastVictim( SPELL_MARKOFNATURE);
        //    m_uiMarkOfNature_Timer = 45000;
        //}
        //else
            //m_uiMarkOfNature_Timer -= uiDiff;

        //Summon Druids
        if (CheckPercentLife(100-(25*m_uiSummonDruidModifier)))
        {
            DoScriptText(SAY_SUMMONDRUIDS, me);

            for(int i = 0; i < 10; ++i)
                DoCastMe(SPELL_SUMMONDRUIDS, true);

            ++m_uiSummonDruidModifier;
        }
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

// Summoned druid script
struct MANGOS_DLL_DECL mob_dementeddruidsAI : public LibDevFSAI
{
    mob_dementeddruidsAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_MOONFIRE,3000,5000);
	}

    void Reset() { ResetTimers(); }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ysondre(Creature* pCreature)
{
    return new boss_ysondreAI(pCreature);
}

CreatureAI* GetAI_mob_dementeddruids(Creature* pCreature)
{
    return new mob_dementeddruidsAI(pCreature);
}

void AddSC_boss_ysondre()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ysondre";
    newscript->GetAI = &GetAI_boss_ysondre;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dementeddruids";
    newscript->GetAI = &GetAI_mob_dementeddruids;
    newscript->RegisterSelf();
}
