#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1000399,
    SAY_SUMMONSHADE         = -1000400,

    //Spells of Taerar
    SPELL_SLEEP             = 24777,
    SPELL_NOXIOUSBREATH     = 24818,
    SPELL_TAILSWEEP         = 15847,
    // SPELL_MARKOFNATURE   = 25040,                        // Not working
    SPELL_ARCANEBLAST       = 24857,
    SPELL_BELLOWINGROAR     = 22686,

    SPELL_SUMMONSHADE_1     = 24841,
    SPELL_SUMMONSHADE_2     = 24842,
    SPELL_SUMMONSHADE_3     = 24843,

    //Spells of Shades of Taerar
    SPELL_POSIONCLOUD       = 24840,
    SPELL_POSIONBREATH      = 20667
};

uint32 m_auiSpellSummonShade[]=
{
    SPELL_SUMMONSHADE_1, SPELL_SUMMONSHADE_2, SPELL_SUMMONSHADE_3
};

struct MANGOS_DLL_DECL boss_taerarAI : public LibDevFSAI
{
    boss_taerarAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEvent(SPELL_SLEEP,urand(15000,20000),8000,7000);
		AddEventOnTank(SPELL_NOXIOUSBREATH,8000,14000,6000);
		AddEventOnMe(SPELL_TAILSWEEP,4000,2000);
		AddEventOnTank(SPELL_ARCANEBLAST,12000,7000,5000);
		AddEventOnTank(SPELL_BELLOWINGROAR,25000,30000);
	}

    //uint32 m_uiMarkOfNature_Timer;
    uint32 m_uiShades_Timer;
    uint32 m_uiShadesSummoned;

    bool m_bShades;

    void Reset()
    {
		ResetTimers();
        //m_uiMarkOfNature_Timer = 45000;
        m_uiShades_Timer = 60000;                               //The time that Taerar is banished
        m_uiShadesSummoned = 0;
        m_bShades = false;
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

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bShades && m_uiShades_Timer < uiDiff)
        {
            //Become unbanished again
            me->setFaction(14);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_bShades = false;
        }
        else if (m_bShades)
        {
            m_uiShades_Timer -= uiDiff;
            //Do nothing while banished
            return;
        }

        //Return since we have no target
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

        //Summon 3 Shades at 75%, 50% and 25% (if bShades is true we already left in line 117, no need to check here again)
        if (!m_bShades && CheckPercentLife(100-(25*m_uiShadesSummoned)))
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                //Inturrupt any spell casting
                me->InterruptNonMeleeSpells(false);

                //horrible workaround, need to fix
                me->setFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                DoScriptText(SAY_SUMMONSHADE, me);

                int iSize = sizeof(m_auiSpellSummonShade) / sizeof(uint32);

                for(int i = 0; i < iSize; ++i)
                    me->CastSpell(pTarget, m_auiSpellSummonShade[i], true);

                ++m_uiShadesSummoned;                       // prevent casting twice at same health
                m_bShades = true;
            }
            m_uiShades_Timer = 60000;
        }
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

// Shades of Taerar Script
struct MANGOS_DLL_DECL boss_shadeoftaerarAI : public LibDevFSAI
{
    boss_shadeoftaerarAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitIA();
		AddEventOnTank(SPELL_POISONCLOUD,8000,30000);
		AddEventOnTank(SPELL_POISONBREATH,12000,12000);
	}

    void Reset()
    {
        ResetTimers();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!CanDoSomething())
            return;

        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_taerar(Creature* pCreature)
{
    return new boss_taerarAI(pCreature);
}

CreatureAI* GetAI_boss_shadeoftaerar(Creature* pCreature)
{
    return new boss_shadeoftaerarAI(pCreature);
}

void AddSC_boss_taerar()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_taerar";
    newscript->GetAI = &GetAI_boss_taerar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_shade_of_taerar";
    newscript->GetAI = &GetAI_boss_shadeoftaerar;
    newscript->RegisterSelf();
}
