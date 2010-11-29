#include "precompiled.h"
#include "ulduar.h"

//boss_auriaya

enum AuriayaSpell
{
	SPELL_SONIC_SCREECH					= 64422,
	SPELL_SONIC_SCREECH_H				= 64688,
	SPELL_TERRIFYING_SCREECH			= 64386,
	SPELL_ESSAIM						= 64396,
	SPELL_SENTINEL_BLAST				= 64389,
	SPELL_SENTINEL_BLAST_H				= 64678,
	SPELL_SUMMON_GUARDS					= 64397,
	SPELL_ENRAGE						= 47008,

	NPC_SUMMON_FERAL					= 34035,
	NPC_SUMMON_GUARD					= 34034,
};

struct MANGOS_DLL_DECL boss_auriaya_AI : public LibDevFSAI
{
    boss_auriaya_AI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        if(m_difficulty)
		{
			AddSummonEvent(NPC_SUMMON_FERAL,60000,RESPAWN_ONE_DAY,0,0,2);
			AddSummonEvent(NPC_SUMMON_GUARD,35000,RESPAWN_ONE_DAY,0,0,8);
			AddEventOnTank(SPELL_SENTINEL_BLAST_H,12000,30000);
			AddEventOnTank(SPELL_SONIC_SCREECH_H,18000,18000);
		}
		else
		{
			AddSummonEvent(NPC_SUMMON_FERAL,60000,RESPAWN_ONE_DAY);
			AddSummonEvent(NPC_SUMMON_GUARD,35000,RESPAWN_ONE_DAY,0,0,6);
			AddEventOnTank(SPELL_SENTINEL_BLAST,12000,30000);
			AddEventOnTank(SPELL_SONIC_SCREECH,30000,28000);
		}
		AddEventMaxPrioOnTank(SPELL_TERRIFYING_SCREECH,9500,30000);
		AddEnrageTimer(TEN_MINS);
		AddTextEvent(15477,"Vous me faîtes perdre mon temps.",TEN_MINS,DAY*HOUR);
		FreezeMob(false);
    }

	bool SentryDown;
	bool DefenderDown;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		SentryDown = false;
		DefenderDown = false;
    }

    void Aggro(Unit* who)
    {
        CallCreature(34014,TEN_MINS,NEAR_7M,AGGRESSIVE_MAIN);
		CallCreature(34014,TEN_MINS,NEAR_7M,AGGRESSIVE_MAIN);
		Speak(CHAT_TYPE_SAY,15473,"Certaines choses ne doivent pas êtres dérangÃ©es !");
    }

	void SetSentryDown()
	{
		SentryDown = true;
	}

	void SetDefenderDown()
	{
		DefenderDown = true;
	}

    void KilledUnit(Unit* victim)
    {
		if(urand(0,1))
			Yell(15474,"Le secret meurt avec vous !");
		else 
			Yell(15475,"Vous n'en rÃ©chapperez pas !");
    }

    void JustDied(Unit *victim)
    {
		Yell(15476,"Aaaaaaaaaaaaaaaaaaaaaaaaaaaaargh");

        if (pInstance)
		{
            SetInstanceData(TYPE_AURIAYA, DONE);
			if(!SentryDown)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3007 : 3006);
			if(DefenderDown)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3077 : 3076);
		}
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_auriaya(Creature* pCreature)
{
    return new boss_auriaya_AI (pCreature);
}

enum FeralSpells
{
	SPELL_FERAL_ESSENCE		=	64455,
	SPELL_FERAL_POUNCE_10	=	64478,
	SPELL_FERAL_POUNCE_25	=	64669,
	SPELL_FERAL_RUSH_10		=	64496,
	SPELL_FERAL_RUSH_25		=	64674,
	SPELL_SEEPING_10		=	64459,
	SPELL_SEEPING_25		=	64675,
};

struct MANGOS_DLL_DECL add_feral_defender_AI : public LibDevFSAI
{
    add_feral_defender_AI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
		if(m_difficulty)
		{
			AddEvent(SPELL_FERAL_RUSH_25,2000,15000,2000);
			AddEventOnTank(SPELL_FERAL_POUNCE_25,10000,10000,2000);
		}
		else
		{
			AddEvent(SPELL_FERAL_RUSH_10,2000,15000,2000);
			AddEventOnTank(SPELL_FERAL_POUNCE_10,10000,10000,2000);
		}
    }

	uint8 numb_lives;

	void Reset()
	{
		numb_lives = 8;
		me->RemoveAurasDueToSpell(SPELL_FERAL_ESSENCE);
		ModifyAuraStack(SPELL_FERAL_ESSENCE,numb_lives);
	}

	void DamageTaken(Unit* done_by, uint32 &damage)
	{
		if(damage >= me->GetHealth())
		{
			if(numb_lives > 0)
			{
				damage = 0;
				numb_lives--;
				me->SetHealth(me->GetMaxHealth());
				if(me->HasAura(SPELL_FERAL_ESSENCE))
					if(me->GetAura(SPELL_FERAL_ESSENCE,0)->GetStackAmount() > 1)
						ModifyAuraStack(SPELL_FERAL_ESSENCE,numb_lives);
					else
						me->RemoveAurasDueToSpell(SPELL_FERAL_ESSENCE);
			}
			else
			{
				if(m_difficulty)
					DoCastVictim(SPELL_SEEPING_25);
				else
					DoCastVictim(SPELL_SEEPING_10);
				if(Creature* Auriaya = GetInstanceCreature(TYPE_AURIAYA))
				{
					((boss_auriaya_AI*)Auriaya->AI())->SetDefenderDown();
				}
				Kill(me);
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!CanDoSomething())
            return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_feral_defender(Creature* pCreature)
{
    return new add_feral_defender_AI (pCreature);
}

enum SentrySpells
{
	SPELL_RIPFLESH_10	=	64375,
	SPELL_RIPFLESH_25	=	64667,
	SPELL_POUNCE_10		=	64374,
	SPELL_POUNCE_25		=	64666,
	SPELL_PACK_STR		=	64381,
};

struct MANGOS_DLL_DECL add_sanctum_sentry_AI : public LibDevFSAI
{
    add_sanctum_sentry_AI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
		if(m_difficulty)
		{
			AddEventOnTank(SPELL_RIPFLESH_25,6000,25000,2000);
			AddEventOnTank(SPELL_POUNCE_25,6000,5000,2000);
		}
		else
		{
			AddEventOnTank(SPELL_RIPFLESH_10,6000,25000,2000);
			AddEventOnTank(SPELL_POUNCE_10,6000,5000,2000);
		}
    }

	void Reset()
	{
		ResetTimers();
		ModifyAuraStack(SPELL_PACK_STR);	
	}

	void JustDied(Unit* pWho)
	{
		if(Creature* Auriaya = GetInstanceCreature(TYPE_AURIAYA))
		{
			((boss_auriaya_AI*)Auriaya->AI())->SetSentryDown();
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!CanDoSomething())
            return;
		
		UpdateEvent(diff);
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_sanctum_sentry(Creature* pCreature)
{
    return new add_sanctum_sentry_AI (pCreature);
}


void AddSC_boss_auriaya()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_auriaya";
    newscript->GetAI = &GetAI_boss_auriaya;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "auriaya_feral_defender";
    newscript->GetAI = &GetAI_feral_defender;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "sanctum_sentry";
    newscript->GetAI = &GetAI_sanctum_sentry;
    newscript->RegisterSelf();
}
