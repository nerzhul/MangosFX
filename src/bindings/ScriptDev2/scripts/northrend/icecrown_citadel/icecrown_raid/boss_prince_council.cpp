#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
	//Darkfallen Orb
	SPELL_INVOCATION_OF_BLOOD               = 70952,

	//Valanar
	SPELL_KINETIC_BOMB                      = 72053,
	NPC_KINETIC_BOMB                        = 38458,
	SPELL_KINETIC_BOMB_EXPLODE              = 72052,
	SPELL_SHOCK_VORTEX                      = 72037,
	NPC_SHOCK_VORTEX                        = 38422,
	SPELL_SHOCK_VORTEX_DAMAGE               = 71944,
	SPELL_SHOCK_VORTEX_2                    = 72039,

	//Taldaram
	SPELL_GLITTERING_SPARKS                 = 71807,
	SPELL_CONJURE_FLAME                     = 71718,
	SPELL_FLAMES                            = 71393,
	SPELL_CONJURE_FLAME_2                   = 72040,
	SPELL_FLAMES_2                          = 71708,

	//Keleseth
	SPELL_SHADOW_LANCE                      = 71405,
	SPELL_SHADOW_LANCE_2                    = 71815,
	SPELL_SHADOW_RESONANCE                  = 71943,
	SPELL_SHADOW_RESONANCE_DAMAGE           = 71822,
	NPC_DARK_NUCLEUS                        = 38369,
};

struct MANGOS_DLL_DECL boss_icc_valanarAI : public LibDevFSAI
{
    boss_icc_valanarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
		}
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_icc_valanar(Creature* pCreature)
{
    return new boss_icc_valanarAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icc_taldaramAI : public LibDevFSAI
{
    boss_icc_taldaramAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        /*if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
		}*/
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_icc_taldaram(Creature* pCreature)
{
    return new boss_icc_taldaramAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icc_kelesethAI : public LibDevFSAI
{
    boss_icc_kelesethAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        /*if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				GiveEmblemsToGroup(GIVRE,1);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
		}*/
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_PRINCE_COUNCIL, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_icc_keleseth(Creature* pCreature)
{
    return new boss_icc_kelesethAI(pCreature);
}

void AddSC_ICC_prince_council()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_icc_valanar";
    NewScript->GetAI = &GetAI_boss_icc_valanar;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_icc_taldaram";
    NewScript->GetAI = &GetAI_boss_icc_taldaram;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_icc_keleseth";
    NewScript->GetAI = &GetAI_boss_icc_keleseth;
    NewScript->RegisterSelf();
}
