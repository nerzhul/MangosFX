#include "precompiled.h"
#include "ulduar.h"
 
enum FreyaSpells
{
	SPELL_ATTUNED_TO_NATURE		= 62519,
	SPELL_ENRAGE				= 47008,
	SPELL_GROUND_TREMOR			= 62437,
	SPELL_GROUND_TREMOR_H		= 62859,
	SPELL_SUMMON_ALLIES			= 62678, // NEED TO IMPLEMENT, NOW WORKS
	SPELL_SUNBEAM				= 62623,
	SPELL_SUNBEAM_H				= 62872,
	SPELL_TOUCH					= 62528,
	SPELL_TOUCH_H				= 62892,
	SPELL_PHOTOSYNTHESIS		= 62209,
	SPELL_UNSTABLE_ENERGY		= 62865,

};

enum FreyaAdds
{
	//vague Type 1 (il faut en pop 10)
	NPC_DETONATING_LASHER		= 32918,
	//vague Type 2 (les 3 toutes les minutes)
	NPC_WATER_SPIRIT			= 33202,
	NPC_STORM_LASHER			= 32919,
	NPC_SNAPLASHER				= 32916,
	//vague Type 3
	NPC_ANCIENT_CONSERVATOR		= 33203,
	NPC_SPORE					= 33215,
};

struct MANGOS_DLL_DECL boss_freyaAI : public LibDevFSAI
{
    boss_freyaAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(720000);
		AddTextEvent(15532,"Vous avez voulu aller trop loin, perdre trop de temps !",720000,DAY*HOUR);
		AddHealEvent(SPELL_PHOTOSYNTHESIS,5000,6000);
		AddNear15mSummonEvent(33228,20000,30000,0,15000);

		if(m_difficulty)
		{
			AddEvent(SPELL_GROUND_TREMOR_H,20000,15000,2000,TARGET_MAIN,2);
			AddEvent(SPELL_SUNBEAM_H,15000,10000,2000,TARGET_RANDOM,2);
			
		}
		else
		{
			AddEvent(SPELL_GROUND_TREMOR,20000,15000,2000,TARGET_MAIN,2);
			AddEvent(SPELL_SUNBEAM,15000,10000,2,TARGET_RANDOM,2);
		}
    }

	bool HardMode;
	uint8 phase;
	uint32 Vague_Timer;
	uint16 Vague_Count;
	uint32 Check_InLife_Timer;
	bool CheckInLife;
	std::vector<uint64> TriAdds;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		ModifyAuraStack(SPELL_ATTUNED_TO_NATURE,150);
		HardMode = false;
		TriAdds.clear();
		phase = 0;
		Vague_Timer = 5000;
		Check_InLife_Timer = 1500;
		CheckInLife = false;
		Vague_Count = 0;
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_FREYA, DONE);
		Say(15531,"Son emprise sur moi se dissipe. J'y vois de nouveau clair. Merci, héros");
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (pInstance)
            pInstance->SetData(TYPE_FREYA, IN_PROGRESS);

		Yell(15526,"Le Jardin doit être protégé !");
		// elder spécial :
		//15527

		phase = 1;
    }

	void HealBy(Unit* pHeal, uint32& heal)
	{
		error_log("HEALBY ?");
		if(me->HasAura(SPELL_ATTUNED_TO_NATURE))
		{
			uint8 stk = me->GetAura(SPELL_ATTUNED_TO_NATURE,0)->GetStackAmount();
			heal = heal * stk * 8 / 100;
		}
	}

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Say(15529,"Pardonnez moi.");
		else
			Say(15530,"De votre mort renaîtra la vie !");
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase == 1)
		{
			if(Check_InLife_Timer <= diff)
			{
				if(!TriAdds.empty())
				{
					uint8 AliveCount = 0;
					for(std::vector<uint64>::iterator itr = TriAdds.begin(); itr!= TriAdds.end(); ++itr)
					{
						if(Creature* cr = GetGuidCreature(*itr))
							if(cr->isAlive())
								AliveCount++;
					}
					if(!CheckInLife)
					{
						if(AliveCount > 0  && AliveCount < 3)
							CheckInLife = true;
					}
					else
					{
						CheckInLife = false;
						if(AliveCount > 0)
						{
							for(std::vector<uint64>::iterator itr = TriAdds.begin(); itr!= TriAdds.end(); ++itr)
							{
								if(Creature* cr = GetGuidCreature(*itr))
									if(!cr->isAlive())
										cr->Respawn();
							}
						}
					}
					
					Check_InLife_Timer = 10000;
				}

				Check_InLife_Timer = 1500;
			}
			else
				Check_InLife_Timer -= diff;

			if(Vague_Timer <= diff)
			{
				bool AllAlive = true;
				Vague_Count++;
				switch(Vague_Count)
				{
					case 1:
					case 3:
					case 5:
						Yell(15533,"Mes enfants, venez m'aider !");
						BossEmote(0,"Des allies de la Nature apparaissent !");
						for(int i=0;i<(m_difficulty ? 10 : 8);i++)
							CallCreature(NPC_DETONATING_LASHER,TEN_MINS,NEAR_15M);
						break;
					case 2:
						Yell(15534,"La nuée des éléments va vous submerger !");
						if(Creature* tmpCr = CallCreature(NPC_WATER_SPIRIT,TEN_MINS,NEAR_15M))
							TriAdds.push_back(tmpCr->GetGUID());
						if(Creature* tmpCr = CallCreature(NPC_STORM_LASHER,TEN_MINS,NEAR_15M))
							TriAdds.push_back(tmpCr->GetGUID());
						if(Creature* tmpCr = CallCreature(NPC_SNAPLASHER,TEN_MINS,NEAR_15M))
							TriAdds.push_back(tmpCr->GetGUID());
						break;
					case 4:
					case 6:
						Say(15528,"Eonar, ta servante a besoin d'aide");
						for(uint8 i=0;i<(m_difficulty ? 12 : 5);i++)
							CallCreature(30391,60000,NEAR_15M,NOTHING);
						CallCreature(NPC_ANCIENT_CONSERVATOR,TEN_MINS,NEAR_15M);
						break;
					case 7:
						for(std::vector<uint64>::iterator itr = TriAdds.begin(); itr!= TriAdds.end(); ++itr)
							if(Creature* cr = GetGuidCreature(*itr))
								if(!cr->isAlive())
									AllAlive = false;
						if(AllAlive)
							HardMode = true;

						phase++;
						break;
					default:
						break;

				}

				Vague_Timer = 60000;
			}
			else
				Vague_Timer -= diff;
		}

		UpdateEvent(diff);
		UpdateEvent(diff,phase);

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_freya(Creature* pCreature)
{
    return new boss_freyaAI(pCreature);
}

struct MANGOS_DLL_DECL detonating_lasherAI : public LibDevFSAI
{
    detonating_lasherAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		AddEventOnTank(62608,500,2500);
	}

	bool death;

    void Reset()
    {
		ResetTimers();
		death = false;
		AggroAllPlayers(150.0f);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(dmg >= me->GetHealth())
		{
			me->SetHealth(1);
			dmg = 0;
			if(!death)
			{
				death = true;
				DoCastMe(m_difficulty ? 62937 : 62598);
				if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
					if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
						ModifyAuraStack(SPELL_ATTUNED_TO_NATURE,-2,Freya);
				me->ForcedDespawn(800);
			}
		}
	}

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_detonating_lasher(Creature* pCreature)
{
    return new detonating_lasherAI(pCreature);
}

struct MANGOS_DLL_DECL freya_water_spiritAI : public LibDevFSAI
{
    freya_water_spiritAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		AddEventOnTank(m_difficulty ? 62935 : 62653,5000,15000,5000);
	}

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
    }

	void JustDied(Unit* pwho)
	{
		if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
			if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
				ModifyAuraStack(SPELL_ATTUNED_TO_NATURE,-10,Freya);
	}

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_freya_water_spirit(Creature* pCreature)
{
    return new freya_water_spiritAI(pCreature);
}

struct MANGOS_DLL_DECL freya_storm_lasherAI : public LibDevFSAI
{
    freya_storm_lasherAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		if(m_difficulty)
		{
			AddEvent(62649,500,4000,1000);
			AddEvent(62648,1000,4000,1000);
		}
		else
		{
			AddEvent(62938,500,4000,1000);
			AddEvent(62939,1000,4000,1000);
		}
	}

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
    }

	void JustDied(Unit* pwho)
	{
		if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
			if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
				ModifyAuraStack(SPELL_ATTUNED_TO_NATURE,-10,Freya);
	}

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_freya_storm_lasher(Creature* pCreature)
{
    return new freya_storm_lasherAI(pCreature);
}

struct MANGOS_DLL_DECL freya_snaplasherAI : public LibDevFSAI
{
    freya_snaplasherAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
	}

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
    }

	void EnterCombat(Unit* pWho)
	{
		DoCastMe(m_difficulty ? 64191 : 62664);
	}

	void JustDied(Unit* pwho)
	{
		if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
			if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
				ModifyAuraStack(SPELL_ATTUNED_TO_NATURE,-10,Freya);
	}

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_freya_snaplasher(Creature* pCreature)
{
    return new freya_snaplasherAI(pCreature);
}

struct MANGOS_DLL_DECL freya_ancient_conservatorAI : public LibDevFSAI
{
    freya_ancient_conservatorAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		AddEventOnTank(m_difficulty ? 63571 : 62589,8000,6000,2000);
		AddEventOnMe(62532,3000,120000);
	}

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
    }

	void JustDied(Unit* pwho)
	{
		if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
			if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
				ModifyAuraStack(SPELL_ATTUNED_TO_NATURE,-25,Freya);
	}

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_freya_ancient_conservator(Creature* pCreature)
{
    return new freya_ancient_conservatorAI(pCreature);
}

struct MANGOS_DLL_DECL freya_giftAI : public LibDevFSAI
{
    freya_giftAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
	}

	bool death;
	uint32 growth_Timer;

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(62619);
		ModifyAuraStack(SPELL_PHOTOSYNTHESIS);
		SetCombatMovement(false);
		me->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.1f);
		growth_Timer = 1000;
		AggroAllPlayers(150.0f);
		BossEmote(0,"Un cadeau d'Eonar commence a pousser");
    }
	
	void JustDied(Unit* pWho)
	{
		if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
			if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
				Freya->RemoveAurasDueToSpell(m_difficulty ? SPELL_TOUCH_H : SPELL_TOUCH);
	}

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		if(growth_Timer <= diff)
		{
			me->SetFloatValue(OBJECT_FIELD_SCALE_X,me->GetFloatValue(OBJECT_FIELD_SCALE_X) + 0.1f);
			if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
				if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
				{
					if(me->GetFloatValue(OBJECT_FIELD_SCALE_X) >= 2.0f)
					{
						DoCast(Freya,m_difficulty ? 64185 : 62584);
						Freya->SetHealth(Freya->GetHealth() + Freya->GetMaxHealth() * (m_difficulty ? 60 : 30) / 100);
						me->ForcedDespawn(800);
					}
					else
						ModifyAuraStack(m_difficulty ? SPELL_TOUCH_H : SPELL_TOUCH, 1, Freya);
				}
			growth_Timer = 1000;
			
		}
		else
			growth_Timer -= diff;

		UpdateEvent(diff);
	}
};

CreatureAI* GetAI_freya_gift(Creature* pCreature)
{
    return new freya_giftAI(pCreature);
}

struct MANGOS_DLL_DECL freya_mushroomAI : public LibDevFSAI
{
    freya_mushroomAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
	}

	bool death;
	uint32 growth_Timer;

    void Reset()
    {
		ResetTimers();
		MakeInvisibleStalker();
		me->setFaction(35);
		ModifyAuraStack(31690);
		DoCastMe(62619);
		SetCombatMovement(false);
		me->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.1f);
		growth_Timer = 1000;
    }

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		if(growth_Timer <= diff)
		{
			me->SetFloatValue(OBJECT_FIELD_SCALE_X,me->GetFloatValue(OBJECT_FIELD_SCALE_X) + 0.1f);
			if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
				if(Freya->isAlive() && pInstance && pInstance->GetData(TYPE_FREYA) == IN_PROGRESS)
				{
					
				}
			growth_Timer = 1000;
			
		}
		else
			growth_Timer -= diff;

		UpdateEvent(diff);
	}
};

CreatureAI* GetAI_freya_mushroom(Creature* pCreature)
{
    return new freya_mushroomAI(pCreature);
}

void AddSC_boss_freya()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_freya";
    newscript->GetAI = &GetAI_boss_freya;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_deton_lasher";
    newscript->GetAI = &GetAI_detonating_lasher;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_snaplasher";
    newscript->GetAI = &GetAI_freya_snaplasher;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_water_spirit";
    newscript->GetAI = &GetAI_detonating_lasher;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_storm_lasher";
    newscript->GetAI = &GetAI_freya_storm_lasher;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_gift";
    newscript->GetAI = &GetAI_freya_gift;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_mushroom";
    newscript->GetAI = &GetAI_freya_mushroom;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_ancient_conservator";
    newscript->GetAI = &GetAI_freya_ancient_conservator;
    newscript->RegisterSelf();
}
