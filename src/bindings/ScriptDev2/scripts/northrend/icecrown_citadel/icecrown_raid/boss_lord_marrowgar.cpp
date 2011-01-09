#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_SABER_LASH     		= 69055,
    SPELL_COLDFLAME      		= 69146,
    SPELL_BONE_SPIKE     		= 69057, 
    SPELL_BONE_SPIKE_IMPALE		= 69065, 
    SPELL_BONE_STORM     		= 69076,
    
    SPELL_BERSERK               = 47008,
    
    NPC_COLDFLAME        		= 36672,
    NPC_BONE_SPIKE              = 38711,
};

struct MANGOS_DLL_DECL boss_marrowgarAI : public LibDevFSAI
{
    boss_marrowgarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        switch(m_difficulty)
        {
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				FlameDespawn = 8000;
				break;
			default:
				FlameDespawn = 3000;
				break;
		}
		AddEventOnTank(SPELL_SABER_LASH,5000,5000);
		AddMaxPrioEvent(SPELL_BONE_SPIKE,15000,60000);
		AddEnrageTimer(TEN_MINS);
    }
	
	uint8 phase;
	uint8 FlameNb;
	uint32 FlameDespawn;
	uint32 Flame_Timer;
	uint32 Storm_Timer;
	uint32 StormTarget_Timer;
	uint32 enrage_Timer;
	uint32 boneSpike_Timer;
	uint32 StormDmg_Timer;
	uint32 FlameSpawn_Timer;
	float ecartX;
	float ecartY;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		Flame_Timer = 7000;
		Storm_Timer = 27000;
		StormTarget_Timer = 15000;
		StormDmg_Timer = 2000;
		phase = 0;
		FlameNb = 2;
		enrage_Timer = TEN_MINS;
		FlameSpawn_Timer = DAY*HOUR;
		boneSpike_Timer = 18000;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            SetInstanceData(TYPE_MARROWGAR, IN_PROGRESS);

		Yell(16941,"Le fléau va déferler sur ce monde, dans un torrent de mort, et de destruction !");
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Say(16942,"Toujours plus d'os pour les offrandes");
		else
			Yell(16943,"Morfondez vous, dans la damnation !");
	}

    void JustDied(Unit* pKiller)
    {
        if(pInstance)
            SetInstanceData(TYPE_MARROWGAR, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,1);
				GiveEmblemsToGroup(TRIOMPHE,1);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,2);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				GiveEmblemsToGroup(TRIOMPHE,1);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
		}

		Yell(16944,"Je ne vois... que les ténèbres...");
    }

    void JustReachedHome()
    {
        if (pInstance)
            SetInstanceData(TYPE_MARROWGAR, FAIL);
    }

	void SpawnBoneSpikes()
	{
		switch(urand(0,2))
		{
			case 0:
				Yell(16947,"Liés, par les os !");
				break;
			case 1:
				Yell(16948,"Ne vous sauvez pas !");
				break;
			case 2:
				Yell(16949,"La seule issue est... la mort !");
				break;
		}
		uint8 number = 1;
		if(m_difficulty == RAID_DIFFICULTY_25MAN_HEROIC || m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
			number = 3;

		uint8 security = 0;
		for(uint8 i=0;i<number;i++)
		{
			security++;
			if(Unit* spTarget = GetRandomUnit())
			{
				if(spTarget->HasAura(SPELL_BONE_SPIKE_IMPALE)  || spTarget == me->getVictim())
				{
					i--;
					if(security > 200)
						break;
					else
						continue;
				}
				else
				{
					CallCreature(NPC_BONE_SPIKE,TEN_MINS,PREC_COORDS,NOTHING,spTarget->GetPositionX(),spTarget->GetPositionY(),spTarget->GetPositionZ(),true);
					ModifyAuraStack(SPELL_BONE_SPIKE_IMPALE,1,spTarget);
				}
			}
		}
	}

	void CallColdFlames()
	{
		Unit* flameTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
		uint8 security = 0;
		
		if(!flameTarget)
			return;
		
		float dist = flameTarget->GetDistance(me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
		ecartX = (flameTarget->GetPositionX() - me->GetPositionX());
		ecartY = (flameTarget->GetPositionY() - me->GetPositionY());
		float coef = 3/dist;
		ecartX *= coef;
		ecartY *= coef;

		FlameSpawn_Timer = 50;
	}

	void SpawnFlame()
	{
		CallCreature(NPC_COLDFLAME,FlameDespawn,PREC_COORDS,NOTHING,me->GetPositionX() + ecartX * FlameNb, me->GetPositionY() + ecartY * FlameNb, me->GetPositionZ() + 1.0f,true);
		if(phase == 1)
		{
			CallCreature(NPC_COLDFLAME,FlameDespawn,PREC_COORDS,NOTHING,me->GetPositionX() - ecartX * FlameNb, me->GetPositionY() + ecartY * FlameNb, me->GetPositionZ() + 1.0f,true);
			CallCreature(NPC_COLDFLAME,FlameDespawn,PREC_COORDS,NOTHING,me->GetPositionX() + ecartX * FlameNb, me->GetPositionY() - ecartY * FlameNb, me->GetPositionZ() + 1.0f,true);
			CallCreature(NPC_COLDFLAME,FlameDespawn,PREC_COORDS,NOTHING,me->GetPositionX() - ecartX * FlameNb, me->GetPositionY() - ecartY * FlameNb, me->GetPositionZ() + 1.0f,true);
		}
		FlameNb++;
		if(FlameNb == 19)
		{
			FlameNb = 2;
			FlameSpawn_Timer = DAY*HOUR;
		}
		else
			FlameSpawn_Timer = 150;
	}

	void DoStormDamage()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if(Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
					{
						uint32 FullDamage = 15000;
						if(m_difficulty == RAID_DIFFICULTY_25MAN_HEROIC || m_difficulty == RAID_DIFFICULTY_10MAN_HEROIC)
							FullDamage = 25000;

						if(pPlayer->GetDistance2d(me) < 5.0f)
							FullDamage /= 2;
						else if(pPlayer->GetDistance2d(me) < 10.0f)
							FullDamage /= 3;
						else if(pPlayer->GetDistance2d(me) < 15.0f)
							FullDamage /= 4;
						else if(pPlayer->GetDistance2d(me) < 20.0f)
							FullDamage /= 7;
						else if(pPlayer->GetDistance2d(me) < 30.0f)
							FullDamage /= 10;
						else 
							FullDamage = 0;
						pPlayer->DealDamage(pPlayer, FullDamage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, GetSpellStore()->LookupEntry(69075), false);
					}
	}
    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(enrage_Timer <= diff)
		{
			Yell(16945,"La rage du maître coule dans mes veines !");
			enrage_Timer = DAY*HOUR;
		}
		else
			enrage_Timer -= diff;

		if(boneSpike_Timer <= diff)
		{
			SpawnBoneSpikes();
			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_10MAN_HEROIC:
				case RAID_DIFFICULTY_25MAN_HEROIC:
					boneSpike_Timer = 45000;
					break;
				default:
					boneSpike_Timer = 60000;
			}
		}
		else 
			boneSpike_Timer -= diff;
        
        if(Flame_Timer <= diff)
		{
			CallColdFlames();
			Flame_Timer = 3000;
		}
		else
			Flame_Timer -= diff;

		if(FlameSpawn_Timer <= diff)
		{
			SpawnFlame();
		}
		else
			FlameSpawn_Timer -= diff;
				
        if(phase == 0)
        {
			if(Storm_Timer <= diff)
			{
				phase = 1;
				me->CastStop();
				DoCastMe(SPELL_BONE_STORM);
				Yell(16946,"Tempête, dans un verre d'os !");
				BossEmote(0,"Gargamoelle commence a incanter une tempete d'os");
				Storm_Timer = 20000;
				me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
				StormTarget_Timer = 5000;
				StormDmg_Timer = 2000;
			}
			else
				Storm_Timer -= diff;
			
			UpdateEvent(diff);
			DoMeleeAttackIfReady();
		}
		else if(phase == 1)
		{
			if(StormTarget_Timer <= diff)
			{
				DoResetThreat();
				Unit* StormTarget = GetRandomUnit();
				if(StormTarget)
					me->AddThreat(StormTarget,100000.0f);
				StormTarget_Timer = 5500;
			}
			else
				StormTarget_Timer -= diff;

			if(StormDmg_Timer <= diff)
			{
				DoStormDamage();
				StormDmg_Timer = 2000;
			}
			else
				StormDmg_Timer -= diff;
				
			if(Storm_Timer <= diff)
			{
				phase = 0;
				DoResetThreat();
				me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
				me->RemoveAurasDueToSpell(SPELL_BONE_STORM);
				Storm_Timer = 40000;
			}
			else
				Storm_Timer -= diff;
		}
    }
};

CreatureAI* GetAI_boss_marrowgar(Creature* pCreature)
{
    return new boss_marrowgarAI(pCreature);
}

struct MANGOS_DLL_DECL flame_marrowgarAI : public LibDevFSAI
{
    flame_marrowgarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(SPELL_COLDFLAME,1200,3000);
		me->setFaction(2212);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		MakeHostileInvisibleStalker();
    }
	
    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		AggroAllPlayers(150.0f);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_flame_marrowgar(Creature* pCreature)
{
    return new flame_marrowgarAI(pCreature);
}

struct MANGOS_DLL_DECL bonespike_marrowgarAI : public LibDevFSAI
{
    bonespike_marrowgarAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		me->setFaction(2212);
    }
	
    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		AggroAllPlayers(150.0f);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }

	void Disempale()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			{
				if(Player* pPlayer = itr->getSource())
				{
					if(!pPlayer->HasAura(SPELL_BONE_SPIKE_IMPALE))
						continue;

					// no bug abuse on bumps :)
					if(pPlayer->GetDistance2d(me) > 3.0f)
						continue;

					if(pPlayer->isAlive())
					{
						pPlayer->RemoveAurasDueToSpell(SPELL_BONE_SPIKE_IMPALE);
						return;
					}
				}
			}
		}
	}

	void JustDied(Unit* killer)
	{
		Disempale();
	}
};

CreatureAI* GetAI_bonespike_marrowgar(Creature* pCreature)
{
    return new bonespike_marrowgarAI(pCreature);
}

void AddSC_boss_marrowgar()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_marrowgar";
    NewScript->GetAI = &GetAI_boss_marrowgar;
    NewScript->RegisterSelf();
    
    NewScript = new Script;
    NewScript->Name = "flame_marrowgar";
    NewScript->GetAI = &GetAI_flame_marrowgar;
    NewScript->RegisterSelf();
    
    NewScript = new Script;
    NewScript->Name = "bonespike_marrowgar";
    NewScript->GetAI = &GetAI_bonespike_marrowgar;
    NewScript->RegisterSelf();
}
