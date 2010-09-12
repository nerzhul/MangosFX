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
	SPELL_ICY_GRIP			=	70117, // teleport all aggroed players
	SPELL_PERMEATING_CHILL	=	70109,
	SPELL_TAIL_SMASH		=	71077, // ok
	SPELL_UNCHAINED_MAGIC	=	69762,
	
	// phase 3 only
	SPELL_MYSTIC_BUFFER		=	70128,
	
	// phase 2 only
	SPELL_ICE_TOMB			=	69712,
	NPC_ICE_TOMB			=	36980,
	SPELL_FROST_BEACON		=	70126,
	SPELL_FROST_BOMB		=	71053,
	SPELL_ASPHYXIATION		=	71665,
	
	// others
	SPELL_ENRAGE			=	26662, // ok
};

struct MANGOS_DLL_DECL boss_sindragosaAI : public LibDevFSAI
{
    boss_sindragosaAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_CLEAVE,3000,5000,1000,1);
		AddEventOnTank(SPELL_CLEAVE,3000,5000,1000,3);
		AddEventOnMe(SPELL_TAIL_SMASH,1000,3000,1000,1);
		AddEventOnMe(SPELL_TAIL_SMASH,1000,3000,1000,3);
		AddEventOnTank(SPELL_BLISTERING_COLD,urand(7000,9000),10000,2000,1);
		AddEventOnTank(SPELL_BLISTERING_COLD,urand(7000,9000),10000,2000,3);
		AddEventMaxPrioOnTank(SPELL_BLISTERING_COLD,25000,25000,0,1);
		AddEventMaxPrioOnTank(SPELL_BLISTERING_COLD,25000,25000,0,3);
		AddEventMaxPrioOnMe(SPELL_ICY_GRIP,24000,25000,0,1);
		AddEventMaxPrioOnMe(SPELL_ICY_GRIP,24000,25000,0,3);

		AddEnrageTimer(TEN_MINS);
		AddTextEvent(17011,"Assez ! Ce petit jeu me fatigue !",TEN_MINS,60000);
    }

	uint8 phase;
	bool firstPhase2;

	uint32 phase2_Timer;
	uint32 phase1_Timer;

    void Reset()
    {
		ResetTimers();
		firstPhase2 = false;
		phase2_Timer = 110000;
		phase1_Timer = 44000;
		phase = 1;
    }

	void SpellHit(Unit* pWho, const SpellEntry* spell)
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
	}

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_SINDRAGOSA, IN_PROGRESS);
		ModifyAuraStack(SPELL_FROST_AURA);
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
		ModifyAuraStack(SPELL_FROST_AURA);
		SetFlying(false);
	}

	void SwitchPhase2()
	{
		phase = 2;
		me->RemoveAurasDueToSpell(SPELL_FROST_AURA);
		SetFlying(true);
	}

	void SwitchPhase3()
	{
		phase = 3;
		ModifyAuraStack(SPELL_FROST_AURA);
		SetFlying(false);
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

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(85) && !firstPhase2)
		{
			firstPhase2 = true;
			SwitchPhase2();
		}

		if(CheckPercentLife(35) && phase == 1)
			SwitchPhase3();

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
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

void AddSC_ICC_Sindragosa()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_sindragosa";
    NewScript->GetAI = &GetAI_boss_sindragosa;
    NewScript->RegisterSelf();
}
