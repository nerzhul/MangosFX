
#include "precompiled.h"
#include "sunwell_plateau.h"

// Lady Sacrolash

#define LADY_SACROLASH 25165

#define SPELL_DARK_TOUCHED 45347
#define SPELL_SHADOW_BLADES 45248//10 secs
#define SPELL_DARK_STRIKE 45271
#define SPELL_SHADOW_NOVA 45329//30-35 secs
#define SPELL_CONFOUNDING_BLOW 45256//25 secs

#define MOB_SHADOW_IMAGE 25214
#define SPELL_SHADOW_FURY 45270
#define SPELL_IMAGE_VISUAL 45263

#define SOUND_INTRO 12484
#define YELL_SHADOW_NOVA "Shadow to the aid of fire!" //only if Alythess is not dead
#define SOUND_SHADOW_NOVA 12485
#define YELL_SISTER_ALYTHESS_DEAD "Alythess! Your fire burns within me!"
#define SOUND_SISTER_ALYTHESS_DEAD 12488
#define YELL_SAC_KILL_1 "Shadow engulf."
#define SOUND_SAC_KILL_1 12486
#define YELL_SAC_KILL_2 "Ee-nok Kryul!"
#define SOUND_SAC_KILL_2 12487
#define SAY_SAC_DEAD "I... fade."
#define YELL_ENRAGE "Time is a luxury you no longer possess!"

//enrage 6 minutes
#define SPELL_ENRAGE 46587
//empower after sister is death
#define SPELL_EMPOWER 45366

//Grand Warlock Alythess
// Don't move only spamm spells ...
#define GRAND_WARLOCK_ALYTHESS 25166

#define SPELL_PYROGENICS 45230//15secs
#define SPELL_FLAME_TOUCHED 45348
#define SPELL_CONFLAGRATION 45342//30-35 secs
#define SPELL_BLAZE 45235//on main target every 3 secs
#define SPELL_FLAME_SEAR 46771
#define SPELL_BLAZE_SUMMON 45236 //187366
#define SPELL_BLAZE_BURN 45246

#define YELL_CANFLAGRATION "Fire to the aid of shadow!" //only if Sacrolash is not dead
#define SOUND_CANFLAGRATION 12489
#define YELL_SISTER_SACROLASH_DEAD "Sacrolash!"
#define SOUND_SISTER_SACROLASH_DEAD 12492
#define YELL_ALY_KILL_1 "Fire consume."
#define SOUND_ALY_KILL_1 12490
#define YELL_ALY_KILL_2 "Ed-ir Halach!"
#define SOUND_ALY_KILL_2 12491
#define YELL_ALY_DEAD "De-ek Anur!"
#define SOUND_ALY_DEAD 12494
#define YELL_BERSERK "Your luck has run its curse!"
#define SOUND_BERSERK 12493


struct MANGOS_DLL_DECL boss_sacrolashAI : public ScriptedAI
{
	boss_sacrolashAI(Creature *c) : ScriptedAI(c)
	{
		pInstance = (c->GetInstanceData()) ? ((ScriptedInstance*)c->GetInstanceData()) : NULL;
		Reset();
	}

	ScriptedInstance *pInstance;
	bool InCombat;
	bool sisterdeath;
	bool blades;

	uint32 darkstrike_timer;
	uint32 shadowblades_timer;
	uint32 shadownova_timer;
	uint32 confoundingblow_timer;
	uint32 shadowimage_timer;

	uint32 conflagration_timer;


	uint32 enrage_timer;

	void Reset()
	{
		InCombat = false;

		shadowblades_timer = 10000;
		shadownova_timer = 30000;
		confoundingblow_timer = 25000;
		shadowimage_timer = 30000;
		conflagration_timer = 30000;
		sisterdeath = false;

		blades = false;
		darkstrike_timer = 5000;

		enrage_timer = 360000;
	}

	void Aggro(Unit *who)
	{
		if(pInstance)
		{
			Unit* Temp = Unit::GetUnit((*me),pInstance->GetData64(DATA_ALYTHESS));
			((Creature*)Temp)->AI()->AttackStart(who);
		}
	}

	void KilledUnit(Unit *victim)
	{
		if(urand(0,4))
		{
			switch (urand(0,1))
			{
				case 0:
				DoPlaySoundToSet(me,SOUND_SAC_KILL_1);
				//DoYell(YELL_SAC_KILL_1 ,LANG_UNIVERSAL,NULL);
				break;
				case 1:
				DoPlaySoundToSet(me,SOUND_SAC_KILL_2);
				//DoYell(YELL_SAC_KILL_2 ,LANG_UNIVERSAL,NULL);
				break;
			}
		}
	}

	void JustDied(Unit* Killer)
	{
		// only if ALY death
		if (sisterdeath)
		{
			//DoYell(SAY_SAC_DEAD ,LANG_UNIVERSAL,NULL);
		}
		else
		{
			me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if(!sisterdeath)
		{
			if (pInstance)
			{
				Unit* Temp = NULL;
				Temp = Unit::GetUnit((*me),pInstance->GetData64(DATA_ALYTHESS));
				if (Temp->isDead())
				{
					//DoYell(YELL_SISTER_ALYTHESS_DEAD ,LANG_UNIVERSAL,NULL);
					DoPlaySoundToSet(me,SOUND_SISTER_ALYTHESS_DEAD);
					sisterdeath = true;

					me->InterruptSpell(CURRENT_GENERIC_SPELL);
					DoCastMe(SPELL_EMPOWER);
				}
			}
		}

		if (!me->SelectHostileTarget() || !me->getVictim())
			return;

		if(sisterdeath)
		{
			if (conflagration_timer < diff)
			{
				if (!me->IsNonMeleeSpellCasted(false))
				{
					me->InterruptSpell(CURRENT_GENERIC_SPELL);
					Unit* target = NULL;
					target = SelectUnit(SELECT_TARGET_RANDOM, 0);
					DoCast(target,SPELL_CONFLAGRATION);
					conflagration_timer = 30000+(rand()%5000);
				}
			}
			else 
				conflagration_timer -= diff;
		}
		else
		{
			if(shadownova_timer < diff)
			{
				if (!me->IsNonMeleeSpellCasted(false))
				{
					Unit* target = NULL;
					target = SelectUnit(SELECT_TARGET_RANDOM, 0);
					DoCast(target,SPELL_SHADOW_NOVA);

					if(!sisterdeath)
					{
						//DoTextEmote("directs Shadow Nova at $N",target);
						DoPlaySoundToSet(me,SOUND_SHADOW_NOVA);
						//DoYell(YELL_SHADOW_NOVA,LANG_UNIVERSAL,NULL);
					}

					shadownova_timer= 30000+(urand(4500,6000));
				}
			}
			else 
				shadownova_timer -=diff;
		}

		if(confoundingblow_timer < diff)
		{
			if (!me->IsNonMeleeSpellCasted(false))
			{
				Unit* target = NULL;
				target = SelectUnit(SELECT_TARGET_RANDOM, 0);
				DoCast(target,SPELL_CONFOUNDING_BLOW);
				confoundingblow_timer = 20000 + (rand()%5000);
			}
		}
		else 
			confoundingblow_timer -=diff;

		if(shadowimage_timer < diff)
		{
			Unit* target = NULL;
			Creature* temp = NULL;
			for(int i = 0;i<3;i++)
			{
				target = SelectUnit(SELECT_TARGET_RANDOM, 0);
				temp = DoSpawnCreature(MOB_SHADOW_IMAGE,0,0,0,0,TEMPSUMMON_TIMED_DESPAWN,10000);
				temp->AI()->AttackStart(target);
			}
			shadowimage_timer = 30000;
		}
		else
			shadowimage_timer -=diff;

		if(shadowblades_timer < diff)
		{
			if (!me->IsNonMeleeSpellCasted(false))
			{
				DoCastMe(SPELL_SHADOW_BLADES);
				shadowblades_timer = 10000;
				blades = true;
				darkstrike_timer = 1000;
			}
		}
		else 
			shadowblades_timer -=diff;

		if(blades && !me->IsNonMeleeSpellCasted(false))
		{
			if (darkstrike_timer < diff)
			{
				Unit* Temp = NULL;
				ThreatList::const_iterator i = me->getThreatManager().getThreatList().begin();
				for (; i != me->getThreatManager().getThreatList().end(); ++i)
				{
					Temp = Unit::GetUnit((*me),(*i)->getUnitGuid());
					if (Temp && Temp->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(Temp, 20))
					DoCast(Temp,SPELL_DARK_STRIKE);
				}
				blades = false;
				darkstrike_timer = 5000;
			}
			else 
				darkstrike_timer -= diff;
		}

		if (enrage_timer < diff)
		{
			me->InterruptSpell(CURRENT_GENERIC_SPELL);
			//DoYell(YELL_ENRAGE ,LANG_UNIVERSAL,NULL);
			DoCastMe(SPELL_ENRAGE);
			enrage_timer = 300000;
		}
		else 
			enrage_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_sacrolash(Creature *_Creature)
{
	return new boss_sacrolashAI (_Creature);
};

struct MANGOS_DLL_DECL boss_alythessAI : public ScriptedAI
{
	boss_alythessAI(Creature *c) : ScriptedAI(c)
	{
		pInstance = (c->GetInstanceData()) ? ((ScriptedInstance*)c->GetInstanceData()) : NULL;
		Reset();
		once = false;
	}

	ScriptedInstance *pInstance;
	bool InCombat;
	bool sisterdeath;
	bool once;
	bool blazesummon;
	uint64 blazetarget;

	uint32 conflagration_timer;
	uint32 blaze_timer;
	uint32 pyrogenics_timer;
	uint32 blazesummon_timer;

	uint32 shadownova_timer;
	uint32 flamesear_timer;

	uint32 enrage_timer;

	void Reset()
	{
		conflagration_timer = 45000;
		blaze_timer = 1000;
		pyrogenics_timer = 15000;
		shadownova_timer = 40000;
		sisterdeath = false;
		enrage_timer = 360000;
		flamesear_timer = 15000;
	}

	void Aggro(Unit *who)
	{
		if(pInstance)
		{
			Unit* Temp = Unit::GetUnit((*me),pInstance->GetData64(DATA_SACROLASH));
			((Creature*)Temp)->AI()->AttackStart(who);
		}
	}

	void AttackStart(Unit *who)
	{
		if (!who)
			return;

		if (who->isTargetableForAttack() && who!= me)
		{
			//DoStartAttackNoMovement(who);
		}
	}

	void MoveInLineOfSight(Unit *who)
	{
		if (!who || me->getVictim())
		return;

		if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(me) && me->IsHostileTo(who))
		{

			float attackRadius = me->GetAttackDistance(who);
			if (me->IsWithinDistInMap(who, attackRadius) && me->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && me->IsWithinLOSInMap(who))
			{
				if(who->HasStealthAura())
					who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH) ;

				//DoStartAttackNoMovement(who);
			}
		}
		else if (!once && me->IsWithinLOSInMap(who)&& me->IsWithinDistInMap(who, 30) )
		{
			DoPlaySoundToSet(me,SOUND_INTRO);
			once = true;
		}
	}

	void KilledUnit(Unit *victim)
	{
		if(rand()%4)
		{
			switch (rand()%2)
			{
				case 0:
				DoPlaySoundToSet(me,SOUND_ALY_KILL_1);
				//DoYell(YELL_ALY_KILL_1 ,LANG_UNIVERSAL,NULL);
				break;
				case 1:
				DoPlaySoundToSet(me,SOUND_ALY_KILL_2);
				//DoYell(YELL_ALY_KILL_2 ,LANG_UNIVERSAL,NULL);
				break;
			}
		}
	}

	void JustDied(Unit* Killer)
	{
		if (sisterdeath)
		{
			//DoYell(YELL_ALY_DEAD ,LANG_UNIVERSAL,NULL);
			DoPlaySoundToSet(me,SOUND_ALY_DEAD);
		}
		else
		{
			me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
		}
	}


	void UpdateAI(const uint32 diff)
	{
		if(!sisterdeath)
		{
			if (pInstance)
			{
				Unit* Temp = NULL;
				Temp = Unit::GetUnit((*me),pInstance->GetData64(DATA_SACROLASH));
				if (Temp->isDead())
				{
					//DoYell(YELL_SISTER_SACROLASH_DEAD ,LANG_UNIVERSAL,NULL);
					DoPlaySoundToSet(me,SOUND_SISTER_SACROLASH_DEAD);
					sisterdeath = true;

					me->InterruptSpell(CURRENT_GENERIC_SPELL);
					DoCastMe(SPELL_EMPOWER);
				}
			}
		}

		if (!me->SelectHostileTarget() || !me->getVictim())
			return;

		if(sisterdeath)
		{
			if(shadownova_timer < diff)
			{
				if (!me->IsNonMeleeSpellCasted(false))
				{
					Unit* target = NULL;
					target = SelectUnit(SELECT_TARGET_RANDOM, 0);
					DoCast(target,SPELL_SHADOW_NOVA);
					shadownova_timer= 30000+(rand()%5000);
				}
			}
			else 
				shadownova_timer -=diff;
		}
		else
		{
			if (conflagration_timer < diff)
			{
				if (!me->IsNonMeleeSpellCasted(false))
				{
					me->InterruptSpell(CURRENT_GENERIC_SPELL);
					Unit* target = NULL;
					target = SelectUnit(SELECT_TARGET_RANDOM, 0);
					DoCast(target,SPELL_CONFLAGRATION);
					conflagration_timer = 30000+(rand()%5000);

					if(!sisterdeath)
					{
						//DoTextEmote("directs Conflagration at $N",target);
						DoPlaySoundToSet(me,SOUND_CANFLAGRATION);
						//DoYell(YELL_CANFLAGRATION,LANG_UNIVERSAL,NULL);
					}

					blaze_timer = 4000;
				}
			}
			else
				conflagration_timer -= diff;
		}

		if (flamesear_timer < diff)
		{
			if (!me->IsNonMeleeSpellCasted(false))
			{
				Unit* target = NULL;
				for(int i = 0;i<5;i++)
				{
					target = SelectUnit(SELECT_TARGET_RANDOM, 0);
					target->CastSpell(target,SPELL_FLAME_SEAR,true);
				}
				flamesear_timer = 15000;
			}
		}
		else 
			flamesear_timer -=diff;

		if (pyrogenics_timer < diff)
		{
			if (!me->IsNonMeleeSpellCasted(false))
			{
				DoCastMe(SPELL_PYROGENICS,true);
				pyrogenics_timer = 15000;
			}
		}
		else
			pyrogenics_timer -= diff;

		if (blaze_timer < diff)
		{
			if (!me->IsNonMeleeSpellCasted(false))
			{
				blazetarget = me->getVictim()->GetGUID();
				DoCastVictim(SPELL_BLAZE);
				blazesummon = false;

				blaze_timer = 3500;
				blazesummon_timer = 2600;
			}
		}
		else 
			blaze_timer -= diff;

		if(!blazesummon)
		{
			if (blaze_timer < diff)
			{
				Unit* Temp = NULL;
				Temp = Unit::GetUnit((*me),blazetarget);
				//if(Temp && Temp->isAlive())
				//me->SummonCreature(187366,Temp->GetPositionX(),Temp->GetPositionY(),Temp->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,10000 );
				Temp = SelectUnit(SELECT_TARGET_RANDOM, 0);
				if (rand()%2 == 0)
				Temp->CastSpell(Temp,SPELL_FLAME_TOUCHED,true);
				blazesummon = true;
				blazesummon_timer = 2500;
			}
			else
				blaze_timer -= diff;
		}

		if (enrage_timer < diff)
		{
			me->InterruptSpell(CURRENT_GENERIC_SPELL);
			DoPlaySoundToSet(me,SOUND_BERSERK);
			//DoYell(YELL_BERSERK ,LANG_UNIVERSAL,NULL);
			DoCastMe(SPELL_ENRAGE);
			enrage_timer = 300000;
		}
		else 
			enrage_timer -= diff;
	}
};

CreatureAI* GetAI_boss_alythess(Creature *_Creature)
{
	return new boss_alythessAI (_Creature);
};

struct MANGOS_DLL_DECL mob_shadow_imageAI : public ScriptedAI
{
	mob_shadow_imageAI(Creature *c) : ScriptedAI(c) {Reset();}

	bool InCombat;
	uint32 shadowfury_timer;
	uint32 touched_timer;

	void Reset()
	{
		InCombat = false;
		shadowfury_timer = 5000 + (rand()%10000);
		touched_timer = 1000;
		DoCastMe(SPELL_IMAGE_VISUAL);
	}

	void Aggro(Unit *who){}

	void UpdateAI(const uint32 diff)
	{
		if (!me->SelectHostileTarget() || !me->getVictim())
			return;

		if(touched_timer < diff)
		{
			DoCastVictim(SPELL_DARK_TOUCHED,true);
			touched_timer = 1000;
		}
		else
			touched_timer -= diff;

		if(shadowfury_timer < diff)
		{
			DoCastMe(SPELL_SHADOW_FURY);
			shadowfury_timer = 10000;
		}
		else 
			shadowfury_timer -=diff;

		DoMeleeAttackIfReady();
		}
	};

CreatureAI* GetAI_mob_shadow_image(Creature *_Creature)
{
	return new mob_shadow_imageAI (_Creature);
};

struct MANGOS_DLL_DECL mob_blaze_fireAI : public ScriptedAI
{
	mob_blaze_fireAI(Creature *c) : ScriptedAI(c) {Reset();}

	uint32 burn_timer;

	void Reset()
	{
		burn_timer = 3000;
	}

	void Aggro(Unit *who){}
	void AttackStart(Unit *who) {}
	void MoveInLineOfSight(Unit *who){}

	void UpdateAI(const uint32 diff)
	{
		if(burn_timer < diff)
		{
			DoCastMe(SPELL_BLAZE_BURN);
			burn_timer = 3000;
		}
		else 
			burn_timer -=diff;
	}
};

CreatureAI* GetAI_mob_blaze_fire(Creature *_Creature)
{
	return new mob_blaze_fireAI (_Creature);
};

void AddSC_boss_eredar_twins()
{
	Script *newscript;
	newscript = new Script;
	newscript->Name="boss_sacrolash";
	newscript->GetAI = GetAI_boss_sacrolash;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="boss_alythess";
	newscript->GetAI = GetAI_boss_alythess;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="mob_shadow_image";
	newscript->GetAI = GetAI_mob_shadow_image;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name="mob_blaze_fire";
	newscript->GetAI = GetAI_mob_blaze_fire;
	newscript->RegisterSelf();

}

