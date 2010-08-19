#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
	//Darkfallen Orb
	SPELL_INVOCATION_OF_BLOOD_V             = 70952,
	SPELL_INVOCATION_OF_BLOOD_K             = 70981,
	SPELL_INVOCATION_OF_BLOOD_T             = 70982,

	//Valanar
	SPELL_KINETIC_BOMB                      = 72053,
	SPELL_KINETIC_BOMB_EXPLODE              = 72052,
	SPELL_SHOCK_VORTEX                      = 72037,
	SPELL_SHOCK_VORTEX_DAMAGE               = 71944,
	SPELL_SHOCK_VORTEX_2                    = 72039,

	NPC_KINETIC_BOMB                        = 38458,
	NPC_SHOCK_VORTEX                        = 38422,

	//Taldaram
	SPELL_GLITTERING_SPARKS                 = 71807, // ok
	SPELL_CONJURE_FLAME                     = 71718,
	SPELL_FLAMES                            = 71393,
	SPELL_CONJURE_FLAME_POW                 = 72040,
	SPELL_FLAMES_2                          = 71708,

	//Keleseth
	SPELL_SHADOW_LANCE                      = 71405, // ok
	SPELL_SHADOW_LANCE_POW                  = 71815, // ok
	SPELL_SHADOW_RESONANCE                  = 71943, // ok
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
		SetInstanceData(TYPE_PRINCE_COUNCIL, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(!me->HasAura(SPELL_INVOCATION_OF_BLOOD_T))
		{
			dmg = 0;
			return;
		}

		if(dmg >= me->GetHealth())
		{
			if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
			{
				Taldaram->CastStop();
				Taldaram->CastSpell(Taldaram,7,true);
			}

			if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
			{
				Keleseth->CastStop();
				Keleseth->CastSpell(Keleseth,7,true);
			}
		}
	}

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, DONE);

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
        SetInstanceData(TYPE_PRINCE_COUNCIL, FAIL);
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
		AddEvent(SPELL_GLITTERING_SPARKS,15000,15000,3000);
    }

    void Reset()
    {
		ResetTimers();
		SetInstanceData(TYPE_PRINCE_COUNCIL, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(!me->HasAura(SPELL_INVOCATION_OF_BLOOD_T))
		{
			dmg = 0;
			return;
		}

		if(dmg >= me->GetHealth())
		{
			if(Creature* Valanar = GetInstanceCreature(DATA_PRINCE_VALANAR))
			{
				Valanar->CastStop();
				Valanar->CastSpell(Valanar,7,true);
			}

			if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
			{
				Keleseth->CastStop();
				Keleseth->CastSpell(Keleseth,7,true);
			}
		}
	}

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
    }

    void JustReachedHome()
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, FAIL);
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
		AddEventOnMe(SPELL_SHADOW_RESONANCE,30000,60000);
		AddTextEvent(16728,"Le sang va couler !",30000,60000);
    }

	uint32 PoweredSpell_Timer;

    void Reset()
    {
		ResetTimers();
		PoweredSpell_Timer = 3000;
		SetInstanceData(TYPE_PRINCE_COUNCIL, NOT_STARTED);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(!me->HasAura(SPELL_INVOCATION_OF_BLOOD_K))
		{
			dmg = 0;
			return;
		}

		if(dmg >= me->GetHealth())
		{
			if(Creature* Valanar = GetInstanceCreature(DATA_PRINCE_VALANAR))
			{
				Valanar->CastStop();
				Valanar->CastSpell(Valanar,7,true);
			}

			if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
			{
				Taldaram->CastStop();
				Taldaram->CastSpell(Taldaram,7,true);
			}
		}
	}

	void EmpowerMe(float pctLife)
	{
		me->SetHealth(me->GetMaxHealth()*pctLife);
		me->CastStop();
		DoCastMe(SPELL_INVOCATION_OF_BLOOD_K);
		Yell(16727,"Quel incroyable pouvoir ! L'orbe des ténébrants m'a rendu invincible !");
	}

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Say(16723,"Quelle menace posez vous ?");
		else
			Say(16724,"Du sang et de l'acier surgit la vérité.");
	}

    void JustDied(Unit* pKiller)
    {
		Say(16725,"Ma reine... ils arrivent...");
    }

    void JustReachedHome()
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(PoweredSpell_Timer <= diff)
		{
			DoCastVictim(me->HasAura(SPELL_INVOCATION_OF_BLOOD_K) ? SPELL_SHADOW_LANCE_POW : SPELL_SHADOW_LANCE);
			PoweredSpell_Timer = urand(3000,5000);
		}
		else
			PoweredSpell_Timer -= diff;

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
