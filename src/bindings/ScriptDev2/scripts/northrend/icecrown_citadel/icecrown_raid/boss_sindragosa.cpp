#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	// phase 1 & 3
	SPELL_FROST_AURA		=	70084,
	SPELL_CLEAVE			=	19983,
	SPELL_BLISTERING_COLD	=	70123,
	SPELL_FROST_BREATH		=	69649,
	SPELL_ICY_GRIP			=	70117,
	SPELL_PERMEATING_CHILL	=	70109,
	SPELL_TAIL_SMASH		=	71077,
	SPELL_UNCHAINED_MAGIC	=	69762,
	
	// phase 3 only
	SPELL_MYSTIC_BUFFER		=	70128,
	SPELL_MYSTIC_BUFFER_NEG	=	70127,
	
	// phase 2 only
	SPELL_BLUE_ARROW		=	70126,
	SPELL_ICE_TOMB			=	69675,
	SPELL_ICE_TOMB_BUFF		=	70157,
	NPC_ICE_TOMB			=	36980,

	SPELL_FROST_BOMB_TAR	=	64627,
	SPELL_FROST_BOMB_AURA	=	64624,
	SPELL_FROST_BOMB		=	71053,
	SPELL_FROST_BOMB_INVK	=	69846,
	NPC_BOMB_TARGET			=	37186,
	NPC_FROST_BOMB			=	37186,
	SPELL_ASPHYXIATION		=	71665,
	
	
	// others
	SPELL_ENRAGE			=	26662,
};

struct MANGOS_DLL_DECL boss_sindragosaAI : public LibDevFSAI
{
    boss_sindragosaAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		AddEventOnTank(SPELL_CLEAVE,3000,5000,1000,1);
		AddEventOnTank(SPELL_CLEAVE,3000,5000,1000,3);
		AddEventOnMe(SPELL_TAIL_SMASH,1000,10000,1000,1);
		AddEventOnMe(SPELL_TAIL_SMASH,1000,10000,1000,3);
		AddEventOnTank(SPELL_FROST_BREATH,25000,35000,2000,1);
		AddEventOnTank(SPELL_FROST_BREATH,25000,35000,2000,3);
		AddEventMaxPrioOnTank(SPELL_BLISTERING_COLD,18000,35000,0,1);
		AddEventMaxPrioOnTank(SPELL_BLISTERING_COLD,18000,35000,0,3);
		AddEventMaxPrioOnMe(SPELL_ICY_GRIP,17000,35000,0,1);
		AddEventMaxPrioOnMe(SPELL_ICY_GRIP,17000,35000,0,3);
		// Script here test for aura with stacks
		AddEvent(SPELL_UNCHAINED_MAGIC,15000,20000,0,TARGET_HAS_MANA,1);
		AddEvent(SPELL_UNCHAINED_MAGIC,15000,20000,0,TARGET_HAS_MANA,3);

		AddEnrageTimer(TEN_MINS);
		AddTextEvent(17011,"Assez ! Ce petit jeu me fatigue !",TEN_MINS,60000);
		InitInstance();
    }

	uint8 phase;
	bool firstPhase2;

	uint32 checkInstability_Timer;

	uint32 phase2_Timer;
	uint32 phase1_Timer;
	uint32 IceTombTarSelect_Timer;

	uint32 bombTarget_Timer;

    void Reset()
    {
		ResetTimers();
		SetFlying(false);
		firstPhase2 = false;
		phase2_Timer = 110000;
		phase1_Timer = 50000;
		checkInstability_Timer = 500;
		IceTombTarSelect_Timer = DAY*HOUR;
		phase = 1;
    }

	void SpellHitTarget(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == SPELL_ICY_GRIP)
		{
			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* pPlayer = itr->getSource())
						if(pPlayer->GetDistance2d(me) <= 250.0f)
							pPlayer->TeleportTo(me->GetMapId(),me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+1.0f,me->GetOrientation());
		}
		else if(spell->Id == SPELL_ICE_TOMB_BUFF)
			CallCreature(NPC_ICE_TOMB,TEN_MINS,PREC_COORDS,NOTHING,pWho->GetPositionX(),pWho->GetPositionY(),pWho->GetPositionZ(),true);
		else if(spell->Id == SPELL_MYSTIC_BUFFER)
		{
			if(Creature* pTomb = pWho->GetClosestCreatureWithEntry(NPC_ICE_TOMB,250.0f))
				if(pWho->HasAura(SPELL_ICE_TOMB_BUFF) || pTomb->isAlive() && pTomb->GetDistance2d(me) < pWho->GetDistance2d(me) && pWho->GetDistance2d(pTomb) < 8.0f)
					pWho->RemoveAurasDueToSpell(SPELL_MYSTIC_BUFFER_NEG);
		}
	}

	void CheckInstability()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* pPlayer = itr->getSource())
						if(Aura* aur = pPlayer->GetAura(69766))
							if(aur->GetAuraDuration() < 1000)
							{
								uint8 stack = aur->GetStackAmount();
								int32 bp0 = int32(stack * 2000);
								pPlayer->CastStop();
								pPlayer->CastCustomSpell(pPlayer,69770,&bp0,NULL,NULL,true,NULL,aur);
								pPlayer->RemoveAurasDueToSpell(69766);
							}
	}

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_SINDRAGOSA, IN_PROGRESS);
		DoCastMe(SPELL_FROST_AURA,true);
		ModifyAuraStack(SPELL_PERMEATING_CHILL);
		Yell(17007,"Qu'il est stupide d'être venus ici. Les vents glacés du Norfendre vont dévorer vos âmes");
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Yell(17008,"Pérrrissss !");
		else
			Yell(17009,"La faille de la condition mortelle !");
	}

	void SwitchPhase1()
	{
		AsphyxiatePlayers();
		GoToGround();
		phase = 1;
		DoCastMe(SPELL_FROST_AURA,true);
		ModifyAuraStack(SPELL_PERMEATING_CHILL);
		SetFlying(false);
	}

	void SwitchPhase2()
	{
		phase = 2;
		IceTombTarSelect_Timer = 3000;
		bombTarget_Timer = 13000;
		me->RemoveAurasDueToSpell(SPELL_FROST_AURA);
		me->RemoveAurasDueToSpellByCancel(SPELL_PERMEATING_CHILL);
		SetFlying(true);
		me->GetMotionMaster()->MovePoint(0,4447.37f,2483.92f,235.0f);
	}

	void SwitchPhase3()
	{
		AsphyxiatePlayers();
		GoToGround();
		phase = 3;
		IceTombTarSelect_Timer = urand(25000,35000);
		DoCastMe(SPELL_FROST_AURA,true);
		ModifyAuraStack(SPELL_PERMEATING_CHILL);
		SetFlying(false);
		DoCastMe(SPELL_MYSTIC_BUFFER,true);
	}

	void GoToGround()
	{
		if(!me->getVictim())
			return;

		me->GetMotionMaster()->MoveChase(me->getVictim(),3.0f);
		Relocate(me->getVictim()->GetPositionX(),me->getVictim()->GetPositionY(),me->getVictim()->GetPositionZ());
	}

	void AsphyxiatePlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if(Player* pPlayer = itr->getSource())
						if(pPlayer->HasAura(SPELL_ICE_TOMB_BUFF))
							ModifyAuraStack(SPELL_ASPHYXIATION,1,pPlayer);
	}

    void JustDied(Unit* pKiller)
    {
        SetInstanceData(TYPE_SINDRAGOSA, DONE);

		Yell(17010,"Libre... enfin !");
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
		}
    }

    void JustReachedHome()
    {
        SetInstanceData(TYPE_SINDRAGOSA, FAIL);
    }

	void SelectIceTombTargets()
	{
		uint8 targetNb = 2;
		if(phase == 3)
			targetNb = 1;
		if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_difficulty == RAID_DIFFICULTY_25MAN_HEROIC)
		{
			if(phase == 3)
				targetNb = 3;
			else
				targetNb = 5;
		}

		if(targetNb > me->GetMap()->GetPlayersCountExceptGMs())
			targetNb = me->GetMap()->GetPlayersCountExceptGMs();

		uint16 breaker = 0;
		for(int8 i=0;i<targetNb;i++)
		{
			breaker++;
			if(Unit* tmpTar = GetRandomUnit())
			{
				if(tmpTar->HasAura(SPELL_BLUE_ARROW) || tmpTar->GetTypeId() != TYPEID_PLAYER)
					i--;
				else
				{
					ModifyAuraStack(SPELL_BLUE_ARROW,1,me->getVictim());
					DoCast(me->getVictim(),SPELL_ICE_TOMB,true);
				}
			}
			if(breaker > 500)
				return;
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(85) && !firstPhase2)
		{
			firstPhase2 = true;
			SwitchPhase2();
			return;
		}

		if(CheckPercentLife(35) && (phase == 1 || phase == 2))
		{
			SwitchPhase3();
			return;
		}

		if(phase == 2 || phase == 3)
		{
			if(IceTombTarSelect_Timer <= diff)
			{
				SelectIceTombTargets();
				if(phase == 2)
					IceTombTarSelect_Timer = DAY*HOUR;
				else
					IceTombTarSelect_Timer = 20000;
			}
			else
				IceTombTarSelect_Timer -= diff;

			if(phase == 2)
			{
				if(bombTarget_Timer <= diff)
				{
					uint32 x = urand(4360,4430);
					uint32 y = urand(2460,2510);
					DoCastXYZ(float(x),float(y),204.0f,SPELL_FROST_BOMB_INVK);
					bombTarget_Timer = 10000;
				}
				else
					bombTarget_Timer -= diff;
			}
		}

		if(checkInstability_Timer <= diff)
		{
			CheckInstability();
			checkInstability_Timer = 500;
		}
		else
			checkInstability_Timer -= diff;

		if(firstPhase2 && phase != 3)
		{
			if(phase2_Timer <= diff)
			{
				SwitchPhase2();
				phase2_Timer = 110000;
			}
			else
				phase2_Timer -= diff;

			if(phase1_Timer <= diff)
			{
				SwitchPhase1();
				phase1_Timer = 110000;
			}
			else
				phase1_Timer -= diff;
		}
		UpdateEvent(diff);
		UpdateEvent(diff,phase);
		if(phase == 1 || phase == 3)
			DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

struct MANGOS_DLL_DECL ice_tombAI : public LibDevFSAI
{
    ice_tombAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		SetCombatMovement(false);
    }

    void Reset()
    {
		ResetTimers();
    }

	void JustDied(Unit* pWho)
	{
		FreeTarget();
	}

	void FreeTarget()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if(Player* pPlayer = itr->getSource())
				{
					if(!pPlayer->HasAura(SPELL_ICE_TOMB_BUFF))
						continue;

					// no bug abuse on bumps :)
					if(pPlayer->GetDistance2d(me) > 3.0f)
						continue;

					if(pPlayer->isAlive())
					{
						pPlayer->RemoveAurasDueToSpell(SPELL_ICE_TOMB_BUFF);
						pPlayer->RemoveAurasDueToSpell(SPELL_ASPHYXIATION);
						return;
					}
				}
	}

    void UpdateAI(const uint32 diff)
    {
    }
};

CreatureAI* GetAI_ice_tomb(Creature* pCreature)
{
    return new ice_tombAI(pCreature);
}

struct MANGOS_DLL_DECL ice_bomb_targetAI : public LibDevFSAI
{
    ice_bomb_targetAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
		if(me->isTemporarySummon())
			((TemporarySummon*)me)->SetDespawnTime(30000);
		SetFlying(true);
    }

	uint32 bomb_Timer;

    void Reset()
    {
		ResetTimers();
		bomb_Timer = 4650;
    }

	void DamageDeal(Unit* pWho, uint32 &dmg)
	{
		
		if(pWho->HasAura(SPELL_ICE_TOMB_BUFF))
		{
			dmg = 0;
			return;
		}

		if(Creature* pTomb = pWho->GetClosestCreatureWithEntry(NPC_ICE_TOMB,250.0f))
		{
			if(pTomb->isAlive() && pTomb->GetDistance2d(me) < pWho->GetDistance2d(me) && pWho->GetDistance2d(pTomb) < 8.0f)
				dmg = 0;
		}
	}

    void UpdateAI(const uint32 diff)
    {
		if(bomb_Timer <= diff)
		{
			if(me->GetPositionZ() < 204.5f)
			{
				DoCastMe(SPELL_FROST_BOMB);
				me->ForcedDespawn(500);
			}
			bomb_Timer = 750;
		}
		else
			bomb_Timer -= diff;
    }
};

CreatureAI* GetAI_ice_bomb_target(Creature* pCreature)
{
    return new ice_bomb_targetAI(pCreature);
}

void AddSC_ICC_Sindragosa()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_sindragosa";
    NewScript->GetAI = &GetAI_boss_sindragosa;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_ice_tomb";
    NewScript->GetAI = &GetAI_ice_tomb;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "icc_ice_bomb_target";
    NewScript->GetAI = &GetAI_ice_bomb_target;
    NewScript->RegisterSelf();
}
