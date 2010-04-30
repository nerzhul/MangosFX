#include "precompiled.h"
#include "Vehicle.h"
#include "ulduar.h"

class Vehicle;
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
	SPELL_FOCUS_EYE_25			= 63976,
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

float LeftArmX;
float LeftArmY;
float LeftArmZ;
float RightArmX;
float RightArmY;
float RightArmZ;

// Rubble
struct MANGOS_DLL_DECL mob_ulduar_rubbleAI : public ScriptedAI
{
    mob_ulduar_rubbleAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		Reset();
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 Death_Timer;

	bool die;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_STONE_NOVA,urand(8000,12000),7000,2000,TARGET_ME);
		die = false;
    }

	void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
		if (uiDamage > me->GetHealth())
			if (!die)
			{
				uiDamage = 0;
				DoCast(me, SPELL_RUMBLE);
				Death_Timer = 500;
				die = true;

			}
    }

	void UpdateAI(const uint32 diff)
    {
		if (Death_Timer < diff && die)
        {
			Kill(me);
        }
		else 
			Death_Timer -= diff;


		Tasks.UpdateEvent(diff);

		if (!die)
			DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_ulduar_rubble(Creature* pCreature)
{
    return new mob_ulduar_rubbleAI(pCreature);
}

// Left Arm
struct MANGOS_DLL_DECL boss_left_armAI : public ScriptedAI
{
    boss_left_armAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		SetCombatMovement(false);
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
		LeftArmX = 1777.636841; LeftArmY = -47.970596; LeftArmZ = 448.805908;
	}

	bool m_bIsRegularMode;
	ScriptedInstance* m_pInstance;

	uint32 Addcount;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		if(m_bIsRegularMode)
			Tasks.AddEvent(SPELL_SHOCKWAVE_H,30000,30000,0,TARGET_ME);
		else
			Tasks.AddEvent(SPELL_SHOCKWAVE,30000,30000,0,TARGET_ME);
		Addcount = 0;
		DoCastMe(SPELL_ARM_VISUAL);
    }

	void Aggro(Unit* pWho)
    {
        if (m_pInstance)
		{
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_RIGHT_ARM))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_KOLOGARN))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
		}
    }

	void DamageTaken(Unit* pDoneBy,uint32 &dmg)
	{
		if(dmg >= me->GetHealth())
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_KOLOGARN))))
				if (pTemp->isAlive())
					pTemp->DealDamage(pTemp, pTemp->GetMaxHealth() / 100 * 15, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_left_arm(Creature* pCreature)
{
    return new boss_left_armAI(pCreature);
}

// Right Arm
struct MANGOS_DLL_DECL boss_right_armAI : public ScriptedAI
{
    boss_right_armAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
		SetCombatMovement(false);
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
		RightArmX = 1779.671753; RightArmY = 1.514701; RightArmZ = 448.810577;
	}

	bool m_bIsRegularMode;
	ScriptedInstance* m_pInstance;

	uint32 Stone_Grip_Timer;
	uint32 Addcount;
	uint32 gripdmg;
	uint32 freedmg;
	Unit* pGripTarget;
	MobEventTasks Tasks;

	bool grip;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Stone_Grip_Timer = 20000;
		Addcount = 0;
		pGripTarget = NULL;
		gripdmg = 0;
		freedmg = 0;
		grip = false;
		DoCast(me, SPELL_ARM_VISUAL);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

	void Aggro(Unit* pWho)
    {
        if (m_pInstance)
		{
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_LEFT_ARM))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_KOLOGARN))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
		}
    }

	void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
		if (grip)
		{
			gripdmg += uiDamage;
			freedmg = m_bIsRegularMode ? 480000 : 100000;
			if (gripdmg > freedmg || uiDamage > me->GetHealth())
			{
				if (pGripTarget)
				{
					if (pGripTarget->HasAura(SPELL_STONE_GRIP))
						pGripTarget->RemoveAurasDueToSpell(SPELL_STONE_GRIP);

					pGripTarget->ExitVehicle();
				}
				grip = false;
				gripdmg = 0;
			}
		}
		if(uiDamage >= me->GetHealth())
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_KOLOGARN))))
				if (pTemp->isAlive())
					pTemp->DealDamage(pTemp, pTemp->GetMaxHealth() / 100 * 15, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

	void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if (Stone_Grip_Timer < diff)
        {
			//stone grip emote
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
			{
				Tasks.SetAuraStack(SPELL_STONE_GRIP,1,target,me);
				pGripTarget = target;
				target->EnterVehicle(me->GetVehicleKit(),0);
				grip = true;
				gripdmg = 0;
				//target->EnterVehicle(me->GetVehicle());
			}
			Stone_Grip_Timer = 30000;
        }
		else 
			Stone_Grip_Timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_right_arm(Creature* pCreature)
{
    return new boss_right_armAI(pCreature);
}

// Kologarn
struct MANGOS_DLL_DECL boss_kologarnAI : public Scripted_NoMovementAI
{
    boss_kologarnAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) 
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
		veh = me->GetVehicleKit();
		Reset();
	}

	ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
	Vehicle* veh;

	uint32 Spell_Timer;
	uint32 Check_Timer;
	uint32 respawnright;
	uint32 respawnleft;
	MobEventTasks Tasks;
	uint32 CheckTimer;

	bool right;
	bool left;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();

		if(m_bIsRegularMode)
			Tasks.AddEvent(SPELL_FOCUS_EYE_25,10000,10000,5000);
		else
			Tasks.AddEvent(SPELL_FOCUS_EYE_10,10000,10000,5000);

		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
		Spell_Timer = 10000;
		Check_Timer = 6300;
		CheckTimer = 1000;
		right = true;
		left = true;
		
    }

	void JustDied(Unit* pKiller)
    {
		Tasks.Speak(CHAT_TYPE_YELL,15593,"Maître... ils arrivent...");
		Tasks.CleanMyAdds();
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_KOLOGARN, DONE);
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_LEFT_ARM))))
				if (pTemp->isAlive())
					Kill(pTemp);
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_RIGHT_ARM))))
				if (pTemp->isAlive())
					Kill(pTemp);

		}
		GiveEmblemsToGroup((m_bIsRegularMode) ? CONQUETE : VAILLANCE);
    }

	void Aggro(Unit* pWho)
    {
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_KOLOGARN, IN_PROGRESS);
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_LEFT_ARM))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_RIGHT_ARM))))
				if (pTemp->isAlive())
					pTemp->SetInCombatWithZone();
		}
		Tasks.Speak(CHAT_TYPE_YELL,15586,"On ne passe pas !");
    }

	void JustReachedHome()
    {
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_KOLOGARN, FAIL);
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_LEFT_ARM))))
				if (!pTemp->isAlive())
					pTemp->Respawn();
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_RIGHT_ARM))))
				if (!pTemp->isAlive())
					pTemp->Respawn();
		}
    }

	void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if (Spell_Timer < diff)
        {
			if (right && left)
				DoCastVictim(!m_bIsRegularMode ? SPELL_OVERHEAD_SMASH : SPELL_OVERHEAD_SMASH_H);
			else
				if (!right && !left)
					DoCastVictim(!m_bIsRegularMode ? SPELL_STONE_SHOUT : SPELL_STONE_SHOUT_H);
				else
					DoCastVictim(!m_bIsRegularMode ? SPELL_ONE_ARMED_SMASH : SPELL_ONE_ARMED_SMASH_H);
            Spell_Timer = 20000;
        }
		else 
			Spell_Timer -= diff;   

		if (respawnleft < diff && !left)
        {
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_LEFT_ARM))))
				if (!pTemp->isAlive())
					pTemp->Respawn();
			//me->GetVehicleKit()->InstallAccessory(32933);
            left = true;
        }
		else 
			respawnleft -= diff;  

		if (respawnright < diff && !right)
        {
			if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_RIGHT_ARM))))
				if (!pTemp->isAlive())
					pTemp->Respawn();
			//me->GetVehicleKit()->InstallAccessory(32934);
            right = true;
        }
		else 
			respawnright -= diff; 

		if (Check_Timer < diff)
        {
			if (Creature* lArm = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_LEFT_ARM))))
				if (!lArm->isAlive() && left)
				{
					left = false;
					for(int i=0;i<5;i++)
						Tasks.CallCreature(MOB_RUBBLE,TEN_MINS,PREC_COORDS,AGGRESSIVE_MAIN,1776.96f,-44.839f,449.0f);
					respawnleft = 60000;
				}
			if (Creature* rArm = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_RIGHT_ARM))))
				if (!rArm->isAlive() && right)
				{
					right = false;
					for(int i=0;i<5;i++)
						Tasks.CallCreature(MOB_RUBBLE,TEN_MINS,PREC_COORDS,AGGRESSIVE_MAIN,1777.81f,-3.539f,449.0f);
					respawnright = 60000;
				}
			if (!me->IsWithinDistInMap(me->getVictim(), 10))
				DoCastVictim(m_bIsRegularMode ? SPELL_PETRIFYING_BREATH : SPELL_PETRIFYING_BREATH_H);
			Check_Timer = 500;
        }
		else 
			Check_Timer -= diff;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();
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
