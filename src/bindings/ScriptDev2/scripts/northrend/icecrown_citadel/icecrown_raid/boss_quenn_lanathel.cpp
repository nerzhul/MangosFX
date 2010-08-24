#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
	SPELL_SHROUD_OF_SORROW                  = 72981, // ok
	SPELL_DELRIOUS_SLASH                    = 71623, // ok
	SPELL_BLOOD_MIRROR_1                    = 70821, // ok
	SPELL_BLOOD_MIRROR_2                    = 70838, // ok
	SPELL_FRENZIED_BLOODTHIST				= 70877, // ok
	SPELL_VAMPIRIC_BITE                     = 71726, // ok : must CM when finish
	SPELL_UNCONTROLLABLE_FRENZY             = 70923,
	SPELL_PACT_OF_DARKFALLEN                = 71340, // ok: TODO : distance damages
	SPELL_SWARMING_SHADOWS                  = 71264, // ok: TODO : spawn event and casting region
	SPELL_FEAR								= 73070, // ok
	SPELL_TWILIGHT_BLOODBOLT                = 71446,
	SPELL_BLOODBOLT_WHIRL                   = 71772, // ok : TODO: proc twilight bolts
	SPELL_PRESENCE_OF_DARKFALLEN            = 71952,
};

enum phases
{
	PHASE_LAND			= 1,
	PHASE_AIR			= 2,
	SUBPHASE_MOVE		= 0,
	SUBPHASE_FEAR		= 1,
	SUBPHASE_STORM		= 2,
	SUBPHASE_LANDING	= 3,
};

struct MANGOS_DLL_DECL boss_lanathelAI : public LibDevFSAI
{
    boss_lanathelAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_DELRIOUS_SLASH,3000,20000,0,PHASE_LAND);
		AddPhase1Event(SPELL_SWARMING_SHADOWS,35000,30000);
		AddEnrageTimer(320000);
		AddTextEvent(16793,"On arrete MAINTENANT !",320000,60000);
		pactTargets = 0;
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_10MAN_HEROIC:
				pactTargets = 2;
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				pactTargets = 3;
				break;
		}
    }

	uint8 phase;
	uint32 phase_Timer;
	uint8 subphase;

	uint32 pact_Timer;
	uint8 pactTargets;

	uint64 tank[2];
	uint32 checkTanks_Timer;
	uint32 checkVampirs_Timer;
	uint32 vampir_Timer;

    void Reset()
    {
		ResetTimers();
		phase = PHASE_LAND;
		phase_Timer = 120000;
		subphase = SUBPHASE_MOVE;
		ModifyAuraStack(SPELL_SHROUD_OF_SORROW);
		pact_Timer = 20000;
		tank[0] = tank[1] = 0;
		checkTanks_Timer = 1500;
		vampir_Timer = 15000;
		checkVampirs_Timer = 30000;
		SetInstanceData(TYPE_LANATHEL, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        SetInstanceData(TYPE_LANATHEL, IN_PROGRESS);
		Yell(16782,"Ce n'est pas une décision... très sage...");
    }

	void KilledUnit(Unit* who)
	{
		if(urand(0,1))
			Say(16791,"Vraiment ? Et c'est tout ?");
		else
			Say(16792,"Quel affreux gachis...");

	}

    void JustDied(Unit* pKiller)
    {
        SetInstanceData(TYPE_LANATHEL, DONE);

		Say(16794,"Mais... tout allait si bien... entre ... nous...");
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				GiveEmblemsToGroup(GIVRE,1);
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
		Yell(16789,"Quel dommage ! Hahahahahaha !");
        SetInstanceData(TYPE_LANATHEL, FAIL);
    }

	void DamageDeal(Unit* pDoneTo, uint32 &dmg)
	{
		if(pDoneTo == me->getVictim())
		{
			if(Unit* MT2 = GetGuidUnit(tank[1]))
				if(MT2->HasAura(SPELL_BLOOD_MIRROR_2))
				{
					const int32 bp0 = dmg;
					me->CastCustomSpell(MT2,50846,&bp0,NULL,NULL,true);
				}
		}
	}

	void CheckVampirStates()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isGameMaster())
						continue;

					if(pPlayer->HasAura(SPELL_FRENZIED_BLOODTHIST) && pPlayer->GetAura(SPELL_FRENZIED_BLOODTHIST)->GetAuraDuration() < 1200)
						;

				}
	}

	Unit* GetNearestMT2()
	{
		if(!me->getVictim())
			return NULL;
		Unit* Nearest = NULL;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isGameMaster())
						continue;

					if(!pPlayer->isAlive())
						continue;
					
					if(pPlayer == me->getVictim())
						continue;
					
					if(Nearest)
					{
						if(Nearest->GetDistance2d(me->getVictim()) > pPlayer->GetDistance2d(me->getVictim()))
							Nearest = pPlayer;
					}
					else
						Nearest = pPlayer;

				}

		return Nearest;
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(phase_Timer <= diff)
		{
			if(phase == PHASE_LAND)
			{
				phase = PHASE_AIR;
				subphase = SUBPHASE_FEAR;
				// animation 2sec
				phase_Timer = 2000;
			}
			else if(phase == PHASE_AIR)
			{
				if(subphase == SUBPHASE_MOVE)
				{
					me->GetMotionMaster()->MovePoint(0,4595.9f,2769.32f,400.14f);
					subphase = SUBPHASE_FEAR;
					phase_Timer = 2000;
				}
				else if(subphase == SUBPHASE_FEAR)
				{
					DoCastVictim(SPELL_FEAR);					
					subphase = SUBPHASE_STORM;
					phase_Timer = 2000;
				}
				else if(subphase == SUBPHASE_STORM)
				{
					DoCastMe(SPELL_BLOODBOLT_WHIRL);
					phase_Timer = 8000;
					subphase = SUBPHASE_LANDING;
				}
				else if(subphase == SUBPHASE_LANDING)
				{
					phase = PHASE_LAND;
					phase_Timer = 90000;
				}
			}
		}
		else
			phase_Timer -= diff;

		UpdateEvent(diff);
		UpdateEvent(diff,phase);

		if(checkVampirs_Timer <= diff)
		{
			CheckVampirStates();
			checkVampirs_Timer = 500;
		}
		else
			checkVampirs_Timer -= diff;
		if(phase == PHASE_LAND)
		{
			if(pact_Timer <= diff)
			{
				uint8 breaker = 0;
				for(int8 i=0;i<pactTargets;i++)
				{
					if(Unit* u = GetRandomUnit())
					{
						if(u->HasAura(SPELL_PACT_OF_DARKFALLEN))
							i--;
						else
							DoCast(u,SPELL_PACT_OF_DARKFALLEN,true);
					}
					breaker++;
					if(breaker >= 50)
						break;
				}
				pact_Timer = 30000;
			}
			else
				pact_Timer -= diff;

			if(checkTanks_Timer <= diff)
			{
				Unit* nUnit = GetNearestMT2();
				if(me->getVictim())
				{
					if(Unit* MT1 = GetGuidUnit(tank[0]))
					{
						if(MT1 != me->getVictim())
						{
							MT1->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_1);
							ModifyAuraStack(SPELL_BLOOD_MIRROR_1,1,me->getVictim());
							tank[0] = me->getVictim()->GetGUID();
						}
					}
					else
					{
						tank[0] = me->getVictim()->GetGUID();
						ModifyAuraStack(SPELL_BLOOD_MIRROR_1,1,me->getVictim());
					}

					if(Unit* MT2 = GetGuidUnit(tank[1]))
					{
						if(nUnit && MT2 != nUnit)
						{
							MT2->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_2);
							ModifyAuraStack(SPELL_BLOOD_MIRROR_2,1,nUnit);
							tank[1] = nUnit->GetGUID();
						}
					}
					else
					{
						if(nUnit)
						{
							tank[1] = nUnit->GetGUID();
							ModifyAuraStack(SPELL_BLOOD_MIRROR_2,1,nUnit);
						}
					}

				}
				checkTanks_Timer = 1000;
			}
			else
				checkTanks_Timer -= diff;

			if(vampir_Timer <= diff)
			{
				vampir_Timer = TEN_MINS;
				if(Unit* u = GetRandomUnit())
				{
					if(u->GetTypeId() == TYPEID_PLAYER)
					{
						u->CastStop();
						u->CastSpell(u,SPELL_VAMPIRIC_BITE,true);
					}
					else
						vampir_Timer = 200;
				}
			}
			else
				vampir_Timer -= diff;

			DoMeleeAttackIfReady();
		}
    }
};

CreatureAI* GetAI_boss_lanathel(Creature* pCreature)
{
    return new boss_lanathelAI(pCreature);
}

void AddSC_ICC_Lanathel()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_lanathel";
    NewScript->GetAI = &GetAI_boss_lanathel;
    NewScript->RegisterSelf();
}
