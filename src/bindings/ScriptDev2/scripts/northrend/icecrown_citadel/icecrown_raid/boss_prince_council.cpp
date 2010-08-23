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
	SPELL_KINETIC_BOMB_2					= 72080,
	SPELL_KINETIC_BOMB_EXPLODE              = 72052,
	SPELL_SHOCK_VORTEX                      = 72037,
	SPELL_SHOCK_VORTEX_PERIODIC             = 71945,
	SPELL_SHOCK_VORTEX_SKIN					= 72633,
	SPELL_SHOCK_VORTEX_POW                  = 72039,

	NPC_KINETIC_BOMB_TARGET                 = 38458,
	NPC_KINETIC_BOMB						= 38454,
	NPC_SHOCK_VORTEX                        = 38422,

	//Taldaram
	SPELL_GLITTERING_SPARKS                 = 71807,
	SPELL_CONJURE_FLAME                     = 71718,
	SPELL_FLAME_EXPLOSION	                = 71393,
	SPELL_FLAME_VISUAL						= 71706,
	SPELL_FLAME_AUTOATTACK					= 71709,
	SPELL_FLAME_POWER						= 71756,
	SPELL_CONJURE_FLAME_POW                 = 72040,
	SPELL_FLAMES_2                          = 71708,

	//Keleseth
	SPELL_SHADOW_LANCE                      = 71405,
	SPELL_SHADOW_LANCE_POW                  = 71815,
	SPELL_SHADOW_RESONANCE                  = 71943,
	SPELL_SHADOW_RESONANCE_DAMAGE           = 71822,

	NPC_DARK_NUCLEUS                        = 38369,
};

struct MANGOS_DLL_DECL boss_icc_prince_bomb_targetAI : public LibDevFSAI
{
    boss_icc_prince_bomb_targetAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

CreatureAI* GetAI_boss_icc_prince_bomb_target(Creature* pCreature)
{
    return new boss_icc_prince_bomb_targetAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icc_prince_bombAI : public LibDevFSAI
{
    boss_icc_prince_bombAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		//MakeHostileInvisibleStalker();
    }

	uint32 checkDist_Timer;

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		checkDist_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
		if(checkDist_Timer <= diff)
		{
			checkDist_Timer = 1000;
			if(Creature* target = me->GetClosestCreatureWithEntry(NPC_KINETIC_BOMB_TARGET,150.0f))
				if(target->GetDistance2d(me) < 10.0f)
					DoCastMe(SPELL_KINETIC_BOMB_EXPLODE);
		}
		else
			checkDist_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_icc_prince_bomb(Creature* pCreature)
{
    return new boss_icc_prince_bombAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icc_prince_shock_vortexAI : public LibDevFSAI
{
    boss_icc_prince_shock_vortexAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
		me->ForcedDespawn(20000);
    }

	uint32 checkDist_Timer;

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(SPELL_SHOCK_VORTEX_SKIN);
		ModifyAuraStack(SPELL_SHOCK_VORTEX_PERIODIC);
		SetCombatMovement(false);
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

CreatureAI* GetAI_boss_icc_prince_shock_vortex(Creature* pCreature)
{
    return new boss_icc_prince_shock_vortexAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icc_prince_flameballAI : public LibDevFSAI
{
    boss_icc_prince_flameballAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
		me->ForcedDespawn(20000);
    }

	uint64 target;
	uint32 checkDist_Timer;

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(SPELL_FLAME_VISUAL);
		target = 0;
		checkDist_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
		if(checkDist_Timer <= diff)
		{
			checkDist_Timer = 1000;
			if(Unit* tar = GetGuidUnit(target))
			{
				if(tar->GetDistance2d(me) < 10.0f)
				{
					DoCast(tar,SPELL_FLAME_EXPLOSION);
					me->ForcedDespawn(500);
				}
			}
		}
		else
			checkDist_Timer -= diff;
    }

	void SetTarget(Unit* who)
	{
		target = who->GetGUID();
		who->AddThreat(me,10000000.0f);
	}
};

CreatureAI* GetAI_boss_icc_prince_flameball(Creature* pCreature)
{
    return new boss_icc_prince_flameballAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icc_prince_flameball_powAI : public LibDevFSAI
{
    boss_icc_prince_flameball_powAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
		me->ForcedDespawn(20000);
    }

	uint64 target;
	uint32 checkDist_Timer;
	uint32 startTimer_Timer;

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(SPELL_FLAME_VISUAL);
		checkDist_Timer = 1000;
		startTimer_Timer = 3000;
		target = 0;
    }

	void SpellHitTarget(Unit* pWho,const SpellEntry* spell)
	{
		ModifyAuraStack(SPELL_FLAME_POWER,-1);
	}

    void UpdateAI(const uint32 diff)
    {
		if(checkDist_Timer <= diff)
		{
			checkDist_Timer = 1000;
			if(Unit* tar = GetGuidUnit(target))
			{
				if(tar->GetDistance2d(me) < 10.0f)
				{
					DoCast(tar,SPELL_FLAME_EXPLOSION);
					me->ForcedDespawn(500);
				}
			}
		}
		else
			checkDist_Timer -= diff;

		if(startTimer_Timer <= diff)
		{
			ModifyAuraStack(SPELL_FLAME_AUTOATTACK);
			ModifyAuraStack(SPELL_FLAME_POWER,20);
			startTimer_Timer = 30000;
		}
		else
			startTimer_Timer -= diff;
    }

	void SetTarget(Unit* who)
	{
		target = who->GetGUID();
		who->AddThreat(me,10000000.0f);
	}
};

struct MANGOS_DLL_DECL boss_icc_taldaramAI : public LibDevFSAI
{
    boss_icc_taldaramAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_GLITTERING_SPARKS,15000,15000,3000);
		AddEnrageTimer(TEN_MINS);
		AddTextEvent(16856,"Ha ha ha ha !",TEN_MINS,60000);
    }

	uint32 PoweredSpell_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		SetInstanceData(TYPE_PRINCE_COUNCIL, NOT_STARTED);
		PoweredSpell_Timer = 20000;
    }

	void SpellHit(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == 71719)
		{
			if(Creature* flame = me->GetClosestCreatureWithEntry(38332,100.0f))
				if(flame->isAlive())
				{
					Unit* target = GetRandomUnit(1);
					if(!target)
						target = me->getVictim();
					if(target)
						((boss_icc_prince_flameballAI*)flame->AI())->SetTarget(target);
				}
		}
		else if(spell->Id == 72041)
		{
			if(Creature* flame = me->GetClosestCreatureWithEntry(38451,100.0f))
				if(flame->isAlive())
				{
					Unit* target = GetRandomUnit(1);
					if(!target)
						target = me->getVictim();
					if(target)
						((boss_icc_prince_flameball_powAI*)flame->AI())->SetTarget(target);
				}
		}
	}

	void EmpowerMe(float pctLife)
	{
		me->SetHealth(me->GetMaxHealth()*pctLife);
		me->CastStop();
		DoCastMe(SPELL_INVOCATION_OF_BLOOD_T);
		Yell(16857,"Tremblez devant Taldaram, mortels, car le pouvoir de l'orbe coule en moi !");
	}

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
		if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
			Keleseth->AddThreat(pWho,1);
		if(Creature* Valanar = GetInstanceCreature(DATA_PRINCE_VALANAR))
			Valanar->AddThreat(pWho,1);
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
		if(urand(0,1))
			Say(16853,"A manger pour les asticots !");
		else
			Say(16853,"Implorez pitié !");
	}

    void JustDied(Unit* pKiller)
    {
		Yell(16855,"Aarrr...");
    }

    void JustReachedHome()
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, FAIL);
		if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
			if(!Keleseth->isAlive())
				Keleseth->Respawn();
		if(Creature* Valanar = GetInstanceCreature(DATA_PRINCE_VALANAR))
			if(!Valanar->isAlive())
				Valanar->Respawn();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(PoweredSpell_Timer <= diff)
		{
			Yell(16858,"Savourez la douleur !");
			DoCastMe(me->HasAura(SPELL_INVOCATION_OF_BLOOD_T) ? SPELL_CONJURE_FLAME_POW : SPELL_CONJURE_FLAME);
			PoweredSpell_Timer = 30000;
		}
		else
			PoweredSpell_Timer -= diff;

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
		AddEventMaxPrioOnMe(SPELL_SHADOW_RESONANCE,13000,15000);
		AddTextEvent(16728,"Le sang va couler !",13000,15000);
		AddEnrageTimer(TEN_MINS);
		AddTextEvent(16726,"Mouhahahahaha !",TEN_MINS,60000);
    }

	uint32 PoweredSpell_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
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
		if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
			Taldaram->AddThreat(pWho,1);
		if(Creature* Valanar = GetInstanceCreature(DATA_PRINCE_VALANAR))
			Valanar->AddThreat(pWho,1);
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
		if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
			if(!Taldaram->isAlive())
				Taldaram->Respawn();
		if(Creature* Valanar = GetInstanceCreature(DATA_PRINCE_VALANAR))
			if(!Valanar->isAlive())
				Valanar->Respawn();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(PoweredSpell_Timer <= diff)
		{
			DoCastVictim(me->HasAura(SPELL_INVOCATION_OF_BLOOD_K) ? SPELL_SHADOW_LANCE_POW : SPELL_SHADOW_LANCE);
			PoweredSpell_Timer = urand(2000,3000);
		}
		else
			PoweredSpell_Timer -= diff;

		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_icc_keleseth(Creature* pCreature)
{
    return new boss_icc_kelesethAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icc_prince_shadowresAI : public LibDevFSAI
{
    boss_icc_prince_shadowresAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_SHADOW_RESONANCE_DAMAGE,1000,3000,0,TARGET_NEAR);
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		AggroAllPlayers(100.0f);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_icc_prince_shadowres(Creature* pCreature)
{
    return new boss_icc_prince_shadowresAI(pCreature);
}


CreatureAI* GetAI_boss_icc_prince_flameball_pow(Creature* pCreature)
{
    return new boss_icc_prince_flameball_powAI(pCreature);
}


struct MANGOS_DLL_DECL boss_icc_valanarAI : public LibDevFSAI
{
    boss_icc_valanarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddMaxPrioEvent(SPELL_KINETIC_BOMB,30000,30000);
		AddEnrageTimer(TEN_MINS);
		AddTextEvent(16684,"A genoux devant le San'len !",TEN_MINS,60000);
    }

	uint32 PoweredSpell_Timer;
	uint32 orb_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		PoweredSpell_Timer = 10000;
		orb_Timer = 50000;
		SetInstanceData(TYPE_PRINCE_COUNCIL, NOT_STARTED);
    }

	void SpellHitTarget(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == SPELL_KINETIC_BOMB)
		{
			me->CastStop();
			DoCastMe(SPELL_KINETIC_BOMB_2);
		}
		else if(spell->Id == SPELL_KINETIC_BOMB_2)
		{
			if(Creature* bomb = me->GetClosestCreatureWithEntry(NPC_KINETIC_BOMB,150.0f))
				if(Creature* bombTarget = me->GetClosestCreatureWithEntry(NPC_KINETIC_BOMB_TARGET,150.0f))
				{
					bomb->AddThreat(bombTarget,1000000.0f);
					bomb->GetMotionMaster()->MoveChase(bombTarget);
				}
		}
	}

	void EmpowerMe(float pctLife)
	{
		me->SetHealth(me->GetMaxHealth()*pctLife);
		me->CastStop();
		DoCastMe(SPELL_INVOCATION_OF_BLOOD_V);
		Yell(16685,"Naxanar ne fut qu'un contretemps. Grâce au pouvoir de l'orbe, Valanar aura sa revanche !");
	}

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, IN_PROGRESS);
		if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
			Keleseth->AddThreat(pWho,1);
		if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
			Taldaram->AddThreat(pWho,1);

		ChangeOrbOwner();		
    }

	void ChangeOrbOwner()
	{
		/**
		* 0 : Valanar
		* 1 : Keleseth
		* 2 : Taldaram
		*/

		float percent = 100.0f;
		int8 lastPrince = -1;
		if(me->HasAura(SPELL_INVOCATION_OF_BLOOD_V))
		{
			lastPrince = 0;
			percent = float(me->GetHealth()) / float(me->GetMaxHealth());
			me->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_V);
		}

		if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
			if(Keleseth->HasAura(SPELL_INVOCATION_OF_BLOOD_K))
			{
				lastPrince = 1;
				percent = float(Keleseth->GetHealth()) / float(Keleseth->GetMaxHealth());
				Keleseth->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_K);
			}

		if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
			if(Taldaram->HasAura(SPELL_INVOCATION_OF_BLOOD_T))
			{
				lastPrince = 2;
				percent = float(Taldaram->GetHealth()) / float(Taldaram->GetMaxHealth());
				Taldaram->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_T);
			}

		switch(lastPrince)
		{
			case -1:
			{
				EmpowerMe(percent);
				if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
					Keleseth->SetHealth(1);
				if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
					Taldaram->SetHealth(1);
				break;
			}
			case 0:
			{
				uint8 selectedPrince = urand(0,1);
				me->SetHealth(1);
				if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
				{
					if(selectedPrince == 0)
						Keleseth->SetHealth(1);
					else
						((boss_icc_kelesethAI*)Keleseth->AI())->EmpowerMe(percent);
				}
				if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
				{
					if(selectedPrince == 1)
						Taldaram->SetHealth(1);
					else
						((boss_icc_taldaramAI*)Taldaram->AI())->EmpowerMe(percent);
				}
				break;
			}
			case 1:
			{
				uint8 selectedPrince = urand(0,1);
				if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
					Keleseth->SetHealth(1);
				
				if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
				{
					if(selectedPrince == 1)
						Taldaram->SetHealth(1);
					else
						((boss_icc_taldaramAI*)Taldaram->AI())->EmpowerMe(percent);
				}

				if(selectedPrince == 0)
					me->SetHealth(1);
				else
					EmpowerMe(percent);
				break;
			}
			case 2:
			{
				uint8 selectedPrince = urand(0,1);
				if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
					Taldaram->SetHealth(1);

				if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
				{
					if(selectedPrince == 1)
						Keleseth->SetHealth(1);
					else
						((boss_icc_kelesethAI*)Keleseth->AI())->EmpowerMe(percent);
				}

				if(selectedPrince == 0)
					me->SetHealth(1);
				else
					EmpowerMe(percent);
				break;
			}
		}
	}

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(!me->HasAura(SPELL_INVOCATION_OF_BLOOD_V))
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
		if(urand(0,1))
			Say(16681,"Le diner est servi.");
		else
			Say(16682,"Voyez vous la puissance des ténèbres à présent ?");
	}

    void JustDied(Unit* pKiller)
    {
        SetInstanceData(TYPE_PRINCE_COUNCIL, DONE);
		Yell(16683,"Arrrr ! Pourquoi.... ?");
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
		if(Creature* Keleseth = GetInstanceCreature(DATA_PRINCE_KELESETH))
			if(!Keleseth->isAlive())
				Keleseth->Respawn();
		if(Creature* Taldaram = GetInstanceCreature(DATA_PRINCE_TALDARAM))
			if(!Taldaram->isAlive())
				Taldaram->Respawn();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(orb_Timer <= diff)
		{
			ChangeOrbOwner();
			orb_Timer = 60000;
		}
		else
			orb_Timer -= diff;

		if(PoweredSpell_Timer <= diff)
		{
			Yell(16686,"Ma coupe déborde");
			DoCastMe(me->HasAura(SPELL_INVOCATION_OF_BLOOD_V) ? SPELL_SHOCK_VORTEX_POW : SPELL_SHOCK_VORTEX);
			PoweredSpell_Timer = urand(15000,18000);
		}
		else
			PoweredSpell_Timer -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_icc_valanar(Creature* pCreature)
{
    return new boss_icc_valanarAI(pCreature);
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

	NewScript = new Script;
    NewScript->Name = "boss_icc_prince_shadowres";
    NewScript->GetAI = &GetAI_boss_icc_prince_shadowres;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_taldaram_ball_flame";
    NewScript->GetAI = &GetAI_boss_icc_prince_flameball;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_taldaram_ball_flame_pow";
    NewScript->GetAI = &GetAI_boss_icc_prince_flameball_pow;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_prince_bomb";
    NewScript->GetAI = &GetAI_boss_icc_prince_bomb;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_prince_bomb_target";
    NewScript->GetAI = &GetAI_boss_icc_prince_bomb_target;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_prince_shock_vortex";
    NewScript->GetAI = &GetAI_boss_icc_prince_shock_vortex;
    NewScript->RegisterSelf();
}
