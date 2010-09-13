#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
	// phase 1 & 3
	SPELL_FROST_AURA		=	70084, // ok
	SPELL_CLEAVE			=	19983, // ok
	SPELL_SOUL_FEAST		=	71203,
	SPELL_BLISTERING_COLD	=	70123, // ok
	SPELL_FROST_BREATH		=	69649, // ok
	SPELL_ICY_GRIP			=	70117, // ok
	SPELL_PERMEATING_CHILL	=	70109, // ok
	SPELL_TAIL_SMASH		=	71077, // ok
	SPELL_UNCHAINED_MAGIC	=	69762, // ok
	
	// phase 3 only
	SPELL_MYSTIC_BUFFER		=	70128, // must handle ice blocks
	
	// phase 2 only
	SPELL_ICE_TOMB			=	69675, // ok ?
	SPELL_ICE_TOMB_BUFF		=	70157,
	NPC_ICE_TOMB			=	36980,
	SPELL_FROST_BOMB		=	71053,
	SPELL_ASPHYXIATION		=	71665,
	SPELL_BLUE_ARROW		=	70126,
	
	// others
	SPELL_ENRAGE			=	26662, // ok
};

struct MANGOS_DLL_DECL boss_sindragosaAI : public LibDevFSAI
{
    boss_sindragosaAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		AddEventOnTank(SPELL_CLEAVE,3000,5000,1000,1);
		AddEventOnTank(SPELL_CLEAVE,3000,5000,1000,3);
		AddEventOnMe(SPELL_TAIL_SMASH,1000,7000,1000,1);
		AddEventOnMe(SPELL_TAIL_SMASH,1000,7000,1000,3);
		AddEventOnTank(SPELL_FROST_BREATH,urand(7000,9000),280000,2000,1);
		AddEventOnTank(SPELL_FROST_BREATH,urand(7000,9000),28000,2000,3);
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

    void Reset()
    {
		ResetTimers();
		firstPhase2 = false;
		phase2_Timer = 110000;
		phase1_Timer = 44000;
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
		else if(spell->Id == SPELL_ICE_TOMB)
		{
			me->CastSpell(pWho,SPELL_ICE_TOMB_BUFF,true);
			CallCreature(NPC_ICE_TOMB,TEN_MINS,PREC_COORDS,NOTHING,pWho->GetPositionX(),pWho->GetPositionY(),pWho->GetPositionZ(),true);
			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* pPlayer = itr->getSource())
					{
						if(pPlayer == pWho)
							continue;
						if(pPlayer->GetDistance2d(pWho) <= 10.0f && !pPlayer->HasAura(SPELL_ICE_TOMB_BUFF))
						{
							me->CastSpell(pPlayer,SPELL_ICE_TOMB_BUFF,true);
							CallCreature(NPC_ICE_TOMB,TEN_MINS,PREC_COORDS,NOTHING,pPlayer->GetPositionX(),pPlayer->GetPositionY(),pPlayer->GetPositionZ(),true);
						}
					}
		}
	}

	void CheckInstability()
	{
		if(phase == 2)
			return;

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
		phase = 1;
		DoCastMe(SPELL_FROST_AURA,true);
		ModifyAuraStack(SPELL_PERMEATING_CHILL);
		SetFlying(false);
	}

	void SwitchPhase2()
	{
		phase = 2;
		IceTombTarSelect_Timer = 5000;
		me->RemoveAurasDueToSpell(SPELL_FROST_AURA);
		me->RemoveAurasDueToSpellByCancel(SPELL_PERMEATING_CHILL);
		SetFlying(true);
	}

	void SwitchPhase3()
	{
		phase = 3;
		IceTombTarSelect_Timer = urand(25000,35000);
		DoCastMe(SPELL_FROST_AURA,true);
		ModifyAuraStack(SPELL_PERMEATING_CHILL);
		SetFlying(false);
		DoCastMe(SPELL_MYSTIC_BUFFER,true);
	}

    void JustDied(Unit* pKiller)
    {
        SetInstanceData(TYPE_SINDRAGOSA, DONE);

		Yell(17010,"Libre... enfin !");
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,1);
				GiveEmblemsToGroup(GIVRE,2);
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
		if(m_difficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_difficulty == RAID_DIFFICULTY_25MAN_HEROIC)
			targetNb = 5;

		
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
					IceTombTarSelect_Timer = 35000;
			}
			else
				IceTombTarSelect_Timer -= diff;
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
				phase2_Timer -= diff;
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
}
