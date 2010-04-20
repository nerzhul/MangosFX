#include "precompiled.h"
#include "trial_of_the_crusader.h"
enum
{
	SPELL_BLADESTORM = 65946,
	SPELL_BLADESTORM2 = 65947,
	SPELL_CHARGE_10 = 65927,
	SPELL_CHARGE_10_H = 68762,
	SPELL_CHARGE_25 = 68763,
	SPELL_CHARGE_25_H = 68764,
	SPELL_DISARM = 65935,
	SPELL_INTIMIDATING_SHOUT = 65930,
	SPELL_MORTAL_STRIKE_10 = 65926,
	SPELL_MORTAL_STRIKE_10_H = 68782,
	SPELL_MORTAL_STRIKE_25 = 68783,
	SPELL_MORTAL_STRIKE_25_H = 68784,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_CHARGE_10,1000,15000,0);
				Tasks.AddEvent(SPELL_MORTAL_STRIKE_10,3000,6000,0,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_CHARGE_25,1000,15000,0);
				Tasks.AddEvent(SPELL_MORTAL_STRIKE_25,3000,6000,0,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_CHARGE_10_H,1000,15000,0);
				Tasks.AddEvent(SPELL_MORTAL_STRIKE_10_H,3000,6000,0,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_CHARGE_25_H,1000,15000,0);
				Tasks.AddEvent(SPELL_MORTAL_STRIKE_25_H,3000,6000,0,TARGET_MAIN);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_INSECT_SWARM_10 = 65855,
	SPELL_INSECT_SWARM_10_H = 67941,
	SPELL_INSECT_SWARM_25 = 67942,
	SPELL_INSECT_SWARM_25_H = 67943,
	SPELL_MOONFIRE_10 = 65856,
	SPELL_MOONFIRE_10_H = 67944,
	SPELL_MOONFIRE_25 = 67945,
	SPELL_MOONFIRE_25_H = 67946,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_STARFIRE_10 = 65854,
	SPELL_STARFIRE_10_H = 67947,
	SPELL_STARFIRE_25 = 67948,
	SPELL_STARFIRE_25_H = 64949,
	SPELL_WRATH_10 = 65862,
	SPELL_WRATH_10_H = 67951,
	SPELL_WRATH_25 = 67952,
	SPELL_WRATH_25_H = 67953,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_INSECT_SWARM_10,2000,18000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_MOONFIRE_10,3000,9000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_STARFIRE_10,5000,9000,2500,TARGET_MAIN);
				Tasks.AddEvent(SPELL_WRATH_10,200,9000,2500,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_INSECT_SWARM_25,2000,18000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_MOONFIRE_25,3000,9000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_STARFIRE_25,5000,9000,2500,TARGET_MAIN);
				Tasks.AddEvent(SPELL_WRATH_25,200,9000,2500,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_INSECT_SWARM_10_H,2000,18000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_MOONFIRE_10_H,3000,9000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_STARFIRE_10,5000,9000,2500,TARGET_MAIN);
				Tasks.AddEvent(SPELL_WRATH_10,200,9000,2500,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_INSECT_SWARM_10,2000,18000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_MOONFIRE_10,3000,9000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_STARFIRE_10,5000,9000,2500,TARGET_MAIN);
				Tasks.AddEvent(SPELL_WRATH_10,200,9000,2500,TARGET_MAIN);
				break;
		}
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
        if (CanDoSomething())
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
	//SPELL_BARKSKIN = 65860,
	SPELL_LIFEBLOOM_10 = 66093,
	SPELL_LIFEBLOOM_10_H = 67957,
	SPELL_LIFEBLOOM_25 = 67958,
	SPELL_LIFEBLOOM_25_H = 67959,
	SPELL_NATURES_GRASP = 66071,
	SPELL_NOURISH_10 = 66066,
	SPELL_NOURISH_10_H = 67965,
	SPELL_NOURISH_25 = 67966,
	SPELL_NOURISH_25_H = 67967,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_REGROWTH_10 = 66067,
	SPELL_REGROWTH_10_H = 67968,
	SPELL_REGROWTH_25 = 67969,
	SPELL_REGROWTH_25_H = 67970,
	SPELL_REJUVENATION_10 = 66065,
	SPELL_REJUVENATION_10_H = 67971,
	SPELL_REJUVENATION_25 = 67972,
	SPELL_REJUVENATION_25_H = 67973,
	SPELL_THORNS = 66068,
	SPELL_TRANQUILITY_10 = 66086,
	SPELL_TRANQUILITY_10_H = 67974,
	SPELL_TRANQUILITY_25 = 67975,
	SPELL_TRANQUILITY_25_H = 67976,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_TRANQUILITY_10,120000,600000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_LIFEBLOOM_10,1000,2000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_NOURISH_10,1500,1500,1500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REGROWTH_10,5000,10000,2500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REJUVENATION_10,3500,3500,1500,HEAL_MY_FRIEND);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_TRANQUILITY_25,120000,600000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_LIFEBLOOM_25,1000,2000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_NOURISH_25,1500,1500,1500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REGROWTH_25,5000,10000,2500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REJUVENATION_25,3500,3500,1500,HEAL_MY_FRIEND);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_TRANQUILITY_10_H,120000,600000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_LIFEBLOOM_10_H,1000,2000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_NOURISH_10_H,1500,1500,1500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REGROWTH_10_H,5000,10000,2500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REJUVENATION_10_H,3500,3500,1500,HEAL_MY_FRIEND);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_TRANQUILITY_25_H,120000,600000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_LIFEBLOOM_25_H,1000,2000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_NOURISH_25_H,1500,1500,1500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REGROWTH_25_H,5000,10000,2500,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_REJUVENATION_25_H,3500,3500,1500,HEAL_MY_FRIEND);
				break;
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
        if (CanDoSomething())
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
	SPELL_AIMED_SHOT_10 = 65883,
	SPELL_AIMED_SHOT_10_H = 67977,
	SPELL_AIMED_SHOT_25 = 67978,
	SPELL_AIMED_SHOT_25_H = 67979,
	SPELL_CALL_PET = 67777,
	SPELL_DETERRENCE = 65871,
	SPELL_DISENGAGE = 65869,
	SPELL_EXPLOSIVE_SHOT_10 = 65866,
	SPELL_EXPLOSIVE_SHOT_10_H = 67983,
	SPELL_EXPLOSIVE_SHOT_25 = 67984,
	SPELL_EXPLOSIVE_SHOT_25_H = 67985,
	SPELL_FROST_TRAP = 65880,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_SHOOT_10 = 65868,
	SPELL_SHOOT_10_H = 67988,
	SPELL_SHOOT_25 = 67989,
	SPELL_SHOOT_25_H = 67990,
	SPELL_STEADY_SHOT = 65867,
	SPELL_WING_CLIP = 66207,
	SPELL_WYVERN_STING = 65877,
};
enum huntpetcat
{
	SPELL_CLAW_10	 = 67793,
	SPELL_CLAW_10_H	 = 67980,
	SPELL_CLAW_25	 = 67981,
	SPELL_CLAW_25_H	 = 67982,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_AIMED_SHOT_10,3000,11000,2000);
				Tasks.AddEvent(SPELL_EXPLOSIVE_SHOT_10,2000,6000,1000);
				Tasks.AddEvent(SPELL_SHOOT_10,100,5000,1200);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_AIMED_SHOT_25,3000,11000,2000);
				Tasks.AddEvent(SPELL_EXPLOSIVE_SHOT_25,2000,6000,1000);
				Tasks.AddEvent(SPELL_SHOOT_25,100,5000,1200);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_AIMED_SHOT_10_H,3000,11000,2000);
				Tasks.AddEvent(SPELL_EXPLOSIVE_SHOT_10_H,2000,6000,1000);
				Tasks.AddEvent(SPELL_SHOOT_10_H,100,5000,1200);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_AIMED_SHOT_25_H,3000,11000,2000);
				Tasks.AddEvent(SPELL_EXPLOSIVE_SHOT_25_H,2000,6000,1000);
				Tasks.AddEvent(SPELL_SHOOT_25_H,100,5000,1200);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_EVISCERATE_10 = 65957,
	SPELL_EVISCERATE_10_H = 68094,
	SPELL_EVISCERATE_25 = 68095,
	SPELL_EVISCERATE_25_H = 68096,
	SPELL_FAN_OF_KNIVES_10 = 65955,
	SPELL_FAN_OF_KNIVES_10_H = 68097,
	SPELL_FAN_OF_KNIVES_25 = 68098,
	SPELL_FAN_OF_KNIVES_25_H = 68099,
	SPELL_HEMORRHAGE = 65954,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_SHADOWSTEP_10 = 66178,
	SPELL_SHADOWSTEP_10_H = 68759,
	SPELL_SHADOWSTEP_25 = 68760,
	SPELL_SHADOWSTEP_25_H = 68761,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_EVISCERATE_10,12000,10000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FAN_OF_KNIVES_10,6000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SHADOWSTEP_10,200,30000,5000);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_EVISCERATE_25,12000,10000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FAN_OF_KNIVES_25,6000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SHADOWSTEP_25,200,30000,5000);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_EVISCERATE_10_H,12000,10000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FAN_OF_KNIVES_10_H,6000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SHADOWSTEP_10_H,200,30000,5000);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_EVISCERATE_25_H,12000,10000,0,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FAN_OF_KNIVES_25_H,6000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SHADOWSTEP_25_H,200,30000,5000);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_CORRUPTION_10 = 65810,
	SPELL_CORRUPTION_10_H = 68133,
	SPELL_CORRUPTION_25 = 68134,
	SPELL_CORRUPTION_25_H = 68135,
	SPELL_CURSE_OF_AGONY_10 = 65814,
	SPELL_CURSE_OF_AGONY_10_H = 68136,
	SPELL_CURSE_OF_AGONY_25 = 68167,
	SPELL_CURSE_OF_AGONY_25_H = 68168,
	SPELL_CURSE_OF_EXHAUSTION = 65815,
	SPELL_FEAR = 65809,
	SPELL_HELLFIRE_10 = 65816,
	SPELL_HELLFIRE_10_H = 68145,
	SPELL_HELLFIRE_25 = 68146,
	SPELL_HELLFIRE_25_H = 68147,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_SEARING_PAIN_10 = 65819,
	SPELL_SEARING_PAIN_10_H = 68148,
	SPELL_SEARING_PAIN_25 = 68149,
	SPELL_SEARING_PAIN_25_H = 68150,
	SPELL_SHADOW_BOLT_10 = 65821,
	SPELL_SHADOW_BOLT_10_H = 68151,
	SPELL_SHADOW_BOLT_25 = 68152,
	SPELL_SHADOW_BOLT_25_H = 68153,
	SPELL_SUMMON_FELHUNTER = 67514,
	SPELL_UNSTABLE_AFFLICTION_10 = 65812,
	SPELL_UNSTABLE_AFFLICTION_10_H = 68154,
	SPELL_UNSTABLE_AFFLICTION_25 = 68155,
	SPELL_UNSTABLE_AFFLICTION_25_H = 68156,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_CORRUPTION_10,5000,12000,4000);
				Tasks.AddEvent(SPELL_CURSE_OF_AGONY_10,1000,10000,4000);
				Tasks.AddEvent(SPELL_HELLFIRE_10,60000,62000,10000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SEARING_PAIN_10,2000,8000,9000);
				Tasks.AddEvent(SPELL_SHADOW_BOLT_10,6000,10000,1000);
				Tasks.AddEvent(SPELL_UNSTABLE_AFFLICTION_10,12000,18000,1000);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_CORRUPTION_25,5000,12000,4000);
				Tasks.AddEvent(SPELL_CURSE_OF_AGONY_25,1000,10000,4000);
				Tasks.AddEvent(SPELL_HELLFIRE_25,60000,62000,10000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SEARING_PAIN_25,2000,8000,9000);
				Tasks.AddEvent(SPELL_SHADOW_BOLT_25,6000,10000,1000);
				Tasks.AddEvent(SPELL_UNSTABLE_AFFLICTION_25,12000,18000,1000);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_CORRUPTION_10_H,5000,12000,4000);
				Tasks.AddEvent(SPELL_CURSE_OF_AGONY_10_H,1000,10000,4000);
				Tasks.AddEvent(SPELL_HELLFIRE_10_H,60000,62000,10000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SEARING_PAIN_10_H,2000,8000,9000);
				Tasks.AddEvent(SPELL_SHADOW_BOLT_10_H,6000,10000,1000);
				Tasks.AddEvent(SPELL_UNSTABLE_AFFLICTION_10_H,12000,18000,1000);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_CORRUPTION_25_H,5000,12000,4000);
				Tasks.AddEvent(SPELL_CURSE_OF_AGONY_25_H,1000,10000,4000);
				Tasks.AddEvent(SPELL_HELLFIRE_25_H,60000,62000,10000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_SEARING_PAIN_25_H,2000,8000,9000);
				Tasks.AddEvent(SPELL_SHADOW_BOLT_25_H,6000,10000,1000);
				Tasks.AddEvent(SPELL_UNSTABLE_AFFLICTION_25_H,12000,18000,1000);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_EARTH_SHOCK_10 = 65973,
	SPELL_EARTH_SHOCK_10_H = 68100,
	SPELL_EARTH_SHOCK_25 = 68101,
	SPELL_EARTH_SHOCK_25_H = 68102,
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
		ResetAggro_Timer = 6000;
		Mana_Timer = 200;
		Tasks.AddEvent(SPELL_BLOODLUST,60000,600000,10000,TARGET_ME,0,0,true);
		Tasks.AddEvent(SPELL_LAVA_LASH,7000,7000,1000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_PVP_TRINKET,120000,120000,6000,TARGET_ME);
		Tasks.AddEvent(SPELL_STORMSTRIKE,3000,6000,1000,TARGET_MAIN);
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_10,8000,10000,1000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_25,8000,10000,1000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_10_H,8000,10000,1000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_25_H,8000,10000,1000,TARGET_MAIN);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_CLEANSE_SPIRIT_10 = 66056,
	SPELL_CLEANSE_SPIRIT_10_H = 68627,
	SPELL_CLEANSE_SPIRIT_25 = 68628,
	SPELL_CLEANSE_SPIRIT_25_H = 68629,
	SPELL_EARTH_SHIELD = 66063,
	//SPELL_EARTH_SHOCK_10 = 65973,
	//SPELL_EARTH_SHOCK_10_H = 68100,
	//SPELL_EARTH_SHOCK_25 = 68101,
	//SPELL_EARTH_SHOCK_25_H = 68102,
	SPELL_HEX = 66054,
	SPELL_LESSER_HEALING_WAVE_10 = 66055,
	SPELL_LESSER_HEALING_WAVE_10_H = 68115,
	SPELL_LESSER_HEALING_WAVE_25 = 68116,
	SPELL_LESSER_HEALING_WAVE_25_H = 68117,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_RIPTIDE_10 = 66053,
	SPELL_RIPTIDE_10_H = 68118,
	SPELL_RIPTIDE_25 = 68119,
	SPELL_RIPTIDE_25_H = 68120,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_10,10000,10000,5000);
				Tasks.AddEvent(SPELL_LESSER_HEALING_WAVE_10,1000,5000,0,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_RIPTIDE_10,3000,10000,0,HEAL_MY_FRIEND);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_25,10000,10000,5000);
				Tasks.AddEvent(SPELL_LESSER_HEALING_WAVE_25,1000,5000,0,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_RIPTIDE_25,3000,10000,0,HEAL_MY_FRIEND);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_10_H,10000,10000,5000);
				Tasks.AddEvent(SPELL_LESSER_HEALING_WAVE_10_H,1000,5000,0,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_RIPTIDE_10_H,3000,10000,0,HEAL_MY_FRIEND);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_EARTH_SHOCK_25_H,10000,10000,5000);
				Tasks.AddEvent(SPELL_LESSER_HEALING_WAVE_25_H,1000,5000,0,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_RIPTIDE_25_H,3000,10000,0,HEAL_MY_FRIEND);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_CLEANSE_10 = 66116,
	SPELL_CLEANSE_10_H = 68621,
	SPELL_CLEANSE_25 = 68622,
	SPELL_CLEANSE_25_H = 68623,
	SPELL_DIVINE_SHIELD = 66010,
	SPELL_FLASH_OF_LIGHT_10 = 66113,
	SPELL_FLASH_OF_LIGHT_10_H = 68008,
	SPELL_FLASH_OF_LIGHT_25 = 68009,
	SPELL_FLASH_OF_LIGHT_25_H = 68010,
	SPELL_HAMMER_OF_JUSTICE = 66613,
	SPELL_HAND_OF_FREEDOM_10 = 66115,
	SPELL_HAND_OF_FREEDOM_10_H = 68756,
	SPELL_HAND_OF_FREEDOM_25 = 68757,
	SPELL_HAND_OF_FREEDOM_25_H = 68758,
	SPELL_HAND_OF_PROTECTION = 66009,
	SPELL_HOLY_LIGHT_10 = 66112,
	SPELL_HOLY_LIGHT_10_H = 68011,
	SPELL_HOLY_LIGHT_25 = 68012,
	SPELL_HOLY_LIGHT_25_H = 68013,
	SPELL_HOLY_SHOCK_10 = 66114,
	SPELL_HOLY_SHOCK_10_H = 68014,
	SPELL_HOLY_SHOCK_25 = 68015,
	SPELL_HOLY_SHOCK_25_H = 68016,
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
		Tasks.AddEvent(SPELL_DIVINE_SHIELD,60000,300000,30000,TARGET_ME);
		Tasks.AddEvent(SPELL_HAMMER_OF_JUSTICE,10000,40000,20000,TARGET_LOW_HP);
		Tasks.AddEvent(SPELL_PVP_TRINKET,90000,600000,0,TARGET_ME);
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_HOLY_SHOCK_10,1000,7000,15000);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_HOLY_SHOCK_25,1000,7000,15000);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_HOLY_SHOCK_10_H,1000,7000,15000);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_HOLY_SHOCK_25_H,1000,7000,15000);
				break;
		}
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
        if (CanDoSomething())
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
	//SPELL_DIVINE_SHIELD = 66010,
	SPELL_DIVINE_STORM = 66006,
	//SPELL_HAMMER_OF_JUSTICE = 66007,
	//SPELL_HAND_OF_PROTECTION = 66009,
	SPELL_JUDGEMENT_OF_COMMAND_10 = 66005,
	SPELL_JUDGEMENT_OF_COMMAND_10_H = 68017,
	SPELL_JUDGEMENT_OF_COMMAND_25 = 68018,
	SPELL_JUDGEMENT_OF_COMMAND_25_H = 68019,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_REPENTANCE = 66008,
	SPELL_SEAL_OF_COMMAND_10 = 66004,
	SPELL_SEAL_OF_COMMAND_10_H = 68020,
	SPELL_SEAL_OF_COMMAND_25 = 68021,
	SPELL_SEAL_OF_COMMAND_25_H = 68022,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_SEAL_OF_COMMAND_10,50,1800000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_JUDGEMENT_OF_COMMAND_10,2000,8000,9000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_SEAL_OF_COMMAND_25,50,1800000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_JUDGEMENT_OF_COMMAND_25,2000,8000,9000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_SEAL_OF_COMMAND_10_H,50,1800000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_JUDGEMENT_OF_COMMAND_10_H,2000,8000,9000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_SEAL_OF_COMMAND_25_H,50,1800000,0,TARGET_ME);
				Tasks.AddEvent(SPELL_JUDGEMENT_OF_COMMAND_25_H,2000,8000,9000,TARGET_MAIN);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_DISPEL_MAGIC_10 = 65546,
	SPELL_DISPEL_MAGIC_10_H = 68624,
	SPELL_DISPEL_MAGIC_25 = 68625,
	SPELL_DISPEL_MAGIC_25_H = 68626,
	SPELL_FLASH_HEAL_10 = 66104,
	SPELL_FLASH_HEAL_10_H = 68023,
	SPELL_FLASH_HEAL_25 = 68024,
	SPELL_FLASH_HEAL_25_H = 68025,
	SPELL_MANA_BURN_10 = 66100,
	SPELL_MANA_BURN_10_H = 68026,
	SPELL_MANA_BURN_25 = 68027,
	SPELL_MANA_BURN_25_H = 68028,
	SPELL_PENANCE_10 = 66098,
	SPELL_PENANCE_10_H = 68029,
	SPELL_PENANCE_25 = 68030,
	SPELL_PENANCE_25_H = 68031,
	SPELL_POWER_WORD_SHIELD_10 = 66099,
	SPELL_POWER_WORD_SHIELD_10_H = 68032,
	SPELL_POWER_WORD_SHIELD_25 = 68033,
	SPELL_POWER_WORD_SHIELD_25_H = 68034,
	SPELL_PSYCHIC_SCREAM = 65543,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_RENEW_10 = 66177,
	SPELL_RENEW_10_H = 68035,
	SPELL_RENEW_25 = 68036,
	SPELL_RENEW_25_H = 68037,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_10,60000,180000);
				Tasks.AddEvent(SPELL_MANA_BURN_10,5000,10000,2000,TARGET_HAS_MANA);
				Tasks.AddEvent(SPELL_FLASH_HEAL_10,1000,5000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_PENANCE_10,12000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_POWER_WORD_SHIELD_10,10000,12000,1,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_RENEW_10,17000,15000,0,HEAL_MY_FRIEND,0,0,true);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_25,60000,180000);
				Tasks.AddEvent(SPELL_MANA_BURN_25,5000,10000,2000,TARGET_HAS_MANA);
				Tasks.AddEvent(SPELL_FLASH_HEAL_25,1000,5000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_PENANCE_25,12000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_POWER_WORD_SHIELD_25,10000,12000,1,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_RENEW_25,17000,15000,0,HEAL_MY_FRIEND,0,0,true);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_10_H,60000,180000);
				Tasks.AddEvent(SPELL_MANA_BURN_10_H,5000,10000,2000,TARGET_HAS_MANA);
				Tasks.AddEvent(SPELL_FLASH_HEAL_10_H,1000,5000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_PENANCE_10_H,12000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_POWER_WORD_SHIELD_10_H,10000,12000,1,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_RENEW_10_H,17000,15000,0,HEAL_MY_FRIEND,0,0,true);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_25_H,60000,180000);
				Tasks.AddEvent(SPELL_MANA_BURN_25_H,5000,10000,2000,TARGET_HAS_MANA);
				Tasks.AddEvent(SPELL_FLASH_HEAL_25_H,1000,5000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_PENANCE_25_H,12000,1000,HEAL_MY_FRIEND);
				Tasks.AddEvent(SPELL_POWER_WORD_SHIELD_25_H,10000,12000,1,HEAL_MY_FRIEND,0,0,true);
				Tasks.AddEvent(SPELL_RENEW_25_H,17000,15000,0,HEAL_MY_FRIEND,0,0,true);
				break;
		}
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
        if (CanDoSomething())
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
	//SPELL_DISPEL_MAGIC_10 = 65546,
	//SPELL_DISPEL_MAGIC_10_H = 68624,
	//SPELL_DISPEL_MAGIC_25 = 68625,
	//SPELL_DISPEL_MAGIC_25_H = 68626,
	SPELL_DISPERSION = 65544,
	SPELL_MIND_BLAST_10 = 65492,
	SPELL_MIND_BLAST_10_H = 68038,
	SPELL_MIND_BLAST_25 = 68039,
	SPELL_MIND_BLAST_25_H = 68040,
	SPELL_MIND_FLAY_10 = 65488,
	SPELL_MIND_FLAY_10_H = 68042,
	SPELL_MIND_FLAY_25 = 68043,
	SPELL_MIND_FLAY_25_H = 68044,
	SPELL_PSYCHIC_HORROR = 65545,
	//SPELL_PSYCHIC_SCREAM = 65543,
	//SPELL_PVP_TRINKET = 65547,
	SPELL_SHADOW_WORD_PAIN_10 = 65541,
	SPELL_SHADOW_WORD_PAIN_10_H = 68088,
	SPELL_SHADOW_WORD_PAIN_25 = 68089,
	SPELL_SHADOW_WORD_PAIN_25_H = 68090,
	SPELL_SILENCE = 65542,
	SPELL_VAMPIRIC_TOUCH_10 = 65490,
	SPELL_VAMPIRIC_TOUCH_10_H = 68091,
	SPELL_VAMPIRIC_TOUCH_25 = 68092,
	SPELL_VAMPIRIC_TOUCH_25_H = 68093,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_10,60000,1800000);
				Tasks.AddEvent(SPELL_MIND_BLAST_10,1500,8000,1000);
				Tasks.AddEvent(SPELL_MIND_FLAY_10,6000,10000,1000);
				Tasks.AddEvent(SPELL_SHADOW_WORD_PAIN_10,2000,6000,3000);
				Tasks.AddEvent(SPELL_VAMPIRIC_TOUCH_10,15000,15000,15000);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_25,60000,1800000);
				Tasks.AddEvent(SPELL_MIND_BLAST_25,1500,8000,1000);
				Tasks.AddEvent(SPELL_MIND_FLAY_25,6000,10000,1000);
				Tasks.AddEvent(SPELL_SHADOW_WORD_PAIN_25,2000,6000,3000);
				Tasks.AddEvent(SPELL_VAMPIRIC_TOUCH_25,15000,15000,15000);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_10_H,60000,1800000);
				Tasks.AddEvent(SPELL_MIND_BLAST_10_H,1500,8000,1000);
				Tasks.AddEvent(SPELL_MIND_FLAY_10_H,6000,10000,1000);
				Tasks.AddEvent(SPELL_SHADOW_WORD_PAIN_10_H,2000,6000,3000);
				Tasks.AddEvent(SPELL_VAMPIRIC_TOUCH_10_H,15000,15000,15000);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_DISPEL_MAGIC_25_H,60000,1800000);
				Tasks.AddEvent(SPELL_MIND_BLAST_25_H,1500,8000,1000);
				Tasks.AddEvent(SPELL_MIND_FLAY_25_H,6000,10000,1000);
				Tasks.AddEvent(SPELL_SHADOW_WORD_PAIN_25_H,2000,6000,3000);
				Tasks.AddEvent(SPELL_VAMPIRIC_TOUCH_25_H,15000,15000,15000);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_DEATH_COIL_10 = 66019,
	SPELL_DEATH_COIL_10_H = 67929,
	SPELL_DEATH_COIL_25 = 67930,
	SPELL_DEATH_COIL_25_H = 67931,
	SPELL_DEATH_GRIP_10 = 66017,
	SPELL_DEATH_GRIP_10_H = 68753,
	SPELL_DEATH_GRIP_25 = 68754,
	SPELL_DEATH_GRIP_25_H = 68755,
	SPELL_FROST_STRIKE_10 = 66047,
	SPELL_FROST_STRIKE_10_H = 67935,
	SPELL_FROST_STRIKE_25 = 67936,
	SPELL_FROST_STRIKE_25_H = 67937,
	SPELL_ICEBOUND_FORTITUDE = 66023,
	SPELL_ICY_TOUCH_10 = 66021,
	SPELL_ICY_TOUCH_10_H = 67938,
	SPELL_ICY_TOUCH_25 = 67939,
	SPELL_ICY_TOUCH_25_H = 67940,
	//SPELL_PVP_TRINKET = 65547,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_DEATH_COIL_10,6000,8000,2000);
				Tasks.AddEvent(SPELL_DEATH_GRIP_10,2000,35000,10000);
				Tasks.AddEvent(SPELL_FROST_STRIKE_10,1000,6000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_ICY_TOUCH_10,4000,8000,1000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_DEATH_COIL_25,6000,8000,2000);
				Tasks.AddEvent(SPELL_DEATH_GRIP_25,2000,35000,10000);
				Tasks.AddEvent(SPELL_FROST_STRIKE_25,1000,6000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_ICY_TOUCH_25,4000,8000,1000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_DEATH_COIL_10_H,6000,8000,2000);
				Tasks.AddEvent(SPELL_DEATH_GRIP_10_H,2000,35000,10000);
				Tasks.AddEvent(SPELL_FROST_STRIKE_10_H,1000,6000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_ICY_TOUCH_10_H,4000,8000,1000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_DEATH_COIL_25_H,6000,8000,2000);
				Tasks.AddEvent(SPELL_DEATH_GRIP_25_H,2000,35000,10000);
				Tasks.AddEvent(SPELL_FROST_STRIKE_25_H,1000,6000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_ICY_TOUCH_25_H,4000,8000,1000,TARGET_MAIN);
				break;
		}
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
        if (CanDoSomething())
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
	SPELL_ARACANE_BARRAGE_10 = 65799,
	SPELL_ARACANE_BARRAGE_10_H = 67994,
	SPELL_ARACANE_BARRAGE_25 = 67995,
	SPELL_ARACANE_BARRAGE_25_H = 67996,
	SPELL_ARCANE_BLAST_10 = 65791,
	SPELL_ARCANE_BLAST_10_H = 67997,
	SPELL_ARCANE_BLAST_25 = 67998,
	SPELL_ARCANE_BLAST_25_H = 67999,
	SPELL_ARCANE_EXPLOSION_10 = 65800,
	SPELL_ARCANE_EXPLOSION_10_H = 68000,
	SPELL_ARCANE_EXPLOSION_25 = 68001,
	SPELL_ARCANE_EXPLOSION_25_H = 68002,
	SPELL_BLINK = 65793,
	SPELL_COUNTERSPELL = 65790,
	SPELL_FROST_NOVA = 65792,
	SPELL_FROSTBOLT_10 = 65807,
	SPELL_FROSTBOLT_10_H = 68003,
	SPELL_FROSTBOLT_25 = 68004,
	SPELL_FROSTBOLT_25_H = 68005,
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
		switch(m_bDifficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				Tasks.AddEvent(SPELL_ARACANE_BARRAGE_10,1000,4000,2000);
				Tasks.AddEvent(SPELL_ARCANE_BLAST_10,3000,4000,4000);
				Tasks.AddEvent(SPELL_ARCANE_EXPLOSION_10,5000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FROSTBOLT_10,7000,3000,4000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddEvent(SPELL_ARACANE_BARRAGE_25,1000,4000,2000);
				Tasks.AddEvent(SPELL_ARCANE_BLAST_25,3000,4000,4000);
				Tasks.AddEvent(SPELL_ARCANE_EXPLOSION_25,5000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FROSTBOLT_25,7000,3000,4000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				Tasks.AddEvent(SPELL_ARACANE_BARRAGE_10_H,1000,4000,2000);
				Tasks.AddEvent(SPELL_ARCANE_BLAST_10_H,3000,4000,4000);
				Tasks.AddEvent(SPELL_ARCANE_EXPLOSION_10_H,5000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FROSTBOLT_10_H,7000,3000,4000,TARGET_MAIN);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddEvent(SPELL_ARACANE_BARRAGE_25_H,1000,4000,2000);
				Tasks.AddEvent(SPELL_ARCANE_BLAST_25_H,3000,4000,4000);
				Tasks.AddEvent(SPELL_ARCANE_EXPLOSION_25_H,5000,10000,1000,TARGET_MAIN);
				Tasks.AddEvent(SPELL_FROSTBOLT_25_H,7000,3000,4000,TARGET_MAIN);
				break;
		}
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
        if (CanDoSomething())
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