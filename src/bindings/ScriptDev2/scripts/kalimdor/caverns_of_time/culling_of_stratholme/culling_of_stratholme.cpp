#include "precompiled.h"
#include "def_culling_of_stratholme.h"
#include "escort_ai.h"

enum
{
    SAY_INTRO01                         = -1594071,    //Arthas
    SAY_INTRO02                         = -1594072,    //Uther
    SAY_INTRO03                         = -1594073,    //Arthas
    SAY_INTRO04                         = -1594074,    //Arthas
    SAY_INTRO05                         = -1594075,    //Uther
    SAY_INTRO06                         = -1594076,    //Arthas
    SAY_INTRO07                         = -1594077,    //Uther
    SAY_INTRO08                         = -1594078,    //Arthas
    SAY_INTRO09                         = -1594079,    //Arthas
    SAY_INTRO10                         = -1594080,    //Uther
    SAY_INTRO11                         = -1594081,    //Arthas
    SAY_INTRO12                         = -1594082,    //Uther
    SAY_INTRO13                         = -1594083,    //Jaina
    SAY_INTRO14                         = -1594084,    //Arthas
    SAY_INTRO15                         = -1594085,    //Uther
    SAY_INTRO16                         = -1594086,    //Arthas
    SAY_INTRO17                         = -1594087,    //Jaina
    SAY_INTRO18                         = -1594088,    //Arthas
    SAY_ENTER01                         = -1594089,    //Arthas
    SAY_ENTER02                         = -1594090,    //Cityman
    SAY_ENTER03                         = -1594091,    //Arthas
    SAY_ENTER04                         = -1594092,    //Crazyman
    SAY_ENTER05                         = -1594093,    //Arthas
    SAY_ENTER06                         = -1594094,    //Malganis
    SAY_ENTER07                         = -1594095,    //Malganis
    SAY_ENTER08                         = -1594096,    //Arthas
    SAY_ENTER09                         = -1594097,    //Arthas
    SAY_PHASE501                        = -1594098,    //Arthas
    SAY_PHASE502                        = -1594099,    //Arthas
    SAY_PEOPLE05                        = -1594100,   //Patricia
    SAY_PEOPLE06                        = -1594101,   //Patricia
    SAY_PEOPLE07                        = -1594103,    //Patricia
    SAY_PEOPLE08                        = -1594105,    //Patricia
    SAY_PEOPLE09                        = -1594106,    //Patricia
    SAY_EPOCH                           = -1594117,   //Arthas Dialog for Epoch
    SAY_MEATHOOK_SPAWN					= -1594110,
    SAY_PEOPLE01                        = -1594107,   //People Run
    SAY_PEOPLE02                        = -1594108,   //People Run
    SAY_PEOPLE03                        = -1594109,   //People Run
    SAY_PEOPLE04                        = -1594104,   //People Run
    SAY_PEOPLE10                        = -1594102,   //People Run
    SAY_PEOPLE11                        = -1594126,   //People Run
    SAY_PEOPLE12                        = -1594127,   //People Run
    SAY_PEOPLE13                        = -1594128,   //People Run
    SAY_PHASE503                        = -1594152,   //Arthas Shkaf 01
    SAY_PHASE504                        = -1594153,   //Arthas Shkaf 02
    SAY_PHASE505                        = -1594142,   //Arthas Glore
    SAY_PHASE506                        = -1594143,   //Arthas That is it
    SAY_PHASE507                        = -1594144,   //Arthas
    SAY_PHASE508                        = -1594145,   //Arthas This Magic Again
    SAY_PHASE509                        = -1594147,   //Arthas We are Close in trap
    SAY_PHASE510                        = -1594146,  //Arthas Lets go
    SAY_PHASE511                        = -1594151,  //Arthas Shkaff tam
    SAY_PHASE601                        = -1594154,  //Arthas Fire
    SAY_PHASE602                        = -1594155,  //Arthas Picnic
    SAY_PHASE603                        = -1594156,  //Arthas Picnic End
    SAY_PHASE604                        = -1594157,  //Arthas Stop Escort ska on ne virybaetsa 
    SAY_PHASE605                        = -1594158,  //Arthas Pipec Malganisy

    NPC_MALGANIS                    = 26533,
    NPC_SCARED_MAN_2				= 31127,
    NPC_DRAKONIAN					= 27744,
    NPC_TIME_RIFT                   = 28409,
    NPC_TIME_RIFT_2                 = 28439,
    NPC_PATRICIA                    = 31028,
    NPC_SCARED_MAN					= 31126,
    NPC_KNIGHT_ESCORT				= 27745,
    NPC_PRIEST_ESCORT				= 27747,
    NPC_JAINA                       = 26497,
    NPC_ARTHAS                      = 26499,
    NPC_UTHER                       = 26528,
    NPC_CITY_MAN                    = 28167,
    NPC_CRAZY_MAN					= 28169,
    NPC_ZOMBIE                      = 27737,
	NPC_INVIS_SIGHT					= 20562,

    SPELL_FEAR                      = 39176,
    SPELL_CHAIN_N					= 52696,
    SPELL_CHAIN_H					= 58823,
    SPELL_EXPLODED_N				= 52666,
    SPELL_EXPLODED_H				= 58821,
    SPELL_FRENZY					= 58841,
    SPELL_ARTHAS_AURA				= 52442,
    SPELL_EXORCISM_N				= 52445,
    SPELL_EXORCISM_H				= 58822,
    SPELL_HOLY_LIGHT				= 52444,

    ENCOUNTER_ZOMBIE_NUMBER			= 4,
    ENCOUNTER_ZOMBIE_NUMBER2		= 8
};

struct MANGOS_DLL_DECL npc_arthasAI : public npc_escortAI
{
    npc_arthasAI(Creature *pCreature) : npc_escortAI(pCreature)
   {
        InitInstance();
        Reset();
   }

	bool Caisses;
	bool EscortStart;
	uint32 Exorcism_Timer;
	uint32 FinalFight;
	bool PhaseC;
	uint32 arthas_event;
	Unit* culling_faction;
	uint64 StalkerMGUID;
	uint64 CrazymanGUID;
	uint64 CitymanGUID;
	uint64 StalkerGUID;
	uint64 TempMalganisGUID;
	uint64 JainaGUID;
	uint64 UtherGUID;
	uint32 phase;
	uint32 phaseAI;
	uint32 phasetim;
	uint64 uiZombieGUID[ENCOUNTER_ZOMBIE_NUMBER];
	uint32 uiZombie_counter;
	uint32 PatriciaEvent;
	uint32 CheckTimer;

   void Reset() 
   {   
		if(arthas_event != 2)
			arthas_event = 0;
       FinalFight = 0;
       phase = 1;
       phasetim = 20000;  
       Exorcism_Timer = 7300;
	   EscortStart = Caisses = false;
	   
	   me->setFaction(35);
		me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
		CheckTimer = 1000;
   }

	void Aggro(Unit* who)
	{
	   DoCastMe( SPELL_ARTHAS_AURA);
	}

    void MoveInLineOfSight(Unit* pWho)
    {
        /*if (!pWho)
            return;*/

		if (!me->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
			me->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(me))
		{
			/*if (!me->canFly() && me->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
				return;*/

			float attackRadius = me->GetAttackDistance(pWho);
			if (me->IsWithinDistInMap(pWho, attackRadius) && me->IsWithinLOSInMap(pWho))
			{
				if (!me->getVictim())
				{
					AttackStart(pWho);
					pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
				}
				else if (me->GetMap()->IsDungeon())
				{
					pWho->SetInCombatWith(me);
					me->AddThreat(pWho, 0.0f);
				}
			}
		}
	}

	void JustDied(Unit *killer)
    {
         if (pInstance)
            pInstance->SetData(TYPE_ARTHAS_EVENT, DONE);
    }

	void AttackStart(Unit* pWho)
	{
		if (!pWho)
			return;

		if (me->Attack(pWho, true))
		{
			me->AddThreat(pWho);
			me->SetInCombatWith(pWho);
			pWho->SetInCombatWith(me);

			if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
				me->GetMotionMaster()->MovementExpired();

			if (IsCombatMovement())
				me->GetMotionMaster()->MoveChase(pWho);
		}
	}

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 1:
				DoScriptText(SAY_PHASE501, me);
                break;
            case 2:
                DoScriptText(SAY_PHASE502, me);
                break;
            case 3:
               me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
               DoScriptText(SAY_PHASE505, me);
                break;
            case 4:
                DoScriptText(SAY_PHASE506, me);
                break;
            case 5:
                DoScriptText(SAY_PHASE507, me);
                break;
            case 9:
                DoScriptText(SAY_PHASE509, me);
                me->SummonCreature(NPC_TIME_RIFT,2410.561f,1187.790f,133.933f,3.15f,TEMPSUMMON_TIMED_DESPAWN,11000);
                me->SummonCreature(NPC_TIME_RIFT,2388.574f,1214.650f,134.239f,3.15f,TEMPSUMMON_TIMED_DESPAWN,11000);     
                me->SummonCreature(NPC_TIME_RIFT,2430.593f,1212.919f,134.124f,3.15f,TEMPSUMMON_TIMED_DESPAWN,11000);                                   
                break;
            case 10:
                DoScriptText(SAY_PHASE510, me);
                break;
            case 11:
                me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                break; 
            case 13:
                 DoScriptText(SAY_PHASE508,me);
                 me->SummonCreature(NPC_TIME_RIFT,2393.985f,1190.519f,148.076f,3.15f,TEMPSUMMON_TIMED_DESPAWN,11000);  
                 me->SummonCreature(NPC_TIME_RIFT,2436.202f,1200.540f,148.077f,3.15f,TEMPSUMMON_TIMED_DESPAWN,11000);  
                break;    
            case 15: 
                DoScriptText(SAY_PHASE511,me);
                me->SummonCreature(NPC_TIME_RIFT_2,2445.629f,1111.500f,148.076f,3.229f,TEMPSUMMON_TIMED_DESPAWN,9000);
                break;                     
            case 16:
                if(Creature* Epoch =me->SummonCreature(26532,2445.629f,1111.500f,148.076f,3.229f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,900000))
				{
					Epoch->setFaction(35);
					me->SetUInt64Value(UNIT_FIELD_TARGET, Epoch->GetGUID());
				}
                break;    
            case 18:
                DoScriptText(SAY_EPOCH, me);
                break;
            case 20:
                me->AddSplineFlag(SPLINEFLAG_WALKMODE);
                break;
            case 23:
                DoScriptText(SAY_PHASE503,me);
                break;
            case 24:
                if (pInstance)
				  {
					 GameObject* pGate = pInstance->instance->GetGameObject(pInstance->GetData64(DATA_GO_SHKAF_GATE));
						  pGate->SetGoState(GO_STATE_ACTIVE);
				  }     
                DoScriptText(SAY_PHASE504,me);
                break;
            case 32:
                DoScriptText(SAY_PHASE601,me);
                me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                break;
            case 34:
               me->AddSplineFlag(SPLINEFLAG_WALKMODE);
               DoScriptText(SAY_PHASE602,me);
				break;
            case 35:
				me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                DoScriptText(SAY_PHASE603,me);
                break;
            case 40:
                DoScriptText(SAY_PHASE604,me);
                break;
            case 41:
                FinalFight = 1;
               me->setFaction(culling_faction->getFaction());
               phaseAI = 95;
               me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
               me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;

             }
    }

   void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

       if(FinalFight == 2)
         {
           switch(phaseAI)
             {
              case 95:
                 me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                 me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                 DoScriptText(SAY_PHASE605, me);
                 me->SummonCreature(NPC_MALGANIS,2296.665f,1502.362f,128.362f,4.961f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,900000);
                 if (pInstance)
                 {
                     GameObject* pGate = pInstance->instance->GetGameObject(pInstance->GetData64(DATA_GO_MAL_GATE1));
                          pGate->SetGoState(GO_STATE_ACTIVE);
                 }                    

                 ++phaseAI;
                 phasetim = 3000;
                 break;
             case 97:
					me->GetMotionMaster()->MovePoint(0, 2303.016f, 1480.070f, 128.139f);
                  ++phaseAI;
                  phasetim = 3000;
                  break;
             case 99:
                  me->setFaction(culling_faction->getFaction());
                  ++phaseAI;
                  phasetim = 3000;
                  break;                       
               }

		   if (phasetim <= diff)
		   {
			   ++phaseAI;
			   phasetim = 330000;
		   } 
		   phasetim -= diff;
	   }

	if(arthas_event == 2)
	{
		if (!(!CanDoSomething()))
		{
			if (Exorcism_Timer <= diff)
			{
				if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
					DoCast(target, m_difficulty ? SPELL_EXORCISM_H : SPELL_EXORCISM_N);

				Exorcism_Timer = 7300;
			}
			else
				Exorcism_Timer -= diff;

			 if(me->GetHealth()*100 / me->GetMaxHealth() < 40)
				 DoCastMe( SPELL_HOLY_LIGHT);

			 DoMeleeAttackIfReady();
		}
	 }  
     else if(arthas_event == 1)
     {
           switch(phase)
           {
              case 1:
					me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
					me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
					me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					if(Creature* Uther =me->SummonCreature(26528,1794.357f,1272.183f,141.558f,1.37f,TEMPSUMMON_TIMED_DESPAWN,180000))
						UtherGUID = Uther->GetGUID();
				   
					if (Creature* pJaina = GetClosestCreatureWithEntry(me, NPC_JAINA, 50.0f))
						JainaGUID = pJaina->GetGUID();
					else if (Unit* pJaina = me->SummonCreature(NPC_JAINA,1895.48f,1292.66f,143.706f,0.023475f,TEMPSUMMON_DEAD_DESPAWN,180000))
						JainaGUID = pJaina->GetGUID();

					if(Creature* Uther = GetGuidCreature(UtherGUID))
					{
						Uther->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
						me->GetMotionMaster()->MovePoint(0, 1903.167f, 1291.573f, 143.32f);
						Uther->GetMotionMaster()->MovePoint(0, 1897.018f, 1287.487f, 143.481f);
						me->SetUInt64Value(UNIT_FIELD_TARGET, Uther->GetGUID());
						Uther->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
					}
					++phase;
					phasetim = 17000;
					break;

             case 3:
					DoScriptText(SAY_INTRO01, me);
					++phase;
					phasetim = 2000;
					break;

             case 5:
					if(Creature* Uther = GetGuidCreature(UtherGUID))
						DoScriptText(SAY_INTRO02, Uther);
					++phase;
					phasetim = 8000;
					break;

             case 7:
					me->AddSplineFlag(SPLINEFLAG_WALKMODE);
					DoScriptText(SAY_INTRO03, me);
					me->GetMotionMaster()->MovePoint(0, 1911.087f, 1314.263f, 150.026f);
					++phase;
					phasetim = 9000;
					break;

             case 9:
					if(Creature* Jaina = GetGuidCreature(JainaGUID))
						Jaina->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
					DoScriptText(SAY_INTRO04, me);
					++phase;
					phasetim = 10000;
					break;

             case 11:
					if(Creature* Uther = GetGuidCreature(UtherGUID))
                       DoScriptText(SAY_INTRO05, Uther);
					++phase;
					phasetim = 1000;
					break;
			 
			 case 13:
	                DoScriptText(SAY_INTRO06, me);
					++phase;
					phasetim = 4000;
					break;

             case 15:
					if(Creature* Uther = GetGuidCreature(UtherGUID))
						DoScriptText(SAY_INTRO07, Uther);
					++phase;
					phasetim = 6000;
					break;

             case 17:
					DoScriptText(SAY_INTRO08, me);
					++phase;
					phasetim = 4000;
					break;

             case 19:
					if(Creature* Uther = GetGuidCreature(UtherGUID))
						DoScriptText(SAY_INTRO09, Uther);
					++phase;
					phasetim = 8000;
					break;

             case 21:
					DoScriptText(SAY_INTRO10, me);
					++phase;
					phasetim = 4000;
					break;

             case 23:
					if(Creature* Uther = GetGuidCreature(UtherGUID))
						DoScriptText(SAY_INTRO11, Uther);
					++phase;
					phasetim = 4000;
					break;

             case 25:
					DoScriptText(SAY_INTRO12, me);
					++phase;
					phasetim = 11000;
					break;

             case 27:
					if(Creature* Jaina = GetGuidCreature(JainaGUID))
						DoScriptText(SAY_INTRO13, Jaina);
					++phase;
					phasetim = 3000;
					break;

             case 29:
					DoScriptText(SAY_INTRO14, me);
					++phase;
					phasetim = 9000;
					break;

             case 31:
					if(Creature* Uther = GetGuidCreature(UtherGUID))
						DoScriptText(SAY_INTRO15, Uther);
					++phase;
					phasetim = 4000;
					break;

             case 33:
					if(Creature* Uther = GetGuidCreature(UtherGUID))
					{
						Uther->AddSplineFlag(SPLINEFLAG_WALKMODE);
						Uther->GetMotionMaster()->MovePoint(0, 1794.357f,1272.183f,140.558f);
					}
					++phase;
					phasetim = 1000;
					break;

             case 35:
					me->SetUInt64Value(UNIT_FIELD_TARGET, JainaGUID);
					if(Creature* Jaina = GetGuidCreature(JainaGUID))
						Jaina->GetMotionMaster()->MovePoint(0, 1794.357f,1272.183f,140.558f);
					++phase;
					phasetim = 1000;
					break;

             case 37:
					DoScriptText(SAY_INTRO16, me);
					++phase;
					phasetim = 1000;
					break;

             case 39:
					if(Creature* Jaina = GetGuidCreature(JainaGUID))
						DoScriptText(SAY_INTRO17, Jaina);
					++phase;
					phasetim = 3000;
					break;

             case 41:
					me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
					me->GetMotionMaster()->MovePoint(0, 1902.959f,1295.127f,143.388f);
					++phase;
					phasetim = 10000;
					break;

             case 43:
					me->GetMotionMaster()->MovePoint(0, 1913.726f,1287.407f,141.927f);
					++phase;
					phasetim = 6000;
					break;

             case 45:
					DoScriptText(SAY_INTRO18, me);
					me->SetUInt64Value(UNIT_FIELD_TARGET, JainaGUID);
					++phase;
					phasetim = 10000;
					break;

             case 47:
					me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
					me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
					if(Creature* Jaina = GetGuidCreature(JainaGUID))
						Jaina->SetVisibility(VISIBILITY_OFF);
					if(Creature* Uther = GetGuidCreature(UtherGUID))
						Uther->SetVisibility(VISIBILITY_OFF);
					me->GetMotionMaster()->MovePoint(0, 1990.833f,1293.391f,145.467f);
					++phase;
					phasetim = 12000;
					break;

             case 49:
					me->GetMotionMaster()->MovePoint(0, 1997.003f,1317.776f,142.963f);
					++phase;
					phasetim = 5000;
					break;

             case 51:
					me->GetMotionMaster()->MovePoint(0, 2019.631f,1326.084f,142.929f);
					++phase;
					phasetim = 4000;
					break;

             case 53:
					me->GetMotionMaster()->MovePoint(0, 2026.469f,1287.088f,143.596f);
					++phase;
					phasetim = 6000;
					break;

             case 55:
					{
					Creature* Cityman =me->SummonCreature(NPC_CITY_MAN,2091.977f,1275.021f,140.757f,0.558f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,45000);//Merging
					if(Cityman)
						CitymanGUID = Cityman->GetGUID();
					
					Creature* Crazyman =me->SummonCreature(NPC_CRAZY_MAN,2093.514f,1275.842f,140.408f,3.801f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,55000); //Merging
					if(Crazyman)
						CrazymanGUID = Crazyman->GetGUID();
					
					me->GetMotionMaster()->MovePoint(0, 2050.660f,1287.333f,142.671f);
					
					++phase;
					phasetim = 6000;
					break;
					}
             case 57:
					if(Creature* Stalker =me->SummonCreature(NPC_INVIS_SIGHT,2026.469f,1287.088f,143.596f,1.37f,TEMPSUMMON_TIMED_DESPAWN,14000))
					{
						StalkerGUID = Stalker->GetGUID();
						me->SetUInt64Value(UNIT_FIELD_TARGET, StalkerGUID);
					}
					++phase;
					phasetim = 1000;
					break;

             case 59:
					DoScriptText(SAY_ENTER01, me);
					++phase;
					phasetim = 12000;
					break;

             case 61:
					me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
					me->AddSplineFlag(SPLINEFLAG_WALKMODE);
					me->GetMotionMaster()->MovePoint(0, 2081.447f,1287.770f,141.3241f);
					++phase;
					phasetim = 15000;
					break;

             case 63:
					me->SetUInt64Value(UNIT_FIELD_TARGET, CitymanGUID);
					if(Creature* Cityman = GetGuidCreature(CitymanGUID))
					{
						Cityman->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
						Cityman->AddSplineFlag(SPLINEFLAG_WALKMODE);
						Cityman->GetMotionMaster()->MovePoint(0, 2088.625f,1279.191f,140.743f);
					}
					++phase;
					phasetim = 2000;
					break;
             case 65:
					if(Creature* Cityman = GetGuidCreature(CitymanGUID))
						DoScriptText(SAY_ENTER02, Cityman);
					++phase;
					phasetim = 4000;
					break;

            case 67:
					me->GetMotionMaster()->MovePoint(0, 2087.689f,1280.344f,140.73f);
					DoScriptText(SAY_ENTER03, me);
					++phase;
					phasetim = 3000;
					break;

            case 69:
					me->HandleEmoteCommand(37);
					++phase;
					phasetim = 1000;
					break;

            case 71:
					if(Creature* Crazyman = GetGuidCreature(CrazymanGUID))
					{
						DoScriptText(SAY_ENTER04, Crazyman);
						me->SetUInt64Value(UNIT_FIELD_TARGET, CrazymanGUID);
						Crazyman->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());	

						if(Creature* Cityman = GetGuidCreature(CitymanGUID))
							Cityman->CastSpell(Cityman,8226,false);
							
					}
					
					me->GetMotionMaster()->MovePoint(0, 2092.154f,1276.645f,140.52f);
					
					++phase;
					phasetim = 5000;
					break;
            case 73:
					me->HandleEmoteCommand(38);
					++phase;
					phasetim = 1000;
					break;

            case 75:
					if(Creature* Crazyman = GetGuidCreature(CrazymanGUID))
						Crazyman->CastSpell(Crazyman,8226,false);
						
					++phase;
					phasetim = 1000;
					break;
           case 77:
	
					me->GetMotionMaster()->MovePoint(0, 2091.179f,1278.065f,140.476f);
					if(Creature* StalkerJ = me->SummonCreature(NPC_INVIS_SIGHT,2081.447f,1287.770f,141.3241f,1.37f,TEMPSUMMON_TIMED_DESPAWN,10000))
					{
						StalkerGUID = StalkerJ->GetGUID();	
						me->SetUInt64Value(UNIT_FIELD_TARGET, StalkerGUID);
					}
			              
					DoScriptText(SAY_ENTER05, me);
					++phase;
					phasetim = 3000;
					break;

           case 79:
					DoScriptText(SAY_TEST, me);
					if(Creature* StalkerM = me->SummonCreature(NPC_INVIS_SIGHT,2117.349f,1288.624f,136.271f,1.37f,TEMPSUMMON_TIMED_DESPAWN,60000))
					{
						StalkerMGUID = StalkerM->GetGUID();
						me->SetUInt64Value(UNIT_FIELD_TARGET, StalkerGUID);
				   
					}
					++phase;
					phasetim = 1000;
					break;

           case 81:
					if(Creature* TempMalganis =me->SummonCreature(26533,2117.349f,1288.624f,136.271f,1.37f,TEMPSUMMON_TIMED_DESPAWN,60000))
					{
						if (Creature* pStalkerM = GetGuidCreature(StalkerMGUID))
							TempMalganis->CastSpell(pStalkerM,63793,false);

						TempMalganisGUID = TempMalganis->GetGUID();
						DoScriptText(SAY_ENTER06, TempMalganis);
						TempMalganis->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
						TempMalganis->setFaction(35);

					me->SetUInt64Value(UNIT_FIELD_TARGET, TempMalganisGUID);
					}
					++phase;
					phasetim = 11000;
					break;

           case 83:
                    phasetim = 500;
                    if (uiZombie_counter < ENCOUNTER_ZOMBIE_NUMBER)
                    {
						if(Creature* StalkerM = GetGuidCreature(StalkerMGUID))
						{
							if (Creature* TempZombie = GetClosestCreatureWithEntry(StalkerM, NPC_CITY_MAN, 100.0f))
							{
								TempZombie->UpdateEntry(NPC_ZOMBIE, 0);
								TempZombie->AddThreat(me,100.0f);
								uiZombie_counter++;
							}
						}
                    }
                    else
                    {
						uiZombie_counter = 0;
                        ++phase;
                    }
                    break;

           case 85:
                    phasetim = 500;
                    if (uiZombie_counter < ENCOUNTER_ZOMBIE_NUMBER)
                    {
						if(Creature* StalkerM = GetGuidCreature(StalkerMGUID))
						{
							if (Creature* TempZombie = GetClosestCreatureWithEntry(StalkerM, NPC_CRAZY_MAN, 100.0f))
							{
								TempZombie->UpdateEntry(NPC_ZOMBIE, 0);
								TempZombie->AddThreat(me,100.0f);
								uiZombie_counter++;
							}
						}
                    }
					else
                    {
						uiZombie_counter = 0;
                        ++phase;
                    }
                    break;            

           case 87:
					if(Creature* TempMalganis = GetGuidCreature(TempMalganisGUID))
	                   DoScriptText(SAY_ENTER07, TempMalganis);

					me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
					++phase;
					phasetim = 17000;
					break;             

           case 89:
					me->SetUInt64Value(UNIT_FIELD_TARGET, StalkerMGUID);
					DoScriptText(SAY_ENTER08, me);
					++phase;
					phasetim = 7000;
					break;

           case 91:
					me->SetUInt64Value(UNIT_FIELD_TARGET, StalkerGUID);
					DoScriptText(SAY_ENTER09, me);
					++phase;
					phasetim = 12000;
					break;

           case 93:
					DoScriptText(SAY_TEST, me);

					pInstance->SetData(TYPE_VAGUE_EVENT,IN_PROGRESS);
					phaseAI = 95;
					me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                    ++phase;
                    phasetim = 1000;
                    break;
              }

		   if (phasetim <= diff)
		   {
			   ++phase;
			   phasetim = 33000;
			  
		   }
		   else
				phasetim -= diff;
       }
	
	
	   if(CheckTimer != -2500)
	   {
		   if(CheckTimer <= diff)
		   {
				if(pInstance)
				{
					if(pInstance->GetData(TYPE_VAGUE_EVENT) == DONE)
					{
						if(!EscortStart)
							if(npc_arthasAI* pEscortAI = dynamic_cast<npc_arthasAI*>(me->AI()))
							{
								me->setFaction(culling_faction->getFaction());
								arthas_event = 2;
								EscortStart = true;
								pEscortAI->Start(false, false, culling_faction->GetGUID(), NULL, false, false);
							}

						CheckTimer = -2500;
					}
					else
						CheckTimer = 1000;
				}
		   }
		   else
			   CheckTimer -= diff;
	   }

	   
    }
};

CreatureAI* GetAI_npc_arthas(Creature* pCreature)
{
    return new npc_arthasAI(pCreature);
}

bool GossipHello_npc_arthas(Player *player, Creature *mCreature)
{
     if (mCreature->isQuestGiver())
        player->PrepareQuestMenu( mCreature->GetGUID());

     if(((npc_arthasAI*)mCreature->AI())->arthas_event == 0)
		player->ADD_GOSSIP_ITEM(0, "Je suis pret pour l'epuration de Stratholme", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

     if(((npc_arthasAI*)mCreature->AI())->FinalFight == 1)
		player->ADD_GOSSIP_ITEM(0, "Nous sommes prets pour le combat final!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    player->PlayerTalkClass->SendGossipMenu(907,mCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_arthas(Player *player, Creature *mCreature, uint32 sender, uint32 action )
{
    if(action == GOSSIP_ACTION_INFO_DEF+2)
    {
         if(((npc_arthasAI*)mCreature->AI())->arthas_event == 0)
          {
           ((npc_arthasAI*)mCreature->AI())->arthas_event = 1;
          }

         if(((npc_arthasAI*)mCreature->AI())->FinalFight == 1)
          {
            ((npc_arthasAI*)mCreature->AI())->FinalFight = 2;
          }

        ((npc_arthasAI*)mCreature->AI())->culling_faction = player;
       		
        player->CLOSE_GOSSIP_MENU();

    }
    return true;
}

struct MANGOS_DLL_DECL npc_patriciaAI : public ScriptedAI
{
    npc_patriciaAI(Creature *c) : ScriptedAI(c) 
	{
	   pInstance = (ScriptedInstance*)c->GetInstanceData();
	   Reset();
	}

	uint32 Step;
	uint32 Steptim;
	bool Event;
	bool Event2;
	bool Event2Com;

   void Reset() 
   {
       Event = false;
       Event2 = true;
       Event2Com = false;
       Step = 1;
       Steptim = 20000;
       if(!Event)
			Event = false;
   }

	void MoveInLineOfSight(Unit *who)
    {      
          if (!Event2 && me->IsWithinDistInMap(who, 20.0f))
            Event2Com = true;

        if (!Event && me->IsWithinDistInMap(who, 20.0f))
            if (pInstance->GetData(TYPE_ARTHAS_EVENT) == IN_PROGRESS)
                  Event = true;
        ScriptedAI::MoveInLineOfSight(who);
    }

	void UpdateAI(const uint32 diff)
    {
		 if(Event == true)
		 {
           switch(Step)
             {
                 case 1:
                           DoScriptText(SAY_PEOPLE05, me);
                           ++Step;
                           Steptim = 5000;
                           break;
                 case 3:
                           me->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                           DoScriptText(SAY_PEOPLE06, me);
                           me->GetMotionMaster()->MovePoint(0, 2395.487f,1203.199f,134.125f); 
                           ++Step;
                           Steptim = 13000;
                           break;
                case 5:
                           me->GetMotionMaster()->MovePoint(0, 2431.674f,1211.797f,134.124f);
                           ++Step;
                           Steptim = 7000;
                           break;
                case 7:
                           me->GetMotionMaster()->MovePoint(0, 2438.028f,1206.680f,133.935f);
                           ++Step;
                           Steptim = 3000;
                           break;
				case 9:
                           me->GetMotionMaster()->MovePoint(0, 2437.524f,1206.971f,133.935f);
                           Event2 = false;
                           ++Step;
                           Steptim = 3000;
                           break; 
				case 11:
                           if(!Event2Com)
							   return;
                           ++Step;
                           Steptim = 1000;
                           break; 
               case 13:
                           DoScriptText(SAY_PEOPLE07, me);
                           ++Step;
                           Steptim = 4000;
                           break; 
               case 15:
                           DoScriptText(SAY_PEOPLE08, me);
                           ++Step;
                           Steptim = 4000;
                           break; 
               case 17:
                           DoScriptText(SAY_PEOPLE09, me);
                           ++Step;
                           Steptim = 9000;
                           break; 
               case 19:
                           me->UpdateEntry(NPC_ZOMBIE, 0);
                           Event = 0;
                           ++Step;
                           Steptim = 7000;
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

	   DoMeleeAttackIfReady();

    }
};

CreatureAI* GetAI_npc_patricia(Creature* pCreature)
{
    return new npc_patriciaAI(pCreature);
}

struct MANGOS_DLL_DECL dark_conversionAI : public ScriptedAI
{
    dark_conversionAI(Creature *c) : ScriptedAI(c) 
	{
	  pInstance = (ScriptedInstance*)c->GetInstanceData();
	  Reset();
	}

	Unit* Target;
	Creature* Arthas;
	bool Conversion;
	uint32 Step;
	uint32 Steptim;

   void Reset() 
   {
       me->setFaction(35);
       Conversion = false;
       Step = 1;
       Steptim = 500;
   }

   void MoveInLineOfSight(Unit *who)
    {
		if (Conversion == false && me->IsWithinDistInMap(who, 30.0f))
		{
			if (pInstance->GetData(TYPE_ARTHAS_EVENT) == IN_PROGRESS)
			{
			  Target = who;
			  Conversion = true;
			}
		}
		ScriptedAI::MoveInLineOfSight(who);
    }

   void UpdateAI(const uint32 diff)
   {

	    if(Conversion == true)
        {
          switch(Step)
          {
           case 1:
                    me->setFaction(14);  //2078
                    me->CastSpell(me,SPELL_FEAR,false);
                            switch(rand()%12)
                                {
                                  case 0: DoScriptText(SAY_PEOPLE01, me); break;
                                  case 1: DoScriptText(SAY_PEOPLE02, me); break;
                                  case 2: DoScriptText(SAY_PEOPLE03, me); break;
                                  case 3: DoScriptText(SAY_PEOPLE04, me); break; 
                                  case 4: DoScriptText(SAY_PEOPLE10, me); break;
                                  case 5: DoScriptText(SAY_PEOPLE11, me); break;
                                  case 6: DoScriptText(SAY_PEOPLE12, me); break;
                                  case 7: DoScriptText(SAY_PEOPLE13, me); break;
                                 }
                      ++Step;
                      Steptim = 5000 + rand()%5000;
                      break;
            case 3:
                     me->UpdateEntry(NPC_ZOMBIE, 0);
                     me->GetMotionMaster()->MovePoint(0, Target->GetPositionX(), Target->GetPositionY(), Target->GetPositionZ());
                     ++Step;
                     Steptim = 1000;
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

	    DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_dark_conversion(Creature* pCreature)
{
    return new dark_conversionAI(pCreature);
}

struct MANGOS_DLL_DECL npc_time_riftCSAI : public ScriptedAI
{
    npc_time_riftCSAI(Creature *c) : ScriptedAI(c) 
	{
		Reset();
	}

	bool Conversion;
	uint32 Step;
	uint32 Steptim;

   void Reset() 
   {
       Conversion = false;
       Step = 1;
       Steptim = 500;
   }

   void UpdateAI(const uint32 diff)
    {
          switch(Step)
          {
			 case 1:
				 if(Creature* Drakonian01 = me->SummonCreature(NPC_DRAKONIAN,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+1,3.229f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,900000))
				 {
					Drakonian01->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
				 }
				 ++Step;
				 Steptim = 3000;
				 break;
			 case 3:
				 if(Creature* Drakonian02 = me->SummonCreature(NPC_DRAKONIAN,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+1,3.229f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,900000))
				 {
					Drakonian02->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
				 }
				 ++Step;
				 Steptim = 3000;
				 break; 
			case 5:   
				 if(Creature*Drakonian03 = me->SummonCreature(NPC_DRAKONIAN,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+1,3.229f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,900000))
				 {
					Drakonian03->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
				 }
				 ++Step;
				 Steptim = 3000;
				 break;  
          }

 	   if (Steptim <= diff)
	   {
		   ++Step;
		   Steptim = 330000;
	   }
	   else
		   Steptim -= diff;

    }
};

CreatureAI* GetAI_npc_time_riftCS(Creature* pCreature)
{
    return new npc_time_riftCSAI(pCreature);
}

void AddSC_culling_of_stratholme()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_arthas";
    newscript->GetAI = &GetAI_npc_arthas;
    newscript->pGossipHello = &GossipHello_npc_arthas;
    newscript->pGossipSelect = &GossipSelect_npc_arthas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "dark_conversion";
    newscript->GetAI = &GetAI_dark_conversion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_patricia";
    newscript->GetAI = &GetAI_npc_patricia;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_time_riftCS";
    newscript->GetAI = &GetAI_npc_time_riftCS;
    newscript->RegisterSelf();
}