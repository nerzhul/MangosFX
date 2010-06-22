#include "precompiled.h"
#include "ulduar.h"

enum
{
	//yells

	//kologarn
	SPELL_OVERHEAD_SMASH		= 63356,
	SPELL_OVERHEAD_SMASH_H		= 64003,
	SPELL_ONE_ARMED_SMASH		= 63573,
	SPELL_ONE_ARMED_SMASH_H		= 64006,
	SPELL_STONE_SHOUT			= 63716,
	SPELL_STONE_SHOUT_H			= 64005,
	SPELL_PETRIFYING_BREATH		= 62030,
	SPELL_PETRIFYING_BREATH_H	= 63980,

	SPELL_FOCUS_EYE_10			= 63346,
	SPELL_TRIGGER_FOCUS_10		= 63347,
	SPELL_FOCUS_EYE_25			= 63976,
	SPELL_TRIGGER_FOCUS_25		= 63977,
	//left arm
	SPELL_SHOCKWAVE				= 63783,
	SPELL_SHOCKWAVE_H			= 63982,
	//right arm
	SPELL_STONE_GRIP			= 64290,
	//both
	SPELL_ARM_VISUAL			= 64753,
	//rubble
	SPELL_RUMBLE				= 63818,
	SPELL_STONE_NOVA			= 63978,
	//NPC ids
	MOB_RUBBLE					= 33768
};

struct MANGOS_DLL_DECL boss_kologarnAI;
// Rubble
struct MANGOS_DLL_DECL mob_ulduar_rubbleAI : public LibDevFSAI
{
    mob_ulduar_rubbleAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		AddEventOnMe(SPELL_STONE_NOVA,urand(8000,12000),7000,2000);
	}

	uint32 Death_Timer;

	bool die;

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
		die = false;
    }

	void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
		if (uiDamage > me->GetHealth())
			if (!die)
			{
				uiDamage = 0;
				DoCastMe(SPELL_RUMBLE);
				Death_Timer = 1000;
				die = true;
			}
    }

	void UpdateAI(const uint32 diff)
    {
		if(die)
		{
			if (Death_Timer < diff)
				DoCastMe(7);
			else 
				Death_Timer -= diff;
		}

		UpdateEvent(diff);

		if (!die)
			DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_ulduar_rubble(Creature* pCreature)
{
    return new mob_ulduar_rubbleAI(pCreature);
}

// Left Arm
struct MANGOS_DLL_DECL boss_left_armAI : public LibDevFSAI
{
    boss_left_armAI(Creature* pCreature) : LibDevFSAI(pCreature)
	{
		InitInstance();
		SetCombatMovement(false);
		AddEventOnMe((m_difficulty) ? SPELL_SHOCKWAVE_H : SPELL_SHOCKWAVE,30000,30000);
	}

	uint32 Addcount;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		Addcount = 0;
		DoCastMe(SPELL_ARM_VISUAL);
    }

	void Aggro(Unit* pWho)
    {
        if (pInstance)
		{
			if (Creature* pTemp = GetInstanceCreature(DATA_RIGHT_ARM))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = GetInstanceCreature(DATA_KOLOGARN))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
		}
    }

	void DamageTaken(Unit* pDoneBy,uint32 &dmg)
	{
		
		if (Creature* pTemp = GetInstanceCreature(DATA_RIGHT_ARM))
			if(pDoneBy == pTemp)
			{
				dmg = 0;
				me->getThreatManager().modifyThreatPercent(pTemp,0);
				pTemp->getThreatManager().modifyThreatPercent(me,0);
			}

		if (Creature* pTemp = GetInstanceCreature(DATA_KOLOGARN))
			if(pDoneBy == pTemp)
			{
				dmg = 0;
				me->getThreatManager().modifyThreatPercent(pTemp,0);
				pTemp->getThreatManager().modifyThreatPercent(me,0);
			}

		if(dmg >= me->GetHealth())
		{
			dmg = 0;
			if (Creature* pTemp = GetInstanceCreature(DATA_KOLOGARN))
				if (pTemp->isAlive())
					DealPercentDamage(pTemp,15);
			FreezeMob();
			me->SetDisplayId(16925);
		}
	}

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(!CheckPlayers())
		{
			me->RemoveAllAuras();
			DoResetThreat();
			return;
		}

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
	
	void ReviveMe()
	{
		me->SetHealth(me->GetMaxHealth());
		FreezeMob(false);
		me->SetDisplayId(me->GetCreatureInfo()->DisplayID_A[0]);
	}

	bool CheckPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();

		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						return true;
				}

		return false;
	}
};

CreatureAI* GetAI_boss_left_arm(Creature* pCreature)
{
    return new boss_left_armAI(pCreature);
}

// Right Arm
struct MANGOS_DLL_DECL boss_right_armAI : public LibDevFSAI
{
    boss_right_armAI(Creature* pCreature) : LibDevFSAI(pCreature)
	{
		InitInstance();
		SetCombatMovement(false);
	}

	uint32 Stone_Grip_Timer;
	uint32 Addcount;
	uint32 gripdmg;
	uint32 freedmg;
	Unit* pGripTarget;

	bool grip;

    void Reset()
    {
		ResetTimers();
		Stone_Grip_Timer = 20000;
		Addcount = 0;
		pGripTarget = NULL;
		gripdmg = 0;
		freedmg = 0;
		grip = false;
		DoCastMe(SPELL_ARM_VISUAL);
		FreezeMob(false);
    }

	void Aggro(Unit* pWho)
    {
		if (Creature* pTemp = GetInstanceCreature(DATA_LEFT_ARM))
			if (pTemp->isAlive())
				pTemp->SetInCombatWithZone();
		if (Creature* pTemp = GetInstanceCreature(DATA_KOLOGARN))
			if (pTemp->isAlive())
				pTemp->SetInCombatWithZone();
    }

	void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
		if (Creature* pTemp = GetInstanceCreature(DATA_LEFT_ARM))
			if(pDoneBy == pTemp)
			{
				uiDamage = 0;
				me->getThreatManager().modifyThreatPercent(pTemp,0);
				pTemp->getThreatManager().modifyThreatPercent(me,0);
			}

		if (Creature* pTemp = GetInstanceCreature(DATA_KOLOGARN))
			if(pDoneBy == pTemp)
			{
				uiDamage = 0;
				me->getThreatManager().modifyThreatPercent(pTemp,0);
				pTemp->getThreatManager().modifyThreatPercent(me,0);
			}

		if (grip)
		{
			gripdmg += uiDamage;
			freedmg = m_difficulty ? 480000 : 100000;
			if (gripdmg > freedmg || uiDamage > me->GetHealth())
			{
				if (pGripTarget)
				{
					if (pGripTarget->HasAura(SPELL_STONE_GRIP))
						pGripTarget->RemoveAurasDueToSpell(SPELL_STONE_GRIP);

					pGripTarget->ExitVehicle();
					pGripTarget->Relocate(1781.764f,-24.704f,449.0f,6.27f);
				}
				grip = false;
				gripdmg = 0;
			}
		}
		if(uiDamage >= me->GetHealth())
		{
			uiDamage = 0;
			if (Creature* pTemp = GetInstanceCreature(DATA_KOLOGARN))
				if (pTemp->isAlive())
					DealPercentDamage(pTemp,15);
			if(pGripTarget)
			{
				pGripTarget->ExitVehicle();
			}
			me->SetDisplayId(16925);
			FreezeMob();
		}
    }

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(!CheckPlayers())
		{
			me->RemoveAllAuras();
			DoResetThreat();
			return;
		}

		if (Stone_Grip_Timer < diff)
        {
			//stone grip emote
			if (Unit* target = GetRandomUnit())
			{
				SetAuraStack(SPELL_STONE_GRIP,1,target,me);
				if(pGripTarget)
					Kill(pGripTarget);
				if(target->GetTypeId() == TYPEID_PLAYER)
				{	
					pGripTarget = target;
					target->EnterVehicle(me->GetVehicleKit());
					grip = true;
					gripdmg = 0;
				}
			}
			Stone_Grip_Timer = 30000;
        }
		else 
			Stone_Grip_Timer -= diff;

		DoMeleeAttackIfReady();
	}
	
	void ReviveMe()
	{
		me->SetHealth(me->GetMaxHealth());
		FreezeMob(false);
		me->SetDisplayId(me->GetCreatureInfo()->DisplayID_A[0]);
	}

	bool CheckPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();

		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						return true;
				}

		return false;
	}
};

CreatureAI* GetAI_boss_right_arm(Creature* pCreature)
{
    return new boss_right_armAI(pCreature);
}


struct MANGOS_DLL_DECL mob_right_focus_eyeAI : public LibDevFSAI
{
    mob_focus_eyeAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		MakeHostileInvisibleStalker();
	}

    void Reset()
    {
		ResetTimers();
		DoCastMe(m_difficulty ? SPELL_TRIGGER_FOCUS_25 : SPELL_TRIGGER_FOCUS_10);
    }

	void SetTarget(Unit* target)
	{
		me->AddThreat(target,1000000.0f);
	}

	void UpdateAI(const uint32 diff)
    {
		
	}
};

struct MANGOS_DLL_DECL mob_left_focus_eyeAI : public LibDevFSAI
{
    mob_left_focus_eyeAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		MakeHostileInvisibleStalker();
	}

    void Reset()
    {
		ResetTimers();
		DoCastMe(m_difficulty ? SPELL_TRIGGER_FOCUS_25 : SPELL_TRIGGER_FOCUS_10);
    }

	void SetTarget(Unit* target)
	{
		me->AddThreat(target,1000000.0f);
	}

	void UpdateAI(const uint32 diff)
    {
		
	}
};
// Kologarn
struct MANGOS_DLL_DECL boss_kologarnAI : public LibDevFSAI
{
    boss_kologarnAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		SetCombatMovement(false);
		veh = me->GetVehicleKit();
		AddEvent(m_difficulty ? SPELL_FOCUS_EYE_25 : SPELL_FOCUS_EYE_10,10000,10000,5000);
	}

	Vehicle* veh;

	uint32 Spell_Timer;
	uint32 Check_Timer;
	uint32 respawnright;
	uint32 respawnleft;
	uint32 CheckTimer;

	bool right;
	bool left;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		FreezeMob(false);
		Spell_Timer = 10000;
		Check_Timer = 6300;
		CheckTimer = 1000;
		right = true;
		left = true;
		if (Creature* pTemp = GetInstanceCreature(DATA_RIGHT_ARM))
			if (pTemp->isAlive())
				((boss_right_armAI*)pTemp->AI())->ReviveMe();
		if (Creature* pTemp = GetInstanceCreature(DATA_LEFT_ARM))
			if (pTemp->isAlive())
				((boss_left_armAI*)pTemp->AI())->ReviveMe();
    }

	void JustDied(Unit* pKiller)
    {
		Yell(15593,"Maître... ils arrivent...");
		CleanMyAdds();
        if (pInstance)
		{
            pInstance->SetData(TYPE_KOLOGARN, DONE);
			if (Creature* pTemp = GetInstanceCreature(DATA_LEFT_ARM))
				if (pTemp->isAlive())
					pTemp->RemoveFromWorld();
			if (Creature* pTemp = GetInstanceCreature(DATA_RIGHT_ARM))
				if (pTemp->isAlive())
					pTemp->RemoveFromWorld();
		}
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE);
    }

	void Aggro(Unit* pWho)
    {
        if (pInstance)
		{
            pInstance->SetData(TYPE_KOLOGARN, IN_PROGRESS);
			if (Creature* pTemp = GetInstanceCreature(DATA_LEFT_ARM))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = GetInstanceCreature(DATA_RIGHT_ARM))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
		}
		Yell(15586,"On ne passe pas !");
		FreezeMob(false);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if (Creature* pTemp = GetInstanceCreature(DATA_RIGHT_ARM))
			if(pDoneBy == pTemp)
			{
				dmg = 0;
				me->getThreatManager().modifyThreatPercent(pTemp,0);
				pTemp->getThreatManager().modifyThreatPercent(me,0);
			}

		if (Creature* pTemp = GetInstanceCreature(DATA_LEFT_ARM))
			if(pDoneBy == pTemp)
			{
				dmg = 0;
				me->getThreatManager().modifyThreatPercent(pTemp,0);
				pTemp->getThreatManager().modifyThreatPercent(me,0);
			}
	}

	void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(!CheckPlayers())
		{
			me->RemoveAllAuras();
			DoResetThreat();
			return;
		}

		if (Spell_Timer < diff)
        {
			if (right && left)
				DoCastVictim(m_difficulty ? SPELL_OVERHEAD_SMASH_H : SPELL_OVERHEAD_SMASH);
			else if (!right && !left)
				DoCastVictim(m_difficulty ? SPELL_STONE_SHOUT_H : SPELL_STONE_SHOUT);
			else
				DoCastVictim(m_difficulty ? SPELL_ONE_ARMED_SMASH_H : SPELL_ONE_ARMED_SMASH);
            Spell_Timer = 20000;
        }
		else 
			Spell_Timer -= diff;   

		if (respawnleft < diff && !left)
        {
			if (Creature* pTemp = GetInstanceCreature(DATA_LEFT_ARM))
				if (pTemp->isAlive())
					((boss_left_armAI*)pTemp->AI())->ReviveMe();
            left = true;
        }
		else 
			respawnleft -= diff;  

		if (respawnright < diff && !right)
        {
			if (Creature* pTemp = GetInstanceCreature(DATA_RIGHT_ARM))
				if (pTemp->isAlive())
					((boss_right_armAI*)pTemp->AI())->ReviveMe();
            right = true;
        }
		else 
			respawnright -= diff; 

		if (Check_Timer < diff)
        {
			if (Creature* lArm = GetInstanceCreature(DATA_LEFT_ARM))
				if (!lArm->isAlive() && left)
				{
					left = false;
					for(int i=0;i<5;i++)
						CallCreature(MOB_RUBBLE,TEN_MINS,PREC_COORDS,AGGRESSIVE_MAIN,1776.96f,-44.839f,449.0f);
					respawnleft = 60000;
				}
			if (Creature* rArm = GetInstanceCreature(DATA_RIGHT_ARM))
				if (!rArm->isAlive() && right)
				{
					right = false;
					for(int i=0;i<5;i++)
						CallCreature(MOB_RUBBLE,TEN_MINS,PREC_COORDS,AGGRESSIVE_MAIN,1777.81f,-3.539f,449.0f);
					respawnright = 60000;
				}
			if (!me->IsWithinDistInMap(me->getVictim(), 10))
				DoCastVictim(m_difficulty ? SPELL_PETRIFYING_BREATH_H : SPELL_PETRIFYING_BREATH);
			Check_Timer = 500;
        }
		else 
			Check_Timer -= diff;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}

	bool CheckPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();

		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						return true;
				}
		return false;
	}
};

CreatureAI* GetAI_boss_kologarn(Creature* pCreature)
{
    return new boss_kologarnAI(pCreature);
}

void AddSC_boss_kologarn()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_kologarn";
    NewScript->GetAI = GetAI_boss_kologarn;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "mob_ulduar_rubble";
    NewScript->GetAI = &GetAI_mob_ulduar_rubble;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_left_arm";
    NewScript->GetAI = &GetAI_boss_left_arm;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "boss_right_arm";
    NewScript->GetAI = &GetAI_boss_right_arm;
    NewScript->RegisterSelf();
}
