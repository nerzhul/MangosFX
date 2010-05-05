#include "precompiled.h"

enum Spells 
{
	SAY_TELEPORT            -1000100
	SPELL_MARKOFFROST        23182
	SPELL_MANASTORM          21097
	SPELL_CHILL              21098
	SPELL_FROSTBREATH        21099
	SPELL_REFLECT            22067
	SPELL_CLEAVE              8255                      //Perhaps not right ID
	SPELL_ENRAGE             23537
}

struct MANGOS_DLL_DECL boss_azuregosAI : public LibDevFSAI
{
    boss_azuregosAI(Creature* pCreature) : LibDevFSAI(pCreature) {Reset();}

    uint32 MarkOfFrost_Timer;
    uint32 ManaStorm_Timer;
    uint32 Chill_Timer;
    uint32 Breath_Timer;
    uint32 Teleport_Timer;
    uint32 Reflect_Timer;
    uint32 Cleave_Timer;
    uint32 Enrage_Timer;
    bool Enraged;

    void Reset()
    {
        MarkOfFrost_Timer = 35000;
        ManaStorm_Timer = urand(5000, 17000);
        Chill_Timer = urand(10000, 30000);
        Breath_Timer = urand(2000, 8000);
        Teleport_Timer = 30000;
        Reflect_Timer = urand(15000, 30000);
        Cleave_Timer = 7000;
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
        }else Teleport_Timer -= diff;

        //        //MarkOfFrost_Timer
        //        if (MarkOfFrost_Timer < diff)
        //        {
        //            DoCastVictim(SPELL_MARKOFFROST);
        //            MarkOfFrost_Timer = 25000;
        //        }else MarkOfFrost_Timer -= diff;

        //Chill_Timer
        if (Chill_Timer < diff)
        {
            DoCastVictim(SPELL_CHILL);
            Chill_Timer = urand(13000, 25000);
        }else Chill_Timer -= diff;

        //Breath_Timer
        if (Breath_Timer < diff)
        {
            DoCastVictim(SPELL_FROSTBREATH);
            Breath_Timer = urand(10000, 15000);
        }else Breath_Timer -= diff;

        //ManaStorm_Timer
        if (ManaStorm_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target,SPELL_MANASTORM);
            ManaStorm_Timer = urand(7500, 12500);
        }else ManaStorm_Timer -= diff;

        //Reflect_Timer
        if (Reflect_Timer < diff)
        {
            DoCastMe(SPELL_REFLECT);
            Reflect_Timer = urand(20000, 35000);
        }else Reflect_Timer -= diff;

        //Cleave_Timer
        if (Cleave_Timer < diff)
        {
            DoCastVictim(SPELL_CLEAVE);
            Cleave_Timer = 7000;
        }else Cleave_Timer -= diff;

        //Enrage_Timer
        if (me->GetHealth()*100 / me->GetMaxHealth() < 26 && !Enraged)
        {
            DoCastMe( SPELL_ENRAGE);
            Enraged = true;
        }

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
