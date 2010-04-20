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

#include "precompiled.h"
#include "ulduar.h"
#include "Vehicle.h"

#define SPELL_PURSUED           62374
#define SPELL_GATHERING_SPEED   62375
#define SPELL_BATTERING_RAM     62376
#define SPELL_FLAME_VENTS       62396
#define SPELL_MISSILE_BARRAGE   62400
#define SPELL_SYSTEMS_SHUTDOWN  62475

#define SPELL_FLAME_CANNON      62395
//#define SPELL_FLAME_CANNON      64692 trigger the same spell

#define SPELL_OVERLOAD_CIRCUIT  62399

#define SPELL_SEARING_FLAME     62402

#define SPELL_BLAZE             62292

#define SPELL_SMOKE_TRAIL       63575

#define SPELL_MIMIRON_INFERNO   62910

#define SPELL_HODIR_FURY        62297

#define SPELL_ELECTROSHOCK      62522

enum Mobs
{
    MOB_MECHANOLIFT = 33214,
    MOB_LIQUID      = 33189,
    MOB_CONTAINER   = 33218,
};

enum Events
{
    EVENT_PURSUE = 1,
    EVENT_MISSILE,
    EVENT_VENT,
    EVENT_SPEED,
    EVENT_SUMMON,
    EVENT_MIMIRON_INFERNO, // Not Blizzlike
    EVENT_HODIR_FURY,      // Not Blizzlike
};

enum Seats
{
    SEAT_PLAYER = 0,
    SEAT_TURRET = 1,
    SEAT_DEVICE = 2,
};

struct MANGOS_DLL_DECL boss_flame_leviathanAI : public ScriptedAI
{
    boss_flame_leviathanAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        assert(vehicle);
    }

    ScriptedInstance* m_pInstance;

    Vehicle *vehicle;

	uint32 pursue_Timer;
	uint32 missile_Timer;
	uint32 vent_Timer;
	uint32 speed_Timer;
	uint32 summon_Timer;

    void Reset()
    {
        pursue_Timer = 0;
        missile_Timer = 1500;
        vent_Timer = 1500;
        speed_Timer = 15000;
        summon_Timer = 0;
    }

    void EnterCombat(Unit *who)
    {
        //events.ScheduleEvent(EVENT_MIMIRON_INFERNO, 60000 + (rand()%60000)); // Not Blizzlike
        //events.ScheduleEvent(EVENT_HODIR_FURY, 60000 + (rand()%60000));      // Not Blizzlike
        if (Creature *turret = CAST_CRE(vehicle->GetPassenger(7)))
            turret->AI()->DoZoneInCombat();
    }

    // TODO: effect 0 and effect 1 may be on different target
    void SpellHitTarget(Unit *pTarget, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_PURSUED)
            AttackStart(pTarget);
    }

    void JustDied(Unit *victim)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEVIATHAN, DONE);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if(spell->Id == 62472)
            vehicle->InstallAllAccessories();
        else if(spell->Id == SPELL_ELECTROSHOCK)
            m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->isInCombat())
            return;

        if (m_creature->getThreatManager().isThreatListEmpty())
        {
            EnterEvadeMode();
            return;
        }

        if (!m_creature->getVictim())
            eventId = EVENT_PURSUE;

		if(pursue_Timer <= uiDiff)
		{
			DoCast(m_creature->getVictim,SPELL_PURSUED, true);
			pursue_Timer = 35000;
		}
		else
			pursue_Timer -= uiDiff;

		if(missile_Timer <= uiDiff)
		{
			DoCast(m_creature->getVictim(), SPELL_MISSILE_BARRAGE);
			missile_Timer = 1500;
		}
		else
			missile_Timer -= uiDiff;

		if(vent_Timer <= uiDiff)
		{
			DoCast(m_creature->getVictim(),SPELL_FLAME_VENTS);
			vent_Timer = 20000;
		}
		else
			vent_Timer -= uiDiff;

		if(speed_Timer <= uiDiff)
		{
			DoCast(m_creature->getVictim,SPELL_GATHERING_SPEED);
			speed_Timer = 15000;
		}
		else
			speed_Timer -= uiDiff;

		if(summon_Timer <= uiDiff)
		{
			if(Creature *lift = DoSummonFlyer(MOB_MECHANOLIFT, me, rand()%20 + 20, 50, 0))
               lift->GetMotionMaster()->MoveRandom(100);
			summon_Timer = 2000;
		}
		else
			summon_Timer -= uiDiff;

        /*switch(eventId)
        {

            case EVENT_MIMIRON_INFERNO: // Not Blizzlike
                DoCast(m_creature->getVictim(), SPELL_MIMIRON_INFERNO);
                events.RepeatEvent(60000 + (rand()%60000));
                return;
            case EVENT_HODIR_FURY:      // Not Blizzlike
                DoCast(m_creature->getVictim(), SPELL_HODIR_FURY);
                events.RepeatEvent(60000 + (rand()%60000));
            default:
                events.PopEvent();
                break;
        }*/

		DoCast(m_creature->getVictim(),SPELL_BATTERING_RAM);
    }
};

//#define BOSS_DEBUG

struct MANGOS_DLL_DECL boss_flame_leviathan_seatAI : public ScriptedAI
{
    boss_flame_leviathan_seatAI(Creature *c) : ScriptedAI(c), vehicle(c->GetVehicleKit())
    {
        assert(vehicle);
	#ifdef BOSS_DEBUG
			m_creature->SetReactState(REACT_AGGRESSIVE);
	#endif
    }

    Vehicle *vehicle;

	#ifdef BOSS_DEBUG
		void MoveInLineOfSight(Unit *who)
		{
			if(who->GetTypeId() == TYPEID_PLAYER && CAST_PLR(who)->isGameMaster()
				&& !who->GetVehicle() && vehicle->GetPassenger(SEAT_TURRET))
				who->EnterVehicle(vehicle, SEAT_PLAYER);
		}
	#endif

    void PassengerBoarded(Unit *who, int8 seatId, bool apply)
    {
        if(!m_creature->GetVehicle())
            return;

        if(seatId == SEAT_PLAYER)
        {
            if(!apply)
                return;

            if(Creature *turret = CAST_CRE(vehicle->GetPassenger(SEAT_TURRET)))
            {
                turret->setFaction(m_creature->GetVehicleBase()->getFaction());
                turret->SetUInt32Value(UNIT_FIELD_FLAGS, 0); // unselectable
                turret->AI()->AttackStart(who);
            }
            if(Unit *device = vehicle->GetPassenger(SEAT_DEVICE))
            {
                device->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                device->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        }
        else if(seatId == SEAT_TURRET)
        {
            if(apply)
                return;

            if(Unit *device = vehicle->GetPassenger(SEAT_DEVICE))
            {
                device->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                device->SetUInt32Value(UNIT_FIELD_FLAGS, 0); // unselectable
            }
        }
    }
};

struct MANGOS_DLL_DECL boss_flame_leviathan_defense_turretAI : public TurretAI
{
    boss_flame_leviathan_defense_turretAI(Creature *c) : TurretAI(c) {}

    void DamageTaken(Unit *who, uint32 &damage)
    {
        if(!CanAIAttack(who))
            damage = 0;
    }

    bool CanAIAttack(const Unit *who) const
    {
        if (who->GetTypeId() != TYPEID_PLAYER || !who->GetVehicle() || who->GetVehicleBase()->GetEntry() != 33114)
            return false;
        return true;
    }
};

struct MANGOS_DLL_DECL boss_flame_leviathan_overload_deviceAI : public PassiveAI
{
    boss_flame_leviathan_overload_deviceAI(Creature *c) : PassiveAI(c) {}

    void DoAction(const int32 param)
    {
        if(param == EVENT_SPELLCLICK)
        {
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if(m_creature->GetVehicle())
            {
                if(Unit *player = m_creature->GetVehicle()->GetPassenger(SEAT_PLAYER))
                {
                    player->ExitVehicle();
                    m_creature->GetVehicleBase()->CastSpell(player, SPELL_SMOKE_TRAIL, true);
                    if(Unit *leviathan = m_creature->GetVehicleBase()->GetVehicleBase())
                        player->GetMotionMaster()->MoveKnockbackFrom(leviathan->GetPositionX(), leviathan->GetPositionY(), 30, 30);
                }
            }
        }
    }
};

struct MANGOS_DLL_DECL boss_flame_leviathan_safety_containerAI : public PassiveAI
{
    boss_flame_leviathan_safety_containerAI(Creature *c) : PassiveAI(c) {}

    void MovementInform(uint32 type, uint32 id)
    {
        if(id == m_creature->GetEntry())
        {
            if(Creature *liquid = DoSummon(MOB_LIQUID, me, 0))
                liquid->CastSpell(liquid, 62494, true);
            m_creature->DisappearAndDie(); // this will relocate creature to sky
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->GetVehicle() && m_creature->isSummon() && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
            m_creature->GetMotionMaster()->MoveFall(409.8f, m_creature->GetEntry());
    }
};

struct MANGOS_DLL_DECL spell_pool_of_tarAI : public TriggerAI
{
    spell_pool_of_tarAI(Creature *c) : TriggerAI(c)
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void DamageTaken(Unit *who, uint32 &damage)
    {
        damage = 0;
    }

    void SpellHit(Unit* caster, const SpellEntry *spell)
    {
        if(spell->SchoolMask & SPELL_SCHOOL_MASK_FIRE && !m_creature->HasAura(SPELL_BLAZE))
            m_creature->CastSpell(me, SPELL_BLAZE, true);
    }
};

CreatureAI* GetAI_boss_flame_leviathan(Creature* pCreature)
{
    return new boss_flame_leviathanAI (pCreature);
}

CreatureAI* GetAI_boss_flame_leviathan_seat(Creature* pCreature)
{
    return new boss_flame_leviathan_seatAI (pCreature);
}

CreatureAI* GetAI_boss_flame_leviathan_defense_turret(Creature* pCreature)
{
    return new boss_flame_leviathan_defense_turretAI (pCreature);
}

CreatureAI* GetAI_boss_flame_leviathan_overload_device(Creature* pCreature)
{
    return new boss_flame_leviathan_overload_deviceAI (pCreature);
}

CreatureAI* GetAI_boss_flame_leviathan_safety_containerAI(Creature* pCreature)
{
    return new boss_flame_leviathan_safety_containerAI(pCreature);
}

CreatureAI* GetAI_spell_pool_of_tar(Creature* pCreature)
{
    return new spell_pool_of_tarAI (pCreature);
}

void AddSC_boss_flame_leviathan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_flame_leviathan";
    newscript->GetAI = &GetAI_boss_flame_leviathan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_flame_leviathan_seat";
    newscript->GetAI = &GetAI_boss_flame_leviathan_seat;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_flame_leviathan_defense_turret";
    newscript->GetAI = &GetAI_boss_flame_leviathan_defense_turret;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_flame_leviathan_overload_device";
    newscript->GetAI = &GetAI_boss_flame_leviathan_overload_device;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_flame_leviathan_safety_container";
    newscript->GetAI = &GetAI_boss_flame_leviathan_safety_containerAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "spell_pool_of_tar";
    newscript->GetAI = &GetAI_spell_pool_of_tar;
    newscript->RegisterSelf();
}
