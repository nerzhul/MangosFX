#include "precompiled.h"

enum Spells 
{
	SAY_TELEPORT           = -1000100,
	SPELL_MARKOFFROST      =  23182,
	SPELL_MANASTORM        =  21097,
	SPELL_CHILL            =  21098,
	SPELL_FROSTBREATH      =  21099,
	SPELL_REFLECT          =  22067,
	SPELL_CLEAVE           =   8255,                      //Perhaps not right ID
	SPELL_ENRAGE           =  23537
};

struct MANGOS_DLL_DECL boss_azuregosAI : public LibDevFSAI
{
    boss_azuregosAI(Creature* pCreature) : LibDevFSAI(pCreature) 
    { 
		InitIA();
		AddEventOnTank(SPELL_MARKOFFROST,35000,25000);
		AddEventOnTank(SPELL_CHILL,urand(10000,30000),13000,12000);
		AddEventOnTank(SPELL_FROSTBREATH,4000,10000,5000);
		AddEvent(SPELL_MANASTORM,urand(5000,17000),7500,5000);
		AddEventOnMe(SPELL_REFLECT,urand(15000,30000),20000,15000);
		AddEventOnTank(SPELL_CLEAVE,7000,7000);
	}

    uint32 Teleport_Timer;
    uint32 Enrage_Timer;
    bool Enraged;

    void Reset()
    {
		ResetTimers();
        Teleport_Timer = 30000;
        Enrage_Timer = 0;
        Enraged = false;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (Teleport_Timer < diff)
        {
            DoScriptText(SAY_TELEPORT, me);

           ThreatList const& m_threatlist = me->getThreatManager().getThreatList();
            ThreatList::const_iterator i = m_threatlist.begin();
            for (i = m_threatlist.begin(); i!= m_threatlist.end();++i)
            {
                Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
                if (pUnit && (pUnit->GetTypeId() == TYPEID_PLAYER))
                {
                    DoTeleportPlayer(pUnit, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()+3, pUnit->GetOrientation());
                }
            }

            DoResetThreat();
            Teleport_Timer = 30000;
        }
        else 
			Teleport_Timer -= diff;

        //Enrage_Timer
        if (CheckPercentLife(26) && !Enraged)
        {
            DoCastMe(SPELL_ENRAGE);
            Enraged = true;
        }
        
        UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_azuregos(Creature* pCreature)
{
    return new boss_azuregosAI(pCreature);
}

void AddSC_boss_azuregos()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_azuregos";
    newscript->GetAI = &GetAI_boss_azuregos;
    newscript->RegisterSelf();
}
