/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "forge_of_souls.h"

enum spells
{
	SPELL_CORRUPT_SOUL			= 68839,
	SPELL_FEAR					= 68950,
	SPELL_MAGIC_BANE			= 68793,
	SPELL_SHADOW_BOLT			= 70043,
	SPELL_SOULSTORM				= 68872,
	SPELL_TELEPORT				= 68988,
	SPELL_CONSUME				= 68858,
	SPELL_SOULSTORM_AURA        = 68921,
};


struct MANGOS_DLL_DECL boss_bronjahmAI : public LibDevFSAI
{
    boss_bronjahmAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_CORRUPT_SOUL,25000,30000,2000);
		AddEvent(SPELL_FEAR,10000,12000,1000);
		AddEventOnTank(SPELL_MAGIC_BANE,urand(8000,15000),8000,7000);
		AddEventOnTank(SPELL_SHADOW_BOLT,2000,2000);
    }

	bool HasTeleported;
	uint32 CorruptedSoulFrag_Timer;
	uint64 frag;
	uint32 CheckDist_Timer;
	uint32 Teleport_Timer;
	uint32 CheckAura_Timer;
	uint8 subphase,phase;

    void Reset()
    {
		ResetTimers();
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		SetCombatMovement(true);
		CorruptedSoulFrag_Timer = 29000;
		CheckDist_Timer = 20000;
		frag = 0;
		Teleport_Timer = 1500;
		subphase = 0;
		phase = 0;
		CheckAura_Timer = 1500;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

		if(phase == 1)
		{
			if(CheckAura_Timer <= diff)
			{
				Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
				if (!lPlayers.isEmpty())
					for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
						if (Player* pPlayer = itr->getSource())
							if(pPlayer->isAlive())
								SetAuraStack(SPELL_SOULSTORM_AURA,1,pPlayer,me,1);
				CheckAura_Timer = 1500;
			}
			else
				CheckAura_Timer -= diff;
		}
		else
		{
			if(CorruptedSoulFrag_Timer <= diff)
			{
				if(Creature* tmp_add = CallCreature(36535,TEN_MINS,NEAR_7M,GO_TO_CREATOR))
				{
					frag = tmp_add->GetGUID();
					tmp_add->SetReactState(REACT_PASSIVE);
					tmp_add->GetMotionMaster()->MoveFollow(me,2.0f,0.0f);
				}
				CorruptedSoulFrag_Timer = 32000;
			}
			else
				CorruptedSoulFrag_Timer -= diff;

			if(CheckDist_Timer <= diff)
			{	
				if(Creature* crfrag = GetGuidCreature(frag))
					if(crfrag->isAlive())
						if(crfrag->GetDistance2d(me) < 4.0f)
						{
							me->CastStop();
							DoCastMe(SPELL_CONSUME);
							crfrag->ForcedDespawn(500);
						}

				CheckDist_Timer = 1000;
			}
			else
				CheckDist_Timer -= diff;

			if(!HasTeleported && CheckPercentLife(30))
			{
				if(Teleport_Timer <= diff)
				{
					if(subphase == 0)
					{
						DoCastMe(SPELL_TELEPORT);
						Teleport_Timer = 2000;
						subphase++;
					}
					else if(subphase == 1)
					{
						Relocate(5297.3f,2506.6f,686.1f);
						me->CastStop();
						DoCastMe(SPELL_SOULSTORM);
						SetCombatMovement(false);
						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
						phase = 1;
						Teleport_Timer = DAY;
					}
				}
				else
					Teleport_Timer -= diff;
			}
			
		}
		UpdateEvent(diff);
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_bronjahm(Creature* pCreature)
{
    return new boss_bronjahmAI (pCreature);
}

void AddSC_boss_bronjahm()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_bronjahm";
    newscript->GetAI = &GetAI_boss_bronjahm;
    newscript->RegisterSelf();

}
