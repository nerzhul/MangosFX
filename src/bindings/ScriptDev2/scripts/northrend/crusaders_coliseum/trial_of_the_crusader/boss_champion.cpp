#include "precompiled.h"
#include "trial_of_the_crusader.h"
enum
{
	SPELL_BLADESTORM = 65946,
	SPELL_BLADESTORM2 = 65947,
	SPELL_CHARGE = 65927,
	SPELL_DISARM = 65935,
	SPELL_INTIMIDATING_SHOUT = 65930,
	SPELL_MORTAL_STRIKE = 65926,
	SPELL_OVERPOWER = 65924,
	SPELL_PVP_TRINKET = 65547,
	SPELL_RETALIATION = 65932,
	SPELL_SHATTERING_THROW = 65940,
	SPELL_SUNDER_ARMOR = 65936,
};

struct MANGOS_DLL_DECL boss_championWarAI : public ScriptedAI
{
    boss_championWarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Tasks.AddEvent(SPELL_DISARM,25000,60000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_INTIMIDATING_SHOUT,15000,120000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_RETALIATION,180000,300000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_PVP_TRINKET,90000,120000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_SHATTERING_THROW,30000,300000);
		Tasks.AddEvent(SPELL_SUNDER_ARMOR,4000,1200,6000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_CHARGE,1000,15000,0);
		Tasks.AddEvent(SPELL_MORTAL_STRIKE,3000,6000,0,TARGET_MAIN);
    }
    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONWAR, DONE);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONWAR, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_championWar(Creature* pCreature)
{
    return new boss_championWarAI(pCreature);
}
enum druidbalancespells
{
	SPELL_BARKSKIN = 65860,
	SPELL_CYCLONE = 65859,
	SPELL_ENTANGLING_ROOTS = 65857,
	SPELL_FAERIE_FIRE = 65863,
	SPELL_FORCE_OF_NATURE = 65861,
	SPELL_INSECT_SWARM = 65855,
	SPELL_MOONFIRE = 65856,
	SPELL_STARFIRE = 65854,
	SPELL_WRATH = 65862,
};
struct MANGOS_DLL_DECL boss_championDruidBalanceAI : public ScriptedAI
{
    boss_championDruidBalanceAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Tasks.AddEvent(SPELL_BARKSKIN,4000,60000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_CYCLONE,20000,60000);
		Tasks.AddEvent(SPELL_ENTANGLING_ROOTS,15000,60000);
		Tasks.AddEvent(SPELL_FAERIE_FIRE,1000,6000);
		Tasks.AddEvent(SPELL_FORCE_OF_NATURE,45000,180000);
		Tasks.AddEvent(SPELL_PVP_TRINKET,180000,120000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_INSECT_SWARM,2000,18000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_MOONFIRE,3000,9000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_STARFIRE,5000,9000,2500,TARGET_MAIN);
		Tasks.AddEvent(SPELL_WRATH,200,9000,2500,TARGET_MAIN);
		Mana_Timer = 200;
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONDRUID, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONDRUID, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }
};

CreatureAI* GetAI_boss_championDruidBalance(Creature* pCreature)
{
    return new boss_championDruidBalanceAI(pCreature);
}

enum druidrestospells
{
	SPELL_LIFEBLOOM = 66093,
	SPELL_NATURES_GRASP = 66071,
	SPELL_NOURISH = 66066,
	SPELL_REGROWTH = 66067,
	SPELL_REJUVENATION = 66065,
	SPELL_THORNS = 66068,
	SPELL_TRANQUILITY = 66086,
};
struct MANGOS_DLL_DECL boss_championDruidRestoAI : public ScriptedAI
{
    boss_championDruidRestoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_BARKSKIN,2000,60000,10000,TARGET_ME);
		Tasks.AddEvent(SPELL_THORNS,500,600000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_PVP_TRINKET,65000,180000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_NATURES_GRASP,3000,60000,10000,TARGET_ME);
		Tasks.AddEvent(SPELL_LIFEBLOOM,1000,2000,1000,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_NOURISH,1500,3500,1500,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_REGROWTH,5000,10000,2500,HEAL_MY_FRIEND,0,0,true);
		Tasks.AddEvent(SPELL_REJUVENATION,3500,3500,1500,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_TRANQUILITY,120000,600000,0,TARGET_ME);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONDRUID, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONDRUID, IN_PROGRESS);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_championDruidResto(Creature* pCreature)
{
    return new boss_championDruidRestoAI(pCreature);
}

enum hunt
{
	SPELL_AIMED_SHOT = 65883,
	SPELL_CALL_PET = 67777,
	SPELL_DETERRENCE = 65871,
	SPELL_DISENGAGE = 65869,
	SPELL_EXPLOSIVE_SHOT = 65866,
	SPELL_FROST_TRAP = 65880,
	SPELL_SHOOT = 65868,
	SPELL_STEADY_SHOT = 65867,
	SPELL_WING_CLIP = 66207,
	SPELL_WYVERN_STING = 65877,
	SPELL_VIPER_STING = 65881,
};
enum huntpetcat
{
	SPELL_CLAW	 = 67793,
};
struct MANGOS_DLL_DECL boss_championHuntAI : public ScriptedAI
{
    boss_championHuntAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_DETERRENCE,35000,905000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_DISENGAGE,20000,35000,20000,TARGET_ME);
		Tasks.AddEvent(SPELL_FROST_TRAP,17000,30000,40000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_PVP_TRINKET,120000,180000,5000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_STEADY_SHOT,10000,12000,2000,TARGET_RANDOM,0,0,true);
		Tasks.AddEvent(SPELL_WING_CLIP,6000,12000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_WYVERN_STING,11000,60000,10000,TARGET_RANDOM,0,0,true);
		Tasks.AddEvent(SPELL_AIMED_SHOT,3000,11000,2000);
		Tasks.AddEvent(SPELL_EXPLOSIVE_SHOT,2000,6000,1000);
		Tasks.AddEvent(SPELL_SHOOT,100,5000,1200);
		Tasks.AddEvent(SPELL_VIPER_STING,8000,12500,500,TARGET_HAS_MANA,0,0,true);
		DoCastMe(SPELL_CALL_PET);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONHUNT, DONE);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONHUNT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championHunt(Creature* pCreature)
{
    return new boss_championHuntAI(pCreature);
}
enum
{
	SPELL_BLADE_FLURRY = 65956,
	SPELL_BLIND = 65960,
	SPELL_CLOAK_OF_SHADOWS = 65961,
	SPELL_EVISCERATE = 65957,
	SPELL_FAN_OF_KNIVES = 65955,
	SPELL_HEMORRHAGE = 65954,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_SHADOWSTEP = 66178,
};
struct MANGOS_DLL_DECL boss_championRogueAI : public ScriptedAI
{
    boss_championRogueAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Tasks.AddEvent(SPELL_BLADE_FLURRY,1000,120000,10000,TARGET_ME);
		Tasks.AddEvent(SPELL_BLIND,10000,120000,10000,TARGET_RANDOM,0,0,true);
		Tasks.AddEvent(SPELL_CLOAK_OF_SHADOWS,30000,180000,5000,TARGET_ME,0,0,true);
		Tasks.AddEvent(SPELL_HEMORRHAGE,500,1500,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_EVISCERATE,12000,10000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_FAN_OF_KNIVES,6000,10000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_SHADOWSTEP,200,30000,5000);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONROGUE, DONE);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONROGUE, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_championRogue(Creature* pCreature)
{
    return new boss_championRogueAI(pCreature);
}
enum warlockk
{
	SPELL_CORRUPTION = 65810,
	SPELL_CURSE_OF_AGONY = 65814,
	SPELL_CURSE_OF_EXHAUSTION = 65815,
	SPELL_DEATH_COIL_DEMO = 65820,
	SPELL_FEAR = 65809,
	SPELL_HELLFIRE = 65816,
	SPELL_SEARING_PAIN = 65819,
	SPELL_SHADOW_BOLT = 65821,
	SPELL_SUMMON_FELHUNTER = 67514,
	SPELL_UNSTABLE_AFFLICTION = 65812,
};

enum warlockdemon
{
	SPELL_DEVOUR_MAGIC = 67518,
	SPELL_SPELL_LOCK = 67519,
};

struct MANGOS_DLL_DECL boss_championWarlockAI : public ScriptedAI
{
    boss_championWarlockAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_CURSE_OF_EXHAUSTION,30000,60000,10000);
		Tasks.AddEvent(SPELL_FEAR,12000,30000,2000);
		Tasks.AddEvent(SPELL_PVP_TRINKET,120000,120000,5000,TARGET_ME);
		Tasks.AddEvent(SPELL_CORRUPTION,5000,12000,4000);
		Tasks.AddEvent(SPELL_CURSE_OF_AGONY,1000,10000,4000);
		Tasks.AddEvent(SPELL_DEATH_COIL_DEMO,50000,120000,0,TARGET_RANDOM,0,0,true);
		Tasks.AddEvent(SPELL_HELLFIRE,60000,62000,10000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_SEARING_PAIN,2000,8000,9000);
		Tasks.AddEvent(SPELL_SHADOW_BOLT,6000,10000,1000);

		DoCastMe(SPELL_SUMMON_FELHUNTER);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONWARLOCK, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONWARLOCK, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championWarlock(Creature* pCreature)
{
    return new boss_championWarlockAI(pCreature);
}

enum shamanenhanspells
{
	SPELL_BLOODLUST = 65980,
	SPELL_EARTH_SHOCK = 65973,
	SPELL_LAVA_LASH = 65974,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_STORMSTRIKE = 65970,
};

struct MANGOS_DLL_DECL boss_championShamanEnhancAI : public ScriptedAI
{
    boss_championShamanEnhancAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		DoCastMe(65978);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_BLOODLUST,60000,600000,10000,TARGET_ME,0,0,true);
		Tasks.AddEvent(SPELL_LAVA_LASH,7000,7000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_PVP_TRINKET,120000,120000,6000,TARGET_ME);
		Tasks.AddEvent(SPELL_STORMSTRIKE,3000,6000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_EARTH_SHOCK,8000,10000,1000,TARGET_MAIN);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONSHAMAN, DONE);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONSHAMAN, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championShamanEnhanc(Creature* pCreature)
{
    return new boss_championShamanEnhancAI(pCreature);
}
enum shamanrestospells
{
	//SPELL_BLOODLUST = 65980,
	SPELL_CLEANSE_SPIRIT = 66056,
	SPELL_EARTH_SHIELD = 66063,
	SPELL_HEX = 66054,
	SPELL_LESSER_HEALING_WAVE = 66055,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_RIPTIDE = 66053,
};
struct MANGOS_DLL_DECL boss_championShamanRestoAI : public ScriptedAI
{
    boss_championShamanRestoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_BLOODLUST,30000,600000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_HEX,15000,60000,15000,TARGET_RANDOM,0,0,true);
		Tasks.AddEvent(SPELL_EARTH_SHOCK,10000,10000,5000);
		Tasks.AddEvent(SPELL_LESSER_HEALING_WAVE,1000,5000,0,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_RIPTIDE,3000,10000,0,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_CLEANSE_SPIRIT,4000,6000,3000,HEAL_MY_FRIEND);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONSHAMAN, DONE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONSHAMAN, IN_PROGRESS);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championShamanResto(Creature* pCreature)
{
    return new boss_championShamanRestoAI(pCreature);
}
enum palaholy
{
	SPELL_CLEANSE = 66116,
	SPELL_DIVINE_SHIELD = 66010,
	SPELL_FLASH_OF_LIGHT = 66113,
	SPELL_HAMMER_OF_JUSTICE = 66613,
	SPELL_HAND_OF_FREEDOM = 66115,
	SPELL_HAND_OF_PROTECTION = 66009,
	SPELL_HOLY_LIGHT = 66112,
	SPELL_HOLY_SHOCK = 66114,
	//SPELL_PVP_TRINKET = 65547,
};

struct MANGOS_DLL_DECL boss_championPaladinHolyAI : public ScriptedAI
{
    boss_championPaladinHolyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_DIVINE_SHIELD,60000,300000,30000,TARGET_ME,0,0,true);
		Tasks.AddEvent(SPELL_HAMMER_OF_JUSTICE,10000,40000,20000,TARGET_LOW_HP);
		Tasks.AddEvent(SPELL_PVP_TRINKET,90000,600000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_FLASH_OF_LIGHT,3000,2000,2000,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_HOLY_LIGHT,12000,5000,1000,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_HOLY_SHOCK,1000,7000,15000);
		Tasks.AddEvent(SPELL_CLEANSE,2000,7000,1500,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_HAND_OF_FREEDOM,50000,180000,0,HEAL_MY_FRIEND,0,0,true);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPALADIN, DONE);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPALADIN, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_championPaladinHoly(Creature* pCreature)
{
    return new boss_championPaladinHolyAI(pCreature);
}
enum palaret
{
	SPELL_AVENGING_WRATH = 66011,
	SPELL_CRUSADER_STRIKE = 66003,
	SPELL_DIVINE_STORM = 66006,
	SPELL_JUDGEMENT_OF_COMMAND = 66005,
	SPELL_REPENTANCE = 66008,
	SPELL_SEAL_OF_COMMAND = 66004,
};

struct MANGOS_DLL_DECL boss_championPaladinRetribAI : public ScriptedAI
{
    boss_championPaladinRetribAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_AVENGING_WRATH,32000,180000,10000,TARGET_ME,0,0,true);
		Tasks.AddEvent(SPELL_CRUSADER_STRIKE,1000,6000,7000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_DIVINE_SHIELD,35000,180000,20000,TARGET_ME);
		Tasks.AddEvent(SPELL_HAMMER_OF_JUSTICE,12000,40000,20000,TARGET_LOW_HP);
		Tasks.AddEvent(SPELL_PVP_TRINKET,60000,180000,20000,TARGET_ME);
		Tasks.AddEvent(SPELL_REPENTANCE,40000,60000,15000,TARGET_LOW_HP);
		Tasks.AddEvent(SPELL_HAND_OF_PROTECTION,30000,180000,0,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_JUDGEMENT_OF_COMMAND,2000,8000,9000,TARGET_MAIN);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPALADIN, DONE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPALADIN, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championPaladinRetrib(Creature* pCreature)
{
    return new boss_championPaladinRetribAI(pCreature);
}
enum
{
	SPELL_DISPEL_MAGIC = 65546,
	SPELL_FLASH_HEAL = 66104,
	SPELL_MANA_BURN = 66100,
	SPELL_PENANCE = 66098,
	SPELL_POWER_WORD_SHIELD = 66099,
	SPELL_PSYCHIC_SCREAM = 65543,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_RENEW = 66177,
};

struct MANGOS_DLL_DECL boss_championPriestDisciAI : public ScriptedAI
{
    boss_championPriestDisciAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_PSYCHIC_SCREAM,12000,30000,30000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_PVP_TRINKET,60000,1800000,60000,TARGET_ME);
		Tasks.AddEvent(SPELL_FLASH_HEAL,1000,5000,1000,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_MANA_BURN,5000,10000,2000,TARGET_HAS_MANA);
		Tasks.AddEvent(SPELL_PENANCE,12000,1000,HEAL_MY_FRIEND);
		Tasks.AddEvent(SPELL_POWER_WORD_SHIELD,10000,12000,1,HEAL_MY_FRIEND,0,0,true);
		Tasks.AddEvent(SPELL_RENEW,17000,15000,0,HEAL_MY_FRIEND,0,0,true);
		Tasks.AddEvent(SPELL_DISPEL_MAGIC,60000,180000);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPRIEST, DONE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPRIEST, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championPriestDisci(Creature* pCreature)
{
    return new boss_championPriestDisciAI(pCreature);
}
enum
{
	SPELL_DISPERSION = 65544,
	SPELL_MIND_BLAST = 65492,
	SPELL_MIND_FLAY = 65488,
	SPELL_PSYCHIC_HORROR = 65545,
	SPELL_SHADOW_WORD_PAIN = 65541,
	SPELL_SILENCE = 65542,
	SPELL_VAMPIRIC_TOUCH = 65490,
};

struct MANGOS_DLL_DECL boss_championPriestShadowAI : public ScriptedAI
{
    boss_championPriestShadowAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_DISPERSION,50000,180000,15000,TARGET_ME);
		Tasks.AddEvent(SPELL_PSYCHIC_HORROR,30000,120000,15000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_PSYCHIC_SCREAM,20000,40000,20000);
		Tasks.AddEvent(SPELL_PVP_TRINKET,90000,180000,60000,TARGET_ME);
		Tasks.AddEvent(SPELL_SILENCE,15000,45000,15000);
		Tasks.AddEvent(SPELL_MIND_BLAST,1500,8000,1000);
		Tasks.AddEvent(SPELL_MIND_FLAY,6000,10000,1000);
		Tasks.AddEvent(SPELL_SHADOW_WORD_PAIN,2000,6000,3000);
		Tasks.AddEvent(SPELL_VAMPIRIC_TOUCH,15000,15000,15000);
		Tasks.AddEvent(SPELL_DISPEL_MAGIC,60000,1800000);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPRIEST, DONE);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONPRIEST, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
		
		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 200;
		}
		else
			Mana_Timer -= diff;
		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championPriestShadow(Creature* pCreature)
{
    return new boss_championPriestShadowAI(pCreature);
}
enum dkspells
{
	SPELL_CHAINS_OF_ICE = 66020,
	SPELL_DEATH_COIL = 66019,
	SPELL_FROST_FEAVER = 67767,
	SPELL_DEATH_GRIP = 66017,
	SPELL_FROST_STRIKE = 66047,
	SPELL_ICEBOUND_FORTITUDE = 66023,
	SPELL_ICY_TOUCH = 66021,
	SPELL_STRANGULATE = 66018,
};

struct MANGOS_DLL_DECL boss_championDeathKnightAI : public ScriptedAI
{
    boss_championDeathKnightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Tasks.AddEvent(SPELL_CHAINS_OF_ICE,4000,8000,4000);
		Tasks.AddEvent(SPELL_ICEBOUND_FORTITUDE,20000,60000,20000,TARGET_ME);
		Tasks.AddEvent(SPELL_PVP_TRINKET,60000,1800000,10000,TARGET_ME);
		Tasks.AddEvent(SPELL_STRANGULATE,10000,30000,10000);
		Tasks.AddEvent(SPELL_DEATH_COIL,6000,8000,2000);
		Tasks.AddEvent(SPELL_FROST_FEAVER,5000,1000,0,TARGET_RANDOM);
		Tasks.AddEvent(SPELL_FROST_STRIKE,1000,6000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_ICY_TOUCH,4000,8000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_DEATH_GRIP,2000,35000,10000);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONDEATHKNIGHT, DONE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONDEATHKNIGHT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championDeathKnight(Creature* pCreature)
{
    return new boss_championDeathKnightAI(pCreature);
}
enum
{
	SPELL_ARACANE_BARRAGE = 65799,
	SPELL_ARCANE_BLAST = 65791,
	SPELL_ARCANE_EXPLOSION = 65800,
	SPELL_BLINK = 65793,
	SPELL_COUNTERSPELL = 65790,
	SPELL_FROST_NOVA = 65792,
	SPELL_FROSTBOLT = 65807,
	SPELL_ICE_BLOCK = 65802,
	SPELL_POLYMORPH = 65801,
	//SPELL_PVP_TRINKET = 65547,
};
struct MANGOS_DLL_DECL boss_championMageAI : public ScriptedAI
{
    boss_championMageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    MobEventTasks Tasks;
    Difficulty m_bDifficulty;
    uint32 ResetAggro_Timer;
	uint32 Mana_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_BLINK,30000,30000,30000,TARGET_ME);
		Tasks.AddEvent(SPELL_COUNTERSPELL,12000,24000,6000,TARGET_HAS_MANA,0,0,true);
		Tasks.AddEvent(SPELL_ICE_BLOCK,90000,300000,30000,TARGET_ME);
		Tasks.AddEvent(SPELL_POLYMORPH,18000,15000,10000,TARGET_RANDOM,0,0,true);
		Tasks.AddEvent(SPELL_ARACANE_BARRAGE,1000,4000,2000);
		Tasks.AddEvent(SPELL_ARCANE_BLAST,3000,4000,4000);
		Tasks.AddEvent(SPELL_ARCANE_EXPLOSION,5000,8000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_FROSTBOLT,7000,3000,4000,TARGET_MAIN);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONMAGE, DONE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CHAMPIONMAGE, IN_PROGRESS);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo->getFaction() == me->getFaction())
		{
			dmg = 0;
			DoResetThreat();
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(ResetAggro_Timer <= diff)
		{
			DoResetThreat();
			ResetAggro_Timer = 6000;
		}
		else
			ResetAggro_Timer -= diff;

		if(Mana_Timer <= diff)
		{
			uint32 npower = me->GetPower(POWER_MANA) + me->GetMaxPower(POWER_MANA) / 100;
			if(npower > me->GetMaxPower(POWER_MANA))
				npower = me->GetMaxPower(POWER_MANA);
			me->SetPower(POWER_MANA,npower);
			Mana_Timer = 500;
		}
		else
			Mana_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();

    }

};

CreatureAI* GetAI_boss_championMage(Creature* pCreature)
{
    return new boss_championMageAI(pCreature);
}

void AddSC_boss_championEdCRaid()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_championWar";
    newscript->GetAI = &GetAI_boss_championWar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championHunt";
    newscript->GetAI = &GetAI_boss_championHunt;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championPriestShadow";
    newscript->GetAI = &GetAI_boss_championPriestShadow;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_championPriestDisci";
    newscript->GetAI = &GetAI_boss_championPriestDisci;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championRogue";
    newscript->GetAI = &GetAI_boss_championRogue;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_championMage";
    newscript->GetAI = &GetAI_boss_championMage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championWarlock";
    newscript->GetAI = &GetAI_boss_championWarlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championDeathKnight";
    newscript->GetAI = &GetAI_boss_championDeathKnight;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championShamanRestau";
    newscript->GetAI = &GetAI_boss_championShamanResto;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_championShamanEnhanc";
    newscript->GetAI = &GetAI_boss_championShamanEnhanc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championPaladinRetrib";
    newscript->GetAI = &GetAI_boss_championPaladinRetrib;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_championPaladinHoly";
    newscript->GetAI = &GetAI_boss_championPaladinHoly;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_championDruidRestau";
    newscript->GetAI = &GetAI_boss_championDruidResto;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_championDruidBalance";
    newscript->GetAI = &GetAI_boss_championDruidBalance;
    newscript->RegisterSelf();

}
