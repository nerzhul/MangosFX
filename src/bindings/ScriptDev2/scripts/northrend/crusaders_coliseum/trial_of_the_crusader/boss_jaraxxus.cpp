#include "precompiled.h"
#include "trial_of_the_crusader.h"
enum
{
	SPELL_BERSERK				= 64238,
	SPELL_FEL_FIREBALL 			= 66532,
	SPELL_FEL_LIGHTNING			= 66528,
	SPELL_INCINERATE_FLESH	 	= 66237,
	SPELL_INFERNAL_ERUPTION		= 66258,
	SPELL_LEGION_FLAME	 		= 66199,

	SPELL_NETHER_PORTAL			= 66269,
	SPELL_NETHER_POWER 			= 66228,
};

struct MANGOS_DLL_DECL boss_jaraxxusAI : public LibDevFSAI
{
    boss_jaraxxusAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_BERSERK,600000,60000);
		AddEventOnMe(SPELL_NETHER_POWER,25000,50000);
		AddMaxPrioEvent(SPELL_INFERNAL_ERUPTION,90000,120000);
		AddEvent(SPELL_FEL_FIREBALL,5000,12000,2000);
		AddEvent(SPELL_FEL_LIGHTNING,5000,18000,500);
		AddEvent(SPELL_LEGION_FLAME,20000,30000);
    }

	uint32 incinerate_Timer;
	uint32 portal_Timer;

    void Reset()
    {
		ResetTimers();
		incinerate_Timer = 15000;
		portal_Timer = 20000;
    }

	void SpellHit(Unit* pWho,const SpellEntry* spell)
	{
		if(!spell)
			return;

		if(spell->Id == 66242 || spell->Id == 67059 ||
			spell->Id == 67060 || spell->Id == 67061)
		{
			me->RemoveAurasDueToSpell(spell->Id);
			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
			{
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
				{
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						ModifyAuraStack(spell->Id,1,pPlayer);
				}
			}
		}
	}

	void DamageTaken(Unit* pWho, uint32 &dmg)
	{
		if(pWho->HasAura(SPELL_INCINERATE_FLESH))
			dmg /= 2;

	}
    void KilledUnit(Unit *victim)
    {
		if(urand(0,1))
			Speak(CHAT_TYPE_SAY,16145,"Cafard insignifiant !");
		else
			Speak(CHAT_TYPE_SAY,16146,"Banni, au néant !");
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
		{
            pInstance->SetData(TYPE_JARAXXUS, DONE);
			Speak(CHAT_TYPE_SAY,16147,"Un autre prendra ma place. Votre monde est condamné.");
			if (Creature* Ann = ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_ANNOUNCER) : 0)))
				((npc_toc10_announcerAI*)Ann->AI())->StartEvent(NULL,EVENT_TYPE_JARAXXUS_OUTRO);
		}
		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(CONQUETE,2);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
		}
		me->ForcedDespawn(TEN_MINS*1000);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();

        if (pInstance)
            pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);

		Yell(16144,"Devant vous se tient Jaraxxus seigneur Eredar de la Légion Ardente !");
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(incinerate_Timer <= diff)
		{
			Yell(16149,"Que brûle la chair !");
			DoCastRandom(SPELL_INCINERATE_FLESH);
			incinerate_Timer = 20000;
		}
		else
			incinerate_Timer -= diff;

		if(portal_Timer <= diff)
		{
			Speak(CHAT_TYPE_SAY,16150,"Viens, ma soeur, ton maître t'appelle.");
			CallCreature(34825,TEN_MINS,NEAR_15M,AGGRESSIVE_MAIN);
			portal_Timer = 120000;
		}
		else
			portal_Timer -= diff;
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_jaraxxus(Creature* pCreature)
{
    return new boss_jaraxxusAI(pCreature);
}

struct MANGOS_DLL_DECL volcan_jaraxxusEdCAI : public LibDevFSAI
{
    volcan_jaraxxusEdCAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(66255,30000,45000);
    }

    void Reset()
    {
		ResetTimers();
				
		SetCombatMovement(false);
    }


    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_volcan_jaraxxusEdC(Creature* pCreature)
{
    return new volcan_jaraxxusEdCAI(pCreature);
}

struct MANGOS_DLL_DECL portal_jaraxxusEdCAI : public ScriptedAI
{
    portal_jaraxxusEdCAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
		dfc = me->GetMap()->GetDifficulty();
    }

	MobEventTasks Tasks;
	Difficulty dfc;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		switch(dfc)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
			case RAID_DIFFICULTY_25MAN_NORMAL:
				Tasks.AddSummonEvent(34826,5000,900000,0,0,1,TEN_MINS,ON_ME,AGGRESSIVE_MAIN);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
			case RAID_DIFFICULTY_25MAN_HEROIC:
				Tasks.AddSummonEvent(34826,5000,35000,0,0,1,TEN_MINS,ON_ME,AGGRESSIVE_MAIN);
				break;
		}
		
		SetCombatMovement(false);
    }

    void UpdateAI(const uint32 diff)
    {
		Tasks.UpdateEvent(diff);
    }

};

CreatureAI* GetAI_portal_jaraxxusEdC(Creature* pCreature)
{
    return new portal_jaraxxusEdCAI(pCreature);
}

struct MANGOS_DLL_DECL mob_fel_infernalAI : public ScriptedAI
{
    mob_fel_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Difficulty = me->GetMap()->GetDifficulty();
        Reset();
		me->setFaction(14);
    }
 
    ScriptedInstance* m_pInstance;
    uint8 Difficulty;
    MobEventTasks Tasks;

	uint32 resetAggro_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(66494,5000,15000,5000,TARGET_MAIN);
		Tasks.AddEvent(66495,1000,6100,100,TARGET_MAIN);
		me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
		resetAggro_Timer = 8000;
	}
 
    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(resetAggro_Timer <= diff)
		{
			DoResetThreat();
			resetAggro_Timer = 10000;
		}
		else
			resetAggro_Timer -= diff;
 
		Tasks.UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }
};
 
CreatureAI* GetAI_mob_fel_infernal(Creature* pCreature)
{
    return new mob_fel_infernalAI(pCreature);
}

struct MANGOS_DLL_DECL mob_mistress_of_painAI : public ScriptedAI
{
    mob_mistress_of_painAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		dfc = me->GetMap()->GetDifficulty();
        Reset();
    }
 
	ScriptedInstance* m_pInstance;
	Difficulty dfc;
	MobEventTasks Tasks;
	uint32 resetAggro_Timer;
 
    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(66378,20000,20000,0,TARGET_MAIN);
		Tasks.AddEvent(66283,5000,15000,5000);
		Tasks.AddEvent(66334,10000,8000,2000,TARGET_HAS_MANA);
		AggroAllPlayers(150.0f);
		me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
		resetAggro_Timer = 4000;
    }
 
    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(resetAggro_Timer <= diff)
		{
			DoResetThreat();
			resetAggro_Timer = 8000;
		}
		else
			resetAggro_Timer -= diff;
	
		Tasks.UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }
};
 
CreatureAI* GetAI_mob_mistress_of_pain(Creature* pCreature)
{
    return new mob_mistress_of_painAI(pCreature);
}

struct MANGOS_DLL_DECL mob_legion_flameAI : public LibDevFSAI
{
    mob_legion_flameAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		MakeHostileInvisibleStalker();
	}

    void Reset()
    {
		ResetTimers();
		ModifyAuraStack(66201);
        me->SetRespawnDelay(DAY);
		SetDespawnTimer(m_difficulty ? 30000 : 60000);
		SetCombatMovement(false);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;
    }
};

CreatureAI* GetAI_mob_legion_flame(Creature* pCreature)
{
    return new mob_legion_flameAI(pCreature);
}

void AddSC_boss_Jaraxxus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_jaraxxus";
    newscript->GetAI = &GetAI_boss_jaraxxus;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "volcan_jaraxxus";
    newscript->GetAI = &GetAI_volcan_jaraxxusEdC;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "portal_jaraxxus";
    newscript->GetAI = &GetAI_portal_jaraxxusEdC;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_fel_infernal";
    newscript->GetAI = &GetAI_mob_fel_infernal;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_mistress_of_pain";
    newscript->GetAI = &GetAI_mob_mistress_of_pain;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_legion_flame";
    newscript->GetAI = &GetAI_mob_legion_flame;
    newscript->RegisterSelf();

}
