#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    //summons
    NPC_BLOOD_BEASTS                        = 38508, // ok
	SPELL_RESISTANT_SKIN                    = 72723,
    SPELL_BLOOD_LINK_B                      = 72176,
	SPELL_SCENT_OF_BLOOD                    = 72769, // Only in heroic, beasts

    //Abilities
    SPELL_BLOOD_LINK                        = 72178,
    SPELL_BLOOD_POWER                       = 72371,
    SPELL_MARK                              = 72293, // ok
    SPELL_FRENZY                            = 72737, // OK
    SPELL_BOILING_BLOOD                     = 72385, // ok
    SPELL_BLOOD_NOVA                        = 72380, // ok
    SPELL_RUNE_OF_BLOOD                     = 72408, // ok
    
};

struct MANGOS_DLL_DECL boss_saurfangAI : public LibDevFSAI
{
    boss_saurfangAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(480000);
		AddTextEvent(16700,"",35000,35000);
		AddTextEvent(16698,"",480000,DAY*HOUR);
		me->setPowerType(POWER_RUNIC_POWER);
		me->SetMaxPower(POWER_RUNIC_POWER,100);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_10MAN_HEROIC:
				AddNear15mSummonEvent(NPC_BLOOD_BEASTS,35000,35000,0,0,2);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				AddNear15mSummonEvent(NPC_BLOOD_BEASTS,35000,35000,0,0,5);
				break;
		}
		AddEvent(SPELL_BOILING_BLOOD,15000,30000);
		AddEvent(SPELL_BLOOD_NOVA,20000,30000);
		AddEventOnTank(SPELL_RUNE_OF_BLOOD,10000,30000);
    }

	uint32 checkRunic_Timer;
    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		checkRunic_Timer = 1000;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, IN_PROGRESS);
		Yell(16694,"");
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Yell(16695,"");
		else
			Yell(16696,"");
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
		}

		Say(16697,"");
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(30) && !me->HasAura(SPELL_FRENZY))
			ModifyAuraStack(SPELL_FRENZY);

		if(checkRunic_Timer <= diff)
		{
			uint32 amount = me->GetPower(POWER_RUNIC_POWER);
			if(amount >= 99)
			{
				me->CastStop();
				DoCastVictim(SPELL_MARK);
				Yell(16699,"");
			}
			checkRunic_Timer = 1000;
		}
		else
			checkRunic_Timer -= diff;

		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_saurfang(Creature* pCreature)
{
    return new boss_saurfangAI(pCreature);
}

struct MANGOS_DLL_DECL ver_saurfangAI : public LibDevFSAI
{
    ver_saurfangAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_SCENT_OF_BLOOD,5000,30000);
    }

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
		ModifyAuraStack(SPELL_RESISTANT_SKIN);
		DoCastMe(SPELL_BLOOD_LINK_B);
    }

	void DamageDeal(Unit* pWho, uint32 &dmg)
	{
		if(pWho->GetTypeId() == TYPEID_PLAYER)
		{
			if(Creature* Saurfang = GetInstanceCreature(TYPE_SAURFANG))
				if(Saurfang->isAlive())
				{
					uint32 amount = Saurfang->GetPower(POWER_RUNIC_POWER) + 10;
					Saurfang->SetPower(POWER_RUNIC_POWER,amount > 100 ? 100 : amount);
				}
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_ver_saurfang(Creature* pCreature)
{
    return new ver_saurfangAI(pCreature);
}

void AddSC_ICC_Saurfang()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_saurfang";
    NewScript->GetAI = &GetAI_boss_saurfang;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "ver_saurfang";
    NewScript->GetAI = &GetAI_boss_saurfang;
    NewScript->RegisterSelf();
}