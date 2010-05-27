#include "precompiled.h"
#include "ulduar.h"

enum spells
{
	// mkII
	SPELL_NAPALM_SHELL			=	63666,
	SPELL_NAPALM_SHELL_H		=	65026,
	SPELL_PLASMA_BLAST			=	62997,
	SPELL_PLASMA_BLAST_H		=	64529,
	SPELL_SHOCK_BLAST			=	63631,
	// others
	SPELL_MINE_EXPLOSION		=	66351,
	SPELL_MINE_EXPLOSION_H		=	63009,
};

enum Npcs
{
	NPC_PROXIM_MINE		=	34362,
};

struct MANGOS_DLL_DECL boss_leviMKIIAI : public LibDevFSAI
{
	// TODO: spawn mines around MKII
    boss_leviMKIIAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEvent(m_difficulty ? SPELL_NAPALM_SHELL_H : SPELL_NAPALM_SHELL,5000,8000,3000);
        AddEventOnTank(m_difficulty ? SPELL_PLASMA_BLAST_H : SPELL_PLASMA_BLAST,15000,30000);
        AddEventMaxPrioOnTank(SPELL_SHOCK_BLAST,25000,30000);
    }

    void Reset()
    {
		ResetTimers();
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

struct MANGOS_DLL_DECL boss_leviMKII_mineAI : public LibDevFSAI
{
    boss_leviMKII_mineAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(m_difficulty ? SPELL_MINE_EXPLOSION_H : SPELL_MINE_EXPLOSION, 5000,DAY);
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
    }


    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_leviMKII_mine(Creature* pCreature)
{
    return new boss_leviMKII_mineAI (pCreature);
}

struct MANGOS_DLL_DECL boss_VX001AI : public LibDevFSAI
{
    boss_VX001AI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
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

    void Reset()
    {
		ResetTimers();
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
    newscript->Name = "boss_leviMKII_mine";
    newscript->GetAI = &GetAI_boss_leviMKII_mine;
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
