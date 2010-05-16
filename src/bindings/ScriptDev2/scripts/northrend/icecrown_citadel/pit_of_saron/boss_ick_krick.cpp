/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "pit_of_saron.h"

enum spells
{
	// ick
	SPELL_PUSTULANT_FLESH			=	69581,
	SPELL_PURSUIT					=	68987,
	SPELL_POISON_WAVE				=	68989,
	// krick
	// common
	SPELL_TOXIC_WASTE				=	69024,
	NPC_EXPLODING_ORB				=	36610,
	SPELL_EXPLODING_ORB				=	69019,
};

struct MANGOS_DLL_DECL boss_ickAI : public LibDevFSAI
{
    boss_ickAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_TOXIC_WASTE,12000,12000);
		AddEvent(SPELL_PUSTULANT_FLESH,6000,8000,4000);
    }

	uint32 PoisonWave_Timer;
	uint32 pursuit_Timer;
	Unit* pursuit_target;
    void Reset()
    {
		ResetTimers();
		pursuit_target = NULL;
		PoisonWave_Timer = 30000;
    }

	void SpellHit(Unit* who, const SpellEntry* sp)
	{
		if(sp->Id == SPELL_PURSUIT)
		{
			pursuit_Timer = 10000;
			DoSelectPursuitTarget();
		}
	}

	void Aggro(Unit* who)
	{
		if(Creature* Krick = GetInstanceCreature(DATA_KRICK))
			if(Krick->isAlive())
				if(who->isAlive())
					Krick->AddThreat(who,2.0f);
	}

	void JustDied(Unit* who)
	{
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
	}

	void DoSelectPursuitTarget()
	{
		if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
			if(target->isAlive())
			{
				pursuit_target = target;
				me->GetMotionMaster()->MoveChase(pursuit_target,0.0f);
				me->AddThreat(pursuit_target,100000.0f);
			}
	}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

		if(me->HasAura(SPELL_PURSUIT,0))
		{
			if(pursuit_target && !pursuit_target->isAlive())
				DoSelectPursuitTarget();
		}

		if(pursuit_Timer <= diff)
		{
			DoResetThreat();
			pursuit_Timer = DAY*7;
		}
		else
			pursuit_Timer -= diff;


		if(PoisonWave_Timer <= diff && !me->HasAura(SPELL_PURSUIT,0))
		{
			me->CastStop();
			DoCastVictim(SPELL_POISON_WAVE);
			Speak(CHAT_TYPE_YELL,16930,"Vite, empoisonne les tous, avant qu'ils ne s'éloignent !",GetInstanceCreature(DATA_KRICK));
			PoisonWave_Timer = 30000;
		}
		else
			PoisonWave_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ick(Creature* pCreature)
{
    return new boss_ickAI (pCreature);
}

struct MANGOS_DLL_DECL boss_krickAI : public LibDevFSAI
{
    boss_krickAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEvent(SPELL_TOXIC_WASTE,18000,12000);
		AddSummonEvent(NPC_EXPLODING_ORB,5000,5000,0,0,m_difficulty ? 2 : 1,THREE_MINS,NEAR_15M,NOTHING);
		event_phase = 0;
		event_Timer = 0;
		FactionChief = NULL;
		Tyrannus = NULL;
    }

	bool Event;
	uint8 event_phase;
	uint32 event_Timer;
	uint32 team;
	uint64 FactionChief;
	Creature* Tyrannus;
	uint32 pursuit_Timer;
	
    void Reset()
    {
		ResetTimers();
		if(Creature* Ick = GetInstanceCreature(DATA_ICK))
			if(!Ick->isAlive())
			{
				Ick->Respawn();
				Ick->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			}
		pursuit_Timer = 30000;
		team = ALLIANCE;
		Event = false;
		me->SetRespawnTime(DAY*7);
    }

	void Aggro(Unit* who)
	{
		Speak(CHAT_TYPE_YELL,16926,"Le travail ne doit pas être interrompu ! Ick sas fois deux");
		if(Creature* Ick = GetInstanceCreature(DATA_ICK))
			if(Ick->isAlive())
				Ick->AddThreat(who,2.0f);
	}

	void DamageTaken(Unit* pDoneby, uint32 &dmg)
	{
		if(pDoneby->GetTypeId() == TYPEID_PLAYER)
			team = ((Player*)pDoneby)->GetTeam();

		if(dmg >= me->GetHealth() && !Event)
		{
			dmg = 0;
			if(Creature* Ick = GetInstanceCreature(DATA_ICK))
				if(Ick->isAlive())
					return;
				else
					Ick->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			
			me->setFaction(35);
			DoResetThreat();
			Speak(CHAT_TYPE_SAY,16934,"Attendez ! Non ! Ne me tuez pas ! je vais tout vous dire !");
			
			if(team == ALLIANCE)
			{
				if(Creature* cr = CallCreature(36993,THREE_MINS,NEAR_7M,NOTHING))
					FactionChief = cr->GetGUID();
			}
			else
			{	if(Creature* cr = CallCreature(36990,THREE_MINS,NEAR_7M,NOTHING))
					FactionChief = cr->GetGUID();
			}

			if(Creature* crFactionChief = GetInstanceCreature(FactionChief))
			{
				crFactionChief->GetMotionMaster()->MoveFollow(me,2.0f,0.0f);
				crFactionChief->SetFacingToObject(me);
				me->SetFacingToObject(crFactionChief);
			}
			Event = true;
			event_phase = 1;
			event_Timer = 4000;
		}

		if(Event)
			dmg = 0;
	}

	void KilledUnit(Unit* who)
	{
		if(who == me)
			return;

		if(urand(0,1))
			Speak(CHAT_TYPE_YELL,16927,"Ooh, ses membres pourraient nous être utiles");
		else
			Speak(CHAT_TYPE_YELL,16928,"On manquait de bras et de jambes, merci pour cette aimable contribution !");
	}

    void UpdateAI(const uint32 diff)
    {
		if(!Event)
		{
			//Return since we have no target
			if (!CanDoSomething())
				return;

			if(pursuit_Timer <= diff)
			{
				if(Creature* ick = GetInstanceCreature(DATA_ICK))
					if(ick->isAlive())
						DoCast(ick,SPELL_PURSUIT);
			}
			else
				pursuit_Timer -= diff;
	      
			UpdateEvent(diff);

			DoMeleeAttackIfReady();
		}
		else
		{
			if(event_Timer <= diff)
			{
				switch(event_phase)
				{
					case 1:
						if(Creature* crFactionChief = GetInstanceCreature(FactionChief))
						{
							if(team == ALLIANCE)
							{
								Speak(CHAT_TYPE_SAY,16611,"Je ne suis pas assez naïve pour croire à ces supplications mais j'écouterai ce que tu as à dire.",crFactionChief);
								event_Timer = 4500;
							}
							else
							{
								Speak(CHAT_TYPE_SAY,17033,"Et pourquoi la reine banshee épargnerai elle ta misérable vie ?",crFactionChief);
								event_Timer = 3200;
							}
						}
						break;
					case 2:
						Speak(CHAT_TYPE_SAY,16935,"Ce que vous cherchez est dans l'antre du maître, mais il faut tuer Tyrannus pour y pénétrer. Dans les Salles des Reflets vous trouverez Deuillegivre. Elle, elle détient la vérité !");
						event_Timer = 14000;
						break;
					case 3:
						if(Creature* crFactionChief = GetInstanceCreature(FactionChief))
						{
							if(team == ALLIANCE)
							{
								Speak(CHAT_TYPE_SAY,16612,"Deuillegivre laissée sans surveillance ? Impossible.",crFactionChief);
								event_Timer = 2300;
							}
							else
							{
								Speak(CHAT_TYPE_SAY,17034,"Deuillegivre ? Le Roi Liche ne se sépare jamais de son épée. Si tu me mens...",crFactionChief);
								event_Timer = 5500;
							}
						}
						break;
					case 4:
						Tyrannus = CallCreature(36794,30000,PREC_COORDS,NOTHING,877.520f,173.945f,557.0f);
						Speak(CHAT_TYPE_YELL,16936,"Je vous jure, je vous jure que c'est vrai ! S'il vous plait, épargnez moi !");
						event_Timer = 3900;
						break;
					case 5:
						Speak(CHAT_TYPE_YELL,16753,"Minable insecte, la mort est tout ce que tu auras",Tyrannus);
						event_Timer = 4500;
						break;
					case 6:
						if(Creature* crFactionChief = GetInstanceCreature(FactionChief))
						{
							if(team == ALLIANCE)
							{
								Speak(CHAT_TYPE_SAY,17033,"Quelle fin cruelle... Venez héros nous devons vérifier si ce que disait ce gnome était vrai. Si nous pouvons séparer Arthas de Deuillegivre nous aurons peut être une chance de l'arrêter",crFactionChief);
								event_Timer = 9500;
							}
							else
							{
								Speak(CHAT_TYPE_SAY,17035,"Une fin parfaite pour un traître. Venez nous devons libérer les esclaves et voir par nous même ce que renferme le sanctuaire du Roi Liche !",crFactionChief);
								event_Timer = 7000;
							}
						}
						break;
					case 7:
						if(Creature* crFactionChief = GetInstanceCreature(FactionChief))
							crFactionChief->CastSpell(me,31008,false);
						Speak(CHAT_TYPE_YELL,16754,"Ne pensez pas que je vais vous laisser pénétrer dans le sanctuaire de mon maître si facilement. Suivez moi si vous l'osez",Tyrannus);
						event_Timer = DAY*7;
						if(Tyrannus)
							Tyrannus->ForcedDespawn();
						Kill(me);
						break;
				}
				event_phase++;
			}
			else
				event_Timer -= diff;
		}
    }

    void JustDied(Unit* killer)
    {
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_krick(Creature* pCreature)
{
    return new boss_krickAI (pCreature);
}

struct MANGOS_DLL_DECL mob_exploding_orbAI : public LibDevFSAI
{
    mob_exploding_orbAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_EXPLODING_ORB,2000,2000);
    }

	uint32 modScale_Timer;
	uint8 size;

    void Reset()
    {
		ResetTimers();
		me->SetVisibility(VISIBILITY_ON);
		me->SetPhaseMask(1,true);
		me->SetFloatValue(OBJECT_FIELD_SCALE_X,0.1);
		SetCombatMovement(false);
		size = 1;
		modScale_Timer = 1500;
    }


    void UpdateAI(const uint32 diff)
    {
		if(modScale_Timer <= diff)
		{
			size++;
			me->SetFloatValue(OBJECT_FIELD_SCALE_X,0.1*size);
			if(size >= 12)
			{
				me->ForcedDespawn();
				return;
			}
			modScale_Timer = 1500;
		}
		else
			modScale_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_exploding_orb(Creature* pCreature)
{
    return new mob_exploding_orbAI (pCreature);
}

void AddSC_boss_ick_and_krick()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ick";
    newscript->GetAI = &GetAI_boss_ick;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "boss_krick";
    newscript->GetAI = &GetAI_boss_krick;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_exploding_orb_ICC5";
    newscript->GetAI = &GetAI_mob_exploding_orb;
    newscript->RegisterSelf();
}
