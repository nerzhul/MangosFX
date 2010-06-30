#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    //summons
	SPELL_BLOOD_BEASTS_1					= 72173,
	SPELL_BLOOD_BEASTS_2					= 72172,
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
    SPELL_BLOOD_NOVA                        = 72378, // ok
    SPELL_RUNE_OF_BLOOD                     = 72410, // ok
    
};

struct MANGOS_DLL_DECL boss_saurfangAI : public LibDevFSAI
{
    boss_saurfangAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(480000);
		AddTextEvent(16700,"Redressez vous, mes serviteurs",25000,35000);
		AddTextEvent(16698,"Je deviens... la mort !",480000,TEN_MINS*10);
		me->setPowerType(POWER_RUNIC_POWER);
		me->SetMaxPower(POWER_RUNIC_POWER,1000);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_10MAN_HEROIC:
				AddEventOnMe(SPELL_BLOOD_BEASTS_1,25000,40000);
				AddEventOnMe(SPELL_BLOOD_BEASTS_2,25500,40000);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				AddEventOnMe(SPELL_BLOOD_BEASTS_1,25000,40000);
				AddEventOnMe(SPELL_BLOOD_BEASTS_2,25300,40000);
				AddEventOnMe(SPELL_BLOOD_BEASTS_1,25600,40000);
				AddEventOnMe(SPELL_BLOOD_BEASTS_2,25900,40000);
				AddEventOnMe(SPELL_BLOOD_BEASTS_1,26200,40000);
				break;
		}
		AddEvent(SPELL_BOILING_BLOOD,15000,18000,4000);
		AddEvent(SPELL_BLOOD_NOVA,20000,30000);
		AddEventOnTank(SPELL_RUNE_OF_BLOOD,10000,20000);
    }

	uint32 checkRunic_Timer;
    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		me->SetPower(POWER_RUNIC_POWER,0);
		checkRunic_Timer = 1000;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, IN_PROGRESS);
		DoCastMe(SPELL_BLOOD_LINK);
		Yell(16694,"Par la puissance du Roi Liche");
    }

	void DamageDeal(Unit* pWho, uint32 &dmg)
	{
		if(dmg > 0)
		{
			InflictDamageToMarkedPlayers();

			if(me->HasAura(SPELL_BLOOD_POWER))
				dmg *= (1 + float(me->GetPower(POWER_RUNIC_POWER)) / 1000);

			if(pWho->HasAura(SPELL_RUNE_OF_BLOOD))
			{
				uint32 _dmg = urand(5100,6900);
				DealDamage(pWho,_dmg);
				me->SetHealth(me->GetHealth() + _dmg * 10);
			}

			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
				case RAID_DIFFICULTY_25MAN_NORMAL:
					if(dmg >= pWho->GetHealth())
						me->SetHealth(me->GetHealth() + 5 / 100 * me->GetMaxHealth());
					IncreasePower(5);
					if(pWho->HasAura(SPELL_BOILING_BLOOD))
						IncreasePower(3);
					break;
				case RAID_DIFFICULTY_10MAN_HEROIC:
				case RAID_DIFFICULTY_25MAN_HEROIC:
					if(dmg >= pWho->GetHealth())
						me->SetHealth(me->GetHealth() + 20 / 100 * me->GetMaxHealth());
					IncreasePower(3);
					if(pWho->HasAura(SPELL_BOILING_BLOOD))
						IncreasePower(2);
					break;
			}
			
		}
	}
	
	void InflictDamageToMarkedPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			{
				if(Player* plr = itr->getSource())
				{
					if(plr->HasAura(SPELL_BLOOD_POWER))
					{
						switch(m_difficulty)
						{
							case RAID_DIFFICULTY_10MAN_NORMAL:
							case RAID_DIFFICULTY_25MAN_NORMAL:
								DealDamage(plr,urand(4275,4725));
								break;
							case RAID_DIFFICULTY_10MAN_HEROIC:
							case RAID_DIFFICULTY_25MAN_HEROIC:
								DealDamage(plr,urand(6125,6825));
								break;
						}
					}
				}
			}
		}
	}

	void IncreasePower(uint32 val)
	{
		me->ModifyPower(POWER_RUNIC_POWER,val*10);
	}

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Yell(16695,"Vous n'êtes rien !");
		else
			Yell(16696,"Ici, pas de salut pour les âmes !");
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

		Say(16697,"Je... suis... libre...");
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
			if(amount >= 999)
			{
				me->CastStop();
				me->SetPower(POWER_RUNIC_POWER,0);
				DoCastRandom(SPELL_MARK);
				ModifyAuraStack(SPELL_BLOOD_POWER);
				Yell(16699,"La terre est rouge de votre sang !");
			}
			checkRunic_Timer = 8000;
		}
		else
			checkRunic_Timer -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
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
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				AddEventOnMe(SPELL_SCENT_OF_BLOOD,5000,30000);
				break;
		}
		
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
					Saurfang->ModifyPower(POWER_RUNIC_POWER,100);
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
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
    NewScript->GetAI = &GetAI_ver_saurfang;
    NewScript->RegisterSelf();
}