
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
		InitIA();
		AddEvent(SPELL_SLEEP,urand(15000,20000),8000,8000);
		AddEventOnTank(SPELL_NOXIOUSBREATH,8000,14000,6000);
		AddEventOnMe(SPELL_TAILSWEEP,4000,2000);
		AddEventOnTank(SPELL_VOLATILEINFECTION,12000,7000,5000);
	}

    //uint32 m_uiMarkOfNature_Timer;
    uint32 m_uiVolatileInfection_Timer;
    uint32 m_uiCorruptionsCasted;

    void Reset()
    {
		ResetTimers();
        //m_uiMarkOfNature_Timer = 45000;
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

        //MarkOfNature_Timer
        //if (m_uiMarkOfNature_Timer < uiDiff)
        //{
        //    DoCastVictim( SPELL_MARKOFNATURE);
        //    m_uiMarkOfNature_Timer = 45000;
        //}
        //else
            //m_uiMarkOfNature_Timer -= uiDiff;


        //CorruptionofEarth at 75%, 50% and 25%
        if (CheckPercentLife(100-(25*m_uiCorruptionsCasted)))
        {
            ++m_uiCorruptionsCasted;                        // prevent casting twice on same hp
            DoScriptText(SAY_CASTCORRUPTION, me);
            DoCastVictim(SPELL_CORRUPTIONOFEARTH);
        }
        
        UpdateEvent(diff);

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
