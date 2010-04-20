/*
 * Copyright (C) 2008 - 2009 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

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
struct MANGOS_DLL_DECL boss_hodirAI : public ScriptedAI
{
    boss_hodirAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = me->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	MobEventTasks Tasks;
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
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_BERSERK,540000,60000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_FREEZE,15000,45000,5000);
		if(m_bIsHeroic)
			Tasks.AddEvent(SPELL_BLOW_H,50000,60000);
		else
			Tasks.AddEvent(SPELL_BLOW,50000,60000);

		Aura_Timer = 1000;
		AuraCheck_Timer = 500;
		Flash_Timer = 35000;
		FlashCast_Timer = DAY;
		roc_pos.clear();
		if (m_pInstance)
            m_pInstance->SetData(TYPE_HODIR, NOT_STARTED);
    }

    void KilledUnit(Unit *victim){}

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HODIR, DONE);
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE);
    }

    void Aggro(Unit* pWho)
    {
		Tasks.Speak(CHAT_TYPE_YELL,15552,"Vous allez souffrir pour cet entretien");
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HODIR, IN_PROGRESS);

		InitPlayers();
		((instance_ulduar*)m_pInstance)->FreezeAllHodirAdds();
    }

	void InitPlayers()
	{
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
					Tasks.SetAuraStack(SPELL_BITING_COLD,stk + 1,pPlayer,me,1);
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
					if((*itr)->plr->GetDistance2d((*itr2)->x,(*itr2)->y) < 7.0f)
						isNear = true;
				}

				if(!isNear)
				{
					if((*itr)->plr->HasAura(SPELL_FREEZE))
						Tasks.Kill((*itr)->plr);
					else
					{
						Tasks.SetAuraStack(SPELL_HODIR_FUROR,1,(*itr)->plr,me,1);
						Tasks.CallCreature(33212,TEN_MINS,PREC_COORDS,NOTHING,(*itr)->plr->GetPositionX(),
							(*itr)->plr->GetPositionY(),(*itr)->plr->GetPositionZ());
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
			Tasks.CallCreature(33169,THREE_MINS,NEAR_30M,NOTHING);
	}

    void UpdateAI(const uint32 diff)
    {
        if (CanDoSomething())
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

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_hodir(Creature* pCreature)
{
    return new boss_hodirAI(pCreature);
}

struct MANGOS_DLL_DECL add_hodirAI : public ScriptedAI
{
    add_hodirAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	uint32 Shatter_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Shatter_Timer = 4000;
    }
	
	void Shatter()
	{
		if(Unit* Hodir = Unit::GetUnit((*me), pInstance->GetData64(TYPE_HODIR)))
			if(Hodir->isAlive())
				((boss_hodirAI*)((Creature*)Hodir)->AI())->SpawnZone(me->GetPositionX(),me->GetPositionY());

		DoCastMe(SPELL_ICICLE);

		me->ForcedDespawn(5000);
	}

    void UpdateAI(const uint32 uiDiff)
    {
	
		if(Shatter_Timer <= uiDiff)
		{
			Shatter();
			Shatter_Timer = DAY;
		}
		else
			Shatter_Timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_add_hodir(Creature* pCreature)
{
    return new add_hodirAI(pCreature);
}

struct MANGOS_DLL_DECL freeze_hodirAI : public Scripted_NoMovementAI
{
    freeze_hodirAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	uint32 Shatter_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.SetAuraStack(62297,1,me,me,1);
		me->setFaction(14);
		me->SetMaxHealth(m_bIsHeroic ? 180000:60000);
		me->SetHealth(m_bIsHeroic ? 180000:60000);
    }
	
    void UpdateAI(const uint32 uiDiff){}

	void FreePlayer()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(62297) && pPlayer->GetDistance2d(me->GetPositionX(),me->GetPositionY()) < 1.5f)
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

