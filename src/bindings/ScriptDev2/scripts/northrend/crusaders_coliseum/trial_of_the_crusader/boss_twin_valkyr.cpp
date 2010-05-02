#include "precompiled.h"
#include "trial_of_the_crusader.h"
enum 
{
	SPELL_BERSERK = 64238,
	SPELL_LIGHT_VORTEX = 66046,
	SPELL_POWER_OF_THE_TWINS = 65916,
	SPELL_SHIELD_OF_LIGHTS = 65858,
	SPELL_SURGE_OF_LIGHT = 65766,
	SPELL_TOUCH_OF_LIGHT = 65950,
	SPELL_TWIN_SPIKE = 66075,
	SPELL_TWINS_PACT2 = 65876,
};

enum 
{
	SPELL_BERSERK_10 = 64238,
	SPELL_BERSERK_25 = 68378,
	SPELL_DARK_VORTEX = 66058,
	SPELL_SHIELD_OF_DARKNESS = 65874,
	SPELL_SURGE_OF_DARKNESS = 65768,
	SPELL_TOUCH_OF_DARKNESS = 66001,
	SPELL_TWIN_SPIKE2 = 66069,
	SPELL_TWINS_PACT = 65875,
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
		Tasks.AddEvent(66107,8000,2000,500,TARGET_RANDOM);
		HealEvent = false;
		HealCast_Timer = 1000;
		Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS,120000,8000,20000,TARGET_ME);
		DoCastMe(SPELL_SURGE_OF_DARKNESS);
		Tasks.AddEvent(SPELL_TOUCH_OF_DARKNESS,20000,20000);
		Tasks.AddEvent(SPELL_TWIN_SPIKE2,18000,21000,1000,TARGET_MAIN);
		switch(difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_BERSERK_10,360000,10000,0,TARGET_ME,0,0,true);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_BERSERK_25,360000,10000,0,TARGET_ME,0,0,true);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_BERSERK_10,360000,10000,0,TARGET_ME,0,0,true);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_BERSERK_25,360000,10000,0,TARGET_ME,0,0,true);
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

				me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
				Fjola->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
			}
			else
				me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

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
			DoCastVictim(SPELL_DARK_VORTEX);
		else
		{
			HealEvent = true;
			DoCastMe(SPELL_SHIELD_OF_DARKNESS);
		}
	}

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(HealEvent)
		{
			if(HealCast_Timer <= diff)
			{
				DoCastMe(SPELL_TWINS_PACT);
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
		Tasks.AddEvent(66078,8000,2000,500,TARGET_RANDOM);
		HealEvent = false;
		HealCast_Timer = 1000;
		Tasks.AddEvent(SPELL_BERSERK,360000,10000,0,TARGET_ME,0,0,true);
		Tasks.AddEvent(SPELL_POWER_OF_THE_TWINS,120000,8000,20000,TARGET_ME);
		Tasks.AddEvent(SPELL_TOUCH_OF_LIGHT,20000,20000);
		DoCastMe(SPELL_SURGE_OF_LIGHT);
		Tasks.AddEvent(SPELL_TWIN_SPIKE,18000,21000,1000,TARGET_MAIN);
		switch(difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Event_Timer = 40000;
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Event_Timer = 30000;
				break;
		}
		switch(difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_BERSERK_10,360000,10000,0,TARGET_ME,0,0,true);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_BERSERK_25,360000,10000,0,TARGET_ME,0,0,true);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_BERSERK_10,360000,10000,0,TARGET_ME,0,0,true);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_BERSERK_25,360000,10000,0,TARGET_ME,0,0,true);
				break;
		}
		SetEquipmentSlots(false, 49303, 47146, 47267);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if (Creature* Eydis = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Eydis_Darkbane) : 0)))
			if(Eydis->isAlive())
				pDoneBy->DealDamage(Eydis,dmg,NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_Fjola_Lightbane, DONE);
            
		if (Creature* Eydis = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(TYPE_Eydis_Darkbane) : 0)))
			if(!Eydis->isAlive())
			{
				m_pInstance->SetData(TYPE_VALKYRS,DONE);
				if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, m_pInstance ? m_pInstance->GetData64(DATA_ANNOUNCER) : 0)))
					((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_VALKYR_OUTRO);
					
				me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
				Eydis->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
			}
			else
				me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
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
			DoCastVictim(SPELL_LIGHT_VORTEX);
		else
		{
			DoCastMe(SPELL_SHIELD_OF_LIGHTS);
			HealEvent = true;
		}
	}

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(HealEvent)
		{
			if(HealCast_Timer <= diff)
			{
				DoCastMe(SPELL_TWINS_PACT2);
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
			DoCastMe(65795);
		else if(me->GetEntry() == 34628)
			DoCastMe(65808);

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
				SetAuraStack(65724,1,u,me,1);
			else
				SetAuraStack(65748,1,u,me,1);
		}
		else
			SetAuraStack(67590,stk + 7,u,me,1);
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
