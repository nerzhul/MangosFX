#include "precompiled.h"
#include "trial_of_the_crusader.h"
enum 
{
	SPELL_BERSERK = 64238,
	SPELL_LIGHT_VORTEX_10 = 66046,
	SPELL_LIGHT_VORTEX_10_H = 67206,
	SPELL_LIGHT_VORTEX_25 = 67207,
	SPELL_LIGHT_VORTEX_25_H = 67208,
	SPELL_SPEED_OF_THE_VALKYR = 67319,
	SPELL_POWER_OF_THE_TWINS_10 = 65916,
	SPELL_POWER_OF_THE_TWINS_10_H = 67248,
	SPELL_POWER_OF_THE_TWINS_25 = 67249,
	SPELL_POWER_OF_THE_TWINS_25_H = 67250,
	SPELL_SHIELD_OF_LIGHTS_10 = 65858,
	SPELL_SHIELD_OF_LIGHTS_10_H = 67260,
	SPELL_SHIELD_OF_LIGHTS_25 = 67259,
	SPELL_SHIELD_OF_LIGHTS_25_H = 67261,
	SPELL_SURGE_OF_LIGHT_10 = 65766,
	SPELL_SURGE_OF_LIGHT_10_H = 67270,
	SPELL_SURGE_OF_LIGHT_25 = 67271,
	SPELL_SURGE_OF_LIGHT_25_H = 67272,
	SPELL_TOUCH_OF_LIGHT_10 = 67297,
	SPELL_TOUCH_OF_LIGHT_25 = 67298,
	SPELL_TWIN_SPIKE_10 = 66075,
	SPELL_TWIN_SPIKE_10_H = 67312,
	SPELL_TWIN_SPIKE_25 = 67313,
	SPELL_TWIN_SPIKE_25_H = 67314,
	SPELL_TWINS_PACT2_10 = 65876,
	SPELL_TWINS_PACT2_10_H = 67306,
	SPELL_TWINS_PACT2_25 = 67307,
	SPELL_TWINS_PACT2_25_H = 67308,
};

enum
{
	SPELL_BERSERK_10 = 64238,
	SPELL_BERSERK_25 = 68378,
	SPELL_DARK_VORTEX_10 = 66058,
	SPELL_DARK_VORTEX_10_H = 67182,
	SPELL_DARK_VORTEX_25 = 67183,
	SPELL_DARK_VORTEX_25_H = 67184,
	/*SPELL_POWER_OF_THE_TWINS_10 = 65879,
	SPELL_POWER_OF_THE_TWINS_10_H = 67244,
	SPELL_POWER_OF_THE_TWINS_25 = 67245,
	SPELL_POWER_OF_THE_TWINS_25_H = 67246,*/
	SPELL_SHIELD_OF_DARKNESS_10 = 65874,
	SPELL_SHIELD_OF_DARKNESS_10_H = 67257,
	SPELL_SHIELD_OF_DARKNESS_25 = 67256,
	SPELL_SHIELD_OF_DARKNESS_25_H = 67258,
	SPELL_SURGE_OF_DARKNESS_10 = 65768,
	SPELL_SURGE_OF_DARKNESS_10_H = 67262,
	SPELL_SURGE_OF_DARKNESS_25 = 67263,
	SPELL_SURGE_OF_DARKNESS_25_H = 67264,
	SPELL_TOUCH_OF_DARKNESS_10 = 67282,
	SPELL_TOUCH_OF_DARKNESS_25 = 67283,
	SPELL_TWIN_SPIKE2_10 = 66069,
	SPELL_TWIN_SPIKE2_10_H = 67309,
	SPELL_TWIN_SPIKE2_25 = 67310,
	SPELL_TWIN_SPIKE2_25_H = 37311,
	SPELL_TWINS_PACT_10 = 65875,
	SPELL_TWINS_PACT_10_H = 67303,
	SPELL_TWINS_PACT_25 = 67304,
	SPELL_TWINS_PACT_25_H = 67305,
};

struct MANGOS_DLL_DECL boss_Eydis_DarkbaneAI : public ScriptedAI
{
    boss_Eydis_DarkbaneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		difficulty = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;
	Difficulty difficulty;
	bool HealEvent;
	uint32 HealCast_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddSummonEvent(34628,8000,2000,0,2000,1,TEN_MINS,NEAR_30M);
		HealEvent = false;
		HealCast_Timer = 1000;
		switch(difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				DoCastMe(SPELL_SURGE_OF_DARKNESS_10);
				Tasks.AddEvent(SPELL_BERSERK_10,360000,10000,0,TARGET_ME,0,0,true);
				Tasks.AddEvent(SPELL_TWIN_SPIKE2_10,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_10,120000,8000,20000,TARGET_ME);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				DoCastMe(SPELL_SURGE_OF_DARKNESS_25);
				Tasks.AddEvent(SPELL_BERSERK_25,360000,10000,0,TARGET_ME,0,0,true);
				Tasks.AddEvent(SPELL_TWIN_SPIKE2_25,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_25,120000,8000,20000,TARGET_ME);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				DoCastMe(SPELL_SURGE_OF_DARKNESS_10_H);
				Tasks.AddEvent(SPELL_BERSERK_10,360000,10000,0,TARGET_ME,0,0,true);
				Tasks.AddEvent(SPELL_TOUCH_OF_DARKNESS_10,20000,20000);
				Tasks.AddEvent(SPELL_TWIN_SPIKE2_10_H,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_10_H,120000,8000,20000,TARGET_ME);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				DoCastMe(SPELL_SURGE_OF_DARKNESS_25_H);
				Tasks.AddEvent(SPELL_BERSERK_25,360000,10000,0,TARGET_ME,0,0,true);
				Tasks.AddEvent(SPELL_TOUCH_OF_DARKNESS_25,20000,20000);
				Tasks.AddEvent(SPELL_TWIN_SPIKE2_25_H,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_25_H,120000,8000,20000,TARGET_ME);
				break;
		}
		SetEquipmentSlots(false, 45990, 47470, 47267);
    }

	void DamageDeal(Unit *pDoneTo, uint32 &uiDamage)
	{
		if(pDoneTo->GetTypeId() == TYPEID_UNIT)
			if(pDoneTo->GetEntry() == 34496 || pDoneTo->GetEntry() == 34497)
				uiDamage = 0;
	}

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if (Creature* Fjola = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Fjola_Lightbane) : 0)))
			if(Fjola->isAlive())
				pDoneBy->DealDamage(Fjola,dmg,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

	void HealBy(Unit* pHealer,uint32 heal)
	{
		if (Creature* Fjola = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Fjola_Lightbane) : 0)))
			if(Fjola->isAlive())
				me->DealHeal(Fjola,heal, /* TODO : spell proto */ NULL);
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_Eydis_Darkbane, DONE);
            
		if (Creature* Fjola = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Fjola_Lightbane) : 0)))
			if(!Fjola->isAlive())
			{
				m_pInstance->SetData(TYPE_VALKYRS,DONE);
				if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
					((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_VALKYR_OUTRO);
			}

		switch(difficulty)
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
            m_pInstance->SetData(TYPE_Eydis_Darkbane, IN_PROGRESS);
    }

	void DoEvent()
	{
		me->CastStop();
		if(urand(0,1))
		{
			switch(difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					DoCastVictim(SPELL_DARK_VORTEX_10);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
					DoCastVictim(SPELL_DARK_VORTEX_25);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
					DoCastVictim(SPELL_DARK_VORTEX_10_H);
					break;
				case RAID_DIFFICULTY_25MAN_HEROIC:
					DoCastVictim(SPELL_DARK_VORTEX_25_H);
					break;
			}
		}
		else
		{
			HealEvent = true;
			switch(difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					DoCastMe(SPELL_SHIELD_OF_DARKNESS_10);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
					DoCastMe(SPELL_SHIELD_OF_DARKNESS_25);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
					DoCastMe(SPELL_SHIELD_OF_DARKNESS_10_H);
					break;
				case RAID_DIFFICULTY_25MAN_HEROIC:
					DoCastMe(SPELL_SHIELD_OF_DARKNESS_25_H);
					break;
			}
		}
	}

    void UpdateAI(const uint32 diff)
    {
		if (!Tasks.CanDoSomething())
            return;

		if(HealEvent)
		{
			if(HealCast_Timer <= diff)
			{
				switch(difficulty)
				{
					case RAID_DIFFICULTY_10MAN_NORMAL:
						DoCastMe(SPELL_TWINS_PACT_10);
						break;
					case RAID_DIFFICULTY_25MAN_NORMAL:
						DoCastMe(SPELL_TWINS_PACT_25);
						break;
					case RAID_DIFFICULTY_10MAN_HEROIC:
						DoCastMe(SPELL_TWINS_PACT_10_H);
						break;
					case RAID_DIFFICULTY_25MAN_HEROIC:
						DoCastMe(SPELL_TWINS_PACT_25_H);
						break;
				}
				HealEvent = false;
				HealCast_Timer = 1000;
			}
			else
				HealCast_Timer -= diff;
		}

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_Eydis_Darkbane(Creature* pCreature)
{
    return new boss_Eydis_DarkbaneAI(pCreature);
}

struct MANGOS_DLL_DECL boss_Fjola_LightbaneAI : public ScriptedAI
{
    boss_Fjola_LightbaneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		difficulty = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;
	Difficulty difficulty;
	bool HealEvent;
	uint32 HealCast_Timer;

	uint32 Event_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddSummonEvent(34630,8000,2000,0,2000,1,TEN_MINS,NEAR_30M);
		HealEvent = false;
		HealCast_Timer = 1000;
		Tasks.AddEvent(SPELL_BERSERK,360000,10000,0,TARGET_ME,0,0,true);
		switch(difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				DoCastMe(SPELL_SURGE_OF_LIGHT_10);
				Tasks.AddEvent(SPELL_TWIN_SPIKE_10,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_10,120000,8000,20000,TARGET_ME);
				Event_Timer = 40000;
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				DoCastMe(SPELL_SURGE_OF_LIGHT_25);
				Tasks.AddEvent(SPELL_TWIN_SPIKE_25,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_25,120000,8000,20000,TARGET_ME);
				Event_Timer = 40000;
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				DoCastMe(SPELL_SURGE_OF_LIGHT_10_H);
				Tasks.AddEvent(SPELL_TOUCH_OF_LIGHT_10,20000,20000);
				Tasks.AddEvent(SPELL_TWIN_SPIKE_10_H,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_10_H,120000,8000,20000,TARGET_ME);
				Event_Timer = 30000;
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				DoCastMe(SPELL_SURGE_OF_LIGHT_25_H);
				Tasks.AddEvent(SPELL_TOUCH_OF_LIGHT_25,20000,20000);
				Tasks.AddEvent(SPELL_TWIN_SPIKE_25_H,18000,21000,1000,TARGET_MAIN);
				if(urand(0,1))
					Tasks.AddEvent(SPELL_SPEED_OF_THE_VALKYR,120000,8000,20000,TARGET_ME);
				else
					Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS_25_H,120000,8000,20000,TARGET_ME);
				Event_Timer = 30000;
				break;
		}
		SetEquipmentSlots(false, 49303, 47146, 47267);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if (Creature* Eydis = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Eydis_Darkbane) : 0)))
			if(Eydis->isAlive())
			{
				pDoneBy->DealDamage(Eydis,dmg,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
					((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_VALKYR_OUTRO);
			}
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_Fjola_Lightbane, DONE);
            
		if (Creature* Eydis = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Eydis_Darkbane) : 0)))
			if(!Eydis->isAlive())		
				m_pInstance->SetData(TYPE_VALKYRS,DONE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_Fjola_Lightbane, IN_PROGRESS);
    }

	void DamageDeal(Unit *pDoneTo, uint32 &uiDamage)
	{
		if(pDoneTo->GetTypeId() == TYPEID_UNIT)
			if(pDoneTo->GetEntry() == 34496 || pDoneTo->GetEntry() == 34497)
				uiDamage = 0;
	}

	void HealBy(Unit* pHealer,uint32 heal)
	{
		if (Creature* Eydis = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Eydis_Darkbane) : 0)))
			if(Eydis->isAlive())		
				me->DealHeal(Eydis,heal, /* TODO : spell proto */ NULL);
	}

	void DoEvent()
	{
		me->CastStop();
		if(urand(0,1))
		{
			switch(difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					DoCastVictim(SPELL_LIGHT_VORTEX_10);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
					DoCastVictim(SPELL_LIGHT_VORTEX_25);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
					DoCastVictim(SPELL_LIGHT_VORTEX_10_H);
					break;
				case RAID_DIFFICULTY_25MAN_HEROIC:
					DoCastVictim(SPELL_LIGHT_VORTEX_25_H);
					break;
			}
		}
		else
		{
			HealEvent = true;
			switch(difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					DoCastMe(SPELL_SHIELD_OF_LIGHTS_10);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
					DoCastMe(SPELL_SHIELD_OF_LIGHTS_25);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
					DoCastMe(SPELL_SHIELD_OF_LIGHTS_10_H);
					break;
				case RAID_DIFFICULTY_25MAN_HEROIC:
					DoCastMe(SPELL_SHIELD_OF_LIGHTS_25_H);
					break;
			}
		}
	}

    void UpdateAI(const uint32 diff)
    {
		if (!Tasks.CanDoSomething())
            return;

		if(HealEvent)
		{
			if(HealCast_Timer <= diff)
			{
				switch(difficulty)
				{
					case RAID_DIFFICULTY_10MAN_NORMAL:
						DoCastMe(SPELL_TWINS_PACT2_10);
						break;
					case RAID_DIFFICULTY_25MAN_NORMAL:
						DoCastMe(SPELL_TWINS_PACT2_25);
						break;
					case RAID_DIFFICULTY_10MAN_HEROIC:
						DoCastMe(SPELL_TWINS_PACT2_10_H);
						break;
					case RAID_DIFFICULTY_25MAN_HEROIC:
						DoCastMe(SPELL_TWINS_PACT2_25_H);
						break;
				}
				HealEvent = false;
				HealCast_Timer = 1000;
			}
			else
				HealCast_Timer -= diff;
		}

		if(Event_Timer <= diff)
		{
			if(urand(0,1))
			{
				if (Creature* Eydis = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Eydis_Darkbane) : 0)))
					if(Eydis->isAlive())
						((boss_Eydis_DarkbaneAI*)Eydis->AI())->DoEvent();
			}
			else
				DoEvent();

			Event_Timer = 40000;
		}
		else
			Event_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_Fjola_Lightbane(Creature* pCreature)
{
    return new boss_Fjola_LightbaneAI(pCreature);
}

struct MANGOS_DLL_DECL Valkyr_BallAI : public ScriptedAI
{
    Valkyr_BallAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		difficulty = me->GetMap()->GetDifficulty();
    }

	MobEventTasks Tasks;
	uint32 move_Timer;
	uint32 checkpDist_Timer;
	ScriptedInstance* pInstance;
	Difficulty difficulty;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		SetCombatMovement(false);
		move_Timer = 15000;
		checkpDist_Timer = 1000;
    }

	void CheckDist()
	{
		bool Event = false;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && !pPlayer->isGameMaster() && pPlayer->GetDistance2d(me) < 7.0f)
						Event = true;

		if (Creature* Darkbane = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Eydis_Darkbane))))
			if(Darkbane->isAlive() && Darkbane->GetDistance2d(me) < 7.0f)
				Event = true;
		if (Creature* Lightbane = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Fjola_Lightbane))))
			if(Lightbane->isAlive() && Lightbane->GetDistance2d(me) < 7.0f)
				Event = true;

		if(Event)
			DoEvent();
	}

	void DoEvent()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && !pPlayer->isGameMaster() && pPlayer->GetDistance2d(me) < 7.0f)
					{
						if(pPlayer->HasAura(65686))
						{
							if(me->GetEntry() == 34630)
								UpdateStacks(pPlayer);
						}
						else if(pPlayer->HasAura(65684))
						{
							if(me->GetEntry() == 34628)
								UpdateStacks(pPlayer);
						}
					}

		if (Creature* Darkbane = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Eydis_Darkbane))))
			if(Darkbane->isAlive() && Darkbane->GetDistance2d(me) < 7.0f)
				UpdateStacks(Darkbane);
		if (Creature* Lightbane = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Fjola_Lightbane))))
			if(Lightbane->isAlive() && Lightbane->GetDistance2d(me) < 7.0f)
				UpdateStacks(Lightbane);

		if(me->GetEntry() == 34630)
		{
			switch(difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					DoCastMe(65795);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
					DoCastMe(67238);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
					DoCastMe(67239);
					break;
				case RAID_DIFFICULTY_25MAN_HEROIC:
					DoCastMe(67240);
					break;
			}
		}
		else if(me->GetEntry() == 34628)
		{
			switch(difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					DoCastMe(67172);
					break;
				case RAID_DIFFICULTY_25MAN_NORMAL:
					DoCastMe(65808);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
					DoCastMe(67173);
					break;
				case RAID_DIFFICULTY_25MAN_HEROIC:
					DoCastMe(67174);
					break;
			}
		}

		me->RemoveFromWorld();
	}

	void UpdateStacks(Unit* u)
	{
		uint32 stk = 0;
		if(u->HasAura(67590))
			stk = u->GetAura(67590,0)->GetStackAmount();
		if((stk + 7) >= 100)
		{
			u->RemoveAurasDueToSpell(67590);
			if(urand(0,1))
				Tasks.SetAuraStack(65724,1,u,me,1);
			else
				Tasks.SetAuraStack(65748,1,u,me,1);
		}
		else
			Tasks.SetAuraStack(67590,stk + 7,u,me,1);
	}

    void UpdateAI(const uint32 diff)
    {
		if(move_Timer <= diff)
		{
			if(Unit* u = SelectUnit(SELECT_TARGET_RANDOM,0))
				me->GetMotionMaster()->MovePoint(0,u->GetPositionX(),u->GetPositionY(),u->GetPositionZ());
			move_Timer = 8000;
		}
		else
			move_Timer -= diff;

		if(checkpDist_Timer <= diff)
		{
			CheckDist();
			checkpDist_Timer = 1000;
		}
		else
			checkpDist_Timer -= diff;
    }

};

CreatureAI* GetAI_Valkyr_Ball(Creature* pCreature)
{
    return new Valkyr_BallAI(pCreature);
}

void AddSC_boss_twin_valkyr()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_Eydis_Darkbane";
    newscript->GetAI = &GetAI_boss_Eydis_Darkbane;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_Fjola_Lightbane";
    newscript->GetAI = &GetAI_boss_Fjola_Lightbane;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "valkyr_ball";
    newscript->GetAI = &GetAI_Valkyr_Ball;
    newscript->RegisterSelf();
}
