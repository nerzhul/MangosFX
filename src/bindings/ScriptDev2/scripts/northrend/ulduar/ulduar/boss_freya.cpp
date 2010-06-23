#include "precompiled.h"
#include "ulduar.h"
 
enum FreyaSpells
{
	SPELL_ATTUNED_TO_NATURE		= 62519,
	SPELL_ENRAGE				= 47008,
	SPELL_GROUND_TREMOR			= 62437,
	SPELL_GROUND_TREMOR_H		= 62859,
	SPELL_SUMMON_ALLIES			= 62678, // NEED TO IMPLEMENT, NOW WORKS
	SPELL_SUNBEAM				= 62623,
	SPELL_SUNBEAM_H				= 62872,
	SPELL_TOUCH					= 62528,
	SPELL_TOUCH_H				= 62892,
	SPELL_PHOTOSYNTHESIS		= 62209,
	SPELL_UNSTABLE_ENERGY		= 62865,

};

enum FreyaAdds
{
	//vague Type 1 (il faut en pop 10)
	NPC_DETONATING_LASHER		= 32918,
	//vague Type 2 (les 3 toutes les minutes)
	NPC_WATER_SPIRIT			= 33202,
	NPC_STORM_LASHER			= 32919,
	NPC_SNAPLASHER				= 32918,
	//vague Type 3
	NPC_ANCIENT_CONSERVATOR		= 33203,
};

/*
#define SAY_AGGRO -1
#define SAY_SLAY -1
*/

struct MANGOS_DLL_DECL boss_freyaAI : public LibDevFSAI
{
    boss_freyaAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(720000);
		AddHealEvent(SPELL_PHOTOSYNTHESIS,5000,6000);
		AddSummonEvent(33328,30000,30000,0,15000);
	
		if(m_difficulty)
		{
			AddEvent(SPELL_GROUND_TREMOR_H,20000,15000,2000,TARGET_MAIN,2);
			AddEvent(SPELL_SUNBEAM_H,15000,10000,2000,TARGET_RANDOM,2);
			AddEventOnMe(SPELL_TOUCH_H,4000,5000);
		}
		else
		{
			AddEvent(SPELL_GROUND_TREMOR,20000,15000,2000,TARGET_MAIN,2);
			AddEvent(SPELL_SUNBEAM,15000,10000,2,TARGET_RANDOM,2);
			AddEventOnMe(SPELL_TOUCH,4000,5000);
		}
    }

	bool HardMode;
	bool TriAddAlive[3];
	uint32 AskRezNextCycle;
	uint8 phase;
	uint32 Vague_Timer;
	uint16 Vague_Count;
	uint32 Check_InLife_Timer;
	std::vector<Creature*> TriAdds;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		SetAuraStack(SPELL_ATTUNED_TO_NATURE,150,me,me,1);
		HardMode = false;
		AskRezNextCycle = RESPAWN_ONE_DAY*1000;
		TriAdds.clear();
		phase = 0;
		Vague_Timer = 5000;
		Check_InLife_Timer = 1500;
		Vague_Count = 0;
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_FREYA, DONE);
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (pInstance)
            pInstance->SetData(TYPE_FREYA, IN_PROGRESS);

		phase = 1;
    }

	void UpdateHealStack(uint8 diff)
	{
		if(!me->HasAura(SPELL_ATTUNED_TO_NATURE))
			return;

		uint8 stk = (me->GetAura(SPELL_ATTUNED_TO_NATURE,0)->GetStackAmount() > 1) ? me->GetAura(SPELL_ATTUNED_TO_NATURE,0)->GetStackAmount() : 1;
		stk -= diff;
		if(stk < 1)
			me->RemoveAurasDueToSpell(SPELL_ATTUNED_TO_NATURE);
		else
			SetAuraStack(SPELL_ATTUNED_TO_NATURE,stk,me,me,1);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase == 1)
		{
			if(Check_InLife_Timer <= diff)
			{
				if(!TriAdds.empty())
				{
					Creature* TriElems[3];
					uint8 i = 0;
					for(std::vector<Creature*>::iterator itr = TriAdds.begin(); itr!= TriAdds.end(); ++itr)
					{
						TriElems[i] = (*itr);
						i++;
					}

					if(TriElems[0] && TriElems[0]->isAlive())
					{
						if(TriElems[1] && !TriElems[1]->isAlive() || TriElems[2] && !TriElems[2]->isAlive())
						{
							AskRezNextCycle = 10000;
						}
					}

					if(TriElems[1] && TriElems[1]->isAlive())

					{
						if(TriElems[0] && !TriElems[0]->isAlive() || TriElems[2] && !TriElems[2]->isAlive())
						{
							AskRezNextCycle = 10000;
						}
					}

					if(TriElems[2] && TriElems[2]->isAlive())
					{
						if(TriElems[1] && !TriElems[1]->isAlive() || TriElems[0] && !TriElems[0]->isAlive())
						{
							AskRezNextCycle = 10000;
						}
					}
				}

				Check_InLife_Timer = 1500;
			}
			else
				Check_InLife_Timer -= diff;

			if(AskRezNextCycle <= diff)
			{
				if(!TriAdds.empty())
					for(std::vector<Creature*>::iterator itr = TriAdds.begin(); itr!= TriAdds.end(); ++itr)
						if(!(*itr)->isAlive())
							(*itr)->Respawn();

				AskRezNextCycle = RESPAWN_ONE_DAY*1000;
			}
			else
				AskRezNextCycle -= diff;

			if(Vague_Timer <= diff)
			{
				bool AllAlive = true;
				Vague_Count++;
				switch(Vague_Count)
				{
					case 1:
					case 3:
					case 5:
						for(int i=0;i<(m_difficulty ? 10 : 8);i++)
							CallCreature(NPC_DETONATING_LASHER,TEN_MINS,NEAR_7M);
						break;
					case 2:
						if(Creature* tmpCr = CallCreature(NPC_WATER_SPIRIT,TEN_MINS,NEAR_7M))
							TriAdds.push_back(tmpCr);
						if(Creature* tmpCr = CallCreature(NPC_STORM_LASHER,TEN_MINS,NEAR_7M))
							TriAdds.push_back(tmpCr);
						if(Creature* tmpCr = CallCreature(NPC_SNAPLASHER,TEN_MINS,NEAR_7M))
							TriAdds.push_back(tmpCr);
						break;
					case 4:
					case 6:
						CallCreature(NPC_ANCIENT_CONSERVATOR,TEN_MINS,NEAR_15M);
						break;
					case 7:
						for(std::vector<Creature*>::iterator itr = TriAdds.begin(); itr!= TriAdds.end(); ++itr)
							if(!(*itr)->isAlive())
								AllAlive = false;
						if(AllAlive)
							HardMode = true;

						phase++;
						me->RemoveAurasDueToSpell(SPELL_ATTUNED_TO_NATURE);
						break;
					default:
						break;

				}

				Vague_Timer = 60000;
			}
			else
				Vague_Timer -= diff;
		}

		UpdateEvent(diff);
		UpdateEvent(diff,phase);

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_freya(Creature* pCreature)
{
    return new boss_freyaAI(pCreature);
}

struct MANGOS_DLL_DECL detonating_lasherAI : public LibDevFSAI
{
    detonating_lasherAI(Creature* pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		AddEventOnTank(62608,500,2500);
	}

	bool death;

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && !death)
		{
			dmg = 0;
			death = true;
			DoCastVictim(m_difficulty ? 62937 : 62598);
			if(Creature* Freya = GetInstanceCreature(TYPE_FREYA))
				if(Freya->isAlive())
					((boss_freyaAI*)Freya->AI())->UpdateHealStack(2);
			me->ForcedDespawn(1000);
		}
	}

	void UpdateAI(const uint32 diff)
    {
		if(!CanDoSomething())
			return;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_detonating_lasher(Creature* pCreature)
{
    return new detonating_lasherAI(pCreature);
}

void AddSC_boss_freya()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_freya";
    newscript->GetAI = &GetAI_boss_freya;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "freya_deton_lasher";
    newscript->GetAI = &GetAI_detonating_lasher;
    newscript->RegisterSelf();
}
