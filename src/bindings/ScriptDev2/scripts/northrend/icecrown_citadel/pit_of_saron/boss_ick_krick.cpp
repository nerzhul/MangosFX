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
    void Reset()
    {
		ResetTimers();
		PoisonWave_Timer = 30000;
    }


    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

		if(PoisonWave_Timer <= diff)
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
    }

	bool Event;
	uint8 event_phase;

    void Reset()
    {
		ResetTimers();
		Event = false;
		me->SetRespawnTime(DAY*7);
    }
	
	void Aggro(Unit* who)
	{
		Speak(CHAT_TYPE_YELL,16926,"Le travail ne doit pas être interrompu ! Ick sas fois deux");
	}

	void DamageTaken(Unit* pDoneby, uint32 &dmg)
	{
		if(dmg >= me->GetHealth())
		{
			me->setFaction(35);
			DoResetThreat();
			Speak(CHAT_TYPE_SAY,16934,
			Event = true;
		}
	}

	void KilledUnit(Unit* who)
	{
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
	      
			UpdateEvent(diff);

			DoMeleeAttackIfReady();
		}
		else
		{

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
    newscript->GetAI = &GetAI_boss_ick;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_exploding_orb_ICC5";
    newscript->GetAI = &GetAI_mob_exploding_orb;
    newscript->RegisterSelf();
}
