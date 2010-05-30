/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "pit_of_saron.h"

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
	bool prefightEvent;
	uint32 EventTimer;
	uint8 EventPhase;
	BattleGroundTeamId bgTeam;

    void Reset()
    {
		ResetTimers();
		//me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		EventPhase = 0;
		prefightEvent = false;
		EventTimer = 1000;
		brand_Timer = 10000;
		Mark_Timer = 8000;
		brandTarget = NULL;
    }

	void Aggro(Unit* who)
	{
		/*if(!prefightEvent)
		{
			prefightEvent = true;
			if(who->GetTypeId() == TYPEID_PLAYER)
				bgTeam = BattleGroundTeamId(((Player*)who)->GetBGTeam());
		}*/
		Yell(16760,"Je ne décevrai pas le Roi Liche ! Venez trouver votre fin !");
		if(Creature* rimefang = GetInstanceCreature(DATA_RIMEFANG))
			if(rimefang->isAlive())
				rimefang->AddThreat(who,2.0f);
	}

	void DamageDeal(Unit* who, uint32 dmg)
	{
		if(brandTarget && brandTarget->isAlive() && brandTarget->HasAura(SPELL_OVERLORD_BRAND,0))
			me->DealDamage(who,dmg,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

    void UpdateAI(const uint32 diff)
    {
		/*if(prefightEvent)
		{
			if(EventTimer <= diff)
			{
				switch(EventPhase)
				{
					case 0:
						Speak(CHAT_TYPE_SAY,16758,"Hélas mes très, très braves aventuriers, votre intrusion touche à sa fin. Entendez vous le son du claquement des os dans le tunnel derrière vous ? C'est le son d'une mort imminente");
						EventTimer = 14000;
						break;
					case 1:
						Speak(CHAT_TYPE_SAY,16759,"Hahahaha ! La vermine s'agite, comme c'est amusant. Quand j'en aurai fini avec vous l'épée de mon maître dévorera vos âmes. Mourrez !");
						EventTimer = 10000;
						break;
					case 2:
						Yell(16760,"Je ne décevrai pas le Roi Liche ! Venez trouver votre fin !");
						me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
						EventTimer = DAY*HOUR;
						prefightEvent = false;
						break;
				}
				EventPhase++;
			}
			else
				EventTimer -= diff;
			

			return;
		}*/
        //Return since we have no target
        if (!CanDoSomething())
            return;
	
		if(brand_Timer <= diff)
		{
			if(Unit* target = GetRandomUnit())
			{
				DoCast(target,SPELL_OVERLORD_BRAND);
				brandTarget = target;
			}
			brand_Timer = 30000;
		}
		else
			brand_Timer -= diff;

		if(Mark_Timer <= diff)
		{
			DoCastRandom(SPELL_MARK_OF_RIMEFANG);
			Yell(16764,"Frigecroc ! Anéantis cet imbécile");
			Mark_Timer = 30000;
		}
		else
			Mark_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
		if(Creature* Rimefang = GetInstanceCreature(DATA_RIMEFANG))
			Rimefang->ForcedDespawn();
		GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
		Speak(CHAT_TYPE_SAY,16763,"Impossible... Frigecroc... avertis...");
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Yell(16761,"Quelle prestation embarassante ! La mort vous va bien mieux.");
		else
			Yell(16762,"Vous auriez peut être du rester... dans les montagnes !");
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
		AddEvent(SPELL_HOARFROST,18000,25000);
		AddEvent(SPELL_ICY_BLAST,10000,12000,2000);
		AddEvent(SPELL_ICY_BLAST_AOE,13000,16000,2000);
		AddEventOnTank(SPELL_HOARFROST,12000,20000,0,1);
		AddEventOnTank(SPELL_ICY_BLAST,8000,9000,2000,0,1);
		AddEventOnTank(SPELL_ICY_BLAST_AOE,10000,13000,2000,0,1);
		me->GetMotionMaster()->MovePoint(0,986.452,186.452f,649.188f);
		Relocate(986.452,186.452f,649.188f);
    }

	Unit* TyrannusTarget;
	uint8 phase;
	uint32 checkTarget_Timer;
	uint8 FlyPoint;
	uint32 MoveTimer;
	uint8 nb_target;

    void Reset()
    {
		ResetTimers();
		phase = 0;
		nb_target = 0;
		MoveTimer = 1000;
		FlyPoint = 0;
		SetFlying(true);
		SetCombatMovement(false);
		Relocate(986.452,186.452f,649.188f);
		checkTarget_Timer = 1000;
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
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
					nb_target++;
					me->AddThreat(TyrannusTarget,nb_target*100000.0f);
					phase = 1;
					return;
				}
			}
		}
		if(!TyrannusTarget)
		{
			phase = 0;
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
    newscript->Name = "boss_rimefang_icc5";
    newscript->GetAI = &GetAI_boss_rimefang;
    newscript->RegisterSelf();

}
