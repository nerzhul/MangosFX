#include "precompiled.h"
#include "ulduar.h"

enum spells
{

};

struct MANGOS_DLL_DECL boss_leviMKIIAI : public LibDevFSAI
{
    boss_leviMKIIAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint8 phase;
    void Reset()
    {
		ResetTimers();
		phase = 0;
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
    }
};

CreatureAI* GetAI_boss_leviMKII(Creature* pCreature)
{
    return new boss_leviMKIIAI (pCreature);
}

struct MANGOS_DLL_DECL boss_VX001AI : public LibDevFSAI
{
    boss_VX001AI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint8 phase;
    void Reset()
    {
		ResetTimers();
		phase = 0;
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
    }
};

CreatureAI* GetAI_boss_VX001(Creature* pCreature)
{
    return new boss_VX001AI (pCreature);
}

struct MANGOS_DLL_DECL boss_aerialCommandUnitAI : public LibDevFSAI
{
    boss_aerialCommandUnitAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint8 phase;
    void Reset()
    {
		ResetTimers();
		phase = 0;
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
    }
};

CreatureAI* GetAI_boss_aerialCommandUnit(Creature* pCreature)
{
    return new boss_aerialCommandUnitAI (pCreature);
}

struct MANGOS_DLL_DECL boss_mimironAI : public LibDevFSAI
{
    boss_mimironAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MIMIRON, DONE);
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE);
    }

    void Aggro(Unit* pWho)
    {

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MIMIRON, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		
        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_mimiron(Creature* pCreature)
{
    return new boss_mimironAI(pCreature);
}

void AddSC_boss_mimiron()
{
    Script *newscript;
    
    newscript = new Script;
    newscript->Name = "boss_mimiron";
    newscript->GetAI = &GetAI_boss_mimiron;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_leviMKII";
    newscript->GetAI = &GetAI_boss_leviMKII;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_aerialCommandUnit";
    newscript->GetAI = &GetAI_boss_aerialCommandUnit;
    newscript->RegisterSelf();
    
    newscript = new Script;
    newscript->Name = "boss_VX001";
    newscript->GetAI = &GetAI_boss_VX001;
    newscript->RegisterSelf();
}
