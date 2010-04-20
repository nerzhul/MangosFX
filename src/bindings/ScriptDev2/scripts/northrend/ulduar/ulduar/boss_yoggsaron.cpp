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

enum YoggEvents
{
	EVENT_POP		=	0,
	EVENT_PHASE2	=	1,
	EVENT_PHASE3	=	2,
};

struct MANGOS_DLL_DECL boss_yoggsaronAI : public Scripted_NoMovementAI
{
    boss_yoggsaronAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        m_pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;
	bool m_bIsHeroic;
	uint32 Event_Timer;
	uint32 TentacleText_Timer;
	uint8 Event;
	uint8 eStep;
	uint8 randomVision;
	uint32 Psychosis_Timer;
	uint32 MaladyMind_Timer;
	uint32 CheckTimer;
	uint32 LunaticGaze_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		me->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.05f);
	    if(!me->HasAura(AURA_SHADOWY_BARRIER_1, 0))
            me->CastSpell(me, AURA_SHADOWY_BARRIER_1, true);

		m_pInstance->SetData(DATA_YOGG_NUAGE,1);
		Event = EVENT_POP;
		Event_Timer = 500;
		eStep = 0;
		AggroAllPlayers(200.0f);
		if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
		Tasks.AddSummonEvent(NPC_TENTACLE_TANK,15000,75000,EVENT_PHASE2,15000,1,TEN_MINS*2,NEAR_45M);
		Tasks.AddSummonEvent(NPC_TENTACLE_CAST,2000,30000,EVENT_PHASE2,3000,2,TEN_MINS*2,NEAR_45M);
		Tasks.AddSummonEvent(NPC_TENTACLE_CONSTRICTOR,20000,45000,EVENT_PHASE2,35000,1,TEN_MINS*2,NEAR_45M);
		Tasks.AddSummonEvent(NPC_IMMORTAL_GUARDIAN,15000,35000,EVENT_PHASE3,0,1,TEN_MINS,NEAR_45M);
		Tasks.AddEvent(SPELL_DEATH_RAY,10000,30000,1000);
		LunaticGaze_Timer = 5000;
		uint8 randomVision = 3;
		TentacleText_Timer = 15000;

		Tasks.AddEvent(SPELL_EXINGUISH_ALL_LIFE,900000 /* - SaraTimer */,60000,0,TARGET_MAIN);
		
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
			Tasks.Speak(CHAT_TYPE_YELL,15757,"Hahahahaha ! hahahahaha !");
		else
			Tasks.Speak(CHAT_TYPE_YELL,15758,"Une souffrance Ã©ternelle vous attend.");
    }

    void JustDied(Unit *victim)
    {
		Tasks.Speak(CHAT_TYPE_YELL,15761,"Votre destin est scellÃ©, la fin des temps est enfin arrivÃ©e pour vous et pour tous les habitants de ces petits bourgeons !");
		GiveEmblemsToGroup((m_bIsHeroic) ? CONQUETE : VAILLANCE,3);
		if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, DONE);
    }

    void Aggro(Unit* pWho)
    {
		SetCombatMovement(false);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
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
						Tasks.Speak(CHAT_TYPE_YELL,15762,"Un milliers de morts...");
						break;
					case 1:
						Tasks.Speak(CHAT_TYPE_YELL,15763,"Ou un unique meurtre");
						break;
					case 2:
						Tasks.Speak(CHAT_TYPE_YELL,15764,"Vos querelles mesquines ne font que me renforcer");
						break;
				}
				break;
			case 1:
				Tasks.Speak(CHAT_TYPE_YELL,15765,"Sa progÃ©niture a vite appris ses leçons. Vous allez vite apprendre la vôtre.");
				break;
			case 2:
				if(urand(0,1))
					Tasks.Speak(CHAT_TYPE_YELL,15766,"Il'Rilne Shol Anal");
				else
					Tasks.Speak(CHAT_TYPE_YELL,15767,"Il apprendra... aucun règne n'est Ã©ternel, sauf celui de la mort");
				break;
			default:
				break;
		}

		for(uint8 i=0;i<6;i++)
		{
			Tasks.CallCreature(0,TEN_MINS,PREC_COORDS,NOTHING,EventLocs[i][randomVision][0],EventLocs[i][randomVision][1],EventLocs[i][randomVision][2]);
		}
		PoPYoggPortals();
	}

	void ControlPlayer(Player* plr)
	{
		Tasks.Kill(plr);
	}

	void PoPYoggPortals()
	{
		

	}

	void CheckLinkedPlayers()
	{
		/*Player* Linked[2];
		Linked[0] = Linked[1] = NULL;
		uint8 i=0;
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(SPELL_BRAIN_LINK) && i < 2)
					{
						Linked[i] = pPlayer;
						i++;
					}

		if(Linked[0] != NULL && Linked[1] != NULL)
			if(Linked[0]->GetDistance(Linked[1]) > 20.0f)
			{
				if(Linked[0]->isAlive())
				{
					DoCast(Linked[0],63803);
					ModifySanity(2,Linked[0]);
				}
				if(Linked[1]->isAlive())
				{
					DoCast(Linked[1],63803);
					ModifySanity(2,Linked[1]);
				}
			}*/
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
			Tasks.SetAuraStack(SPELL_Sanity,stk - count,tar,tar,1);
	}

	void GoPhase3()
	{
		Event = EVENT_PHASE3;
		Tasks.Speak(CHAT_TYPE_YELL,15755,"Contemplez le vrai visage de la mort et sachez que votre fin approche");
		me->RemoveAurasDueToSpell(AURA_SHADOWY_BARRIER_1);
	}

    void UpdateAI(const uint32 diff)
    {
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
					}
					else
						Event_Timer = 500;
				}
				else
					Event_Timer -= diff;
				break;
			case EVENT_PHASE2:
				if(Event_Timer <= diff)
				{
					LaunchVisionEvent();
					Event_Timer = 90000;
				}
				else
					Event_Timer -= diff;

				if(TentacleText_Timer <= diff)
				{
					Tasks.Speak(CHAT_TYPE_YELL,15756,"La folie vous emportera !");

					TentacleText_Timer = urand(60000,75000);
				}
				else
					TentacleText_Timer -= diff;

				if(Psychosis_Timer <= diff)
				{
					Unit* target;
					uint16 count = 0;
					do
					{
						target = SelectUnit(SELECT_TARGET_RANDOM,0);
						count++;
					}
					while(!target && target->GetDistance2d(me) > 200.0f 
						&& target->GetDistanceZ(me) > 10.0f && count < 50);

					if(target)
						DoCast(target,(m_bIsHeroic ? SPELL_PSYCHOSIS_25 : SPELL_PSYCHOSIS_10));
					ModifySanity((m_bIsHeroic ? 9 : 12),target);

					Psychosis_Timer = urand(10000,15000);
				}
				else
					Psychosis_Timer -= diff;

				if(MaladyMind_Timer <= diff)
				{
					Unit* target;
					uint16 count = 0;
					do
					{
						target = SelectUnit(SELECT_TARGET_RANDOM,0);
						count++;
					}
					while(!target && target->GetDistance2d(me) > 200.0f 
						&& target->GetDistanceZ(me) > 10.0f && count < 50);

					if(target)
						DoCast(target,SPELL_MALADY_OF_THE_MIND);
					ModifySanity(3,target);

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
					Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
					if (!lPlayers.isEmpty())
						for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
							if (Player* pPlayer = itr->getSource())
								if(pPlayer->isAlive())
									if(pPlayer->HasInArc( M_PI, me ))
										ModifySanity(4,pPlayer);
					DoCastVictim(SPELL_LUNATIC_GAZE);
					LunaticGaze_Timer = 20000;
				}
				else
					LunaticGaze_Timer -= diff;
				
					break;
			default:
				break;
		}
		
		Tasks.UpdateEvent(diff,Event);
		Tasks.UpdateEvent(diff);

		if(!me->SelectHostileTarget() && !me->getVictim())
		{
			if(Creature* Sara = (Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_YOGG_SARA)))
				Sara->Respawn();
		}	
	}
};

struct MANGOS_DLL_DECL yogg_brainAI : public Scripted_NoMovementAI
{
    yogg_brainAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	bool phase3;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		phase3 = false;
		me->setFaction(14);
    }

	void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		float damage = dmg / me->GetMaxHealth();
		if(Unit* Yogg = Unit::GetUnit((*me), pInstance->GetData64(TYPE_YOGGSARON)))
			if(Yogg->isAlive())
				Yogg->DealDamage(Yogg,Yogg->GetMaxHealth() * damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!me->SelectHostileTarget() && !me->getVictim())
            return;
	
		if(me->GetHealth() * 100 / me->GetMaxHealth() < 30 && !phase3)
		{
			phase3 = true;
			if(Unit* Yogg = Unit::GetUnit((*me), pInstance->GetData64(TYPE_YOGGSARON)))
				if(Yogg->isAlive())
					((boss_yoggsaronAI*)((Creature*)Yogg)->AI())->GoPhase3();
		}

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
       
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
        if (!me->SelectHostileTarget() || !me->getVictim())
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
							Tasks.SetAuraStack(SPELL_Sanity,nbstack,pPlayer,me,1);
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
struct MANGOS_DLL_DECL boss_yogg_nuageAI : public ScriptedAI
{
    boss_yogg_nuageAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		Tasks.AddSummonEvent(NPC_GUARDIAN_OF_YOGG,urand(15000,360000),urand(180000,360000));
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
							Tasks.CallCreature(NPC_GUARDIAN_OF_YOGG);
						return;
					}
	}

    void UpdateAI(const uint32 diff)
    {
		if (m_pInstance && m_pInstance->GetData(TYPE_YOGGSARON) != IN_PROGRESS)
			return;

		if(Check_Timer <= diff)
		{
			CheckDistance();
			Check_Timer = 6000;
		}
		else
			Check_Timer -= diff;
		
		Tasks.UpdateEvent(diff);
    }

};

struct MANGOS_DLL_DECL npc_saraAI : public ScriptedAI
{
    npc_saraAI(Creature *c) : ScriptedAI(c)
    {
        m_pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;
	bool EventStarted;
	uint32 CheckTimer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		if(!me->HasAura(SPELL_SHADOWY_BARRIER))
			DoCastMe(SPELL_SHADOWY_BARRIER);

		Tasks.AddEvent(SPELL_SARAS_FERVOR,10000,15000,15000,PLAYER_RANDOM);
		Tasks.AddEvent(SPELL_SARAS_BLESSING,21000,20000,10000,PLAYER_RANDOM);
		Tasks.AddEvent(SPELL_SARAS_ANGER,12000,18000,12000);

		EventStarted = false;
		CheckTimer = 1000;
		m_pInstance->SetData(DATA_YOGG_NUAGE,0);
		if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, NOT_STARTED);
    }

    void KilledUnit(Unit *victim)
    {
		if(urand(0,1))
			Tasks.Speak(CHAT_TYPE_YELL,15778,"Incapables !");
		else
			Tasks.Speak(CHAT_TYPE_YELL,15779,"Aurait-il pu être sauvÃ© ?");
    }

	void JustDied(Unit *victim)
    {
		Tasks.Speak(CHAT_TYPE_YELL,15754,"Je suis le rêve Ã©veillÃ©, le monstre de vos cauchemars, le dÃ©mon aux milliers de visages, tremblez devant mon vÃ©ritable aspect, à genoux devant le dieu de la mort !");
		Tasks.CallCreature(NPC_YOGGSARON,DAY*1000,PREC_COORDS,AGGRESSIVE_RANDOM,1976.812f, -25.675f, 328.980f,true);
	}

    void Aggro(Unit* pWho)
    {
		Tasks.Speak(CHAT_TYPE_YELL,15775,"Il sera bientôt temps de frapper la tête de la bête, concentrez vos rage sur ces laquais");
    }

	void StartEvent()
	{
		if(EventStarted)
			return;

		EventStarted = true;
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		if(urand(0,1))
			Tasks.Speak(CHAT_TYPE_YELL,15771,"Aaaaaaaaaaaaaaaaah ! Au secours ! Je vous en prie, arrêtez les !");
		else
			Tasks.Speak(CHAT_TYPE_YELL,15772,"Qu'est ce que vous me voulez ? Laissez moi tranquille !");

		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive())
						if(me->GetDistance2d(pPlayer) < 350.0f && !pPlayer->isGameMaster())
							Tasks.SetAuraStack(SPELL_Sanity,100,pPlayer,pPlayer,1);

		if (m_pInstance)
            m_pInstance->SetData(TYPE_YOGGSARON, IN_PROGRESS);
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
						if(!EventStarted && !pPlayer->isGameMaster()&&  me->GetDistance2d(pPlayer) < 90.0f && me->GetDistanceZ(pPlayer) < 15.0f)
						{
							StartEvent();
							find = true;
							return;
						}
					}
		if(!find)
			Reset();
	}

    void UpdateAI(const uint32 diff)
    {
		if(!EventStarted)
		{
			if(CheckTimer <= diff)
			{
				CheckDistance();
				CheckTimer = 1000;
			}
			else
				CheckTimer -= diff;
			if(!EventStarted)
				return;
		}
		else
		{

			Tasks.UpdateEvent(diff);
			DoMeleeAttackIfReady();
		}
    }

};

struct MANGOS_DLL_DECL npc_yoggguardAI : public ScriptedAI
{
    npc_yoggguardAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	uint32 DeathTimer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_DARK_VOLLEY,5000,10000,2000,TARGET_MAIN);
		DeathTimer = DAY;
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!Tasks.CanDoSomething())
		{
			if(Unit* Sara = Unit::GetUnit((*me), pInstance->GetData64(DATA_YOGG_SARA)))
				if(Sara->isAlive())
					AggroAllPlayers(200.0f);
		}

		if(DeathTimer <= uiDiff)
		{
			me->RemoveFromWorld();
		}
		else
			DeathTimer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &dmg)
	{
		if(dmg > me->GetHealth())
		{
			dmg = 0;
			me->CastStop();
			DoCastMe((m_bIsHeroic ? SPELL_SHADOW_NOVA_H : SPELL_SHADOW_NOVA));
			if(Unit* Sara = Unit::GetUnit((*me), pInstance->GetData64(DATA_YOGG_SARA)))
			{
				if(Sara && Sara->isAlive())
					if(Sara->GetDistance2d(me) < 8.0f)
						me->DealDamage(Sara,Sara->GetMaxHealth() * 12 / 100, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

			}
			DeathTimer = 1000;
		}
	}
};


struct MANGOS_DLL_DECL add_YoggTentacleTankAI : public ScriptedAI
{
    add_YoggTentacleTankAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	uint32 CheckDist_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		SetCombatMovement(false);
		DoCastMe(SPELL_ERUPT);
		Tasks.AddEvent(SPELL_FOCUSED_ANGER,500,DAY*1000,0,TARGET_ME);
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

    void UpdateAI(const uint32 uiDiff)
    {
        if(CheckDist_Timer <= uiDiff)
		{
			CheckPlayers();
			CheckDist_Timer = 1000;
		}
		else
			CheckDist_Timer -= uiDiff;
      
		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL add_YoggTentacleCastAI : public ScriptedAI
{
    add_YoggTentacleCastAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		SetCombatMovement(false);
		DoCastMe(SPELL_ERUPT);
		Tasks.AddEvent(SPELL_APATHY,2000,20000,3000);
		Tasks.AddEvent(SPELL_BLACK_PLAGUE,5000,7000,7000,TARGET_HAS_MANA);
		Tasks.AddEvent(SPELL_CURSE_OF_DOOM,20000,20000,10000);
		Tasks.AddEvent(SPELL_DRAINING_POISON,8000,15000,15000,TARGET_HAS_MANA);
		AggroAllPlayers(150.0f);
    }


    void UpdateAI(const uint32 uiDiff)
    {
		Tasks.UpdateEvent(uiDiff);
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL add_YoggTentacleConstAI : public ScriptedAI
{
    add_YoggTentacleConstAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;
	uint32 squeeze_Timer;
	std::vector<Unit*> targets;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		SetCombatMovement(false);
		DoCastMe(SPELL_ERUPT);
		targets.clear();
		squeeze_Timer = 1000;
		AggroAllPlayers(150.0f);
    }


    void UpdateAI(const uint32 uiDiff)
    {
		if(squeeze_Timer <= uiDiff)
		{
			if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
				if(target->isAlive())
				{
					DoCast(target,(m_bIsHeroic ? SPELL_SQUEEZE_25 : SPELL_SQUEEZE_10));
					DoCast(target,66830);
					targets.push_back(target);
				}

			squeeze_Timer = 35000;
		}
		else
			squeeze_Timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

	void JustDied(Unit* pWho)
	{
		for(std::vector<Unit*>::iterator itr = targets.begin(); itr!= targets.end(); ++itr)
			if((*itr) && (*itr)->isAlive())
			{
				(*itr)->RemoveAurasDueToSpell(SPELL_SQUEEZE_25);
				(*itr)->RemoveAurasDueToSpell(SPELL_SQUEEZE_10);
				(*itr)->RemoveAurasDueToSpell(66830);
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

void AddSC_boss_yoggsaron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_yoggsaron";
    newscript->GetAI = &GetAI_boss_yoggsaron;
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
    newscript->Name = "npc_yogg_brain";
    newscript->GetAI = &GetAI_yogg_brain;
    newscript->RegisterSelf();
}

