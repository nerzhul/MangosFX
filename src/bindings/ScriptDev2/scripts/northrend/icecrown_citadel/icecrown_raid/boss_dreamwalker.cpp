#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	// Dreamwalker
	SPELL_NIGHTMARE_PORTAL		=	72224,
	SPELL_DREAMWALKER_RAGE		=	71189,
	SPELL_DREAMWALKER_CORRUPT	=	70904,

	SPELL_DREAM_STATE			=	70766,
	SPELL_EMERALD_VIGOR			=	70873,

	SPELL_DISAPEAR				=	71196,
	SPELL_POP_CHEST				=	71207,

	// adds
	// Archmages
	SPELL_FROSTBOLT_VOLLEY		=	70759,
	SPELL_MANA_VOID				=	71085,
	NPC_MANA_VOID				=	38068,
	SPELL_COLUMN_OF_FROST		=	70702,
	// Blazing Skeletons
	SPELL_LAY_WASTE				=	69325,
	SPELL_FIREBALL				=	70754,
	// Suppressors
	SPELL_SUPPRESSION			=	70588,
	// Blistering Zombies
	SPELL_CORROSION				=	70751,
	SPELL_ACID_BURST			=	70744,
	// Gluttonous Abominations
	SPELL_GUT_SPRAY				=	71283,
	// Rot
	NPC_ROT_WORM				=	37907,
	SPELL_FLESH_ROT				=	72963,
};

const static float SpawnLoc[6][3]=
{
    {4203.470215f, 2484.500000f, 364.872009f},  // 0 Valithria
    {4240.688477f, 2405.794678f, 364.868591f},  // 1 Valithria Room 1 raid 25
    {4165.112305f, 2405.872559f, 364.872925f},  // 2 Valithria Room 2 raid 10
    {4166.216797f, 2564.197266f, 364.873047f},  // 3 Valithria Room 3 raid 10
    {4239.579102f, 2566.753418f, 364.868439f},  // 4 Valithria Room 4 raid 25
};

const static uint32 SpawnId[5] = {36791,37863,37868,37886,37934};

struct MANGOS_DLL_DECL boss_dreamwalkerAI : public LibDevFSAI
{
    boss_dreamwalkerAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		SetCombatMovement(false);
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_10MAN_HEROIC:
				raid25 = false;
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				raid25 = true;
				break;
		}
    }

	uint32 pop_Timer;
	bool raid25;
	bool eventStarted;
	bool lowHsaid;
	bool highHsaid;
	uint16 eventStep;
	uint32 OpenPortal_Timer;

    void Reset()
    {
		ResetTimers();
		pop_Timer = 12000;
		eventStarted = false;
		lowHsaid = false;
		highHsaid = false;
		eventStep = 0;
		OpenPortal_Timer = 40000;
		me->SetHealth(me->GetMaxHealth() / 2);
		SetInstanceData(TYPE_DREAMWALKER,NOT_STARTED);
    }

	void KilledUnit(Unit* who)
	{
		if(who->GetTypeId() == TYPEID_PLAYER)
			Yell(17066,"Une perte tragique...");
		else
			Yell(17065,"Pas de répit pour le mal !");
	}

	void LaunchEvent()
	{
		eventStarted = true;
		pop_Timer = 12000;
		if(Creature* LichKing = GetInstanceCreature(TYPE_LICHKING))
			Yell(17251,"Des intrus se sont introduits dans le sanctuaire. Hatez vous d'achever le dragon vert. Ne gardez que les os et les tendons pour la réanimation !",LichKing);
		ModifyAuraStack(SPELL_DREAMWALKER_CORRUPT);
		SetInstanceData(TYPE_DREAMWALKER, IN_PROGRESS);
		CallCreature(NPC_DREAMWALKER_IMAGE,TEN_MINS,PREC_COORDS,NOTHING,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+15.0f);
	}

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(GetPercentLife() < 25 && !lowHsaid)
		{
			Yell(17069,"Je ne tiendrai plus très longtemps.");
			lowHsaid = true;
		}

		if(dmg >= me->GetHealth())
		{
			dmg = 0;
			SetInstanceData(TYPE_DREAMWALKER, FAIL);
			eventStarted = false;
			Yell(17072,"Pardonnez moi je ne peux pas... Seuls subsistent les cauchemars !");
		}
	}

	void HealBy(Unit* pHealer, uint32 &heal)
	{
		if(pInstance && !(pInstance->GetData(TYPE_DREAMWALKER) == IN_PROGRESS))
		{
			heal = 0;
			return;
		}

		if(GetPercentLife() > 75 && !highHsaid)
		{
			Yell(17070,"Mes forces me reviennent. Continuez, héros !");
			highHsaid = true;
		}

		if(GetPercentLife() > 99 && pInstance && pInstance->GetData(TYPE_DREAMWALKER) == IN_PROGRESS)
		{
			me->RemoveAurasDueToSpell(SPELL_DREAMWALKER_CORRUPT);
			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_10MAN_NORMAL:
					GiveEmblemsToGroup(TRIOMPHE,2);
					GiveEmblemsToGroup(GIVRE,1);
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
			eventStarted = false;
			pop_Timer = 5000;
			eventStep = 0;
			DoCastMe(SPELL_DREAMWALKER_RAGE);
			Yell(17071,"Je revis ! Ysera accorde moi ta faveur que je donne le repos à ces horribles créatures !");
			SetInstanceData(TYPE_DREAMWALKER, DONE);
		}    
	}

	void PopAdds()
	{
		uint8 addType = urand(0,4);
		CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0],SpawnLoc[2][1],SpawnLoc[2][2]);
		if(SpawnId[addType] == 37863)
		{
			CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]+1.0f,SpawnLoc[2][1]+1.0f,SpawnLoc[2][2]);
			CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]-1.0f,SpawnLoc[2][1]-1.0f,SpawnLoc[2][2]);
		}
		addType = urand(0,4);
		CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[3][0],SpawnLoc[3][1],SpawnLoc[3][2]);
		if(SpawnId[addType] == 37863)
		{
			CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]+1.0f,SpawnLoc[2][1]+1.0f,SpawnLoc[2][2]);
			CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]-1.0f,SpawnLoc[2][1]-1.0f,SpawnLoc[2][2]);
		}
		if(raid25)
		{
			addType = urand(0,4);
			CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[1][0],SpawnLoc[1][1],SpawnLoc[1][2]);
			if(SpawnId[addType] == 37863)
			{
				CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]+1.0f,SpawnLoc[2][1]+1.0f,SpawnLoc[2][2]);
				CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]-1.0f,SpawnLoc[2][1]-1.0f,SpawnLoc[2][2]);
			}
			addType = urand(0,4);
			CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[4][0],SpawnLoc[4][1],SpawnLoc[4][2]);
			if(SpawnId[addType] == 37863)
			{
				CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]+1.0f,SpawnLoc[2][1]+1.0f,SpawnLoc[2][2]);
				CallCreature(SpawnId[addType],TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[2][0]-1.0f,SpawnLoc[2][1]-1.0f,SpawnLoc[2][2]);
			}
		}
	}

	void OpenPortals()
	{
		DoCastMe(SPELL_NIGHTMARE_PORTAL);
		Yell(17068,"J'ai ouvert un portail vers le rêve. Vous y trouverez votre salut, héros.");
	}

    void UpdateAI(const uint32 diff)
    {
		if(eventStarted)
		{
			if(pop_Timer <= diff)
			{
				if(eventStep == 0)
				{
					Yell(17064,"Héros, venez moi en aide. Je... je ne pourrai pas tenir plus longtemps, vous devez soigner mes blessures !");
					me->SetHealth(me->GetMaxHealth() / 2);
					me->RemoveAurasDueToSpell(SPELL_DREAMWALKER_CORRUPT);
					ModifyAuraStack(SPELL_DREAMWALKER_CORRUPT);
					pop_Timer = 6000;
				}
				eventStep++;

				if(eventStep > 1)
					PopAdds();

				if(eventStep >= 20)
					pop_Timer = 9000;
				if(eventStep >= 14)
					pop_Timer = 11000;
				else if(eventStep >= 8)
					pop_Timer = 13000;
				else if(eventStep > 1)
					pop_Timer = 15000;
			}
			else
				pop_Timer -= diff;

			if(OpenPortal_Timer <= diff)
			{
				OpenPortals();
				OpenPortal_Timer = 30000;
			}
			else
				OpenPortal_Timer -= diff;
		}
		else
		{
			if(pInstance && pInstance->GetData(TYPE_DREAMWALKER) == DONE)
			{
				if(pop_Timer <= diff)
				{
					switch(eventStep)
					{
						case 0:
							DoCastMe(SPELL_DISAPEAR);
							pop_Timer = 4000;
							break;
						case 1:
							DoCastMe(SPELL_POP_CHEST);
							pop_Timer = DAY*HOUR;
							break;
					}
					eventStep++;
				}
				else
					pop_Timer -= diff;
			}
		}
    }
};

CreatureAI* GetAI_boss_dreamwalker(Creature* pCreature)
{
    return new boss_dreamwalkerAI(pCreature);
}

struct MANGOS_DLL_DECL dw_archmageAI : public LibDevFSAI
{
    dw_archmageAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_FROSTBOLT_VOLLEY,10000,20000,5000);
		AddEvent(SPELL_COLUMN_OF_FROST,5000,12000,2000);
    }

    void Reset()
    {
		ResetTimers();
		if(me->GetDBTableGUIDLow() != 0)
			if(pInstance && pInstance->GetData(DATA_DREAMWALKER_GUARD) == 0 && pInstance->GetData(TYPE_DREAMWALKER) == FAIL)
				if(Creature* Dreamwalker = GetInstanceCreature(TYPE_DREAMWALKER))
					((boss_dreamwalkerAI*)Dreamwalker->AI())->Reset();
		AggroAllPlayers();
    }

	void JustDied(Unit* pWho)
	{
		if(me->GetDBTableGUIDLow() != 0)
		{
			SetInstanceData(DATA_DREAMWALKER_GUARD,DONE);
			if(pInstance && pInstance->GetData(DATA_DREAMWALKER_GUARD) == 4 && !(pInstance->GetData(TYPE_DREAMWALKER) == IN_PROGRESS))
				if(Creature* Dreamwalker = GetInstanceCreature(TYPE_DREAMWALKER))
					((boss_dreamwalkerAI*)Dreamwalker->AI())->LaunchEvent();
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

CreatureAI* GetAI_boss_dw_archmage(Creature* pCreature)
{
    return new dw_archmageAI(pCreature);
}

struct MANGOS_DLL_DECL dw_manavoidAI : public LibDevFSAI
{
    dw_manavoidAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		SetCombatMovement(false);
    }

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(SPELL_MANA_VOID);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_manavoid(Creature* pCreature)
{
    return new dw_manavoidAI(pCreature);
}

struct MANGOS_DLL_DECL dw_blazingskeletonsAI : public LibDevFSAI
{
    dw_blazingskeletonsAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventMaxPrioOnMe(SPELL_LAY_WASTE,5000,17000,5000);
		AddEventOnTank(SPELL_FIREBALL,1000,2000,3000);
    }

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_blazingskeletons(Creature* pCreature)
{
    return new dw_blazingskeletonsAI(pCreature);
}

struct MANGOS_DLL_DECL dw_suppressorAI : public LibDevFSAI
{
    dw_suppressorAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_SUPPRESSION,2000,20000);
    }

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_suppressor(Creature* pCreature)
{
    return new dw_suppressorAI(pCreature);
}

struct MANGOS_DLL_DECL dw_zombieAI : public LibDevFSAI
{
    dw_zombieAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_CORROSION,3000,5000);
    }

	bool die;
    void Reset()
    {
		ResetTimers();
		die = false;
		AggroAllPlayers();
    }

	void DamageTaken(Unit* pDoneby, uint32 &dmg)
	{
		if(dmg >= me->GetHealth())
		{
			dmg = 0;
			if(!die)
			{
				DoCastMe(SPELL_ACID_BURST);
				die = true;
				me->ForcedDespawn(500);
			}
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

CreatureAI* GetAI_boss_dw_zombie(Creature* pCreature)
{
    return new dw_zombieAI(pCreature);
}

struct MANGOS_DLL_DECL dw_abominationAI : public LibDevFSAI
{
    dw_abominationAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_GUT_SPRAY,3000,10000,6000);
    }

	bool die;
    void Reset()
    {
		ResetTimers();
		die = false;
		AggroAllPlayers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth())
		{
			if(!die)
			{
				dmg = 0;
				for(uint8 i=0;i<urand(8,10);i++)
					CallCreature(NPC_ROT_WORM,THREE_MINS);
				me->ForcedDespawn(1000);
				die = true;
			}
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

CreatureAI* GetAI_boss_dw_abomination(Creature* pCreature)
{
    return new dw_abominationAI(pCreature);
}

struct MANGOS_DLL_DECL dw_fleshrotAI : public LibDevFSAI
{
    dw_fleshrotAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(72962);
		AggroAllPlayers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dw_fleshrot(Creature* pCreature)
{
    return new dw_fleshrotAI(pCreature);
}

struct MANGOS_DLL_DECL dw_portalAI : public LibDevFSAI
{
    dw_portalAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		SetCombatMovement(false);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    }

	uint32 activate_Timer;

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(71304);
		activate_Timer = 15000;
    }

    void UpdateAI(const uint32 diff)
    {
        if(activate_Timer <= diff)
		{
			ModifyAuraStack(70763);
			me->ForcedDespawn(7000);
			me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			activate_Timer = DAY*HOUR;
		}
		else
			activate_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_dw_portal(Creature* pCreature)
{
    return new dw_portalAI(pCreature);
}

bool GossipHello_dw_portal(Player* pPlayer, Creature* pCreature)
{
	((dw_portalAI*)pCreature->AI())->ModifyAuraStack(SPELL_DREAM_STATE,1,pPlayer);
	pCreature->ForcedDespawn(100);
    return true;
}

struct MANGOS_DLL_DECL dw_imageAI : public LibDevFSAI
{
    dw_imageAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		AddEventOnMe(SPELL_EMERALD_VIGOR,1000,3000);
        InitInstance();
		SetCombatMovement(false);
		SetFlying(true);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		
    }

	uint32 CheckP_Timer;

    void Reset()
    {
		ResetTimers();
		CheckP_Timer = 1000;
    }

	void CheckPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();

		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->GetPhaseMask() == 16)
						ModifyAuraStack(SPELL_EMERALD_VIGOR,1,pPlayer);
	}

    void UpdateAI(const uint32 diff)
    {
		if(CheckP_Timer <= diff)
		{
			CheckPlayers();
			CheckP_Timer = 3000;
		}
		else
			CheckP_Timer -= diff;
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_dreamwalker_image(Creature* pCreature)
{
    return new dw_imageAI(pCreature);
}

void AddSC_ICC_DreamWalker()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_dreamwalker";
    NewScript->GetAI = &GetAI_boss_dreamwalker;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_archmage";
    NewScript->GetAI = &GetAI_boss_dw_archmage;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_manavoid";
    NewScript->GetAI = &GetAI_boss_dw_manavoid;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_blazingskeleton";
    NewScript->GetAI = &GetAI_boss_dw_blazingskeletons;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_suppressor";
    NewScript->GetAI = &GetAI_boss_dw_suppressor;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_zombie";
    NewScript->GetAI = &GetAI_boss_dw_zombie;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_abomination";
    NewScript->GetAI = &GetAI_boss_dw_abomination;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_fleshrot";
    NewScript->GetAI = &GetAI_boss_dw_fleshrot;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dreamwalker_image";
    NewScript->GetAI = &GetAI_boss_dreamwalker_image;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "dw_portal";
    NewScript->GetAI = &GetAI_boss_dw_portal;
	NewScript->pGossipHello = &GossipHello_dw_portal;
    NewScript->RegisterSelf();
}
