#include "precompiled.h"
#include "trial_of_the_crusader.h"

//boss_GormoktheImpaler

enum GormoktheImpalerSpells
{
	SPELL_IMPALE = 66331,
	SPELL_STAGGERING_STOMP = 66330,
	SPELL_RISING_ANGER = 66636,
	NPC_FRIGBOLD = 34800,
};

struct MANGOS_DLL_DECL boss_GormoktheImpaler_AI : public LibDevFSAI
{
    boss_GormoktheImpaler_AI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEvent(26662,540000,10000,0,TARGET_ME);
		AddEvent(SPELL_IMPALE,20000,10000,0,TARGET_MAIN);
		AddEvent(SPELL_STAGGERING_STOMP,30000,25000,0,TARGET_MAIN);
    }

	uint8 Frigibold_count;
	uint32 frigibold_Timer;
	uint32 Spawn_Timer;
	bool Spawnable;

    void Reset()
    {		
		CleanMyAdds();
		
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Frigibold_count = 4;
				Spawnable = false;
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Frigibold_count = 5;
				Spawnable = false;
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Frigibold_count = 4;
				Spawnable = true;
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Frigibold_count = 5;
				Spawnable = true;
				break;
		}
		Spawn_Timer = 180000;
		frigibold_Timer = 45000;
		SetEquipmentSlots(false, 50760, 48040, 47267);
		AggroAllPlayers(150.0f);
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
		{
			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
				case RAID_DIFFICULTY_25MAN_NORMAL:
					pInstance->SetData(TYPE_GormoktheImpaler, DONE);
					if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_ANNOUNCER) : 0)))
						((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_JORM);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
				case RAID_DIFFICULTY_25MAN_HEROIC:
					if(Spawnable)
					{
						pInstance->SetData(TYPE_GormoktheImpaler, DONE);
						if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_ANNOUNCER) : 0)))
							((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_JORM);
					}
					break;
			}
		}
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void Aggro(Unit* pWho)
    {
        if (pInstance)
		{
            pInstance->SetData(TYPE_GormoktheImpaler, IN_PROGRESS);
			pInstance->SetData(TYPE_EVENT_BEAST, IN_PROGRESS);
		}
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(Frigibold_count > 0)
		{
			if(frigibold_Timer <= diff)
			{
				if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
					CallCreature(NPC_FRIGBOLD,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,target->GetPositionX(),target->GetPositionY(),target->GetPositionZ());

				Frigibold_count--;
				ModifyAuraStack(SPELL_RISING_ANGER,5 - Frigibold_count);
				frigibold_Timer = 60000;
			}
			else
				frigibold_Timer -= diff;
		}

		if(Spawnable)
		{
			if(Spawn_Timer <= diff)
			{
				if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_ANNOUNCER) : 0)))
					((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_JORM);
				Spawn_Timer = 180000;
				Spawnable = false;
			}
			else
				Spawn_Timer -= diff;
		}
		UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_GormoktheImpaler(Creature* pCreature)
{
    return new boss_GormoktheImpaler_AI(pCreature);
}

enum add_GormoktheImpaler
{
	SPELL_BATTER = 66408,  
	SPELL_FIRE_BOMB = 66313,
	SPELL_HEAD_CRACK = 66407, 
};

struct MANGOS_DLL_DECL add_GormoktheImpaler_AI : public LibDevFSAI
{
    add_GormoktheImpaler_AI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEvent(SPELL_BATTER,12000,7000,3000,TARGET_MAIN);
		AddEvent(SPELL_FIRE_BOMB,14000,20000,2000);
		AddEvent(SPELL_HEAD_CRACK,12000,26000,1000,TARGET_MAIN);
    }

    void Reset(){}

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_add_GormoktheImpaler(Creature* pCreature)
{
    return new add_GormoktheImpaler_AI(pCreature);
};

enum AcidmawSpells
{
	SPELL_ACIDIC_SPEW = 66818,
	SPELL_PARALYTIC_BITE	= 66824,
	SPELL_SLIME_POOL = 66883,
	SPELL_ACID_SPIT = 66880,
	SPELL_PARALYTIC_SPRAY = 66901,
	SPELL_SWEEP = 66794,
	SPELL_PARALYTIC_TOXIN = 66823,
	SPELL_ENRAGE = 68335
};

struct MANGOS_DLL_DECL boss_Acidmaw_AI : public LibDevFSAI
{
    boss_Acidmaw_AI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(26662,360000,10000);
		AddEvent(SPELL_ACID_SPIT,7000,9000,1000,TARGET_RANDOM,2);
		AddNoTankEvent(SPELL_ACID_SPIT,1500);
		AddEventOnTank(SPELL_PARALYTIC_BITE,15000,15000,0,1);
		AddEventOnTank(SPELL_PARALYTIC_SPRAY,17000,15000,1000,2);
		AddEventOnTank(SPELL_SLIME_POOL,25000,25000,2000,1);
		AddEventOnTank(SPELL_SWEEP,14000,14000,1000,2);
		AddEventOnTank(SPELL_ACIDIC_SPEW,6000,9000,0,1); // doit tourner pour toucher tout le monde
    }

	uint8 phase;
	uint32 phase_Timer;
	bool Spawnable;
	uint32 Spawn_Timer;
	uint32 Spew_Timer;

    void Reset()
    {		
		ResetTimers();
		ActivateManualMoveSystem();
		SetMovePhase();
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Spawnable = false;
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Spawnable = true;
				break;
		}
		phase = 1;
		phase_Timer = 10000;
		Spawn_Timer = 180000;
		Spew_Timer = 15100;
		AggroAllPlayers(150.0f);
    }
	
	void ApplyPoisonInArc()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			if (Player* pPlayer = itr->getSource())
			{
				if(pPlayer->isAlive() && !pPlayer->isGameMaster() && me->HasInArc(M_PI/2, pPlayer))
					ModifyAuraStack(SPELL_PARALYTIC_TOXIN,1,pPlayer);
			}
		}
	}

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_Acidmaw, DONE);
		if (Creature* Dreadscale = GetInstanceCreature(TYPE_Dreadscale))
		{
			if(Dreadscale->isAlive())
			{
				switch(m_difficulty)
				{
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						ModifyAuraStack(SPELL_ENRAGE,1,Dreadscale,Dreadscale);
						break;
				}
			}
			else
			{
				switch(m_difficulty)
				{
					case RAID_DIFFICULTY_10MAN_NORMAL:
					case RAID_DIFFICULTY_25MAN_NORMAL:
						pInstance->SetData(TYPE_Acidmaw, DONE);
						if (Creature* Ann = GetInstanceCreature(DATA_ANNOUNCER))
							((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
						break;
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						if(Spawnable)
						{
							pInstance->SetData(TYPE_Acidmaw, DONE);
							if (Creature* Ann = GetInstanceCreature(DATA_ANNOUNCER))
								((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
						}
						break;
				}
				Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
				if (!lPlayers.isEmpty())
				{
					for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* pPlayer = itr->getSource())
					{
						if(pPlayer->isAlive() && pPlayer->HasAura(66830))
							pPlayer->RemoveAurasDueToSpell(66830);
					}
				}
			}
		}
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

		if (Creature* Dreadscale = GetInstanceCreature(TYPE_Dreadscale))
			if(Dreadscale->isAlive())
				Dreadscale->AddThreat(pWho);

        if (pInstance)
            pInstance->SetData(TYPE_Acidmaw, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(phase_Timer <= diff)
		{
			DoResetThreat();
			if(phase == 1)
			{
				phase = 2;
				SetMovePhase(false);
			}
			else
			{
				phase = 1;
				SetMovePhase();
			}

			phase_Timer = 45000;
		}
		else
			phase_Timer -= diff;

		if(phase == 1)
		{
			if(Spew_Timer <= diff)
			{
				ApplyPoisonInArc();
				Spew_Timer = 9000;
			}
			else
				Spew_Timer -= diff;
		}

		UpdateEvent(diff,phase);

		if(Spawnable)
		{
			if(Spawn_Timer <= diff)
			{
				if (Creature* Ann = GetInstanceCreature(DATA_ANNOUNCER))
					((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
				Spawn_Timer = 180000;
				Spawnable = false;
			}
			else
				Spawn_Timer -= diff;
		}

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_Acidmaw(Creature* pCreature)
{
    return new boss_Acidmaw_AI(pCreature);
};


enum DraedscaleSpells
{
	SPELL_BURNING_BITE = 66879,
	SPELL_MOLTEN_SPEW = 66821,
	//SPELL_SLIME_POOL = 66883,
	SPELL_FIRE_SPIT = 66796,
	SPELL_BURNING_SPRAY = 66902,
	//SPELL_SWEEP = 66794,
	SPELL_BURNING_BILE = 66869,
	//SPELL_ENRAGE = 68335,
};

struct MANGOS_DLL_DECL boss_Dreadscale_AI : public LibDevFSAI
{
    boss_Dreadscale_AI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(26662,360000,10000);
		AddEventOnTank(SPELL_BURNING_BITE,9000,12000,1000,1);
		AddEventOnTank(SPELL_BURNING_SPRAY,17000,15000,1000,2);
		AddEvent(SPELL_FIRE_SPIT,6000,9000,1000,TARGET_RANDOM,2);
		AddNoTankEvent(SPELL_FIRE_SPIT,3000);
		AddEventOnTank(SPELL_MOLTEN_SPEW,15000,15000,1000,1);
    }

	uint8 phase;
	uint32 phase_Timer;
	uint32 CheckDistanceTimer;
	bool Spawnable;
	uint32 Spawn_Timer;
	uint32 Check_Para_Timer;

    void Reset()
    {
		ResetTimers();
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Spawnable = false;
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Spawnable = true;
				break;
		}
		phase = 1;
		phase_Timer = 55000;
		Spawn_Timer = 180000;
		CheckDistanceTimer = 1000;
		Check_Para_Timer = 1000;
		AggroAllPlayers(150.0f);
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_Dreadscale, DONE);
		if (Creature* Acidmaw = GetInstanceCreature(TYPE_Acidmaw))
		{
			if(Acidmaw->isAlive())
			{
				switch(m_difficulty)
				{
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						ModifyAuraStack(SPELL_ENRAGE,1,Acidmaw,Acidmaw);
						break;
				}
			}
			else
			{
				switch(m_difficulty)
				{
					case RAID_DIFFICULTY_10MAN_NORMAL:
					case RAID_DIFFICULTY_25MAN_NORMAL:
						if(pInstance)
							pInstance->SetData(TYPE_Dreadscale, DONE);
						if (Creature* Ann = GetInstanceCreature(DATA_ANNOUNCER))
							((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
						break;
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						if(Spawnable)
						{
							if(pInstance)
								pInstance->SetData(TYPE_Dreadscale, DONE);
							if (Creature* Ann = GetInstanceCreature(DATA_ANNOUNCER))
								((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
						}
						break;
				}

				Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
				if (!lPlayers.isEmpty())
				{
					for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* pPlayer = itr->getSource())
					{
						if(pPlayer->isAlive() && pPlayer->HasAura(66830))
							pPlayer->RemoveAurasDueToSpell(66830);
					}
				}
			}
		}
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

		if (Creature* Acidmaw = GetInstanceCreature(TYPE_Acidmaw))
			if(Acidmaw->isAlive())
				Acidmaw->AddThreat(pWho);
        if (pInstance)
            pInstance->SetData(TYPE_Dreadscale, IN_PROGRESS);

    }

	void CheckParalyse()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isAlive())
						if(pPlayer->HasAura(SPELL_PARALYTIC_TOXIN))
						{
							Aura* aur = pPlayer->GetAura(SPELL_PARALYTIC_TOXIN);
							if(aur)
							{
								int32 duration = aur->GetAuraDuration();
								float rate = float(duration) / 60000;
								pPlayer->SetSpeedRate(MOVE_RUN,rate,true);
								if(duration < 10000)
									ModifyAuraStack(66830,1,pPlayer);
							}

							if(me->getVictim() && me->getVictim()->HasAura(SPELL_BURNING_BILE) && pPlayer->GetDistance2d(me->getVictim()) < 5.0f)
							{
								pPlayer->RemoveAurasDueToSpell(SPELL_PARALYTIC_TOXIN);
								pPlayer->RemoveAurasDueToSpell(66830);
							}
								
						}
						else
							pPlayer->SetSpeedRate(MOVE_RUN,1.0f,true);						
				}
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase_Timer <= diff)
		{
			DoResetThreat();
			if(phase == 1)
				phase = 2;
			else
				phase = 1;

			phase_Timer = 45000;
		}
		else
			phase_Timer -= diff;

		if(CheckDistanceTimer < diff)
		{
			if(phase == 1 && me->getVictim() && me->getVictim()->GetDistance2d(me) > 8.0f)
				me->GetMotionMaster()->MoveChase(me->getVictim());
			else if (phase == 2)
			{
				me->GetMotionMaster()->MoveIdle();
				me->StopMoving();
			}
			if(phase == 1)
				CheckDistanceTimer = 1500;
			else
				CheckDistanceTimer = 100;
		}
		else
			CheckDistanceTimer -= diff;

		if(Spawnable)
		{
			if(Spawn_Timer <= diff)
			{
				if (Creature* Acidmaw = GetInstanceCreature(TYPE_Acidmaw))
					if(!Acidmaw->isAlive())
					{
						if (Creature* Ann = GetInstanceCreature(DATA_ANNOUNCER))
							((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
					}
				Spawn_Timer = 180000;
				Spawnable = false;
			}
			else
				Spawn_Timer -= diff;
		}

		if(Check_Para_Timer <= diff)
		{
			CheckParalyse();			
			Check_Para_Timer = 1000;
		}
		else
			Check_Para_Timer -= diff;

		UpdateEvent(diff,phase);

        DoMeleeAttackIfReady();

    }
};

CreatureAI* GetAI_boss_Dreadscale(Creature* pCreature)
{
    return new boss_Dreadscale_AI(pCreature);
}

enum IcehowlSpells
{
	SPELL_FEROCIOUS_BUTT	= 66770,
	SPELL_ARCTIC_BREATH	= 66689,
	SPELL_WHIRL = 67345,
	SPELL_MASSIVE_CRASH = 66683,
	SPELL_SURGE_OF_ADRENALINE = 68667,
	SPELL_TRAMPLE = 66734,
	SPELL_STAGGERED_DAZE = 66758,
	SPELL_FROTHING_RAGE = 66759,
};

struct MANGOS_DLL_DECL boss_Icehowl_AI : public ScriptedAI
{
    boss_Icehowl_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();

        Reset();
    }

	MobEventTasks Tasks;
	Difficulty m_bIsHeroic;
	uint8 phase;
	uint32 phase_Timer;
	Unit* target;
	uint32 CheckDistanceTimer;
	float x,y,z;
	bool inCrash;

    void Reset()
    {		
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(26662,180000,10000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_ARCTIC_BREATH,15000,25000,2500,TARGET_MAIN,1);
		Tasks.AddEvent(SPELL_FEROCIOUS_BUTT,25000,30000,5000,TARGET_MAIN,1);
		Tasks.AddEvent(SPELL_WHIRL,urand(4000,8000),15000,15000,TARGET_ME,1);
		phase = 1;
		me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
		phase_Timer = 45000;
		CheckDistanceTimer = 1000;
		AggroAllPlayers(150.0f);
		inCrash = false;
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
		{
            pInstance->SetData(TYPE_Icehowl, DONE);
			pInstance->SetData(TYPE_EVENT_BEAST, DONE);
			if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_FORDRING))))
				Speak(CHAT_TYPE_SAY,16041,"La menagerie monstrueuse est vaincue !",tmp);
			if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_ANNOUNCER))))
				tmp->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		}
		switch(m_bIsHeroic)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(CONQUETE,2);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
		}
		me->ForcedDespawn(TEN_MINS*1000);
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			if(pInstance)
				pInstance->SetData(TYPE_TRY,1);
	}

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (pInstance)
            pInstance->SetData(TYPE_Icehowl, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(phase_Timer <= diff)
		{
			switch(phase)
			{
				case 1:
					me->CastStop();
					Relocate(563.723f,141.723f,393.9f);
					DoCastVictim(SPELL_MASSIVE_CRASH);
					phase_Timer = 6000;
					phase++;
					inCrash = true;
					break;
				case 2:
				{
					std::string txt = "Glace-hurlante regarde ";
					
					if(target = SelectUnit(SELECT_TARGET_RANDOM,0))
					{
						txt += std::string(target->GetName());
						Speak(CHAT_TYPE_BOSS_EMOTE,0,txt);
						me->SetFacingToObject(target);
						x = target->GetPositionX();
						y = target->GetPositionY();
						z = target->GetPositionZ() + 0.3;
					}
					phase++;
					switch(m_bIsHeroic)
					{
						case RAID_DIFFICULTY_10MAN_NORMAL:
						case RAID_DIFFICULTY_25MAN_NORMAL:
						{
							Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
							if (!lPlayers.isEmpty())
							{
								for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
									if (Player* pPlayer = itr->getSource())
										if(pPlayer->isAlive())
											ModifyAuraStack(SPELL_SURGE_OF_ADRENALINE,1,pPlayer);
							}
							break;
						}
					}
					
					phase_Timer = 4000;
					break;
				}
				case 3:
					inCrash = false;
					if(target)
						me->GetMotionMaster()->MovePoint(0,x,y,z);
					phase++;
					phase_Timer = 2500;
					break;
				case 4:
					DoCastMe(SPELL_TRAMPLE);
					Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
					bool PlayerHit = false;
					if (!lPlayers.isEmpty())
					{
						for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
						{
							if (Player* pPlayer = itr->getSource())
								if(pPlayer->isAlive() && !pPlayer->isGameMaster() && pPlayer->GetDistance2d(me) < 14.0f)
								{
									Kill(pPlayer);
									PlayerHit = true;
								}
						}
					}
					if(PlayerHit)
						ModifyAuraStack(SPELL_FROTHING_RAGE);
					else
						ModifyAuraStack(SPELL_STAGGERED_DAZE);
					phase_Timer = 60000;
					phase = 1;
					break;
			}
		}
		else
			phase_Timer -= diff;

		if(!me->HasAura(SPELL_STAGGERED_DAZE))
		{
			if(CheckDistanceTimer < diff)
			{
				if (inCrash)
				{
					me->GetMotionMaster()->MoveIdle();
					me->StopMoving();
				}
				CheckDistanceTimer = 100;
			}
			else
				CheckDistanceTimer -= diff;
		}
		Tasks.UpdateEvent(diff,phase);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_Icehowl(Creature* pCreature)
{
    return new boss_Icehowl_AI(pCreature);
}


struct MANGOS_DLL_DECL boss_jorm_flaqueAI : public LibDevFSAI
{
    boss_jorm_flaqueAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
    }

	uint32 poison_Timer;
	uint8 poisonForce;

	void DamageDeal(Unit* pwho, uint32 &dmg)
	{
		if(pwho->GetMapId() == me->GetMapId())
			if(me->GetDistance2d(pwho) > poisonForce)
				dmg = 0;
	}

    void Reset()
    {
		SetCombatMovement(false);
		
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
		ModifyAuraStack(30914);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_NORMAL:
				me->ForcedDespawn(30000);
				AddEventOnTank(67638,500,1000);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				me->ForcedDespawn(45000);
				AddEventOnTank(67639,500,1000);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				me->ForcedDespawn(60000);
				AddEventOnTank(67640,500,1000);
				break; 
		}
		poisonForce = 1;
		me->SetPhaseMask(1,true);
		poison_Timer = 1000;
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void UpdateAI(const uint32 diff)
    {
		if(poison_Timer <= diff)
		{
			if(poisonForce <= 30)
				poisonForce++;
			poison_Timer = 1000;
		}
		else
			poison_Timer -= diff;

		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_jorm_flaque(Creature* pCreature)
{
    return new boss_jorm_flaqueAI (pCreature);
}

struct MANGOS_DLL_DECL boss_gormok_flameAI : public LibDevFSAI
{
    boss_gormok_flameAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
    }

    void Reset()
    {
		ModifyAuraStack(66318);
		SetCombatMovement(false);
		MakeHostileInvisibleStalker();
		me->SetPhaseMask(1,true);
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_gormok_flame(Creature* pCreature)
{
    return new boss_gormok_flameAI (pCreature);
}

void AddSC_boss_EDCBeasts()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gormok";
    newscript->GetAI = &GetAI_boss_GormoktheImpaler;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_gormok_add";
    newscript->GetAI = &GetAI_add_GormoktheImpaler;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_acidmaw";
    newscript->GetAI = &GetAI_boss_Acidmaw;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_dreadscale";
    newscript->GetAI = &GetAI_boss_Dreadscale;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "jorm_flaque";
    newscript->GetAI = &GetAI_boss_jorm_flaque;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "gormok_flame";
    newscript->GetAI = &GetAI_boss_gormok_flame;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_icehowl";
    newscript->GetAI = &GetAI_boss_Icehowl;
    newscript->RegisterSelf();
}
