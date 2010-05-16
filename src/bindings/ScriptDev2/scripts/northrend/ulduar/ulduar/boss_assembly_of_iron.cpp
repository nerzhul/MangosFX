/*
 * Copyright (C) 2008 - 2009 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Assembly of Iron encounter
SD%Complete: 60%
SDComment: Runes need DB support, chain lightning won't cast, supercharge won't cast (pTarget error?) - it worked before during debugging.
SDCategory: Ulduar - Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

// Any boss
#define SPELL_SUPERCHARGE   61920
#define SPELL_BERSERK       47008   // Hard enrage, don't know the correct ID.

// Steelbreaker
#define SPELL_HIGH_VOLTAGE           61890
#define SPELL_HIGH_VOLTAGE_H         63498
#define SPELL_FUSION_PUNCH           61903
#define SPELL_FUSION_PUNCH_H         63493
#define SPELL_STATIC_DISRUPTION      44008
#define SPELL_STATIC_DISRUPTION_H    63494
#define SPELL_OVERWHELMING_POWER_H   61888
#define SPELL_OVERWHELMING_POWER     64637
#define SPELL_ELECTRICAL_CHARGE      61902

// Runemaster Molgeim
#define SPELL_SHIELD_OF_RUNES        62274
#define SPELL_SHIELD_OF_RUNES_H      63489
#define SPELL_RUNE_OF_POWER          64320
#define SPELL_RUNE_OF_DEATH          62269
#define SPELL_RUNE_OF_SUMMONING      62273
#define SPELL_LIGHTNING_BLAST        62054
#define SPELL_LIGHTNING_BLAST_H      63491
#define CREATURE_RUNE_OF_SUMMONING   33051

// Stormcaller Brundir
#define SPELL_CHAIN_LIGHTNING_N      61879
#define SPELL_CHAIN_LIGHTNING_H      63479
#define SPELL_OVERLOAD               61869
#define SPELL_OVERLOAD_H             63481
#define SPELL_LIGHTNING_WHIRL        61915
#define SPELL_LIGHTNING_WHIRL_H      63483
#define SPELL_LIGHTNING_TENDRILS     61887
#define SPELL_LIGHTNING_TENDRILS_H   63486
#define SPELL_STORMSHIELD            64187

bool IsEncounterComplete(ScriptedInstance* pInstance, Creature* me)
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



void boss_steelbreakerAI::Reset()
{
	MobsTasks.SetObjects(this,me);
	MobsTasks.AddEvent(SPELL_BERSERK,900000,60000,0,TARGET_ME);

	if(m_bIsHeroic)
	{
		MobsTasks.AddEvent(SPELL_OVERWHELMING_POWER_H,35000,35000,0,TARGET_MAIN,3);
		MobsTasks.AddEvent(SPELL_FUSION_PUNCH_H,15000,20000,40000,TARGET_MAIN);
	}
	else
	{
		MobsTasks.AddEvent(SPELL_OVERWHELMING_POWER,60000,60000,0,TARGET_MAIN,3);
		MobsTasks.AddEvent(SPELL_FUSION_PUNCH,15000,20000,40000,TARGET_MAIN);
	}
	me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    phase = 0;
    me->RemoveAllAuras();
    if(pInstance)
        pInstance->SetData(TYPE_ASSEMBLY, NOT_STARTED);

	if(Creature* Molgeim = (Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_MOLGEIM) : 0))
        Molgeim->Respawn();
	if(Creature* Brundir = (Creature*)Unit::GetUnit((*me), pInstance ? pInstance->GetData64(DATA_BRUNDIR) : 0))
		Brundir->Respawn();
}

void boss_steelbreakerAI::EnterCombat(Unit *who)
{
	Yell(15674,"Vous ne vainquerez pas si facilement l'assemblÃ©e de fer, envahisseur !");
	DoCastMe( (m_bIsHeroic)? SPELL_HIGH_VOLTAGE : SPELL_HIGH_VOLTAGE_H);
    UpdatePhase();
	if(Unit* Brundir = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_BRUNDIR) : 0))
        Brundir->AddThreat(me->getVictim());
	if(Unit* Molgeim = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_MOLGEIM) : 0))
        Molgeim->AddThreat(me->getVictim());
}

void boss_steelbreakerAI::UpdatePhase()
{
    ++phase;
	if(phase > 1)
		SetAuraStack(SPELL_SUPERCHARGE,phase - 1,me,me,1);
    if(phase >= 2)
        Disruption_Timer = 30000;
}

void boss_steelbreakerAI::DamageTaken(Unit* pKiller, uint32 &damage)
{
    if(damage >= me->GetHealth())
    {
		damage = 0;
		pKiller->CastStop();
		pKiller->AttackStop();
        if(Unit* Brundir = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_BRUNDIR) : 0))
            if(Brundir->isAlive())
            {
                Brundir->SetHealth(Brundir->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_stormcaller_brundirAI*)((Creature*)Brundir)->AI())->UpdatePhase();
            }

        if(Unit* Molgeim = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_MOLGEIM) : 0))
            if(Molgeim->isAlive())
            {
                Molgeim->SetHealth(Molgeim->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_runemaster_molgeimAI*)((Creature*)Molgeim)->AI())->UpdatePhase();
            }
		
		Kill(me);
    }
}

void boss_steelbreakerAI::JustDied(Unit* Killer)
{
    if(IsEncounterComplete(pInstance, me) && pInstance)
        pInstance->SetData(TYPE_ASSEMBLY, DONE);
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

	if(phase >= 2)
	{
		if(Disruption_Timer <= diff)
		{
			if(Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
				DoCast(pTarget, (m_bIsHeroic) ? SPELL_STATIC_DISRUPTION_H : SPELL_STATIC_DISRUPTION);
			Disruption_Timer = urand(20000,40000);
		}
		else
			Disruption_Timer -= diff;
	}

	MobsTasks.UpdateEvent(diff);
	MobsTasks.UpdateEvent(diff,phase);

    DoMeleeAttackIfReady();
}



void boss_runemaster_molgeimAI::Reset()
{
	me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    if(pInstance)
        pInstance->SetData(TYPE_ASSEMBLY, NOT_STARTED);
    me->RemoveAllAuras();
    phase = 0;
	Shield_Timer = 27000;
	RuneD_Timer = 30000;
	RuneS_Timer = urand(20000,30000);
	if(Creature* Steelbreaker = (Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_STEELBREAKER) : 0))
        Steelbreaker->Respawn();
	if(Creature* Brundir = (Creature*)Unit::GetUnit((*me), pInstance ? pInstance->GetData64(DATA_BRUNDIR) : 0))
		Brundir->Respawn();

	MobsTasks.SetObjects(this,me);
	MobsTasks.AddEvent(SPELL_BERSERK,900000,60000,0,TARGET_ME);
	MobsTasks.AddEvent(SPELL_RUNE_OF_POWER,60000,60000,0,TARGET_LOW_HP);
	MobsTasks.AddEvent(SPELL_RUNE_OF_SUMMONING,20000,20000,10000,TARGET_RANDOM,3);

	if(m_bIsHeroic)
	{
		MobsTasks.AddEvent(SPELL_LIGHTNING_BLAST_H,15000,20000,20000,TARGET_MAIN);
		MobsTasks.AddEvent(SPELL_SHIELD_OF_RUNES_H,27000,27000,7000,TARGET_ME);
	}
	else
	{
		MobsTasks.AddEvent(SPELL_LIGHTNING_BLAST,15000,20000,20000,TARGET_MAIN);
		MobsTasks.AddEvent(SPELL_SHIELD_OF_RUNES,27000,27000,7000,TARGET_ME);
	}
}

void boss_runemaster_molgeimAI::EnterCombat(Unit* who)
{
	if(Unit* Steelbreaker = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_STEELBREAKER) : 0))
        Steelbreaker->AddThreat(me->getVictim());
	if(Unit* Brundir = Unit::GetUnit((*me), pInstance ? pInstance->GetData64(DATA_BRUNDIR) : 0))
        Brundir->AddThreat(me->getVictim());
    Yell(15657,"Seule votre extermination complète me conviendra !");
    UpdatePhase();
}

void boss_runemaster_molgeimAI::UpdatePhase()
{
    ++phase;
	if(phase > 1)
		SetAuraStack(SPELL_SUPERCHARGE,phase - 1,me,me,1);

	RuneD_Timer = 30000;
	if(phase == 3)
		RuneS_Timer = urand(20000,30000);
}

void boss_runemaster_molgeimAI::DamageTaken(Unit* pKiller, uint32 &damage)
{
    if(damage >= me->GetHealth())
    {
		damage = 0;
		pKiller->CastStop();
		pKiller->AttackStop();
		if(Unit* Steelbreaker = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_STEELBREAKER) : 0))
            if(Steelbreaker->isAlive())
            {
                Steelbreaker->SetHealth(Steelbreaker->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_steelbreakerAI*)((Creature*)Steelbreaker)->AI())->UpdatePhase();
            }

        if(Unit* Brundir = Unit::GetUnit((*me), pInstance ? pInstance->GetData64(DATA_BRUNDIR) : 0))
            if(Brundir->isAlive())
            {
                Brundir->SetHealth(Brundir->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_stormcaller_brundirAI*)((Creature*)Brundir)->AI())->UpdatePhase();
            }

		Kill(me);
    }
}

void boss_runemaster_molgeimAI::JustDied(Unit* Killer)
{
    if(IsEncounterComplete(pInstance, me) && pInstance)
        pInstance->SetData(TYPE_ASSEMBLY, DONE);
	if(urand(0,1))
		Yell(15662,"L'hÃ©ritage des tempêtes ne sera pas anÃ©anti...");
	else
		Speak(CHAT_TYPE_SAY,15663,"Que vous apporte ma chute ? Votre destin n'en est pas moins scellÃ©, mortel.");
}

void boss_runemaster_molgeimAI::UpdateAI(const uint32 diff)
{
    if (!CanDoSomething())
        return;

	if(phase >= 2)
	{
		if(RuneD_Timer <= diff)
		{
			if(Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
				DoCast(pTarget, SPELL_RUNE_OF_DEATH);
			RuneD_Timer = urand(30000,40000);
		}
		else
			RuneD_Timer -= diff;
	}

	MobsTasks.UpdateEvent(diff);
	MobsTasks.UpdateEvent(diff,phase);
    DoMeleeAttackIfReady();
}

struct MANGOS_DLL_DECL mob_lightning_elementalAI : public ScriptedAI
{
    mob_lightning_elementalAI(Creature *c) : ScriptedAI(c)
    {
        Charge();
		m_bIsHeroic = c->GetMap()->IsRegularDifficulty();
		Reset();
    }

	void Reset(){}
    Unit* Target;
	bool m_bIsHeroic;

    void Charge()
    {
		Target = me->SelectNearbyTarget();
        me->AddThreat(Target, 5000000.0f);
        AttackStart(Target);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!me->isInCombat())
            return;

		if(me->GetDistance2d(Target) < 8.0f)
        {
			DoCast(Target, (!m_bIsHeroic) ? SPELL_LIGHTNING_BLAST_H : SPELL_LIGHTNING_BLAST);
            me->DealDamage(me,me->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        me->GetMotionMaster()->MoveChase(Target); // needed at every update?
    }

};

struct MANGOS_DLL_DECL mob_rune_of_summoningAI : public ScriptedAI
{
    mob_rune_of_summoningAI(Creature *c) : ScriptedAI(c)
    {
		Reset();
        SummonLightningElemental();
    }

	void Reset(){}
    void SummonLightningElemental()
    {
        me->SummonCreature(CREATURE_RUNE_OF_SUMMONING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
        me->DealDamage(me,me->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }
};

void boss_stormcaller_brundirAI::Reset()
{
	me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    if(pInstance)
        pInstance->SetData(TYPE_ASSEMBLY, NOT_STARTED);
    me->RemoveAllAuras();
    phase = 0;
	if(Creature* Steelbreaker = (Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_STEELBREAKER) : 0))
        Steelbreaker->Respawn();
	if(Creature* Molgeim = (Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_MOLGEIM) : 0))
    	Molgeim->Respawn();

	MobsTasks.SetObjects(this,me);
	MobsTasks.AddEvent(SPELL_BERSERK,900000,60000,0,TARGET_ME);

	if(m_bIsHeroic)
	{
		MobsTasks.AddEvent(SPELL_CHAIN_LIGHTNING_H,urand(9000,17000),9000,8000,TARGET_RANDOM);
		MobsTasks.AddEvent(SPELL_OVERLOAD_H,urand(60000,125000),60000,125000,TARGET_ME);
	}
	else
	{
		MobsTasks.AddEvent(SPELL_CHAIN_LIGHTNING_N,urand(9000,17000),9000,8000,TARGET_RANDOM);
		MobsTasks.AddEvent(SPELL_OVERLOAD,urand(60000,125000),60000,125000,TARGET_ME);
	}
}

void boss_stormcaller_brundirAI::EnterCombat(Unit* who)
{
	Yell(15684,"Que vous soyez les plus grandes punaises ou les plus grands hÃ©ros de ce monde, vous n'êtes jamais que des mortels");
    UpdatePhase();
	if(Unit* Steelbreaker = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_STEELBREAKER) : 0))
        Steelbreaker->AddThreat(me->getVictim());
	if(Unit* Molgeim = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_MOLGEIM) : 0))
        Molgeim->AddThreat(me->getVictim());
}

void boss_stormcaller_brundirAI::UpdatePhase()
{
    ++phase;
	if(phase > 1)
		SetAuraStack(SPELL_SUPERCHARGE,phase - 1,me,me,1);
    if(phase >= 2)
		LightW_Timer = urand(20000,40000);
    if(phase >= 3)
    {
        DoCastMe(SPELL_STORMSHIELD);
		LightW_Timer = urand(40000,80000);
    }

}

void boss_stormcaller_brundirAI::DamageTaken(Unit* pKiller, uint32 &damage)
{
    if(damage >= me->GetHealth())
    {
		damage = 0;
		pKiller->CastStop();
		pKiller->AttackStop();
        if(Unit* Steelbreaker = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_STEELBREAKER) : 0))
            if(Steelbreaker->isAlive())
            {
                Steelbreaker->SetHealth(Steelbreaker->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_steelbreakerAI*)((Creature*)Steelbreaker)->AI())->UpdatePhase();
            }

        if(Unit* Molgeim = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_MOLGEIM) : 0))
            if(Molgeim->isAlive())
            {
                Molgeim->SetHealth(Molgeim->GetMaxHealth());
				me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
				((boss_runemaster_molgeimAI*)((Creature*)Molgeim)->AI())->UpdatePhase();
            }
		
		Kill(me);        
    }
}

void boss_stormcaller_brundirAI::JustDied(Unit* Killer)
{
    if(IsEncounterComplete(pInstance, me) && pInstance)
        pInstance->SetData(TYPE_ASSEMBLY, DONE);
	if(urand(0,1))
		Speak(CHAT_TYPE_SAY,15689,"La puissance de la tempête survivra...");
	else
		Speak(CHAT_TYPE_SAY,15690,"Vous courrez tout droit... à la gueule de la folie...");
}

void boss_stormcaller_brundirAI::UpdateAI(const uint32 diff)
{
    if (!CanDoSomething())
        return;

	if(phase >= 2)
	{
		if(LightW_Timer <= diff)
		{
			DoCastMe( m_bIsHeroic ? SPELL_LIGHTNING_WHIRL_H : SPELL_LIGHTNING_WHIRL);
			LightW_Timer = urand(20000,40000);
		}
		else
			LightW_Timer -= diff;
	}

	if(phase == 3)
	{
		if(LightT_Timer <= diff)
		{
			DoCastMe( m_bIsHeroic ? SPELL_LIGHTNING_TENDRILS_H : SPELL_LIGHTNING_TENDRILS);
			DoResetThreat();
			LightT_Timer = urand(15000,20000);
		}
		else
			LightT_Timer -= diff;
	}
    
	MobsTasks.UpdateEvent(diff);
	MobsTasks.UpdateEvent(diff,phase);

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