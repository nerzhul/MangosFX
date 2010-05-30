#include "precompiled.h"
#include "halls_of_reflection.h"

struct MANGOS_DLL_DECL HoR_WarriorAI : public LibDevFSAI
{
    HoR_WarriorAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
            return;
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Warrior(Creature* pCreature)
{
    return new HoR_WarriorAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_RogueAI : public LibDevFSAI
{
    HoR_RogueAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
            return;
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Rogue(Creature* pCreature)
{
    return new HoR_RogueAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_MageAI : public LibDevFSAI
{
    HoR_MageAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
            return;
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Mage(Creature* pCreature)
{
    return new HoR_MageAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_HuntAI : public LibDevFSAI
{
    HoR_HuntAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
            return;
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Hunt(Creature* pCreature)
{
    return new HoR_HuntAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_PriestAI : public LibDevFSAI
{
    HoR_PriestAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }


    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
            return;
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Priest(Creature* pCreature)
{
    return new HoR_PriestAI (pCreature);
}
void AddSC_halls_of_reflection()
{
	Script *newscript;

	newscript = new Script;
    newscript->Name = "HoR_Warrior";
    newscript->GetAI = &GetAI_HoR_Warrior;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Priest";
    newscript->GetAI = &GetAI_HoR_Priest;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Mage";
    newscript->GetAI = &GetAI_HoR_Mage;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Rogue";
    newscript->GetAI = &GetAI_HoR_Rogue;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Hunt";
    newscript->GetAI = &GetAI_HoR_Hunt;
    newscript->RegisterSelf();
}