/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Yogg-Saron
SD%Complete: 0
SDComment: PH.
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"


#define SOUND_UR_YoggSaron_Insanity01   15759 // Ta volontÃ© ne t'appartient plus
#define SOUND_UR_YoggSaron_Insanity02   15760 // DÃ©truit les laquais, ton maître te l'ordonne !
#define SOUND_UR_FemaleYogg_Help01      15773 // Oui, oui ! Pas de pitiÃ© pour eux. Attaquez sans rÃ©pit
#define SOUND_UR_FemaleYogg_Help02      15774 // Laissez la haine et la colère guider vos bras !
#define SOUND_UR_FemaleYogg_Special01   15776 // Que votre propre haine vous suffoque !
#define SOUND_UR_FemaleYogg_Special02   15777 // Tremblez mortels avant que votre fin n'arrive !
#define SOUND_UR_FemaleYogg_Insanity01  15780 // ImbÃ©cile, faible d'esprit !

// Sara - Phase 2
#define SPELL_SHADOWY_BARRIER           64775 
#define AURA_SHADOWY_BARRIER_1          63894

// Freya
#define SPELL_SANITY_WELL               64170
#define AURA_RESILIENCE_OF_NATURE       62670

// Hodir - Freundlicher Watcher
#define AURA_FORTITUDE_OF_FROST         62650
#define SPELL_HODIRS_PROTECTIVE_GAZE    64174

// Mimiron - Freundlicher Watcher
#define AURA_SPEED_OF_INVENTION         62671
#define SPELL_DESTABILIZATION_MATRIX    65210

// Thorim - Freundlicher Watcher
#define AURA_FURY_OF_THE_STORM          62702
#define SPELL_TITANIC_STORM             64171

// Ominase Wolke - Phase 1
#define AURA_SUMMON_GUARDIAN            62979

// Sanity Well
#define AURA_SANITY_WELL                63288

// Schmettertentakel - Phase 2
#define SPELL_CRUSH                     64147

enum Spells
{
	// p1
	SPELL_SARAS_FERVOR			= 63138,
	SPELL_SARAS_BLESSING		= 63134,
	SPELL_SARAS_ANGER			= 43147,
	
	SPELL_SHADOW_NOVA_10		= 62714,
	SPELL_SHADOW_NOVA_25		= 65209,

	// p2
	SPELL_Sanity				= 63050,
	SPELL_PSYCHOSIS_10			= 65301,
	SPELL_PSYCHOSIS_25			= 63795,
	SPELL_MALADY_OF_THE_MIND	= 63830,
	SPELL_BRAIN_LINK			= 63802,
	SPELL_DEATH_RAY				= 63891,
	SPELL_LUNATIC_GAZE			= 64167,
	SPELL_SHATTERED_ILLUSION	= 65238,

	SPELL_DOMINATE_MIND         = 63120,

	// tentacles
	SPELL_ERUPT					= 64144,
	// - tank
	SPELL_DIMINISH_POWER        = 64145,
	SPELL_FOCUSED_ANGER         = 57688,
	// - constrictor
	SPELL_SQUEEZE_10			= 64125,
	SPELL_SQUEEZE_25			= 64126,
	// - caster
	SPELL_APATHY				= 64156,
	SPELL_BLACK_PLAGUE			= 64153,
	SPELL_CURSE_OF_DOOM			= 64157,
	SPELL_DRAINING_POISON		= 64152,

	// brain
	SPELL_CRAZY_BRAIN			= 64059,

	// Death ray
	SPELL_DEATH_RAY_PERIODIC	= 63883,
	SPELL_DEATH_RAY_VISUAL		= 63893,
	
	// p3
	SPELL_SHADOW_BEACON			= 64465,
	SPELL_Deafening_Roar		= 64189,

	// nuage
	SPELL_NUAGE					= 63084,

	// guardian
	SPELL_DARK_VOLLEY			= 63038,
	SPELL_SHADOW_NOVA			= 62714,
	SPELL_SHADOW_NOVA_H			= 65209,

	SPELL_EXINGUISH_ALL_LIFE    = 64166,
};

enum Adds
{
	NPC_GUARDIAN_OF_YOGG		=	33136,
	NPC_NUAGE					=	33292,
	NPC_TENTACLE_TANK			=	33966,
	NPC_TENTACLE_CAST			=	33985,
	NPC_TENTACLE_CONSTRICTOR	=	33983,
	NPC_PORTAL_TO_MADNESS		=	34072,
	OBJ_FLEE_TO_SURFACE			=	194625,
	NPC_IMMORTAL_GUARDIAN		=	33988,
	NPC_YOGGSARON_BRAIN			=	33890,
	NPC_TENTACLE_INFLUENCE		=	33943,
	NPC_SANITY_WELL				=	33991,
};
const static float Locs[6][3] =
{
	{1909.245f,	100.353f,	239.666f},	// stormwind
	{2178.169f,	-24.379f,	246.806f},	// chamber of aspects
	{1902.473f,	-160.262f,	239.989f},	// icecrown cathedral

	{1990.530f,	36.647f,	331.6f	},	// freya well 1
	{2045.205f,	-23.607f,	329.8f	},	// freya well 2
	{1986.114f,	-96.745f,	330.7f	},	// freya well 3
};

const static float EventLocs[6][3][3] =
{
	{{1930.534f,	52.564f,	241.043f },{2103.959f,	-16.906f,	242.646f},{1948.654f,	-84.910f,	239.989f}},
	{{1895.398f,	71.765f,	239.666f },{2089.393f,	-61.722f,	239.719f},{1948.333f,	-120.078f,	239.989f}},
	{{1920.601f,	86.908f,	240.376f },{2050.807f,	-26.485f,	239.719f},{1915.779f,	-106.437f,	239.989f}},
	{{1930.691f,	68.498f,	242.376f },{2079.520f,	-2.875f,	239.787f},{1932.162f,	-128.573f,	239.989f}},
	{{1947.385f,	37.697f,	242.400f },{2110.745f,	-37.402f,	242.646f},{1938.041f,	-99.739f,	239.989f}},
	{{1918.318f,	61.510f,	241.709f },{2141.577f,	0.087f,		239.731f},{1958.820f,	-91.566f,	239.989f}},
};

const static float PortalLoc[10][3] = 
{
	{1952.69f,-35.41f,325.9f},
	{2009.76f,-23.71f,325.93f},
	{1989.63f,3.36f,326.5f},
	{1968.7f,-55.52f,326.14f},
	{2005.842f,-8.36f,326.44f},
	{2012.838f,-26.72f,326.59f},
	{1985.43f,-58.79f,325.92f},
	{1952.14f,-42.16f,325.86f},
	{1950.98f,-19.36f,326.52f},
	{1973.24f,4.05f,326.45f}
};

const static float GuardianLoc[4][4] =
{
	{1938.109f,-91.683f,338.46f,33213}, // hodir
	{2037.712f,-74.302f,338.42f,33242}, // thorim
	{2037.307f,26.558f,338.42f,33244}, // mimiron
	{1938.523f,43.349f,338.46f,33241} // freya
};

enum YoggEvents
{
	EVENT_POP		=	0,
	EVENT_PHASE2	=	1,
	EVENT_PHASE3	=	2,
};

struct MANGOS_DLL_DECL boss_yoggsaronAI : public LibDevFSAI
{
    boss_yoggsaronAI(Creature *c) : LibDevFSAI(c)
    {
        InitInstance();
		AddSummonEvent(NPC_TENTACLE_TANK,15000,75000,EVENT_PHASE2,15000,1,TEN_MINS*2,NEAR_45M);
		AddSummonEvent(NPC_TENTACLE_CAST,2000,30000,EVENT_PHASE2,3000,2,TEN_MINS*2,NEAR_45M);
		AddSummonEvent(NPC_TENTACLE_CONSTRICTOR,20000,45000,EVENT_PHASE2,35000,1,TEN_MINS*2,NEAR_45M);
		AddSummonEvent(NPC_IMMORTAL_GUARDIAN,15000,35000,EVENT_PHASE3,0,1,TEN_MINS,NEAR_45M);
		AddEvent(SPELL_SHADOW_BEACON,12000,30000,5000,HEAL_MY_FRIEND,EVENT_PHASE3);
		AddEvent(SPELL_DEATH_RAY,10000,30000,1000);
		AddEvent(SPELL_MALADY_OF_THE_MIND,18000,18000,7000,TARGET_RANDOM,EVENT_PHASE2);
		AddEventOnMe(SPELL_BRAIN_LINK,20000,32000,0,EVENT_PHASE2);
    }

	uint32 Event_Timer;
	uint32 TentacleText_Timer;
	uint8 Event;
	uint8 eStep;
	uint8 randomVision;
	uint32 Psychosis_Timer;
	uint32 CheckTimer;
	uint32 LunaticGaze_Timer;
	uint32 CheckPlayerSight_Timer;
	uint32 EndPortal_Timer;
	uint32 SaraTimer;
	uint32 ExitDream_Timer;

	std::vector<uint64> DreamAdds;

	bool isBrainPhase;
	bool AchCrazy;

    void Reset()
    {
		ResetTimers();
		UnControlPlayers();
		me->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.05f);
	    if(!me->HasAura(AURA_SHADOWY_BARRIER_1, 0))
            me->CastSpell(me, AURA_SHADOWY_BARRIER_1, true);

		CheckPlayerSight_Timer = 1000;		
		Event = EVENT_POP;
		Event_Timer = 500;
		EndPortal_Timer = 60000;
		ExitDream_Timer = DAY*10;
		eStep = 0;
		AggroAllPlayers(200.0f);
		SaraTimer = 900000;
		DreamAdds.clear();
		if (pInstance)
		{
			pInstance->SetData(DATA_YOGG_NUAGE,1);
            pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
		}
		
		isBrainPhase = false;
		LunaticGaze_Timer = 5000;
		uint8 randomVision = 3;
		TentacleText_Timer = 15000;
		Psychosis_Timer = 15000;
		CheckTimer = 500;
		AchCrazy = true;
    }

	uint8 GetVision()
	{
		return randomVision;
	}

    void KilledUnit(Unit *victim)
    {
		if(urand(0,1))
			Yell(15757,"Hahahahaha ! hahahahaha !");
		else
			Yell(15758,"Une souffrance Ã©ternelle vous attend.");
    }

    void JustDied(Unit *victim)
    {
		UnControlPlayers();
		Yell(15761,"Votre destin est scellÃ©, la fin des temps est enfin arrivÃ©e pour vous et pour tous les habitants de ces petits bourgeons !");
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE,3);
		if (pInstance)
		{
            pInstance->SetData(TYPE_YOGGSARON, DONE);
			uint8 guardians = 0;
			if(pInstance->GetData(TYPE_HODIR) == DONE)
				guardians++;
			if(pInstance->GetData(TYPE_THORIM) == DONE)
				guardians++;
			if(pInstance->GetData(TYPE_MIMIRON) == DONE)
				guardians++;
			if(pInstance->GetData(TYPE_FREYA) == DONE)
				guardians++;
			if(guardians < 4)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3161 : 3157);
			if(guardians < 3)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3162 : 3141);
			if(guardians < 2)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3163 : 3158);
			if(guardians < 1)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3164 : 3159);
			if(AchCrazy)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3010 : 3008);
			if(SaraTimer > 480000)
				pInstance->CompleteAchievementForGroup(m_difficulty ? 3013 : 3012);

		}
    }

    void Aggro(Unit* pWho)
    {
		SetCombatMovement(false);

        if (pInstance)
            pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
    }

	void LaunchVisionEvent()
	{
		randomVision = urand(0,2);
		switch(randomVision)
		{
			case 0:
				switch(urand(0,2))
				{
					case 0:
						Yell(15762,"Un milliers de morts...");
						break;
					case 1:
						Yell(15763,"Ou un unique meurtre");
						break;
					case 2:
						Yell(15764,"Vos querelles mesquines ne font que me renforcer");
						break;
				}
				break;
			case 1:
				Yell(15765,"Sa progÃ©niture a vite appris ses leçons. Vous allez vite apprendre la vôtre.");
				break;
			case 2:
				if(urand(0,1))
					Yell(15766,"Il'Rilne Shol Anal");
				else
					Yell(15767,"Il apprendra... aucun règne n'est Ã©ternel, sauf celui de la mort");
				break;
			default:
				break;
		}

		for(uint8 i=0;i<6;i++)
			if(Creature* cr = CallCreature(NPC_TENTACLE_INFLUENCE,TEN_MINS,PREC_COORDS,NOTHING,EventLocs[i][randomVision][0],EventLocs[i][randomVision][1],EventLocs[i][randomVision][2]))
				DreamAdds.push_back(cr->GetGUID());

		/*if(Creature* Brain = GetInstanceCreature(DATA_YOGG_BRAIN))
			Brain->CastSpell(Brain,SPELL_CRAZY_BRAIN,false);*/

		PoPYoggPortals();
	}

	void ControlPlayer(Player* plr)
	{
		ModifyAuraStack(SPELL_DOMINATE_MIND,1,plr);
		AchCrazy = false;
	}

	void UnControlPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
					{
						if(pPlayer->HasAura(SPELL_DOMINATE_MIND))
							Kill(pPlayer);
					}
	}

	void PoPYoggPortals()
	{
		uint8 portalCount = 10;
		if(!m_difficulty)
			portalCount = 4;
		for(uint8 i=0;i<portalCount;i++)
			CallCreature(NPC_PORTAL_TO_MADNESS,10000,PREC_COORDS,NOTHING,PortalLoc[i][0],PortalLoc[i][1],PortalLoc[i][2]);

		if(pInstance)
			pInstance->SetData(DATA_YOGG_END_PORTALS,1);

		BossEmote(0,"Un portail s'ouvre vers l'esprit de Yogg Saron");
	}

	void CheckLinkedPlayers()
	{
		uint64 Linked[2];
		Linked[0] = Linked[1] = 0;
		uint8 i=0;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
					{
						if(pPlayer->HasAura(SPELL_BRAIN_LINK) && i < 2)
						{
							Linked[i] = pPlayer->GetGUID();
							i++;
						}
						if(!pPlayer->HasAura(SPELL_DOMINATE_MIND) && !pPlayer->HasAura(SPELL_Sanity))
							ControlPlayer(pPlayer);
					}

		if(Linked[0] && Linked[1])
		{
			if(Unit* u1 = GetGuidUnit(Linked[0]))
				if(Unit* u2 = GetGuidUnit(Linked[1]))
				{
					if(u1->GetDistance(u1) > 20.0f)
					{
						if(u1->isAlive())
						{
							DoCast(u2,63803);
							ModifySanity(2,u1);
						}
						if(u2->isAlive())
						{
							DoCast(u1,63803);
							ModifySanity(2,u2);
						}
					}
				}
		}
	}

	void ModifySanity(uint16 count,Unit* tar)
	{
		if(!tar || count < 1 || !tar->HasAura(SPELL_Sanity))
			return;
		
		int32 stk = 0;
		if(tar->HasAura(SPELL_Sanity))
			stk = tar->GetAura(SPELL_Sanity,0)->GetStackAmount();
		if(stk - count < 1)
		{
			tar->RemoveAurasDueToSpell(SPELL_Sanity);
			if(tar->GetTypeId() == TYPEID_PLAYER)
				ControlPlayer((Player*)tar);
		}
		else
			ModifyAuraStack(SPELL_Sanity,-count,tar,tar);
	}

	void SpellHitTarget(Unit* pWho, const SpellEntry* spell)
	{
		switch(spell->Id)
		{
			case 64164:
				ModifySanity(4,pWho);
				break;
			case 64168:
				ModifySanity(2,pWho);
				break;
			case SPELL_MALADY_OF_THE_MIND:
				ModifySanity(3,pWho);
				break;
		}
	}
	void CreateEndPortals()
	{
		BossEmote(0,"Les illusions se dissipent et un portail vers la surface apparait");
		if(pInstance)
			pInstance->SetData(DATA_YOGG_END_PORTALS,0);
	}

	void CheckPlayerSight()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasInArc(M_PI,me) && pPlayer->GetPositionZ() > 310.0f)
					{
						if(Event == EVENT_PHASE3)
						{
							if(m_difficulty)
								DoCast(pPlayer,64164);
							else
								DoCast(pPlayer,64168);
						}
						
						if((Event == EVENT_PHASE2 || Event == EVENT_PHASE3) && me->HasInArc(M_PI,pPlayer))
							ModifySanity(1,pPlayer);
					}
	}

	void ControlAllDreamPlayers()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->GetPositionZ() < 280.0f && !pPlayer->isGameMaster())
						ControlPlayer(pPlayer);
	}

	void SetEnrageTimer(uint32 time)
	{
		SaraTimer = 900000 - time;
	}

	void GoPhase3()
	{
		Event = EVENT_PHASE3;
		Yell(15755,"Contemplez le vrai visage de la mort et sachez que votre fin approche");
		me->RemoveAurasDueToSpell(AURA_SHADOWY_BARRIER_1);
	}

    void UpdateAI(const uint32 diff)
    {
		if(CheckPlayerSight_Timer <= diff)
		{
			CheckPlayerSight();
			CheckPlayerSight_Timer = 1000;
		}
		else
			CheckPlayerSight_Timer -= diff;

		if(SaraTimer <= diff)
		{
			me->CastStop();
			DoCastVictim(SPELL_EXINGUISH_ALL_LIFE);
			SaraTimer = 900000;
		}
		else
			SaraTimer -= diff;

		switch(Event)
		{
			case EVENT_POP:
				if(Event_Timer <= diff)
				{
					me->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.05f * (eStep + 2));
					eStep++;
					if(eStep == 18)
					{
						eStep = 0;
						Event = EVENT_PHASE2;
						Event_Timer = 50000;
						EndPortal_Timer = 110000;
					}
					else
						Event_Timer = 500;
				}
				else
					Event_Timer -= diff;
				return;
			case EVENT_PHASE2:
				if(Event_Timer <= diff)
				{
					LaunchVisionEvent();
					isBrainPhase = true;
					if(pInstance)
						pInstance->SetData(DATA_YOGGBRAIN_DOOR,0);
					Event_Timer = 120000;
				}
				else
					Event_Timer -= diff;

				if(EndPortal_Timer <= diff)
				{
					CreateEndPortals();
					if(pInstance) pInstance->SetData(DATA_YOGG_TENTACLES_FROZEN,1);
					me->RemoveAurasDueToSpell(SPELL_SHATTERED_ILLUSION);
					isBrainPhase = false;
					EndPortal_Timer = 120000;
					ExitDream_Timer = 15000;
				}
				else
					EndPortal_Timer -= diff;

				if(ExitDream_Timer <= diff)
				{
					ControlAllDreamPlayers();
					ExitDream_Timer = 120000;
				}
				else
					ExitDream_Timer -= diff;

				if(TentacleText_Timer <= diff)
				{
					Yell(15756,"La folie vous emportera !");
					TentacleText_Timer = urand(60000,75000);
				}
				else
					TentacleText_Timer -= diff;

				if(Psychosis_Timer <= diff)
				{
					if(Unit* target = GetRandomUnit())
					{
						DoCast(target,(m_difficulty ? SPELL_PSYCHOSIS_25 : SPELL_PSYCHOSIS_10));
						ModifySanity((m_difficulty ? 9 : 12),target);
					}
					Psychosis_Timer = urand(10000,15000);
				}
				else
					Psychosis_Timer -= diff;

				if(CheckTimer <= diff)
				{
					CheckLinkedPlayers();
					if(isBrainPhase)
					{
						bool Alive = false;
						for(std::vector<uint64>::const_iterator itr = DreamAdds.begin(); itr != DreamAdds.end(); ++itr)
							if(Creature* cr = GetGuidCreature(*itr))
								if(cr->isAlive())
									Alive = true;
						if(!Alive && pInstance)
						{
							pInstance->SetData(DATA_YOGG_TENTACLES_FROZEN,0);
							ModifyAuraStack(SPELL_SHATTERED_ILLUSION);
							switch(randomVision)
							{
								case 0:
									pInstance->SetData(DATA_YOGGBRAIN_DOOR,1);
									break;
								case 1:
									pInstance->SetData(DATA_YOGGBRAIN_DOOR,2);
									break;
								case 2:
									pInstance->SetData(DATA_YOGGBRAIN_DOOR,3);
									break;
							}
						}
					}
					CheckTimer = 1000;
				}
				else
					CheckTimer -= diff;
				break;
			case EVENT_PHASE3:
				DoMeleeAttackIfReady();
				break;
			default:
				break;
		}
		
		UpdateEvent(diff,Event);
		UpdateEvent(diff);

		if(!me->SelectHostileTarget() && !me->getVictim())
		{
			if(Creature* Sara = GetInstanceCreature(DATA_YOGG_SARA))
			{
				Sara->Respawn();
				me->ForcedDespawn(1000);
			}
		}	
	}
};

struct MANGOS_DLL_DECL yogg_brainAI : public LibDevFSAI
{
    yogg_brainAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }
	bool phase3;

    void Reset()
    {
		phase3 = false;
		SetCombatMovement(false);
		me->setFaction(14);
		me->GetMotionMaster()->MovePoint(0,me->GetPositionX()+0.2f,me->GetPositionY(),me->GetPositionZ()+0.2f);
    }

	void SpellHit(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == SPELL_CRAZY_BRAIN)
		{
			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* pPlayer = itr->getSource())
						if(Creature* Yogg = GetInstanceCreature(TYPE_YOGGSARON))
							if(Yogg->isAlive() && pPlayer->isAlive() && pPlayer->GetPositionZ() < 300.0f && !pPlayer->isGameMaster())
							{
								((boss_yoggsaronAI*)((Creature*)Yogg)->AI())->ModifySanity(100,pPlayer);
								pPlayer->TeleportTo(pPlayer->GetMap()->GetId(),1980.528f,-29.373f,324.9f,0);
							}
		}
	}

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		float pct = float(me->GetHealth() - dmg) / float(me->GetMaxHealth());
		if(Creature* Yogg = GetInstanceCreature(TYPE_YOGGSARON))
			if(Yogg->isAlive())
				Yogg->SetHealth(pct * Yogg->GetMaxHealth());
		if(CheckPercentLife(30))
		{
			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* pPlayer = itr->getSource())
						if(pPlayer->isAlive() && pPlayer->GetPositionZ() < 300.0f)
							pPlayer->TeleportTo(pPlayer->GetMap()->GetId(),1980.528f,-29.373f,324.9f,0);
		}
	}

    void UpdateAI(const uint32 diff)
    {
		if(CheckPercentLife(30) && !phase3)
		{
			phase3 = true;
			if(Creature* Yogg = GetInstanceCreature(TYPE_YOGGSARON))
				if(Yogg->isAlive())
					((boss_yoggsaronAI*)((Creature*)Yogg)->AI())->GoPhase3();
		}
    }
};

struct MANGOS_DLL_DECL npc_freya_helpAI : public LibDevFSAI
{
    npc_freya_helpAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_SANITY_WELL,2000,45000);
    }

    void Reset()
    {
		ResetTimers();
	    if(!me->HasAura(AURA_RESILIENCE_OF_NATURE, 0))
            DoCastMe(AURA_RESILIENCE_OF_NATURE);
    }

	void SpellHitTarget(Unit* pWho, const SpellEntry* spell)
	{
		if(spell->Id == SPELL_SANITY_WELL)
			if(Creature* Yogg = GetInstanceCreature(TYPE_YOGGSARON))
				if(Yogg->isAlive())
					((boss_yoggsaronAI*)((Creature*)Yogg)->AI())->CallCreature(NPC_SANITY_WELL,60000,NEAR_45M,NOTHING);
	}

    void UpdateAI(const uint32 diff)
    {
        UpdateEvent(diff);
    }
};

struct MANGOS_DLL_DECL npc_thorim_helpAI : public LibDevFSAI
{
    npc_thorim_helpAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_TITANIC_STORM,5000,THREE_MINS);
    }

    void Reset()
    {
		ResetTimers();
	    if(!me->HasAura(AURA_FURY_OF_THE_STORM, 0))
            me->CastSpell(me, AURA_FURY_OF_THE_STORM, true);
    }
    void UpdateAI(const uint32 diff)
    {
        UpdateEvent(diff);
    }
};

struct MANGOS_DLL_DECL npc_hodir_helpAI : public LibDevFSAI
{
    npc_hodir_helpAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_HODIRS_PROTECTIVE_GAZE,6000,30000);
    }

    void Reset()
    {
		ResetTimers();
	    if(!me->HasAura(AURA_FORTITUDE_OF_FROST, 0))
            me->CastSpell(me, AURA_FORTITUDE_OF_FROST, true);
    }

    void UpdateAI(const uint32 diff)
    {
        UpdateEvent(diff);
    }
};

// Mimiron - Freundlicher Watcher
struct MANGOS_DLL_DECL npc_mimiron_helpAI : public LibDevFSAI
{
    npc_mimiron_helpAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

    void Reset()
    {
		ResetTimers();
	    if(!me->HasAura(AURA_SPEED_OF_INVENTION , 0))
            me->CastSpell(me, AURA_SPEED_OF_INVENTION , true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
    }
};


// Sanity Well
struct MANGOS_DLL_DECL npc_sanity_wellAI : public LibDevFSAI
{
    npc_sanity_wellAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint32 Check_Timer;

    void Reset()
    {
		SetCombatMovement(false);
	    if(!me->HasAura(64169))
            ModifyAuraStack(64169);
		if(!me->HasAura(AURA_SANITY_WELL))
            ModifyAuraStack(AURA_SANITY_WELL);

		Check_Timer = 2000;
    }

	void CheckDistance()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
					{
						if(Creature* Sara = GetInstanceCreature(DATA_YOGG_SARA))
							if(pPlayer->HasAura(SPELL_Sanity) && pPlayer->GetDistance2d(me) < 12.0f)
								ModifyAuraStack(SPELL_Sanity,20,pPlayer,Sara);
					}
	}
    void UpdateAI(const uint32 diff)
    {
		if(Check_Timer <= diff)
		{
			CheckDistance();
			Check_Timer = 2000;
		}
		else
			Check_Timer -= diff;
    }

};

struct MANGOS_DLL_DECL boss_yogg_nuageAI : public LibDevFSAI
{
    boss_yogg_nuageAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint32 Check_Timer;

    void Reset()
    {
	    if(!me->HasAura(SPELL_NUAGE,0))
            DoCastMe(SPELL_NUAGE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		Check_Timer = 500;
    }

	void CheckDistance()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
					{
						if(me->GetDistance2d(pPlayer) < 7.0f)
						{
							CallCreature(NPC_GUARDIAN_OF_YOGG);
							return;
						}
					}
	}

    void UpdateAI(const uint32 diff)
    {
		if(Check_Timer <= diff)
		{
			CheckDistance();
			Check_Timer = 6000;
		}
		else
			Check_Timer -= diff;
		
		UpdateEvent(diff);
    }

};

struct MANGOS_DLL_DECL npc_saraAI : public LibDevFSAI
{
    npc_saraAI(Creature *c) : LibDevFSAI(c)
    {
        InitInstance();
		AddEvent(SPELL_SARAS_FERVOR,10000,15000,10000,PLAYER_RANDOM);
		AddEvent(SPELL_SARAS_BLESSING,21000,15000,10000,PLAYER_RANDOM);
		AddEvent(SPELL_SARAS_ANGER,12000,18000,12000);
		AddTextEvent(15775,"Il sera bientôt temps de frapper la tête de la bête, concentrez vos rage sur ces laquais",12000,DAY*100);
    }

	bool EventStarted;
	uint32 CheckTimer;
	uint32 SpawnEvent_Timer;
	uint32 Enrage_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		if(!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		if(!me->HasAura(SPELL_SHADOWY_BARRIER))
			DoCastMe(SPELL_SHADOWY_BARRIER);

		me->SetHealth(me->GetMaxHealth());

		EventStarted = false;
		CheckTimer = 1000;
		Enrage_Timer = 0;
		if (pInstance)
		{
			pInstance->SetData(DATA_YOGG_NUAGE,0);
            pInstance->SetData(TYPE_YOGGSARON, NOT_STARTED);
			pInstance->SetData(DATA_YOGG_TENTACLES_FROZEN,1);
		}
		SpawnEvent_Timer = 15000;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
						if(me->GetDistance2d(pPlayer) < 350.0f && !pPlayer->isGameMaster())
							me->RemoveAurasDueToSpell(SPELL_Sanity);
    }

    void KilledUnit(Unit *victim)
    {
		if(urand(0,1))
			Yell(15778,"Incapables !");
		else
			Yell(15779,"Aurait-il pu être sauvÃ© ?");
    }

	void JustDied(Unit *victim)
    {
		Yell(15754,"Je suis le rêve Ã©veillÃ©, le monstre de vos cauchemars, le dÃ©mon aux milliers de visages, tremblez devant mon vÃ©ritable aspect, à genoux devant le dieu de la mort !");
		CallCreature(NPC_YOGGSARON,DAY*1000,PREC_COORDS,AGGRESSIVE_RANDOM,1976.812f, -25.675f, 328.980f,true);
		if(Creature* Yogg = GetInstanceCreature(TYPE_YOGGSARON))
			if(Yogg->isAlive())
				((boss_yoggsaronAI*)((Creature*)Yogg)->AI())->SetEnrageTimer(Enrage_Timer);
		CallCreature(NPC_YOGGSARON_BRAIN,DAY*1000,PREC_COORDS,NOTHING,1981.512f,-22.89f,255.712f,true);
	}

	void StartEvent()
	{
		if(EventStarted)
			return;

		EventStarted = true;
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		if(urand(0,1))
			Yell(15771,"Aaaaaaaaaaaaaaaaah ! Au secours ! Je vous en prie, arrêtez les !");
		else
			Yell(15772,"Qu'est ce que vous me voulez ? Laissez moi tranquille !");

		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
						if(me->GetDistance2d(pPlayer) < 350.0f && !pPlayer->isGameMaster())
						{
							me->RemoveAurasDueToSpell(SPELL_Sanity);
							ModifyAuraStack(SPELL_Sanity,100,pPlayer);
						}

		if(pInstance)
		{
            pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
			if(pInstance->GetData(TYPE_HODIR) == DONE)
				if(Creature* cr = CallCreature(GuardianLoc[0][3],TEN_MINS*1.5,PREC_COORDS,NOTHING,GuardianLoc[0][0],GuardianLoc[0][1],GuardianLoc[0][2]))
					cr->SetInFront(me);
			if(pInstance->GetData(TYPE_THORIM) == DONE)
				if(Creature* cr = CallCreature(GuardianLoc[1][3],TEN_MINS*1.5,PREC_COORDS,NOTHING,GuardianLoc[1][0],GuardianLoc[1][1],GuardianLoc[1][2]))
					cr->SetInFront(me);
			if(pInstance->GetData(TYPE_MIMIRON) == DONE)
				if(Creature* cr = CallCreature(GuardianLoc[2][3],TEN_MINS*1.5,PREC_COORDS,NOTHING,GuardianLoc[2][0],GuardianLoc[2][1],GuardianLoc[2][2]))
					cr->SetInFront(me);
			if(pInstance->GetData(TYPE_FREYA) == DONE)
				if(Creature* cr = CallCreature(GuardianLoc[3][3],TEN_MINS*1.5,PREC_COORDS,NOTHING,GuardianLoc[3][0],GuardianLoc[3][1],GuardianLoc[3][2]))
					cr->SetInFront(me);
		}
	}

	void CheckDistance()
	{
		bool find = false;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
					{
						if(!EventStarted && !pPlayer->isGameMaster() &&  me->GetDistance2d(pPlayer) < 90.0f && me->GetDistanceZ(pPlayer) < 15.0f)
						{
							StartEvent();
							find = true;
							return;
						}

						if(EventStarted)
							if(!pPlayer->isGameMaster() && me->GetDistance2d(pPlayer) < 130.0f && me->GetDistanceZ(pPlayer) < 15.0f)
							{
								find = true;
								return;
							}
					}
		if(!find)
			Reset();
	}

    void UpdateAI(const uint32 diff)
    {
		if(CheckTimer <= diff)
		{
			CheckDistance();
			CheckTimer = 1000;
		}
		else
			CheckTimer -= diff;

		if(EventStarted)
		{
			Enrage_Timer += diff;

			if(SpawnEvent_Timer <= diff)
			{
				if(Creature* nuage = GetInstanceCreature(DATA_YOGG_NUAGE))
				{
					((boss_yogg_nuageAI*)(nuage->AI()))->CallCreature(NPC_GUARDIAN_OF_YOGG);
				}
				SpawnEvent_Timer = 45000;
			}
			else
				SpawnEvent_Timer -= diff;
			UpdateEvent(diff);
			DoMeleeAttackIfReady();
		}
    }

};

struct MANGOS_DLL_DECL npc_yoggguardAI : public LibDevFSAI
{
    npc_yoggguardAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_DARK_VOLLEY,5000,10000,2000);
    }

	bool die;
    void Reset()
    {
		ResetTimers();
		die = false;
		AggroAllPlayers(200.0f);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
			return;

		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(pDoneBy == me)
			return;

		if(dmg >= me->GetHealth())
		{
			dmg = 0;
			if(Unit* Sara = GetInstanceCreature(DATA_YOGG_SARA))
			{
				if(Sara && Sara->isAlive() && !die)
					if(Sara->GetDistance2d(me) < 8.0f)
					{
						me->CastStop();
						DoCastMe((m_difficulty ? SPELL_SHADOW_NOVA_H : SPELL_SHADOW_NOVA));
						me->DealDamage(Sara,Sara->GetMaxHealth() * 12 / 100, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
					}
			}
			die = true;
			me->ForcedDespawn(1000);
		}
	}
};

struct MANGOS_DLL_DECL npc_yoggimmortalAI : public LibDevFSAI
{
    npc_yoggimmortalAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(64159,10000,12000,2000);
    }

    void Reset()
    {
		ResetTimers();
		AggroAllPlayers(150.0f);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && !me->HasAura(64162))
			dmg = 0;
	}
};

struct MANGOS_DLL_DECL add_YoggTentacleTankAI : public LibDevFSAI
{
    add_YoggTentacleTankAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_FOCUSED_ANGER,500,DAY*1000);
    }

	uint32 CheckDist_Timer;

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		DoCastMe(SPELL_ERUPT);
		CheckDist_Timer = 1000;
		AggroAllPlayers(150.0f);
		me->GetMotionMaster()->MovePoint(0,me->GetPositionX()+0.2f,me->GetPositionY(),me->GetPositionZ()+0.2f);
    }

	void CheckPlayers()
	{	
		bool MustCast = true;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->GetDistance2d(me) < 7.0f)
						MustCast = false;
		}
		if(MustCast)
			DoCastRandom(SPELL_DIMINISH_POWER);
		else
			me->CastStop();
	}

    void UpdateAI(const uint32 diff)
    {
		if(pInstance)
			if(!pInstance->GetData(DATA_YOGG_TENTACLES_FROZEN))
				return;

        if(CheckDist_Timer <= diff)
		{
			CheckPlayers();
			CheckDist_Timer = 1000;
		}
		else
			CheckDist_Timer -= diff;
      
		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL add_YoggTentacleEventAI : public LibDevFSAI
{
    add_YoggTentacleEventAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		me->GetMotionMaster()->MovePoint(0,me->GetPositionX()+0.2f,me->GetPositionY(),me->GetPositionZ()+0.2f);
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL YoggDeathRayAI : public LibDevFSAI
{
    YoggDeathRayAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
		me->GetMotionMaster()->MovePoint(0,me->GetPositionX()+0.2f,me->GetPositionY(),me->GetPositionZ()+0.2f);
    }

    void Reset()
    {
		ResetTimers();
		DoCastMe(SPELL_DEATH_RAY_VISUAL);
		AggroAllPlayers(100.0f);
    }

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

struct MANGOS_DLL_DECL add_YoggTentacleCastAI : public LibDevFSAI
{
    add_YoggTentacleCastAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_APATHY,2000,20000,3000);
		AddEvent(SPELL_BLACK_PLAGUE,5000,7000,7000,TARGET_HAS_MANA);
		AddEvent(SPELL_CURSE_OF_DOOM,20000,20000,10000);
		AddEvent(SPELL_DRAINING_POISON,8000,15000,15000,TARGET_HAS_MANA);
    }

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		DoCastMe(SPELL_ERUPT);
		AggroAllPlayers(150.0f);
		me->GetMotionMaster()->MovePoint(0,me->GetPositionX()+0.2f,me->GetPositionY(),me->GetPositionZ()+0.2f);
    }


    void UpdateAI(const uint32 diff)
    {
		if(pInstance)
			if(!pInstance->GetData(DATA_YOGG_TENTACLES_FROZEN))
				return;

		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL add_YoggTentacleConstAI : public LibDevFSAI
{
    add_YoggTentacleConstAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
    }

	uint32 squeeze_Timer;
	uint64 targetGUID;

    void Reset()
    {
		ResetTimers();
		SetCombatMovement(false);
		DoCastMe(SPELL_ERUPT);
		squeeze_Timer = 1000;
		me->GetMotionMaster()->MovePoint(0,me->GetPositionX()+0.2f,me->GetPositionY(),me->GetPositionZ()+0.2f);
    }


    void UpdateAI(const uint32 diff)
    {
		if(pInstance)
			if(!pInstance->GetData(DATA_YOGG_TENTACLES_FROZEN))
				return;

		if(squeeze_Timer <= diff)
		{
			if(Unit* target = GetRandomUnit())
				if(target->isAlive())
				{
					ModifyAuraStack((m_difficulty ? SPELL_SQUEEZE_25 : SPELL_SQUEEZE_10),1,target);
					target->EnterVehicle(me);
					targetGUID = target->GetGUID();
				}

			squeeze_Timer = DAY*HOUR;
		}
		else
			squeeze_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

	void JustDied(Unit* pWho)
	{
		if(Unit* u = GetGuidUnit(targetGUID))
			if(u->isAlive())
			{
				u->RemoveAurasDueToSpell(SPELL_SQUEEZE_25);
				u->RemoveAurasDueToSpell(SPELL_SQUEEZE_10);
			};
	}
};

CreatureAI* GetAI_add_YoggTentacleTank(Creature *_Creature)
{
    return new add_YoggTentacleTankAI(_Creature);
}

CreatureAI* GetAI_add_YoggTentacleEvent(Creature *_Creature)
{
    return new add_YoggTentacleEventAI(_Creature);
}

CreatureAI* GetAI_add_YoggTentacleCast(Creature *_Creature)
{
    return new add_YoggTentacleCastAI(_Creature);
}

CreatureAI* GetAI_add_YoggTentacleConst(Creature *_Creature)
{
    return new add_YoggTentacleConstAI(_Creature);
}

CreatureAI* GetAI_npc_yoggguard(Creature *_Creature)
{
    return new npc_yoggguardAI(_Creature);
}

CreatureAI* GetAI_npc_sara(Creature *_Creature)
{
    return new npc_saraAI(_Creature);
}

CreatureAI* GetAI_npc_sanity_well(Creature* pCreature)
{
    return new npc_sanity_wellAI(pCreature);
}

CreatureAI* GetAI_npc_mimiron_help(Creature* pCreature)
{
    return new npc_mimiron_helpAI(pCreature);
}

CreatureAI* GetAI_npc_hodir_help(Creature* pCreature)
{
    return new npc_hodir_helpAI(pCreature);
}

CreatureAI* GetAI_npc_thorim_help(Creature* pCreature)
{
    return new npc_thorim_helpAI(pCreature);
}

CreatureAI* GetAI_npc_freya_help(Creature* pCreature)
{
    return new npc_freya_helpAI(pCreature);
}

CreatureAI* GetAI_boss_yoggsaron(Creature *_Creature)
{
    return new boss_yoggsaronAI(_Creature);
}

CreatureAI* GetAI_npc_yogg_nuage(Creature *_Creature)
{
    return new boss_yogg_nuageAI(_Creature);
}

CreatureAI* GetAI_yogg_brain(Creature *_Creature)
{
    return new yogg_brainAI(_Creature);
}

CreatureAI* GetAI_npc_yoggimmortal(Creature *_Creature)
{
    return new npc_yoggimmortalAI(_Creature);
}

CreatureAI* GetAI_YoggDeathRayAI(Creature *_Creature)
{
    return new YoggDeathRayAI(_Creature);
}

bool GossipHello_yogg_portal(Player* pPlayer, Creature* pCreature)
{
	if(Unit* YoggSaron = Unit::GetUnit((*pCreature), ((ScriptedInstance*)pCreature->GetInstanceData())->GetData64(TYPE_YOGGSARON)))
		if(YoggSaron->isAlive())
		{
			uint8 vis = ((boss_yoggsaronAI*)((Creature*)YoggSaron)->AI())->GetVision();
			//YoggSaron->getHostileRefManager().deleteReference(pPlayer);
			if(vis > 2)
				return false;
			pPlayer->TeleportTo(pPlayer->GetMap()->GetId(),Locs[vis][0],Locs[vis][1],Locs[vis][2],0);
			pCreature->ForcedDespawn(100);
		}
    return true;
}

bool GossipHello_yogg_end_portal(Player* pPlayer, GameObject* pGo)
{
	pPlayer->TeleportTo(pPlayer->GetMap()->GetId(),1980.528f,-29.373f,324.9f,0);
    return true;
}

void AddSC_boss_yoggsaron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_yoggsaron";
    newscript->GetAI = &GetAI_boss_yoggsaron;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_yoggimmortal";
    newscript->GetAI = &GetAI_npc_yoggimmortal;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "yogg_death_ray";
    newscript->GetAI = &GetAI_YoggDeathRayAI;
    newscript->RegisterSelf();


	newscript = new Script;
    newscript->Name = "npc_freya_help";
    newscript->GetAI = &GetAI_npc_freya_help;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_thorim_help";
    newscript->GetAI = &GetAI_npc_thorim_help;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_hodir_help";
    newscript->GetAI = &GetAI_npc_hodir_help;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mimiron_help";
    newscript->GetAI = &GetAI_npc_mimiron_help;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_sanity_well";
    newscript->GetAI = &GetAI_npc_sanity_well;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_sara";
    newscript->GetAI = &GetAI_npc_sara;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_yogg_nuage";
    newscript->GetAI = &GetAI_npc_yogg_nuage;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_yogg_guardian";
    newscript->GetAI = &GetAI_npc_yoggguard;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_yogg_tentConst";
    newscript->GetAI = &GetAI_add_YoggTentacleConst;
    newscript->RegisterSelf();
	
	newscript = new Script;
    newscript->Name = "npc_yogg_tentCast";
    newscript->GetAI = &GetAI_add_YoggTentacleCast;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_yogg_tentTank";
    newscript->GetAI = &GetAI_add_YoggTentacleTank;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "yogg_portal";
    newscript->pGossipHello = &GossipHello_yogg_portal;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "yogg_end_portal";
    newscript->pGOHello = &GossipHello_yogg_end_portal;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_yogg_brain";
    newscript->GetAI = &GetAI_yogg_brain;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_yogg_tentEvent";
    newscript->GetAI = &GetAI_add_YoggTentacleEvent;
    newscript->RegisterSelf();
}

