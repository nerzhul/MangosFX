#include "precompiled.h"
#include "def_culling_of_stratholme.h"

enum
{
    SAY_MALGANIS_AGGRO      = -1594159,  
    SAY_MALGANIS_ESCAPE02	= -1594169, 
    SAY_MALGANIS_ESCAPE01	= -1594176, 
    SAY_MALGANIS_SLAY01     = -1594160, 
    SAY_MALGANIS_SLAY02     = -1594161, 
    SAY_MALGANIS_SLAY03     = -1594162,
    SAY_MALGANIS_SLAY04     = -1594163,
    SAY_MALGANIS_SLAY05     = -1594164,
    SAY_MALGANIS_SLAY06     = -1594165,
    SAY_MALGANIS_SLAY07     = -1594166,
    SAY_MALGANIS_SLEEP01    = -1594174,
    SAY_MALGANIS_SLEEP02    = -1594175,
    SAY_MALGANIS_Sleep      = -1594167,
    SAY_MALGANIS_15HP       = -1594168,
    SAY_MALGANIS_OUTRO      = -1594171,
    SAY_ARTHAS_OUTRO01      = -1594170,
    SAY_ARTHAS_OUTRO02      = -1594172,
    SAY_ARTHAS_OUTRO03      = -1594173,

    SPELL_SWAMP_N           = 52720,
    SPELL_SWAMP_H           = 58852,
    SPELL_MIND_BLAST_N      = 52722,
    SPELL_MIND_BLAST_H      = 58850,
    SPELL_SLEEP_N           = 52721,
    SPELL_SLEEP_H           = 58849,
    SPELL_VAMPIRE           = 52723,

    NPC_ARTHAS              = 26499
};

struct MANGOS_DLL_DECL boss_malganisAI : public LibDevFSAI
{
   boss_malganisAI(Creature *c) : LibDevFSAI(c)
   {
        InitInstance();
		AddEvent(m_difficulty ? SPELL_SWAMP_H : SPELL_SWAMP_N,6300,6700,200);
		AddEvent(m_difficulty ? SPELL_MIND_BLAST_H : SPELL_MIND_BLAST_N,11300,11300);
   }

	bool Finish;
	bool Sleep;
	bool Vampire;
	uint32 Phase;
	uint64 MalganisGUID;
	uint64 ArthasGUID;
	bool Outro;
	uint32 Step;
	uint32 Steptim;
	uint32 Motion;
	uint32 Sleep_Timer;
	uint32 Vampire_Timer;

   void Reset() 
   { 
		Sleep = false;
		Vampire = false;
		Phase = 1;
		Outro = false;
		Step = 1;
		Steptim = 7000;
		Motion = 0;
		Sleep_Timer = 17300;
		Vampire_Timer = 30000;
		if(!Finish)
			Finish = false;
   }
   
	void Aggro(Unit* who)
	{
	   DoScriptText(SAY_MALGANIS_AGGRO, me);
	   if (Creature* pArthas = GetClosestCreatureWithEntry(me, NPC_ARTHAS, 150.0f))
		  ArthasGUID = pArthas->GetGUID();

	   if (pInstance)
	   {
			GameObject* pGate = pInstance->instance->GetGameObject(pInstance->GetData64(DATA_GO_MAL_GATE2));
			if (pGate && !pGate->GetGoState())
			{
				pGate->SetGoState(GO_STATE_READY);
			}
       }

	}

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0,6))
        {
            case 0: DoScriptText(SAY_MALGANIS_SLAY01, me); break;
            case 1: DoScriptText(SAY_MALGANIS_SLAY02, me); break;
            case 2: DoScriptText(SAY_MALGANIS_SLAY03, me); break;
            case 3: DoScriptText(SAY_MALGANIS_SLAY04, me); break;
            case 4: DoScriptText(SAY_MALGANIS_SLAY05, me); break;
            case 5: DoScriptText(SAY_MALGANIS_SLAY06, me); break;
            case 6: DoScriptText(SAY_MALGANIS_SLAY07, me); break;
        }
    }

	void UpdateAI(const uint32 diff)
	{
   		if(!Outro)
		{
			if (!CanDoSomething())
				return;

			if(CheckPercentLife(40))
			{
				if(!false)
				{
					Sleep = true;
					DoScriptText(SAY_MALGANIS_Sleep, me); 
				}

				if (Sleep_Timer <= diff)
				{
					Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
					if(target)
						DoCast(target, m_difficulty ? SPELL_SLEEP_H : SPELL_SLEEP_N);
					switch(rand()%2)
					{
					  case 0: DoScriptText(SAY_MALGANIS_SLEEP01, me); break;
					  case 1: DoScriptText(SAY_MALGANIS_SLEEP02, me); break;
					}
					Sleep_Timer = urand(15500,18000);
				}
				else
					Sleep_Timer -= diff;
			}

			if(CheckPercentLife(25))
			{
				if(!Vampire)
				{
					 Vampire = true;
					 DoScriptText(SAY_MALGANIS_15HP, me); 
					 DoCastMe(SPELL_VAMPIRE);
				}

				if (Vampire_Timer <= diff)
				{
					DoCastMe(SPELL_VAMPIRE);
					Vampire_Timer = 30000;
				}
				else 
					Vampire_Timer -= diff;

			 }

			 if(CheckPercentLife(5))
				Finish = true;
			
			UpdateEvent(diff);
			DoMeleeAttackIfReady();

			if(Creature* Arthas = GetGuidCreature(ArthasGUID))
			{
				 if(Arthas->isDead())
				 {
					   me->setFaction(35);
					   me->RemoveAllAuras();
					   me->DeleteThreatList();
					   me->CombatStop(true);
					   me->InterruptNonMeleeSpells(false);
					   me->SetVisibility(VISIBILITY_OFF);
				 }
			 }
		}

		if(Finish)
		{
				 switch(Step)
				 {
					  case 1:
						   Outro = true;
						   MalganisGUID = me->GetGUID();
						   if(Creature* Malganis = GetGuidCreature(MalganisGUID))
						   {
							   DoScriptText(SAY_MALGANIS_ESCAPE01, Malganis);
							   if (Creature* pArthas = GetClosestCreatureWithEntry(me, NPC_ARTHAS, 150.0f))
									ArthasGUID = pArthas->GetGUID();
							   if(Creature* Arthas = GetGuidCreature(ArthasGUID))
							   {
								   Arthas->setFaction(35);
								   Arthas->RemoveAllAuras();
								   Arthas->DeleteThreatList();
								   Arthas->CombatStop(true);
								   Arthas->InterruptNonMeleeSpells(false);
								   Malganis->setFaction(35);
								   Malganis->RemoveAllAuras();
								   Malganis->DeleteThreatList();
								   Malganis->CombatStop(true);
								   Malganis->InterruptNonMeleeSpells(false);
								   Malganis->GetMotionMaster()->MovePoint(0, 2286.963f, 1484.449f, 127.850f);
								   Arthas->GetMotionMaster()->MovePoint(0, 2299.289f, 1491.944f, 128.362f);
								   Arthas->SetUInt64Value(UNIT_FIELD_TARGET, MalganisGUID);
								   Malganis->SetUInt64Value(UNIT_FIELD_TARGET, ArthasGUID);
							   }
						   }
						   ++Step;
						   Steptim = 10000;
						   break;
					 case 3:
							if(Creature* Malganis = GetGuidCreature(MalganisGUID))
								DoScriptText(SAY_MALGANIS_ESCAPE02, Malganis);
							++Step;
							Steptim = 10000;
							break;
					 case 5:
							if(Creature* Arthas = GetGuidCreature(ArthasGUID))
								DoScriptText(SAY_ARTHAS_OUTRO01, Arthas);
							++Step;
							Steptim = 5000;
							break;
					 case 7:
							if(Creature* Malganis = GetGuidCreature(MalganisGUID))
								DoScriptText(SAY_MALGANIS_OUTRO, Malganis);
							++Step;
							Steptim = 20000;
							break;
					 case 9:
							if(Creature* Malganis = GetGuidCreature(MalganisGUID))
								if(Creature* Arthas = GetGuidCreature(ArthasGUID))
								{
								   Malganis->SetVisibility(VISIBILITY_OFF);
								   Arthas->SetUInt64Value(UNIT_FIELD_TARGET, 0);
								   Arthas->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
								   Arthas->GetMotionMaster()->MovePoint(0, Malganis->GetPositionX(), Malganis->GetPositionY(), Malganis->GetPositionZ());
								}
						   ++Step;
						   Steptim = 3000;
						   break;
					 case 11:
							if(Creature* Arthas = GetGuidCreature(ArthasGUID))
								DoScriptText(SAY_ARTHAS_OUTRO02, Arthas);
							++Step;
							Steptim = 6000;
							break;
					 case 13:
							if(Creature* Arthas = GetGuidCreature(ArthasGUID))
								DoScriptText(SAY_ARTHAS_OUTRO03, Arthas);
							if (pInstance)
							{
								  GameObject* pChest = pInstance->instance->GetGameObject(pInstance->GetData64(DATA_GO_MAL_CHEST));
								  if (pChest && !pChest->isSpawned())
								   {
									 pChest->SetRespawnTime(180000);
								   }
							 }
							++Step;
							Steptim =11000;
							break;
					 case 15:
							if (pInstance)
							   SetInstanceData(TYPE_ARTHAS_EVENT, DONE);
							if(Creature* Arthas = GetGuidCreature(ArthasGUID))
								Arthas->GetMotionMaster()->MovePoint(0, 2243.311f, 1476.025f, 132.352f);
							++Step;
							Steptim =11000;
							break;
					 case 17:
							if(Creature* Arthas = GetGuidCreature(ArthasGUID))
								Arthas->SetVisibility(VISIBILITY_OFF);
							GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
							GiveRandomReward();
							Outro = false;
							++Step;
							Steptim =11000;
							break;
				 }
		}
		else 
			return;

	   if (Steptim <= diff)
	   {
		   ++Step;
		   Steptim = 330000;
	   }
	   else
		Steptim -= diff;
	}
};

CreatureAI* GetAI_boss_malganis(Creature *_Creature)
{
    return new boss_malganisAI(_Creature);
};

void AddSC_boss_malganis()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_malganis";
    newscript->GetAI = &GetAI_boss_malganis;
    newscript->RegisterSelf();

}