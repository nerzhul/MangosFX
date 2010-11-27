#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	SPELL_SOUL_FEAST	=	71203,
};

struct MANGOS_DLL_DECL icc_the_damnedAI : public LibDevFSAI
{
    icc_the_damnedAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(70960,20000,90000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(70961);
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_icc_the_damned(Creature* pCreature)
{
    return new icc_the_damnedAI (pCreature);
}

struct MANGOS_DLL_DECL Deathbound_WardAI : public LibDevFSAI
{
    Deathbound_WardAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(71021,4000,5000,500);
		AddEventMaxPrioOnMe(71022,12000,15000);
    }

    void Reset()
    {
		ResetTimers();
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
    }

	void Aggro(Unit* pWho)
	{
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
	}

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_Deathbound_Ward(Creature* pCreature)
{
    return new Deathbound_WardAI (pCreature);
}

struct MANGOS_DLL_DECL Servant_of_the_ThroneAI : public LibDevFSAI
{
    Servant_of_the_ThroneAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(71029,2000,8000,1000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_Servant_of_the_Throne(Creature* pCreature)
{
    return new Servant_of_the_ThroneAI (pCreature);
}

struct MANGOS_DLL_DECL Ancient_Skeletal_SoldierAI : public LibDevFSAI
{
    Ancient_Skeletal_SoldierAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(70964,2000,12000,3000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_Ancient_Skeletal_Soldier(Creature* pCreature)
{
    return new Ancient_Skeletal_SoldierAI (pCreature);
}

struct MANGOS_DLL_DECL Nerubar_BroodkeeperAI : public LibDevFSAI
{
    Nerubar_BroodkeeperAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(71020,5000,12000,4000,HEAL_MY_FRIEND);
		AddEvent(70980,8000,20000);
    }

	uint32 web_Timer;

    void Reset()
    {
		ResetTimers();
		web_Timer = 11000;
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(web_Timer <= diff)
		{
			if(Unit* target = GetRandomUnit())
			{
				CallCreature(38028,THREE_MINS,PREC_COORDS,NOTHING,target->GetPositionX(),target->GetPositionY(),target->GetPositionZ());
				ModifyAuraStack(28622,1,target);
			}
			web_Timer = 20000;
		}
		else 
			web_Timer -= diff;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_Nerubar_Broodkeeper(Creature* pCreature)
{
    return new Nerubar_BroodkeeperAI (pCreature);
}

struct MANGOS_DLL_DECL Nerubar_webkeepAI : public LibDevFSAI
{
    Nerubar_webkeepAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
    }

    void UpdateAI(const uint32 diff)
    {
    }

	void JustDied(Unit* pwho)
	{
		FreeMan();
	}

	void FreeMan()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if(Player* pPlayer = itr->getSource())
				{
					if(!pPlayer->HasAura(28622))
						continue;

					if(pPlayer->GetDistance2d(me) > 4.0f)
						continue;

					if(pPlayer->isAlive())
					{
						pPlayer->RemoveAurasDueToSpell(28622);
						return;
					}
				}
	}
};

CreatureAI* GetAI_Nerubar_webkeep(Creature* pCreature)
{
    return new Nerubar_webkeepAI(pCreature);
}

struct MANGOS_DLL_DECL deathspeaker_servantAI : public LibDevFSAI
{
    deathspeaker_servantAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(69404,1000,3000,1000);
		AddEventOnTank(69576,2000,10000,1000);
		AddEvent(69405,5000,20000,1000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_deathspeaker_servant(Creature* pCreature)
{
    return new deathspeaker_servantAI (pCreature);
}

struct MANGOS_DLL_DECL deathspeaker_attendantAI : public LibDevFSAI
{
    deathspeaker_attendantAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(69387,1000,3000,500);
		AddMaxPrioEvent(69355,5000,10000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_deathspeaker_attendant(Creature* pCreature)
{
    return new deathspeaker_attendantAI (pCreature);
}

struct MANGOS_DLL_DECL deathspeaker_discipleAI : public LibDevFSAI
{
    deathspeaker_discipleAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddHealEventMaxPrio(69389,10000,10000,1000);
		AddHealEvent(69391,3000,5000,1000);
		AddEvent(69387,500,3000,1000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_deathspeaker_disciple(Creature* pCreature)
{
    return new deathspeaker_discipleAI (pCreature);
}

struct MANGOS_DLL_DECL deathspeaker_zeloteAI : public LibDevFSAI
{
    deathspeaker_zeloteAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(69492,1000,6000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_deathspeaker_zelote(Creature* pCreature)
{
    return new deathspeaker_zeloteAI (pCreature);
}

struct MANGOS_DLL_DECL deathspeaker_priestAI : public LibDevFSAI
{
    deathspeaker_priestAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(69483,5000,10000,5000);
    }

    void Reset()
    {
		ResetTimers();
		DoCastMe(69491);
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_deathspeaker_priest(Creature* pCreature)
{
    return new deathspeaker_priestAI (pCreature);
}

struct MANGOS_DLL_DECL blighted_abominationAI : public LibDevFSAI
{
    blighted_abominationAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(71140,1000,15000,5000);
		AddEventOnTank(40504,2000,3000);
		AddEvent(71150,4000,10000,2000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_blighted_abomination(Creature* pCreature)
{
    return new blighted_abominationAI (pCreature);
}

struct MANGOS_DLL_DECL vengeful_fleshreaperAI : public LibDevFSAI
{
    vengeful_fleshreaperAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(71164,1000,5000,2000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_vengeful_fleshreaper(Creature* pCreature)
{
    return new vengeful_fleshreaperAI(pCreature);
}

struct MANGOS_DLL_DECL plague_scientistAI : public LibDevFSAI
{
    plague_scientistAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(73079,3000,10000,2000);
		AddEvent(69871,12000,15000,2000);
		AddEvent(71103,6000,6000,1000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_plague_scientist(Creature* pCreature)
{
    return new plague_scientistAI(pCreature);
}

struct MANGOS_DLL_DECL decaying_colossusAI : public LibDevFSAI
{
    decaying_colossusAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(71114,3000,8000,2000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && pWho != me)
		{
			dmg = 0;
			DoCastMe(SPELL_SOUL_FEAST);
			Kill(me);
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

CreatureAI* GetAI_decaying_colossus(Creature* pCreature)
{
    return new decaying_colossusAI(pCreature);
}

enum TpDest
{
	BASE			=	0,
	MARROWGAR		=	1,
	DEATHWHISPER	=	2,
	BATTLECANON		=	3,
	SAURCROC		=	4,
	LICHKING		=	5,
};
bool GoHello_icc_teleporter( Player *pPlayer, GameObject *pGO )
{
    InstanceData* pInstance = pGO->GetInstanceData();
    if(!pInstance) return true;

    pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation au Marteau de Lumiere", GOSSIP_SENDER_MAIN, BASE);
	if(pInstance->GetData(TYPE_MARROWGAR) == DONE || pPlayer->isGameMaster())
    {
		pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation a La Fleche", GOSSIP_SENDER_MAIN, MARROWGAR);
		if(pInstance->GetData(TYPE_DEATHWHISPER) == DONE || pPlayer->isGameMaster())
		{
			pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation au Rempart des Cranes", GOSSIP_SENDER_MAIN, DEATHWHISPER);
			//if(pInstance->GetData(TYPE_BATTLE_OF_CANNONS) == DONE || pPlayer->isGameMaster())
			{
				pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation a la Cime du Porte-Mort", GOSSIP_SENDER_MAIN, BATTLECANON);
				if(pInstance->GetData(TYPE_SAURFANG) == DONE || pPlayer->isGameMaster())
				{
					pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation a La Fleche (partie haute)", GOSSIP_SENDER_MAIN, SAURCROC);
					if(pInstance->GetData(TYPE_SINDRAGOSA) == DONE || pPlayer->isGameMaster())
					{
						pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation au Trone de Glace", GOSSIP_SENDER_MAIN, LICHKING);
					}
				}
			}
		}
	}

    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pGO->GetGUID());

    return true;
}

bool GOSelect_icc_teleporter( Player *pPlayer, GameObject *pGO, uint32 sender, uint32 action )
{
    if(sender != GOSSIP_SENDER_MAIN) 
		return true;
    if(!pPlayer->getAttackers().empty()) 
		return true;

    switch(action)
    {
		case BASE:
			pPlayer->TeleportTo(631, -17.275f, 2211.47f, 30.116, 3.09f);
			pPlayer->CLOSE_GOSSIP_MENU(); break;
		case MARROWGAR:
			pPlayer->TeleportTo(631, -503.634f, 2211.42f, 62.83f, 2.89f);
			pPlayer->CLOSE_GOSSIP_MENU(); break;
		case DEATHWHISPER:
			pPlayer->TeleportTo(631, -615.383f, 2211.47f, 199.973f, 6.15f);
			pPlayer->CLOSE_GOSSIP_MENU(); break;
		case BATTLECANON:
			pPlayer->TeleportTo(631, -549.595f, 2211.331f, 539.290f, 0.01f);
			pPlayer->CLOSE_GOSSIP_MENU(); break;
		case SAURCROC:
			pPlayer->TeleportTo(631, 4199.126f, 2769.197f, 351.06f, 0.07f);
			pPlayer->CLOSE_GOSSIP_MENU(); break;
		case LICHKING:
			pPlayer->TeleportTo(631, 529.299f, -2124.699f, 1045.0f, 3.14f);
			pPlayer->CLOSE_GOSSIP_MENU(); break;
    }

    return true;
}

void AddSC_ICC10_mobs()
{
	Script *newscript;

	newscript = new Script;
    newscript->Name = "icc_decaying_colossus";
    newscript->GetAI = &GetAI_decaying_colossus;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_plague_scientist";
    newscript->GetAI = &GetAI_plague_scientist;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "icc_vengeful_fleshreaper";
    newscript->GetAI = &GetAI_vengeful_fleshreaper;
	newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_the_damned";
    newscript->GetAI = &GetAI_icc_the_damned;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_the_damned";
    newscript->GetAI = &GetAI_icc_the_damned;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_servant_of_the_throne";
    newscript->GetAI = &GetAI_Servant_of_the_Throne;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_ancient_skeletal_soldier";
    newscript->GetAI = &GetAI_Ancient_Skeletal_Soldier;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_nerubar_broodkeeper";
    newscript->GetAI = &GetAI_Nerubar_Broodkeeper;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_nerubar_webkeeper";
    newscript->GetAI = &GetAI_Nerubar_webkeep;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_deathspeaker_servant";
    newscript->GetAI = &GetAI_deathspeaker_servant;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_deathspeaker_attendant";
    newscript->GetAI = &GetAI_deathspeaker_attendant;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_deathspeaker_disciple";
    newscript->GetAI = &GetAI_deathspeaker_attendant;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_deathspeaker_zelote";
    newscript->GetAI = &GetAI_deathspeaker_zelote;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_deathspeaker_priest";
    newscript->GetAI = &GetAI_deathspeaker_priest;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_blighted_abomination";
    newscript->GetAI = &GetAI_blighted_abomination;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "icc_teleporter";
    newscript->pGOHello = &GoHello_icc_teleporter;
    newscript->pGOSelect = &GOSelect_icc_teleporter;
    newscript->RegisterSelf();
}
