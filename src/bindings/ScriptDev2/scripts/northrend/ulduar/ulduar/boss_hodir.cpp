#include "precompiled.h"
#include "ulduar.h"

enum
{
	SPELL_BERSERK		= 26662,
	SPELL_BITING_COLD	= 62039,
	SPELL_FLASH			= 61968,
	SPELL_BLOW			= 62478,
	SPELL_BLOW_H		= 63512,
	SPELL_FREEZE		= 62469,
	SPELL_ICICLE		= 62460,
	SPELL_INSTANT_COLD	= 62148,
	SPELL_HODIR_FUROR	= 62297,
	//SPELL_HODIR_FUROR_2 = 62533,

	SPELL_STALAGMITE	= 69424,

	NPC_STALAGMITE		= 33169,
};

enum npc_druids
{
	SPELL_WRATH			= 62793,
	SPELL_STARLIGHT		= 62807,
};

enum npc_shamans
{
	SPELL_LAVA_BURST_10		= 61924,
	SPELL_STORM_CLOUD_10	= 65123,
	SPELL_STORM_CLOUD_25	= 65133,
	SPELL_STORM_POWER_10	= 65134,
};

enum npc_Priests
{
	SPELL_SMITE			= 61923,
	SPELL_GREAT_HEAL	= 62809,
	SPELL_DISPEL_MAGIC	= 63499,
};

enum npc_magi
{
	SPELL_FIREBALL				= 61909,
	SPELL_CONJURE_TOASTY_FIRE	= 62823,
	SPELL_MELT_ICE				= 64528,
	SPELL_SINGED				= 65280,
};

struct Player_Position
{
	float x,y;
	Player* plr;
};
struct MANGOS_DLL_DECL boss_hodirAI : public LibDevFSAI
{
    boss_hodirAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
        AddEventOnMe(SPELL_BERSERK,540000,60000);
		AddEvent(SPELL_FREEZE,15000,45000,5000);
		AddEvent(m_difficulty ? SPELL_BLOW_H : SPELL_BLOW,50000,60000);
    }

	uint32 Aura_Timer;
	uint32 Flash_Timer;
	uint32 FlashCast_Timer;
	uint32 IceBlock_Timer;
	uint32 Avanlanche_Timer;
	uint32 AuraCheck_Timer;
	std::vector<Player_Position*> pPos;
	std::vector<Player_Position*> roc_pos;

    void Reset()
    {
		ResetTimers();
		Aura_Timer = 1000;
		AuraCheck_Timer = 500;
		Flash_Timer = 35000;
		FlashCast_Timer = DAY;
		pPos.clear();
		roc_pos.clear();
		if (pInstance)
            pInstance->SetData(TYPE_HODIR, NOT_STARTED);
    }

    void KilledUnit(Unit *victim){}

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_HODIR, DONE);
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE);
    }

    void Aggro(Unit* pWho)
    {
		Yell(15552,"Vous allez souffrir pour cet entretien");
        me->SetInCombatWithZone();

        if (pInstance)
		{
            pInstance->SetData(TYPE_HODIR, IN_PROGRESS);
			((instance_ulduar*)pInstance)->FreezeAllHodirAdds();
		}
		InitPlayers();
		
    }

	void InitPlayers()
	{
		pPos.clear();
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			{
				if (Player* pPlayer = itr->getSource())
				{
					Player_Position* tPos = new Player_Position;
					tPos->x = pPlayer->GetPositionX();
					tPos->y = pPlayer->GetPositionY();
					tPos->plr = pPlayer;
					pPos.push_back(tPos);
				}
			}
		}
	}
	void UpdateStacksOnPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			{
				if (Player* pPlayer = itr->getSource())
				{
					uint32 stk = 0;
					if(pPlayer->HasAura(SPELL_BITING_COLD))
						stk = pPlayer->GetAura(SPELL_BITING_COLD,0)->GetStackAmount();
					SetAuraStack(SPELL_BITING_COLD,stk + 1,pPlayer,me,1);
					me->DealDamage(pPlayer, 200*(stk + 1), NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FROST, GetSpellStore()->LookupEntry(SPELL_BITING_COLD), false);
				}
			}
		}
	}

	void DoFlashEvent()
	{
		for(std::vector<Player_Position*>::const_iterator itr = pPos.begin(); itr != pPos.end(); ++itr)
		{
			if((*itr)->plr->isAlive())
			{
				if((*itr)->plr->GetMapId() != 603 || me->GetDistance2d((*itr)->plr->GetPositionX(),(*itr)->plr->GetPositionY()) || (*itr)->plr->isGameMaster())
				{
					//pPos.erase(itr->getSource());
					continue;
				}
				bool isNear = false;
				
				for(std::vector<Player_Position*>::const_iterator itr2 = roc_pos.begin(); itr2 != roc_pos.end(); ++itr2)
				{
					if((*itr)->plr)
						if((*itr)->plr->GetDistance2d((*itr2)->x,(*itr2)->y) < 7.0f)
							isNear = true;
				}

				if(!isNear)
				{
					if((*itr)->plr->HasAura(SPELL_FREEZE))
						Kill((*itr)->plr);
					else
					{
						if((*itr)->plr)
						{
							SetAuraStack(SPELL_HODIR_FUROR,1,(*itr)->plr,me,1);
							CallCreature(33212,TEN_MINS,PREC_COORDS,NOTHING,(*itr)->plr->GetPositionX(),
								(*itr)->plr->GetPositionY(),(*itr)->plr->GetPositionZ());
						}
					}
				}
			}
		}
		roc_pos.clear();
	}

	void SpawnZone(float x_, float y_)
	{
		Player_Position* tPos = new Player_Position;
		tPos->plr = NULL;
		tPos->x = x_;
		tPos->y = y_;
		roc_pos.push_back(tPos);
	}

	void DoAvalancheEvent()
	{
		for(int i=0;i<3;i++)
			CallCreature(33169,THREE_MINS,NEAR_30M,NOTHING);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(AuraCheck_Timer <= diff)
		{
			for(std::vector<Player_Position*>::const_iterator itr = pPos.begin(); itr != pPos.end(); ++itr)
			{
				if((*itr)->x - (*itr)->plr->GetPositionX() > 1.0f || (*itr)->x - (*itr)->plr->GetPositionX() < -1.0f ||
					(*itr)->y - (*itr)->plr->GetPositionY() > 1.0f || (*itr)->y - (*itr)->plr->GetPositionY() < -1.0f)
					if((*itr)->plr->HasAura(SPELL_BITING_COLD))
						(*itr)->plr->RemoveAurasDueToSpell(SPELL_BITING_COLD);
				(*itr)->x = (*itr)->plr->GetPositionX();
				(*itr)->y = (*itr)->plr->GetPositionY();

			}
			AuraCheck_Timer = 500;
		}
		else
			AuraCheck_Timer -= diff;

		if(Flash_Timer <= diff)
		{
			DoCastMe(SPELL_FLASH);
			DoAvalancheEvent();
			Flash_Timer = 80000;
			FlashCast_Timer = 9100;
		}
		else
			Flash_Timer -= diff;

		if(FlashCast_Timer <= diff)
		{
			me->CastStop();
			DoCastMe(SPELL_INSTANT_COLD);
			DoFlashEvent();
			FlashCast_Timer = DAY;
		}
		else
			FlashCast_Timer -= diff;

		if(Aura_Timer <= diff)
		{
			UpdateStacksOnPlayers();
			Aura_Timer = 3000;
		}
		else
			Aura_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_hodir(Creature* pCreature)
{
    return new boss_hodirAI(pCreature);
}

struct MANGOS_DLL_DECL add_hodirAI : public LibDevFSAI
{
    add_hodirAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint32 Shatter_Timer;

    void Reset()
    {
		ResetTimers();
		Shatter_Timer = 4000;
    }
	
	void Shatter()
	{
		if(Unit* Hodir = GetInstanceCreature(TYPE_HODIR))
			if(Hodir->isAlive())
				((boss_hodirAI*)((Creature*)Hodir)->AI())->SpawnZone(me->GetPositionX(),me->GetPositionY());

		DoCastMe(SPELL_ICICLE);

		me->ForcedDespawn(5000);
	}

    void UpdateAI(const uint32 diff)
    {
		if(Shatter_Timer <= diff)
		{
			Shatter();
			Shatter_Timer = DAY;
		}
		else
			Shatter_Timer -= diff;

		UpdateEvent(diff);

    }
};

CreatureAI* GetAI_add_hodir(Creature* pCreature)
{
    return new add_hodirAI(pCreature);
}

struct MANGOS_DLL_DECL freeze_hodirAI : public LibDevFSAI
{
    freeze_hodirAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
    }

	uint32 Shatter_Timer;

    void Reset()
    {
		ResetTimers();
		SetAuraStack(62297,1,me,me,1);
		me->setFaction(14);
		me->SetMaxHealth(m_difficulty ? 180000:60000);
		me->SetHealth(m_difficulty ? 180000:60000);
    }
	
    void UpdateAI(const uint32 diff){}

	void FreePlayer()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(62297) && pPlayer->GetDistance2d(me->GetPositionX(),me->GetPositionY()) < 2.5f)
					{
						pPlayer->RemoveAurasDueToSpell(62297);
						return;
					}

	}

	void JustDied(Unit* pWho)
	{
		FreePlayer();
		((instance_ulduar*)pInstance)->FreeHodirAdd(me->GetPositionX(),me->GetPositionY());
	}
};

CreatureAI* GetAI_freeze_hodir(Creature* pCreature)
{
    return new freeze_hodirAI(pCreature);
}

void AddSC_boss_hodir()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_hodir";
    newscript->GetAI = &GetAI_boss_hodir;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_hodir";
    newscript->GetAI = &GetAI_add_hodir;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freeze_hodir";
    newscript->GetAI = &GetAI_freeze_hodir;
    newscript->RegisterSelf();
}

