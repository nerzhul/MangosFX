/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum spells
{
	// tyrannus
	SPELL_FORCEFUL_SMASH		=	69155,
	SPELL_OVERLORD_BRAND		=	69172,
	SPELL_UNHOLY_POWER			=	69167,
	SPELL_MARK_OF_RIMEFANG		=	69275,

	// Rimefang
	SPELL_HOARFROST				=	69245,
	SPELL_ICY_BLAST				=	69233,
	SPELL_ICY_BLAST_AOE			=	69238,
};

struct MANGOS_DLL_DECL boss_tyrannusAI : public LibDevFSAI
{
    boss_tyrannusAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_FORCEFUL_SMASH,15000,25000);
		AddEventOnMe(SPELL_UNHOLY_POWER,30000,30000,2000);
    }

	uint32 brand_Timer;
	Unit* brandTarget;
	uint32 Mark_Timer;

    void Reset()
    {
		ResetTimers();
		brand_Timer = 10000;
		Mark_Timer = 8000;
		brandTarget = NULL;
    }

	void DamageDeal(Unit* who, uint32 dmg)
	{
		if(brandTarget && brandTarget->isAlive() && brandTarget->HasAura(SPELL_OVERLORD_BRAND,0))
			me->DealDamage(who,dmg,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;
	
		if(brand_Timer <= diff)
		{
			if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
			{
				DoCast(target,SPELL_OVERLORD_BRAND);
				brandTarget = target;
			}
			brand_Timer = 30000;
		}
		else
			brand_Timer = diff;

		if(Mark_Timer <= diff)
		{
			DoCastRandom(SPELL_MARK_OF_RIMEFANG);
			Mark_Timer = 30000;
		}
		else
			Mark_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_tyrannus(Creature* pCreature)
{
    return new boss_tyrannusAI (pCreature);
}

struct MANGOS_DLL_DECL boss_rimefangAI : public LibDevFSAI
{
    boss_rimefangAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		TyrannusTarget = NULL;
		AddEvent(SPELL_HOARFROST,20000,30000);
		AddEvent(SPELL_ICY_BLAST,15000,15000,2000);
		AddEvent(SPELL_ICY_BLAST_AOE,17000,15000,2000);
		AddEventOnTank(SPELL_HOARFROST,20000,30000,0,1);
		AddEventOnTank(SPELL_ICY_BLAST,15000,15000,2000,0,1);
		AddEventOnTank(SPELL_ICY_BLAST_AOE,17000,15000,2000,0,1);
    }

	Unit* TyrannusTarget;
	uint8 phase;
	uint32 checkTarget_Timer;

    void Reset()
    {
		ResetTimers();
		phase = 0;
		SetFlying(true);
		checkTarget_Timer = 1000;
    }

	void UpdateTarget()
	{
		TyrannusTarget = NULL;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			if (Player* pPlayer = itr->getSource())
			{
				if(pPlayer->isAlive() && pPlayer->HasAura(SPELL_MARK_OF_RIMEFANG))
				{
					TyrannusTarget = pPlayer;
					me->AddThreat(TyrannusTarget,100000.0f);
					phase = 1;
					return;
				}
			}
		}
		if(!TyrannusTarget)
		{
			phase = 0;
			DoResetThreat();
		}
	}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

		if(checkTarget_Timer <= diff)
		{
			UpdateTarget();
			checkTarget_Timer = 1000;
		}
		else
			checkTarget_Timer -= diff;
	
		if(!TyrannusTarget)
			UpdateEvent(diff);
		else
			UpdateEvent(diff,1);
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_rimefang(Creature* pCreature)
{
    return new boss_rimefangAI (pCreature);
}

void AddSC_boss_Tyrannus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_tyrannus";
    newscript->GetAI = &GetAI_boss_tyrannus;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_rimefang";
    newscript->GetAI = &GetAI_boss_rimefang;
    newscript->RegisterSelf();

}
