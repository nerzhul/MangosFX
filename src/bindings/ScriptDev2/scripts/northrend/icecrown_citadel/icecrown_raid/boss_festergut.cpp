#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_GASEOUS_BLIGHT_0			= 70138, //
    SPELL_GASEOUS_BLIGHT_1			= 69161, //
    SPELL_GASEOUS_BLIGHT_2			= 70468, //
    SPELL_INHALE_BLIGHT				= 69165, //
    SPELL_PUNGENT_BLIGHT			= 69195,
    SPELL_PUNGENT_BLIGHT_VISUAL		= 69126, //
    SPELL_GAS_SPORE					= 69278,
    SPELL_INOCULATE					= 72103,
    SPELL_GASTRIC_BLOAT				= 72219, //
    SPELL_GASTRIC_EXPLOSION			= 72227, //
    SPELL_VILE_GAS					= 72272,
};

struct MANGOS_DLL_DECL boss_festergutAI : public LibDevFSAI
{
    boss_festergutAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(300000);
		AddTextEvent(16905,"On arrête, de jouer !",300000,60000);
		AddEventOnTank(SPELL_GASTRIC_BLOAT,6000,11500);
		AddEventOnMe(SPELL_INHALE_BLIGHT,45000,60000);
    }

	uint32 Inhale_Timer;
	uint8 Inhale_Count;

    void Reset()
    {
		ResetTimers();
		me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_0);
		me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_1);
		me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_2);
		Inhale_Timer = 46500;
		Inhale_Count = 0;
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
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_FESTERGUT, FAIL);
    }

	void CheckPlayersConditions()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			{
				if(Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isAlive())
						if(pPlayer->HasAura(SPELL_GASTRIC_BLOAT))
						{
							uint8 stk = pPlayer->GetAura(SPELL_GASTRIC_BLOAT,0)->GetStackAmount();
							if(stk == 10)
							{
								pPlayer->CastStop();
								pPlayer->CastSpell(pPlayer,SPELL_GASTRIC_EXPLOSION,true);
							}
						}
				}
			}
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(Inhale_Timer <= diff)
		{
			Inhale_Count++;
			switch(Inhale_Count)
			{
				case 1:
					me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_0);
					ModifyAuraStack(SPELL_GASEOUS_BLIGHT_1);
					break;
				case 2:
					me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_1);
					me->RemoveAurasDueToSpell(SPELL_PUNGENT_BLIGHT_VISUAL);
					ModifyAuraStack(SPELL_GASEOUS_BLIGHT_2);
					break;
				case 3:
					me->CastStop();
					DoCastVictim(SPELL_PUNGENT_BLIGHT);
					me->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_2);
					ModifyAuraStack(SPELL_GASEOUS_BLIGHT_0);
					ModifyAuraStack(SPELL_PUNGENT_BLIGHT_VISUAL);
					Inhale_Count = 0;
					break;
				default:
					break;
			}
			Inhale_Timer = 60000;
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
