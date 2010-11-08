#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_GASEOUS_BLIGHT_0			= 69157, //
    SPELL_GASEOUS_BLIGHT_1			= 69162, // 
    SPELL_GASEOUS_BLIGHT_2			= 69164, //
    SPELL_INHALE_BLIGHT				= 69165, //
	SPELL_INHALE_BLIGHT_BUFF		= 69166, //
    SPELL_PUNGENT_BLIGHT			= 69195, //
    SPELL_PUNGENT_BLIGHT_VISUAL		= 69126, //
	SPELL_PUNGENT_SPORE				= 69290, //
    SPELL_GAS_SPORE					= 69279, // 
    SPELL_INOCULATE					= 72103, // 
    SPELL_GASTRIC_BLOAT				= 72219, //
    SPELL_GASTRIC_EXPLOSION			= 72227, //
    SPELL_VILE_GAS					= 72272, //
};

struct MANGOS_DLL_DECL boss_festergutAI : public LibDevFSAI
{
    boss_festergutAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(300000);
		AddTextEvent(16905,"On arrête, de jouer !",300000,60000);
		AddEventOnTank(SPELL_GASTRIC_BLOAT,6000,10000);
		AddEvent(SPELL_VILE_GAS,urand(7000,10000),14000,5000);
    }

	uint32 Inhale_Timer;
	uint8 Inhale_Count;
	uint32 Spore_Timer;
	uint32 check_Timer;

    void Reset()
    {
		ResetTimers();
		me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_0);
		me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_1);
		me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_2);
		me->RemoveAurasDueToSpell(SPELL_PUNGENT_BLIGHT_VISUAL);
		Inhale_Timer = 25000;
		Inhale_Count = 0;
		Spore_Timer = 12500;
		check_Timer = 700;
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_FESTERGUT, IN_PROGRESS);

		Yell(16901,"On joue ????");
		ModifyAuraStack(SPELL_GASEOUS_BLIGHT_0);
		ModifyAuraStack(SPELL_PUNGENT_BLIGHT_VISUAL);
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Say(16902,"Papa. J'ai réussi.");
		else
			Say(16903,"Mort, mort, mort.");
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_FESTERGUT, DONE);

		Yell(16904,"Pa.... pa... aie...");

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

		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if(Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
						if(pPlayer->HasAura(SPELL_PUNGENT_BLIGHT_VISUAL))
							pPlayer->RemoveAurasDueToSpell(SPELL_PUNGENT_BLIGHT_VISUAL);
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_FESTERGUT, FAIL);
    }

	void CheckPlayerConditions()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if(Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isAlive())
					{
						if(pPlayer->HasAura(SPELL_GASTRIC_BLOAT))
						{
							uint8 stk = pPlayer->GetAura(SPELL_GASTRIC_BLOAT)->GetStackAmount();
							if(stk == 10)
							{
								pPlayer->CastStop();
								pPlayer->CastSpell(pPlayer,SPELL_GASTRIC_EXPLOSION,true);
								break;
							}
						}

						if(pPlayer->HasAura(SPELL_GAS_SPORE))
						{
							if(pPlayer->GetAura(SPELL_GAS_SPORE)->GetAuraDuration() < 1000)
								ModifyAuraStack(SPELL_PUNGENT_SPORE,1,pPlayer);
						}

						if(pPlayer->HasAura(SPELL_PUNGENT_SPORE))
						{
							if(pPlayer->GetAura(SPELL_PUNGENT_SPORE)->GetAuraDuration() < 1000)
							{
								uint8 stk = 1;
								if(pPlayer->HasAura(SPELL_INOCULATE))
									stk = pPlayer->GetAura(SPELL_INOCULATE)->GetStackAmount();
								stk++;
								ModifyAuraStack(SPELL_INOCULATE,stk > 3 ? 3 : stk,pPlayer);
							}
						}
					}
				}
	}

	void SpellHitTarget(Unit* u, const SpellEntry* sp)
	{
		if(sp->Id == SPELL_PUNGENT_BLIGHT)
			u->RemoveAurasDueToSpell(SPELL_INOCULATE);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(check_Timer <= diff)
		{
			CheckPlayerConditions();
			check_Timer = 500;
		}
		else
			check_Timer -= diff;

		if(Spore_Timer <= diff)
		{
			uint8 antiFreeze = 0;
			uint8 nbspore = 2;
			switch(m_difficulty)
			{
				case RAID_DIFFICULTY_25MAN_NORMAL:
				case RAID_DIFFICULTY_25MAN_HEROIC:
					nbspore = 3;
					break;
			}
			for(uint8 i=0;i<nbspore;i++)
			{
				antiFreeze++;
				if(Unit* u = GetRandomUnit(2))
				{
					if(u->GetTypeId() == TYPEID_PLAYER && !u->HasAura(SPELL_GAS_SPORE))
					{
						ModifyAuraStack(SPELL_GAS_SPORE,1,u);
					}
					else
						i--;
				}
				if(antiFreeze > 50)
					break;
			}

			Spore_Timer = 30000;
		}
		else
			Spore_Timer -= diff;

		if(Inhale_Timer <= diff)
		{
			Inhale_Count++;
			switch(Inhale_Count)
			{
				case 1:
					me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_0);
					ModifyAuraStack(SPELL_GASEOUS_BLIGHT_1);
					ModifyAuraStack(SPELL_PUNGENT_BLIGHT_VISUAL);
					DoCastVictim(SPELL_INHALE_BLIGHT);
					break;
				case 2:
					me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_1);
					ModifyAuraStack(SPELL_GASEOUS_BLIGHT_2);
					ModifyAuraStack(SPELL_PUNGENT_BLIGHT_VISUAL);
					DoCastVictim(SPELL_INHALE_BLIGHT);
					break;
				case 3:
					me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_2);
					me->RemoveAurasDueToSpell(SPELL_PUNGENT_BLIGHT_VISUAL);
					DoCastVictim(SPELL_INHALE_BLIGHT);
					break;
				case 4:
					me->CastStop();
					DoCastVictim(SPELL_PUNGENT_BLIGHT);
					ModifyAuraStack(SPELL_GASEOUS_BLIGHT_0);
					ModifyAuraStack(SPELL_PUNGENT_BLIGHT_VISUAL);
					me->RemoveAurasDueToSpell(SPELL_INHALE_BLIGHT_BUFF);
					Inhale_Count = 0;
					break;
				default:
					break;
			}
			Inhale_Timer = 33000;
		}
		else
			Inhale_Timer -= diff;

		UpdateEvent(diff);
		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_festergut(Creature* pCreature)
{
    return new boss_festergutAI(pCreature);
}

void AddSC_ICC_Festergut()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_festergut";
    NewScript->GetAI = &GetAI_boss_festergut;
    NewScript->RegisterSelf();
}
