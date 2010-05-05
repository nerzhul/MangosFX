
#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1000401,
    SAY_CASTCORRUPTION      = -1000402,

    SPELL_SLEEP             = 24777,
    SPELL_NOXIOUSBREATH     = 24818,
    SPELL_TAILSWEEP         = 15847,
    //SPELL_MARKOFNATURE    = 25040,                        // Not working
    SPELL_VOLATILEINFECTION = 24928,
    SPELL_CORRUPTIONOFEARTH = 24910
};

struct MANGOS_DLL_DECL boss_emerissAI : public LibDevFSAI
{
    boss_emerissAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    {
		InitBoss();
	}

    uint32 m_uiSleep_Timer;
    uint32 m_uiNoxiousBreath_Timer;
    uint32 m_uiTailSweep_Timer;
    //uint32 m_uiMarkOfNature_Timer;
    uint32 m_uiVolatileInfection_Timer;
    uint32 m_uiCorruptionsCasted;

    void Reset()
    {
        m_uiSleep_Timer = urand(15000, 20000);
        m_uiNoxiousBreath_Timer = 8000;
        m_uiTailSweep_Timer = 4000;
        //m_uiMarkOfNature_Timer = 45000;
        m_uiVolatileInfection_Timer = 12000;
        m_uiCorruptionsCasted = 0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //Sleep_Timer
        if (m_uiSleep_Timer < uiDiff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SLEEP);

            m_uiSleep_Timer = urand(8000, 16000);
        }
        else
            m_uiSleep_Timer -= uiDiff;

        //NoxiousBreath_Timer
        if (m_uiNoxiousBreath_Timer < uiDiff)
        {
            DoCastVictim( SPELL_NOXIOUSBREATH);
            m_uiNoxiousBreath_Timer = urand(14000, 20000);
        }
        else
            m_uiNoxiousBreath_Timer -= uiDiff;

        //Tailsweep every 2 seconds
        if (m_uiTailSweep_Timer < uiDiff)
        {
            DoCastMe( SPELL_TAILSWEEP);
            m_uiTailSweep_Timer = 2000;
        }
        else
            m_uiTailSweep_Timer -= uiDiff;

        //MarkOfNature_Timer
        //if (m_uiMarkOfNature_Timer < uiDiff)
        //{
        //    DoCastVictim( SPELL_MARKOFNATURE);
        //    m_uiMarkOfNature_Timer = 45000;
        //}
        //else
            //m_uiMarkOfNature_Timer -= uiDiff;

        //VolatileInfection_Timer
        if (m_uiVolatileInfection_Timer < uiDiff)
        {
            DoCastVictim( SPELL_VOLATILEINFECTION);
            m_uiVolatileInfection_Timer = urand(7000, 12000);
        }
        else
            m_uiVolatileInfection_Timer -= uiDiff;

        //CorruptionofEarth at 75%, 50% and 25%
        if ((me->GetHealth()*100 / me->GetMaxHealth()) <= (100-(25*m_uiCorruptionsCasted)))
        {
            ++m_uiCorruptionsCasted;                        // prevent casting twice on same hp
            DoScriptText(SAY_CASTCORRUPTION, me);
            DoCastVictim( SPELL_CORRUPTIONOFEARTH);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_emeriss(Creature* pCreature)
{
    return new boss_emerissAI(pCreature);
}

void AddSC_boss_emeriss()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_emeriss";
    newscript->GetAI = &GetAI_boss_emeriss;
    newscript->RegisterSelf();
}
