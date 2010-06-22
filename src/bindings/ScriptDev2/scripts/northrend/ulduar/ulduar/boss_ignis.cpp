#include "precompiled.h"
#include "ulduar.h"

enum ignis_spells
{
	SPELL_FLAME_JETS        =   62680,
	SPELL_FLAME_JETS_H		=	63472,
	SPELL_SCORCH            =   62546,
	SPELL_SCORCH_H			=	63473,
	SPELL_SLAG_POT          =   62717,
	SPELL_SLAG_POT_H		=	63477,
	SPELL_CREATOR_STRENGH	=	64473,
};

enum add_spec_spells
{
	SPELL_FUSION		=	62373,
	SPELL_CHALEUR		=	62343,
	SPELL_ACTIVATE		=	62488,
	SPELL_FRAGILE		=	62382,
	SPELL_CONE_10		=	62548,
	SPELL_CONE_25		=	63476,
};

#define NPC_FIRE		33221


const static float Ignis_Coords[2][2] =
{
	{661.214f,	276.659f},
	{517.598f,	276.068f},
};

struct MANGOS_DLL_DECL boss_ignis_AI : public LibDevFSAI
{
    boss_ignis_AI(Creature *pCreature) : LibDevFSAI(pCreature) {
		InitInstance();
		if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
		{
			AddEvent(SPELL_FLAME_JETS_H,32000,25000,0,TARGET_MAIN);
			AddEvent(SPELL_SCORCH_H,10000,25000,0,TARGET_MAIN);
			AddEvent(SPELL_SLAG_POT_H,15000,30000);
			Assemblage_Timer = 30000;
		}
		else
		{
			AddEvent(SPELL_FLAME_JETS,32000,25000,0,TARGET_MAIN);
			AddEvent(SPELL_SCORCH,10000,25000,0,TARGET_MAIN);
			AddEvent(SPELL_SLAG_POT,15000,30000);
			Assemblage_Timer = 40000;
		}
	}

	uint32 Assemblage_Timer;
	uint32 Fire_Timer;
	std::vector<Unit*> IgnisAdds;
	uint32 Vehicle_Timer;
	uint64 catchPlayer;

    void Reset()
    {
		ResetTimers();
		for (std::vector<Unit*>::iterator itr = IgnisAdds.begin(); itr != IgnisAdds.end();++itr)
        {
			if((*itr))
			{
				((Creature*)(*itr))->Respawn();
				FreezeMob(true,((Creature*)(*itr)));
			}
		}
		IgnisAdds.clear();
		if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
			Assemblage_Timer = 30000;
		else
			Assemblage_Timer = 40000;
		
		Fire_Timer = 10000;
		Vehicle_Timer = 15000;
		catchPlayer = 0;
		ActivateTimeDown(240000);
    }

    void EnterCombat(Unit* who)
    {
        Yell(15564,"Jeunes insolents ! Les lames qui serviront � reconquérir ce monde seront trempées dans votre sang !");
    }
    
    void KilledUnit(Unit* victim)
    {
        if(urand(0,1))
			Yell(15569,"Encore des déchets pour la décharge !");
		else
			Yell(15570,"Vos ossements serviront de petit bois !");
    }

    void JustDied(Unit *victim)
    {
        Yell(15572,"J'ai... échoué...");
		for (std::vector<Unit*>::iterator itr = IgnisAdds.begin(); itr != IgnisAdds.end();++itr)
        {
				Kill((*itr));
		}
		if(pInstance)
		{
			pInstance->SetData(TYPE_IGNIS,DONE);
			if(TimeDownSucceed())
				pInstance->CompleteAchievementForGroup(m_difficulty ? 2929 : 2930);
		}
		
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE);
    }

    void MoveInLineOfSight(Unit* who) 
	{
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		me->RemoveAurasDueToSpell(SPELL_CHALEUR);

		if(Vehicle_Timer <= diff)
		{
			if(Unit* catched = GetGuidUnit(catchPlayer))
			{
				if(catched->isAlive())
				{
					catched->ExitVehicle();
					if(pInstance && catched->GetTypeId() == TYPEID_PLAYER)
						pInstance->CompleteAchievementForPlayer((Player*)catched,m_difficulty ? 2928 : 2927);
				}
				catchPlayer = 0;
				Vehicle_Timer = 20000;
			}
			else
			{
				if(Unit* tmp = GetRandomUnit(0))
				{
					catchPlayer = tmp->GetGUID();
					tmp->EnterVehicle(me);
				}
				Vehicle_Timer = 10000;
			}
		}
		else 
			Vehicle_Timer -= diff;
		if(Assemblage_Timer <= diff)
		{
			if(Unit* tmpcr = GetInstanceCreature(DATA_IGNIS_ADDS))
			{
				Yell(15565,"Levez vous soldats du Creuset de fer ! Que la volonté du faiseur s'accomplisse !");
				IgnisAdds.push_back(tmpcr);
				DoCast(tmpcr,SPELL_ACTIVATE);
				FreezeMob(false,(Creature*)tmpcr);
				tmpcr->AddThreat(me->getVictim());
				me->CastStop();
				SetAuraStack(SPELL_CREATOR_STRENGH,1,me,me);
			}

			if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
				Assemblage_Timer = 40000;
			else
				Assemblage_Timer = 30000;
		}
		else
			Assemblage_Timer -= diff;

		if(Fire_Timer <= diff)
		{
			if(Creature* fire = CallCreature(NPC_FIRE,25000,ON_ME,NOTHING))
				pInstance->SetData64(DATA_IGNIS_FIRE,fire->GetGUID());

			if(urand(0,1))
				Yell(15567,"Que la fournaise vous consume !");
			else
				Yell(15568,"Br�leeeeeeeeeeeez !");

			if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
				Fire_Timer = 20000;
			else
				Fire_Timer = 25000;
		}
		else
			Fire_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL add_ignis_AI : public LibDevFSAI
{
    add_ignis_AI(Creature *pCreature) : LibDevFSAI(pCreature) {
		InitInstance();
	}

	uint32 Check_Timer;
	uint16 nb_stack;
	uint32 CheckReset_Timer;

	void Reset()
	{
		ResetTimers();
		nb_stack = 0;
		FreezeMob(true,me,true);
		Check_Timer = 1000;
		CheckReset_Timer = 5000;
	}

	void DamageTaken(Unit* pDoneBy, uint32 &damage)
	{
		bool KillMe = false;
		if(pDoneBy != me)
		{
			if(me->HasAura(SPELL_FRAGILE) && damage > 3000)
			{
				damage = 0;
				KillMe = true;
			}
			else if(me->HasAura(SPELL_FRAGILE) && damage > 5000)
			{
				damage = 0;
				KillMe = true;
			}
		}
		if(KillMe)
		{
			if(Unit* Ignis = GetInstanceCreature(TYPE_IGNIS))
				if(Ignis->isAlive())
				{
					uint8 stk = (Ignis->GetAura(SPELL_CREATOR_STRENGH,0)->GetStackAmount() > 1) ? Ignis->GetAura(SPELL_CREATOR_STRENGH,0)->GetStackAmount() - 1 : 1;
					SetAuraStack(SPELL_CREATOR_STRENGH,stk,Ignis,Ignis,1);
				}
			Kill(me);
		}
	}

	void JustDied(Unit* pWho)
	{
		if(pInstance)
			pInstance->SetData(DATA_IGNIS_ADD_MONO,DONE);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!CanDoSomething())
		{
			if(CheckReset_Timer <= diff)
			{
				if(!me->HasAura(66830))
					DoCastMe(66830);
				CheckReset_Timer = 5000;
			}
			else
				CheckReset_Timer -= diff;
			return;
		}
            

		if(me->HasAura(SPELL_FUSION,0) && (me->GetDistance2d(Ignis_Coords[0][0],Ignis_Coords[0][1]) < 13.0f) || (me->GetDistance2d(Ignis_Coords[1][0],Ignis_Coords[1][1]) < 15.0f))
		{
			nb_stack = 0;
			me->RemoveAurasDueToSpell(SPELL_FUSION);
			me->RemoveAurasDueToSpell(SPELL_CHALEUR);
			DoCastMe(SPELL_FRAGILE);
		}

		if(me->HasAura(SPELL_FRAGILE))
			me->RemoveAurasDueToSpell(SPELL_CHALEUR);

		if(Check_Timer <= diff)
		{
			if(Unit* fire = GetInstanceCreature(DATA_IGNIS_FIRE))
			{
				if(fire->isAlive() && !me->HasAura(SPELL_FUSION,0) && !me->HasAura(SPELL_FRAGILE,0))
				{
					if(!me->HasAura(SPELL_CHALEUR))
						nb_stack = 0;
					if(me->GetDistance(fire) < 25.0f)
					{
						me->getVictim()->CastSpell(me,SPELL_CHALEUR,true);
						nb_stack++;
						if(nb_stack > 19)
						{
							me->RemoveAurasDueToSpell(SPELL_CHALEUR);
							DoCastMe(SPELL_FUSION,true);
						}
					}
				}
			}
			Check_Timer = 1000;
		}
		else
			Check_Timer -= diff;

		DoMeleeAttackIfReady();
	}
};

struct MANGOS_DLL_DECL add_scorchAI : public LibDevFSAI
{
    add_scorchAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
    }
	uint32 CheckTimer;

    void Reset()
    {
		ResetTimers();
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		SetCombatMovement(false);
		CheckTimer = 10;
    }

	void Fire()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->GetDistance2d(me) < 13.0f)
						me->DealDamage(pPlayer,(m_difficulty ? 3500 : 2200), NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE, NULL, false);
		}
	}

    void UpdateAI(const uint32 diff)
    {
      if(CheckTimer <= diff)
	  {
		  if(me->GetDistance2d(Ignis_Coords[0][0],Ignis_Coords[0][1]) < 10.0f || me->GetDistance2d(Ignis_Coords[1][0],Ignis_Coords[1][1]))
			  me->ForcedDespawn(1000);

			if(m_difficulty)
			{
				if(!me->HasAura(SPELL_CONE_25))
					DoCastMe(SPELL_CONE_25);
			}
			else
			{
				if(!me->HasAura(SPELL_CONE_10))
					DoCastMe(SPELL_CONE_10);
			}

			Fire();
			CheckTimer = 1000;
	  }
	  else
		  CheckTimer -= diff;

    }
};

CreatureAI* GetAI_boss_ignis(Creature* pCreature)
{
    return new boss_ignis_AI (pCreature);
}

CreatureAI* GetAI_add_ignis(Creature* pCreature)
{
    return new add_ignis_AI (pCreature);
}

CreatureAI* GetAI_add_scorch(Creature* pCreature)
{
    return new add_scorchAI (pCreature);
}

void AddSC_boss_ignis()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ignis";
    newscript->GetAI = &GetAI_boss_ignis;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_ignis";
    newscript->GetAI = &GetAI_add_ignis;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_ignis_fire";
    newscript->GetAI = &GetAI_add_scorch;
    newscript->RegisterSelf();
}
