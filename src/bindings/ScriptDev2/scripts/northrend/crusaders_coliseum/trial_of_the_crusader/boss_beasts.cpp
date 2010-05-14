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
		CleanMyAdds();
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

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

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
				SetAuraStack(SPELL_RISING_ANGER,5 - Frigibold_count,me,me,1);
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

struct MANGOS_DLL_DECL boss_Acidmaw_AI : public ScriptedAI
{
    boss_Acidmaw_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();

        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;
	Difficulty m_bIsHeroic;
	uint8 phase;
	uint32 phase_Timer;
	uint32 CheckDistanceTimer;
	bool Spawnable;
	uint32 Spawn_Timer;

    void Reset()
    {		
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(26662,360000,10000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_ACID_SPIT,7000,9000,1000,TARGET_RANDOM,2);
		Tasks.AddNoTankEvent(SPELL_ACID_SPIT,1500);
		Tasks.AddEvent(SPELL_PARALYTIC_BITE,15000,15000,0,TARGET_MAIN,1);
		Tasks.AddEvent(SPELL_PARALYTIC_SPRAY,17000,15000,1000,TARGET_MAIN,2);
		Tasks.AddEvent(SPELL_SLIME_POOL,25000,25000,2000,TARGET_MAIN,1);
		Tasks.AddEvent(SPELL_SWEEP,14000,14000,1000,TARGET_MAIN,2);
		switch(m_bIsHeroic)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_ACIDIC_SPEW,6000,9000,1000,TARGET_MAIN,1);
				Spawnable = false;
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_ACIDIC_SPEW,6000,9000,1000,TARGET_MAIN,1);
				Spawnable = false;
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_ACIDIC_SPEW,6000,9000,1000,TARGET_MAIN,1);
				Spawnable = true;
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_ACIDIC_SPEW,6000,9000,1000,TARGET_MAIN,1);
				Spawnable = true;
				break;
		}
		phase = 1;
		phase_Timer = 10000;
		Spawn_Timer = 180000;
		CheckDistanceTimer = 1000;
		AggroAllPlayers(150.0f);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_Acidmaw, DONE);
		if (Creature* Dreadscale = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Dreadscale) : 0)))
		{
			if(Dreadscale->isAlive())
			{
				switch(m_bIsHeroic)
				{
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						SetAuraStack(SPELL_ENRAGE,1,Dreadscale,Dreadscale,1);
						break;
				}
			}
			else
			{
				switch(m_bIsHeroic)
				{
					case RAID_DIFFICULTY_10MAN_NORMAL:
					case RAID_DIFFICULTY_25MAN_NORMAL:
						m_pInstance->SetData(TYPE_Acidmaw, DONE);
						if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
							((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
						break;
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						if(Spawnable)
						{
							m_pInstance->SetData(TYPE_Acidmaw, DONE);
							if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
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

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

		if (Creature* Dreadscale = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Dreadscale) : 0)))
			if(Dreadscale->isAlive())
				Dreadscale->AddThreat(pWho);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_Acidmaw, IN_PROGRESS);
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

		Tasks.UpdateEvent(diff,phase);

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
				if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
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

struct MANGOS_DLL_DECL boss_Dreadscale_AI : public ScriptedAI
{
    boss_Dreadscale_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;
	Difficulty m_bIsHeroic;
	uint8 phase;
	uint32 phase_Timer;
	uint32 CheckDistanceTimer;
	bool Spawnable;
	uint32 Spawn_Timer;
	uint32 Check_Para_Timer;

    void Reset()
    {		
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(26662,360000,10000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_BURNING_BITE,9000,12000,1000,TARGET_MAIN,1);
		Tasks.AddEvent(SPELL_BURNING_SPRAY,17000,15000,1000,TARGET_MAIN,2);
		Tasks.AddEvent(SPELL_FIRE_SPIT,6000,9000,1000,TARGET_RANDOM,2);
		Tasks.AddNoTankEvent(SPELL_FIRE_SPIT,3000);
		Tasks.AddEvent(SPELL_MOLTEN_SPEW,15000,15000,1000,TARGET_MAIN,1);
		switch(m_bIsHeroic)
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
        if (m_pInstance)
            m_pInstance->SetData(TYPE_Dreadscale, DONE);
		if (Creature* Acidmaw = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Acidmaw) : 0)))
		{
			if(Acidmaw->isAlive())
			{
				switch(m_bIsHeroic)
				{
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						SetAuraStack(SPELL_ENRAGE,1,Acidmaw,Acidmaw,1);
						break;
				}
			}
			else
			{
				switch(m_bIsHeroic)
				{
					case RAID_DIFFICULTY_10MAN_NORMAL:
					case RAID_DIFFICULTY_25MAN_NORMAL:
						m_pInstance->SetData(TYPE_Dreadscale, DONE);
						if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
							((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_BEASTS_YETI);
						break;
					case RAID_DIFFICULTY_10MAN_HEROIC:
					case RAID_DIFFICULTY_25MAN_HEROIC:
						if(Spawnable)
						{
							m_pInstance->SetData(TYPE_Dreadscale, DONE);
							if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
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

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

		if (Creature* Acidmaw = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Acidmaw) : 0)))
			if(Acidmaw->isAlive())
				Acidmaw->AddThreat(pWho);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_Dreadscale, IN_PROGRESS);

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
							pPlayer->SetSpeedRate(MOVE_RUN,float(pPlayer->GetAura(SPELL_PARALYTIC_TOXIN,0)->GetAuraDuration()) / 60000,true);

							if(me->getVictim() && me->getVictim()->HasAura(SPELL_BURNING_BILE) && pPlayer->GetDistance2d(me->getVictim()) < 5.0f)
							{
								pPlayer->RemoveAurasDueToSpell(SPELL_PARALYTIC_TOXIN);
								pPlayer->RemoveAurasDueToSpell(66830);
							}
							else if(pPlayer->GetAura(SPELL_PARALYTIC_TOXIN,0)->GetAuraDuration() < 10000)
								SetAuraStack(66830,1,pPlayer,me,1);
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
				if (Creature* Acidmaw = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Acidmaw) : 0)))
					if(!Acidmaw->isAlive())
					{
						if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
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

		Tasks.UpdateEvent(diff,phase);

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
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();

        Reset();
    }

    ScriptedInstance* m_pInstance;
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
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_Icehowl, DONE);
			m_pInstance->SetData(TYPE_EVENT_BEAST, DONE);
			if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_FORDRING))))
				Speak(CHAT_TYPE_SAY,16041,"La menagerie monstrueuse est vaincue !",tmp);
			if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_ANNOUNCER))))
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
				GiveEmblemsToGroup(CONQUETE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
		}
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_Icehowl, IN_PROGRESS);
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
											SetAuraStack(SPELL_SURGE_OF_ADRENALINE,1,pPlayer,me,1);
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
								if(pPlayer->isAlive() && pPlayer->GetDistance2d(me) < 14.0f)
								{
									Kill(pPlayer);
									PlayerHit = true;
								}
						}
					}
					if(PlayerHit)
						SetAuraStack(SPELL_FROTHING_RAGE,1,me,me,1);
					else
						SetAuraStack(SPELL_STAGGERED_DAZE,1,me,me,1);
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


struct MANGOS_DLL_DECL boss_jorm_flaqueAI : public ScriptedAI
{
    boss_jorm_flaqueAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	Difficulty m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
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
		Tasks.SetObjects(this,me);
		SetCombatMovement(false);
		
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
		SetAuraStack(30914,1,me,me,1);
		switch(m_bIsHeroic)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_NORMAL:
				me->ForcedDespawn(30000);
				Tasks.AddEvent(67638,500,1000,0,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				me->ForcedDespawn(45000);
				Tasks.AddEvent(67639,500,1000,0,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				me->ForcedDespawn(60000);
				Tasks.AddEvent(67640,500,1000,0,TARGET_MAIN);
				break; 
		}
		poisonForce = 1;
		me->SetPhaseMask(1,true);
		poison_Timer = 1000;
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

		Tasks.UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_jorm_flaque(Creature* pCreature)
{
    return new boss_jorm_flaqueAI (pCreature);
}

struct MANGOS_DLL_DECL boss_gormok_flameAI : public ScriptedAI
{
    boss_gormok_flameAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
		Reset();
    }

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		SetAuraStack(66318,1,me,me,1);
		SetCombatMovement(false);
		
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
		me->setFaction(14);
		me->SetPhaseMask(1,true);
    }

    void UpdateAI(const uint32 diff)
    {
		Tasks.UpdateEvent(diff);
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
