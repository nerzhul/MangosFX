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

// Freya - Freundlicher Watcher
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
#define SPELL_TITANIC_STORM             64172

// Ominase Wolke - Phase 1
#define AURA_SUMMON_GUARDIAN            62979

// Sanity Well
#define AURA_SANITY_WELL                63288

// Watcher von Yogg-Saron - Phase 1
#define SPELL_DOMINATE_MIND             63042

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
	SPELL_SHATTERED_ILLUSION	= 64173,
	SPELL_INDUCE_MADNESS		= 64059,

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
	
	// p3
	SPELL_SHADOW_BEACON			= 64465,
	SPELL_EMPOWERING_SHADOWS_10	= 64468,
	SPELL_EMPOWERING_SHADOWS_25	= 64486,
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
		AddEvent(SPELL_DEATH_RAY,10000,30000,1000);
		AddEventOnTank(SPELL_EXINGUISH_ALL_LIFE,900000 /* - SaraTimer */,60000);
		AddEventOnMe(SPELL_BRAIN_LINK,20000,32000,0,EVENT_PHASE2);
    }

	uint32 Event_Timer;
	uint32 TentacleText_Timer;
	uint8 Event;
	uint8 eStep;
	uint8 randomVision;
	uint32 Psychosis_Timer;
	uint32 MaladyMind_Timer;
	uint32 CheckTimer;
	uint32 LunaticGaze_Timer;
	uint32 CheckPlayerSight_Timer;
	uint32 EndPortal_Timer;

    void Reset()
    {
		ResetTimers();
		me->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.05f);
	    if(!me->HasAura(AURA_SHADOWY_BARRIER_1, 0))
            me->CastSpell(me, AURA_SHADOWY_BARRIER_1, true);

		CheckPlayerSight_Timer = 1000;		
		Event = EVENT_POP;
		Event_Timer = 500;
		EndPortal_Timer = 60000;
		eStep = 0;
		AggroAllPlayers(200.0f);
		if (pInstance)
		{
			pInstance->SetData(DATA_YOGG_NUAGE,1);
            pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
		}
		
		LunaticGaze_Timer = 5000;
		uint8 randomVision = 3;
		TentacleText_Timer = 15000;
		Psychosis_Timer = 15000;
		MaladyMind_Timer = 18000;
		CheckTimer = 500;
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
		Yell(15761,"Votre destin est scellÃ©, la fin des temps est enfin arrivÃ©e pour vous et pour tous les habitants de ces petits bourgeons !");
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE,3);
		if (pInstance)
            pInstance->SetData(TYPE_YOGGSARON, DONE);
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
			CallCreature(NPC_TENTACLE_INFLUENCE,TEN_MINS,PREC_COORDS,NOTHING,EventLocs[i][randomVision][0],EventLocs[i][randomVision][1],EventLocs[i][randomVision][2]);
		PoPYoggPortals();
	}

	void ControlPlayer(Player* plr)
	{
		Kill(plr);
	}

	void PoPYoggPortals()
	{
		uint8 portalCount = 10;
		if(!m_difficulty)
			portalCount = 4;
		for(uint8 i=0;i<portalCount;i++)
			CallCreature(NPC_PORTAL_TO_MADNESS,10000,PREC_COORDS,NOTHING,PortalLoc[i][0],PortalLoc[i][1],PortalLoc[i][2]);

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
					if(pPlayer->isAlive() && pPlayer->HasAura(SPELL_BRAIN_LINK) && i < 2)
					{
						Linked[i] = pPlayer->GetGUID();
						i++;
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

	void CreateEndPortals()
	{
		BossEmote(0,"Les illusions se dissipent et un portail vers la surface apparait");
	}

	void CheckPlayerSight()
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasInArc(M_PI,me))
						ModifySanity(1,pPlayer);
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
					Event_Timer = 90000;
				}
				else
					Event_Timer -= diff;

				if(EndPortal_Timer <= diff)
				{
					CreateEndPortals();
					EndPortal_Timer = 90000;
				}
				else
					EndPortal_Timer -= diff;

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

				if(MaladyMind_Timer <= diff)
				{
					if(Unit* target = GetRandomUnit())
					{
						DoCast(target,SPELL_MALADY_OF_THE_MIND);
						ModifySanity(3,target);
					}
					MaladyMind_Timer = urand(18000,25000);
				}
				else
					MaladyMind_Timer -= diff;

				if(CheckTimer <= diff)
				{
					CheckLinkedPlayers();
					CheckTimer = 1000;
				}
				else
					CheckTimer -= diff;
				break;
			case EVENT_PHASE3:
				if(LunaticGaze_Timer <= diff)
				{
					DoCastVictim(SPELL_LUNATIC_GAZE);
					LunaticGaze_Timer = 20000;
				}
				else
					LunaticGaze_Timer -= diff;
				
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
		SetFlying(true);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		float damage = dmg / me->GetMaxHealth();
		if(Unit* Yogg = Unit::GetUnit((*me), pInstance->GetData64(TYPE_YOGGSARON)))
			if(Yogg->isAlive())
				Yogg->DealDamage(Yogg,Yogg->GetMaxHealth() * damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

    void UpdateAI(const uint32 diff)
    {
		if(me->GetHealth() * 100 / me->GetMaxHealth() < 30 && !phase3)
		{
			phase3 = true;
			if(Unit* Yogg = Unit::GetUnit((*me), pInstance->GetData64(TYPE_YOGGSARON)))
				if(Yogg->isAlive())
					((boss_yoggsaronAI*)((Creature*)Yogg)->AI())->GoPhase3();
		}
    }
};

// Freya - Freundlicher Watcher
struct MANGOS_DLL_DECL npc_freya_helpAI : public ScriptedAI
{
    npc_freya_helpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
	    if(!me->HasAura(AURA_RESILIENCE_OF_NATURE, 0))
            me->CastSpell(me, AURA_RESILIENCE_OF_NATURE, true);
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);

    }

};

// Thorim - Freundlicher Watcher
struct MANGOS_DLL_DECL npc_thorim_helpAI : public ScriptedAI
{
    npc_thorim_helpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
	    if(!me->HasAura(AURA_FURY_OF_THE_STORM, 0))
            me->CastSpell(me, AURA_FURY_OF_THE_STORM, true);
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() && !me->getVictim())
            return;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);

    }

};


// Hodir - Freundlicher Watcher
struct MANGOS_DLL_DECL npc_hodir_helpAI : public ScriptedAI
{
    npc_hodir_helpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
	    if(!me->HasAura(AURA_FORTITUDE_OF_FROST, 0))
            me->CastSpell(me, AURA_FORTITUDE_OF_FROST, true);
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() && !me->getVictim())
            return;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);

    }

};

// Mimiron - Freundlicher Watcher
struct MANGOS_DLL_DECL npc_mimiron_helpAI : public ScriptedAI
{
    npc_mimiron_helpAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
	    if(!me->HasAura(AURA_SPEED_OF_INVENTION , 0))
            me->CastSpell(me, AURA_SPEED_OF_INVENTION , true);
    }

    void KilledUnit(Unit *victim)
    {
    }

    void JustDied(Unit *victim)
    {
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() && !me->getVictim())
            return;

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);

    }

};


// Sanity Well
struct MANGOS_DLL_DECL npc_sanity_wellAI : public ScriptedAI
{
    npc_sanity_wellAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	uint32 Check_Timer;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		SetCombatMovement(false);
	    if(!me->HasAura(64169, 0))
            me->CastSpell(me, 64169, true);
		if(!me->HasAura(AURA_SANITY_WELL, 0))
            me->CastSpell(me, AURA_SANITY_WELL, true);

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
						if(pPlayer->HasAura(SPELL_Sanity))
						{
							uint32 nbstack = pPlayer->GetAura(SPELL_Sanity,0)->GetStackAmount() + 20;
							if(nbstack> 100)
								nbstack = 100;
							ModifyAuraStack(SPELL_Sanity,nbstack,pPlayer);
						}
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

// Yogg_saron_cloud
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
						if(me->GetDistance2d(pPlayer) < 10.0f)
							CallCreature(NPC_GUARDIAN_OF_YOGG);
						return;
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
		AddTextEvent(15775,"Il sera bientôt temps de frapper la tête de la bête, concentrez vos rage sur ces laquais",12000,DAY*HOUR);
    }

	bool EventStarted;
	uint32 CheckTimer;
	uint32 SpawnEvent_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		if(!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		if(!me->HasAura(SPELL_SHADOWY_BARRIER))
			DoCastMe(SPELL_SHADOWY_BARRIER);

		EventStarted = false;
		CheckTimer = 1000;
		if (pInstance)
		{
			pInstance->SetData(DATA_YOGG_NUAGE,0);
            pInstance->SetData(TYPE_YOGGSARON, NOT_STARTED);
		}
		SpawnEvent_Timer = 15000;
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
		CallCreature(NPC_YOGGSARON_BRAIN,DAY*HOUR,PREC_COORDS,NOTHING,1981.512f,-22.89f,255.712f);
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
							ModifyAuraStack(SPELL_Sanity,100,pPlayer,pPlayer);

		if(pInstance)
            pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
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
			if(SpawnEvent_Timer <= diff)
			{
				if(Creature* nuage = GetInstanceCreature(DATA_YOGG_NUAGE))
				{
					((boss_yogg_nuageAI*)(nuage->AI()))->CallCreature(NPC_GUARDIAN_OF_YOGG);
				}
				SpawnEvent_Timer = 25000;
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
			me->CastStop();
			DoCastMe((m_difficulty ? SPELL_SHADOW_NOVA_H : SPELL_SHADOW_NOVA));
			if(Unit* Sara = GetInstanceCreature(DATA_YOGG_SARA))
			{
				if(Sara && Sara->isAlive() && !die)
					if(Sara->GetDistance2d(me) < 8.0f)
					{
						me->DealDamage(Sara,Sara->GetMaxHealth() * 12 / 100, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
						me->ForcedDespawn(1000);
					}
			}
			die = true;
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
		if(pDoneBy == me)
			return;

		if(dmg >= me->GetHealth())
		{
			dmg = 0;
		}
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
    }


    void UpdateAI(const uint32 diff)
    {
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
		AggroAllPlayers(150.0f);
    }


    void UpdateAI(const uint32 diff)
    {
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

bool GossipHello_yogg_portal(Player* pPlayer, Creature* pCreature)
{
	if(Unit* YoggSaron = Unit::GetUnit((*pCreature), ((ScriptedInstance*)pCreature->GetInstanceData())->GetData64(TYPE_YOGGSARON)))
		if(YoggSaron->isAlive())
		{
			uint8 vis = ((boss_yoggsaronAI*)((Creature*)YoggSaron)->AI())->GetVision();
			if(vis > 2)
				return false;
			pPlayer->TeleportTo(pPlayer->GetMap()->GetId(),Locs[vis][0],Locs[vis][1],Locs[vis][2],0);
			pCreature->ForcedDespawn(100);
		}
    return true;
}

bool GossipHello_yogg_end_portal(Player* pPlayer, Creature* pCreature)
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
    newscript->pGossipHello = &GossipHello_yogg_end_portal;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_yogg_brain";
    newscript->GetAI = &GetAI_yogg_brain;
    newscript->RegisterSelf();
}

