#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
// Any boss
	SPELL_SUPERCHARGE				 = 61920,
	SPELL_BERSERK					 = 47008,   // Hard enrage, don't know the correct ID.

	// Steelbreaker
	SPELL_HIGH_VOLTAGE				 = 61890,
	SPELL_HIGH_VOLTAGE_H			 = 63498,
	SPELL_FUSION_PUNCH				 = 61903,
	SPELL_FUSION_PUNCH_H			 = 63493,
	SPELL_STATIC_DISRUPTION			 = 44008,
	SPELL_STATIC_DISRUPTION_H		 = 63494,
	SPELL_OVERWHELMING_POWER_H		 = 61888,
	SPELL_OVERWHELMING_POWER		 = 64637,
	SPELL_ELECTRICAL_CHARGE			 = 61902,

	// Runemaster Molgeim
	SPELL_SHIELD_OF_RUNES			 = 62274,
	SPELL_SHIELD_OF_RUNES_H			 = 63489,
	SPELL_RUNE_OF_POWER				 = 64320,
	SPELL_RUNE_OF_DEATH				 = 62269,
	SPELL_RUNE_OF_SUMMONING			 = 62273,
	SPELL_LIGHTNING_BLAST			 = 62054,
	SPELL_LIGHTNING_BLAST_H			 = 63491,
	CREATURE_RUNE_OF_SUMMONING		 = 33051,

	// Stormcaller Brundir
	SPELL_CHAIN_LIGHTNING_N			 = 61879,
	SPELL_CHAIN_LIGHTNING_H			 = 63479,
	SPELL_OVERLOAD					 = 61869,
	SPELL_OVERLOAD_H				 = 63481,
	SPELL_LIGHTNING_WHIRL			 = 61915,
	SPELL_LIGHTNING_WHIRL_H			 = 63483,
	SPELL_LIGHTNING_TENDRILS		 = 61887,
	SPELL_LIGHTNING_TENDRILS_H		 = 63486,
	SPELL_STORMSHIELD				 = 64187
};

bool IsEncounterComplete(InstanceData* pInstance, Creature* me)
{
   if (!pInstance || !me)
        return false;

    for (uint8 i = 0; i < 3; ++i)
    {
        uint64 guid = pInstance->GetData64(DATA_STEELBREAKER+i);
        if(!guid)
            return false;

        if(Unit *boss = (Unit::GetUnit((*me), guid)))
        {
            if(boss->isAlive())
                return false;

            continue;
        }
        else
            return false;
    }

	Creature* Molgeim = (Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_MOLGEIM) : 0);
    Creature* Brundir = (Creature*)Unit::GetUnit((*me), pInstance ? pInstance->GetData64(DATA_BRUNDIR) : 0);
	Creature* Steelbreaker = (Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_STEELBREAKER) : 0);
       
	if(Molgeim && Brundir && Steelbreaker && !Molgeim->isAlive() && !Brundir->isAlive() && !Steelbreaker->isAlive())
	{
		Molgeim->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		Brundir->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		Steelbreaker->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
	}
    return true;
}

boss_steelbreakerAI::boss_steelbreakerAI(Creature *c) : LibDevFSAI(c)
{
	InitInstance();
	AddEnrageTimer(900000);
	if(m_difficulty)
	{
		AddEventOnTank(SPELL_OVERWHELMING_POWER_H,35000,35000,0,3);
		AddEventOnTank(SPELL_FUSION_PUNCH_H,15000,20000,40000);
		AddEvent(SPELL_STATIC_DISRUPTION_H,20000,20000,20000,TARGET_RANDOM,2);
		AddEvent(SPELL_STATIC_DISRUPTION_H,20000,20000,20000,TARGET_RANDOM,3);
	}
	else
	{
		AddEventOnTank(SPELL_OVERWHELMING_POWER,60000,60000,0,3);
		AddEventOnTank(SPELL_FUSION_PUNCH,15000,20000,40000);
		AddEvent(SPELL_STATIC_DISRUPTION,20000,20000,20000,TARGET_RANDOM,2);
		AddEvent(SPELL_STATIC_DISRUPTION,20000,20000,20000,TARGET_RANDOM,3);
	}
}
void boss_steelbreakerAI::Reset()
{
	ResetTimers();
	me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    phase = 0;
    me->RemoveAllAuras();
    if(pInstance)
        SetInstanceData(TYPE_ASSEMBLY, NOT_STARTED);

	if(Creature* Molgeim = GetInstanceCreature(DATA_MOLGEIM))
        Molgeim->Respawn();
	if(Creature* Brundir = GetInstanceCreature(DATA_BRUNDIR))
		Brundir->Respawn();
}

void boss_steelbreakerAI::EnterCombat(Unit *who)
{
	Yell(15674,"Vous ne vainquerez pas si facilement l'assemblÃ©e de fer, envahisseur !");
	DoCastMe((m_difficulty)? SPELL_HIGH_VOLTAGE : SPELL_HIGH_VOLTAGE_H);
    UpdatePhase();
	if(pInstance)
		SetInstanceData(TYPE_ASSEMBLY,IN_PROGRESS);
	if(Unit* Brundir = GetInstanceCreature(DATA_BRUNDIR))
        Brundir->AddThreat(me->getVictim());
	if(Unit* Molgeim = GetInstanceCreature(DATA_MOLGEIM))
        Molgeim->AddThreat(me->getVictim());
}

void boss_steelbreakerAI::UpdatePhase()
{
    ++phase;
	if(phase > 1)
		ModifyAuraStack(SPELL_SUPERCHARGE,phase - 1);
}

void boss_steelbreakerAI::DamageTaken(Unit* pKiller, uint32 &damage)
{
    if(damage >= me->GetHealth())
    {
		damage = 0;
		pKiller->CastStop();
		pKiller->AttackStop();
        if(Creature* Brundir = GetInstanceCreature(DATA_BRUNDIR))
            if(Brundir->isAlive())
            {
                Brundir->SetHealth(Brundir->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_stormcaller_brundirAI*)Brundir->AI())->UpdatePhase();
            }

        if(Creature* Molgeim = GetInstanceCreature(DATA_MOLGEIM))
            if(Molgeim->isAlive())
            {
                Molgeim->SetHealth(Molgeim->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_runemaster_molgeimAI*)Molgeim->AI())->UpdatePhase();
            }
		
		Kill(me);
    }
}

void boss_steelbreakerAI::JustDied(Unit* Killer)
{
    if(IsEncounterComplete(pInstance, me) && pInstance)
	{
        SetInstanceData(TYPE_ASSEMBLY, DONE);
		pInstance->CompleteAchievementForGroup(m_difficulty ? 2944 : 2941);
	}
}

void boss_steelbreakerAI::KilledUnit(Unit *who)
{
    if(phase == 3)
        DoCastMe(SPELL_ELECTRICAL_CHARGE);
}

void boss_steelbreakerAI::UpdateAI(const uint32 diff)
{
    if (!CanDoSomething())
        return;

	UpdateEvent(diff);
	UpdateEvent(diff,phase);

    DoMeleeAttackIfReady();
}

boss_runemaster_molgeimAI::boss_runemaster_molgeimAI(Creature *c) : LibDevFSAI(c)
{
	InitInstance();
	AddEnrageTimer(900000);
	AddEvent(SPELL_RUNE_OF_POWER,60000,60000,0,TARGET_LOW_HP);
	AddEvent(SPELL_RUNE_OF_SUMMONING,20000,20000,10000,TARGET_RANDOM,3);
	AddEvent(SPELL_RUNE_OF_DEATH,30000,30000,10000,TARGET_RANDOM,2);
	AddEvent(SPELL_RUNE_OF_DEATH,30000,30000,10000,TARGET_RANDOM,3);
	if(m_difficulty)
	{
		AddEventOnTank(SPELL_LIGHTNING_BLAST_H,15000,20000,20000);
		AddEventOnMe(SPELL_SHIELD_OF_RUNES_H,27000,27000,7000);
	}
	else
	{
		AddEventOnTank(SPELL_LIGHTNING_BLAST,15000,20000,20000);
		AddEventOnMe(SPELL_SHIELD_OF_RUNES,27000,27000,7000);
	}
}

void boss_runemaster_molgeimAI::Reset()
{
	ResetTimers();
	me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    if(pInstance)
        SetInstanceData(TYPE_ASSEMBLY, NOT_STARTED);
    me->RemoveAllAuras();
    phase = 0;
	if(Creature* Steelbreaker = GetInstanceCreature(DATA_STEELBREAKER))
        Steelbreaker->Respawn();
	if(Creature* Brundir = GetInstanceCreature(DATA_BRUNDIR))
		Brundir->Respawn();
}

void boss_runemaster_molgeimAI::EnterCombat(Unit* who)
{
	if(Unit* Steelbreaker = GetInstanceCreature(DATA_STEELBREAKER))
        Steelbreaker->AddThreat(me->getVictim());
	if(Unit* Brundir = GetInstanceCreature(DATA_BRUNDIR))
        Brundir->AddThreat(me->getVictim());
	if(pInstance)
		SetInstanceData(TYPE_ASSEMBLY,IN_PROGRESS);
    Yell(15657,"Seule votre extermination complète me conviendra !");
    UpdatePhase();
}

void boss_runemaster_molgeimAI::UpdatePhase()
{
    ++phase;
	if(phase > 1)
		ModifyAuraStack(SPELL_SUPERCHARGE,phase - 1);

}

void boss_runemaster_molgeimAI::DamageTaken(Unit* pKiller, uint32 &damage)
{
    if(damage >= me->GetHealth())
    {
		damage = 0;
		pKiller->CastStop();
		pKiller->AttackStop();
		if(Creature* Steelbreaker = GetInstanceCreature(DATA_STEELBREAKER))
            if(Steelbreaker->isAlive())
            {
                Steelbreaker->SetHealth(Steelbreaker->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_steelbreakerAI*)Steelbreaker->AI())->UpdatePhase();
            }

        if(Creature* Brundir = GetInstanceCreature(DATA_BRUNDIR))
            if(Brundir->isAlive())
            {
                Brundir->SetHealth(Brundir->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_stormcaller_brundirAI*)Brundir->AI())->UpdatePhase();
            }

		Kill(me);
    }
}

void boss_runemaster_molgeimAI::JustDied(Unit* Killer)
{
    if(IsEncounterComplete(pInstance, me) && pInstance)
	{
        SetInstanceData(TYPE_ASSEMBLY, DONE);
		pInstance->CompleteAchievementForGroup(m_difficulty ? 2942 : 2939);
	}
	if(urand(0,1))
		Yell(15662,"L'hÃ©ritage des tempêtes ne sera pas anÃ©anti...");
	else
		Speak(CHAT_TYPE_SAY,15663,"Que vous apporte ma chute ? Votre destin n'en est pas moins scellÃ©, mortel.");
}

void boss_runemaster_molgeimAI::UpdateAI(const uint32 diff)
{
    if (!CanDoSomething())
        return;

	UpdateEvent(diff);
	UpdateEvent(diff,phase);
    DoMeleeAttackIfReady();
}

struct MANGOS_DLL_DECL mob_lightning_elementalAI : public LibDevFSAI
{
    mob_lightning_elementalAI(Creature *c) : LibDevFSAI(c)
    {
        Charge();
		InitInstance();
    }

	uint64 Target;
	
	void Reset()
	{
		Target = 0;
	}
    
    void Charge()
    {
		Unit* target = me->SelectNearbyTarget();
		if(target)
		{
			me->AddThreat(target, 5000000.0f);
			AttackStart(target);
			Target = target->GetGUID();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!me->isInCombat())
            return;

		if(Unit* target = GetGuidUnit(Target))
		{
			if(me->GetDistance2d(target) < 8.0f)
			{
				DoCast(target, (m_difficulty) ? SPELL_LIGHTNING_BLAST_H : SPELL_LIGHTNING_BLAST);
				Kill(me);
			}
			me->GetMotionMaster()->MoveChase(target);
		}
    }

};

struct MANGOS_DLL_DECL mob_rune_of_summoningAI : public LibDevFSAI
{
    mob_rune_of_summoningAI(Creature *c) : LibDevFSAI(c)
    {
		InitInstance();
        SummonLightningElemental();
    }

	void Reset(){}
    void SummonLightningElemental()
    {
		CallCreature(CREATURE_RUNE_OF_SUMMONING,TEN_MINS,ON_ME,NOTHING);
        Kill(me);
    }
};


boss_stormcaller_brundirAI::boss_stormcaller_brundirAI(Creature *c) : LibDevFSAI(c)
{
	InitInstance();
	AddEnrageTimer(900000);
	AddEventOnMe(SPELL_BERSERK,900000,60000);

	if(m_difficulty)
	{
		AddEvent(SPELL_CHAIN_LIGHTNING_H,urand(9000,17000),9000,8000);
		AddEventOnMe(SPELL_OVERLOAD_H,urand(60000,125000),60000,125000);
		AddEventOnMe(SPELL_LIGHTNING_WHIRL_H,20000,20000,20000,2);
		AddEventOnMe(SPELL_LIGHTNING_WHIRL_H,20000,20000,20000,3);
	}
	else
	{
		AddEvent(SPELL_CHAIN_LIGHTNING_N,urand(9000,17000),9000,8000);
		AddEventOnMe(SPELL_OVERLOAD,urand(60000,125000),60000,125000);
		AddEventOnMe(SPELL_LIGHTNING_WHIRL,20000,20000,20000,2);
		AddEventOnMe(SPELL_LIGHTNING_WHIRL,20000,20000,20000,3);
	}
}
void boss_stormcaller_brundirAI::Reset()
{
	me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    if(pInstance)
        SetInstanceData(TYPE_ASSEMBLY, NOT_STARTED);
    me->RemoveAllAuras();
    phase = 0;
	if(Creature* Steelbreaker = GetInstanceCreature(DATA_STEELBREAKER))
        Steelbreaker->Respawn();
	if(Creature* Molgeim = GetInstanceCreature(DATA_MOLGEIM))
    	Molgeim->Respawn();
}

void boss_stormcaller_brundirAI::EnterCombat(Unit* who)
{
	Yell(15684,"Que vous soyez les plus grandes punaises ou les plus grands hÃ©ros de ce monde, vous n'êtes jamais que des mortels");
    UpdatePhase();
	if(Unit* Steelbreaker = GetInstanceCreature(DATA_STEELBREAKER))
        Steelbreaker->AddThreat(me->getVictim());
	if(Unit* Molgeim = GetInstanceCreature(DATA_MOLGEIM))
        Molgeim->AddThreat(me->getVictim());
	if(pInstance)
		SetInstanceData(TYPE_ASSEMBLY,IN_PROGRESS);
}

void boss_stormcaller_brundirAI::UpdatePhase()
{
    ++phase;
	if(phase > 1)
		ModifyAuraStack(SPELL_SUPERCHARGE,phase - 1);
    if(phase >= 3)
        DoCastMe(SPELL_STORMSHIELD);
}

void boss_stormcaller_brundirAI::DamageTaken(Unit* pKiller, uint32 &damage)
{
    if(damage >= me->GetHealth())
    {
		damage = 0;
		pKiller->CastStop();
		pKiller->AttackStop();
        if(Creature* Steelbreaker = GetInstanceCreature(DATA_STEELBREAKER))
            if(Steelbreaker->isAlive())
            {
                Steelbreaker->SetHealth(Steelbreaker->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_steelbreakerAI*)Steelbreaker->AI())->UpdatePhase();
            }

        if(Creature* Molgeim = GetInstanceCreature(DATA_MOLGEIM))
            if(Molgeim->isAlive())
            {
                Molgeim->SetHealth(Molgeim->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_runemaster_molgeimAI*)Molgeim->AI())->UpdatePhase();
            }
		
		Kill(me);        
    }
}

void boss_stormcaller_brundirAI::JustDied(Unit* Killer)
{
    if(IsEncounterComplete(pInstance, me) && pInstance)
	{
        SetInstanceData(TYPE_ASSEMBLY, DONE);
		pInstance->CompleteAchievementForGroup(m_difficulty ? 2943 : 2940);
	}
	if(urand(0,1))
		Speak(CHAT_TYPE_SAY,15689,"La puissance de la tempête survivra...");
	else
		Speak(CHAT_TYPE_SAY,15690,"Vous courrez tout droit... à la gueule de la folie...");
}

void boss_stormcaller_brundirAI::UpdateAI(const uint32 diff)
{
    if (!CanDoSomething())
        return;

	if(phase == 3)
	{
		if(LightT_Timer <= diff)
		{
			DoCastMe( m_difficulty ? SPELL_LIGHTNING_TENDRILS_H : SPELL_LIGHTNING_TENDRILS);
			DoResetThreat();
			LightT_Timer = urand(15000,20000);
		}
		else
			LightT_Timer -= diff;
	}
    
	UpdateEvent(diff);
	UpdateEvent(diff,phase);

    DoMeleeAttackIfReady();
}

CreatureAI* GetAI_boss_steelbreaker(Creature* pCreature)
{
    return new boss_steelbreakerAI (pCreature);
}

CreatureAI* GetAI_boss_runemaster_molgeim(Creature* pCreature)
{
    return new boss_runemaster_molgeimAI (pCreature);
}

CreatureAI* GetAI_boss_stormcaller_brundir(Creature* pCreature)
{
    return new boss_stormcaller_brundirAI (pCreature);
}

CreatureAI* GetAI_mob_lightning_elemental(Creature* pCreature)
{
    return new mob_lightning_elementalAI (pCreature);
}

CreatureAI* GetAI_mob_rune_of_summoning(Creature* pCreature)
{
    return new mob_rune_of_summoningAI (pCreature);
}

void AddSC_boss_assembly_of_iron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_steelbreaker";
    newscript->GetAI = &GetAI_boss_steelbreaker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_molgeim";
    newscript->GetAI = &GetAI_boss_runemaster_molgeim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_brundir";
    newscript->GetAI = &GetAI_boss_stormcaller_brundir;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_lightning_elemental";
    newscript->GetAI = &GetAI_mob_lightning_elemental;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_rune_of_summoning";
    newscript->GetAI = &GetAI_mob_rune_of_summoning;
    newscript->RegisterSelf();

}
