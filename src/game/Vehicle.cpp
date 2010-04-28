/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
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

#include "Common.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "Unit.h"
#include "Util.h"
#include "WorldPacket.h"
#include "TemporarySummon.h"

#include "CreatureAI.h"
#include "ZoneScript.h"

Vehicle::Vehicle(Unit *unit, VehicleEntry const *vehInfo) : Creature(CREATURE_SUBTYPE_VEHICLE), m_vehicleId(0), me(unit), m_vehicleInfo(vehInfo), m_usableSeatNum(0)
{
    m_updateFlag = (UPDATEFLAG_LIVING | UPDATEFLAG_HAS_POSITION | UPDATEFLAG_VEHICLE);
    for (uint32 i = 0; i < 8; ++i)
    {
        if(uint32 seatId = m_vehicleInfo->m_seatID[i])
            if(VehicleSeatEntry const *veSeat = sVehicleSeatStore.LookupEntry(seatId))
            {
				VehicleSeat vSeat(veSeat);
				vSeat.passenger = NULL;
				vSeat.flags = SEAT_FREE;
                m_Seats.insert(std::make_pair(i, vSeat));
                if(veSeat->IsUsable())
                    ++m_usableSeatNum;
            }
    }
    ASSERT(!m_Seats.empty());
}

Vehicle::~Vehicle()
{
    for (SeatMap::const_iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
        ASSERT(!itr->second.passenger);
}

void Vehicle::Install()
{
    if(Creature *cre = dynamic_cast<Creature*>(me))
    {
        if(m_vehicleInfo->m_powerType == POWER_STEAM)
        {
            me->setPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 100);
        }
        else if(m_vehicleInfo->m_powerType == POWER_PYRITE)
        {
            me->setPowerType(POWER_ENERGY);
            me->SetMaxPower(POWER_ENERGY, 50);
        }
        else
        {
            for (uint32 i = 0; i < 6; ++i)
            {
                if(!cre->m_spells[i])
                    continue;

                SpellEntry const *spellInfo = sSpellStore.LookupEntry(cre->m_spells[i]);
                if(!spellInfo)
                    continue;

                if(spellInfo->powerType == POWER_MANA)
                    break;

                if(spellInfo->powerType == POWER_ENERGY)
                {
                    me->setPowerType(POWER_ENERGY);
                    me->SetMaxPower(POWER_ENERGY, 100);
                    break;
                }
            }
        }
    }

    Reset();
}

void Vehicle::InstallAllAccessories()
{
    switch(me->GetEntry())
    {
        case 27850:InstallAccessory(27905,1);break;
        case 28782:InstallAccessory(28768,0,false);break; // Acherus Deathcharger
        case 28312:InstallAccessory(28319,7);break;
        case 32627:InstallAccessory(32629,7);break;
        case 32930:
            InstallAccessory(32933,0);
            InstallAccessory(32934,1);
            break;
        case 33109:InstallAccessory(33167,1);break;
        case 33060:InstallAccessory(33067,7);break;
        case 33113:
            InstallAccessory(33114,0);
            InstallAccessory(33114,1);
            InstallAccessory(33114,2);
            InstallAccessory(33114,3);
            InstallAccessory(33139,7);
            break;
        case 33114:
            InstallAccessory(33143,2); // Overload Control Device
            InstallAccessory(33142,1); // Leviathan Defense Turret
            break;
        case 33214:InstallAccessory(33218,1,false);break; // Mechanolift 304-A
        case 35637:InstallAccessory(34705,0,false);break;
        case 35633:InstallAccessory(34702,0,false);break;
        case 35768:InstallAccessory(34701,0,false);break;
        case 34658:InstallAccessory(34657,0,false);break;
        case 35636:InstallAccessory(34703,0,false);break;
        case 35638:InstallAccessory(35572,0,false);break;
        case 35635:InstallAccessory(35569,0,false);break;
        case 35640:InstallAccessory(35571,0,false);break;
        case 35641:InstallAccessory(35570,0,false);break;
        case 35634:InstallAccessory(35617,0,false);break;
    	case 33298:InstallAccessory(35332,0);break; //Darnassian Nightsaber
        case 33416:InstallAccessory(35330,0);break; //Exodar Elekk
        case 33297:InstallAccessory(35328,0);break; //Stormwind Steed
        case 33414:InstallAccessory(35327,0);break; //Forsaken Warhorse
        case 33301:InstallAccessory(35331,0);break; //Gnomeregan Mechanostrider
        case 33408:InstallAccessory(35329,0);break; //Ironforge Ram
        case 33300:InstallAccessory(35325,0);break; //Thunder Bluff Kodo
        case 33409:InstallAccessory(35314,0);break; //Orgrimmar Wolf
        case 33418:InstallAccessory(35326,0);break; //Silvermoon Hawkstrider
        case 33299:InstallAccessory(35323,0);break; //Darkspear Raptor
        case 35491:InstallAccessory(35451,0,false);break; //Black Knight
    }
}

void Vehicle::Uninstall()
{
    sLog.outDebug("Vehicle::Uninstall %u", me->GetEntry());
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
        if(Unit *passenger = itr->second.passenger)
            if(passenger->HasUnitTypeMask(UNIT_MASK_ACCESSORY))
                ((TemporarySummon*)passenger)->UnSummon();
    RemoveAllPassengers();
}

void Vehicle::Die()
{
    sLog.outDebug("Vehicle::Die %u", me->GetEntry());
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
        if(Unit *passenger = itr->second.passenger)
            if(passenger->HasUnitTypeMask(UNIT_MASK_ACCESSORY))
                passenger->setDeathState(JUST_DIED);
    RemoveAllPassengers();
}

void Vehicle::Reset()
{
    sLog.outDebug("Vehicle::Reset");
    if(m_usableSeatNum || me->GetEntry() == 32930)
    {
        if (me->GetTypeId() == TYPEID_PLAYER)
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
        }
        else
        {
            InstallAllAccessories();
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    }
}

void Vehicle::RemoveAllPassengers()
{
    sLog.outDebug("Vehicle::RemoveAllPassengers");
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
        if(Unit *passenger = itr->second.passenger)
        {
            if(passenger->IsVehicle())
                passenger->GetVehicleKit()->RemoveAllPassengers();
            if(passenger->GetVehicle() != this)
                sLog.outError("Vehicle %u has invalid passenger %u.", me->GetEntry(), passenger->GetEntry());
            passenger->ExitVehicle();
            if(itr->second.passenger)
            {
                sLog.outError("Vehicle %u cannot remove passenger %u. %u is still on it.", me->GetEntry(), passenger->GetEntry(), itr->second.passenger->GetEntry());
                //ASSERT(!itr->second.passenger);
                itr->second.passenger = NULL;
            }
        }
}

bool Vehicle::HasEmptySeat(int8 seatId) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);
    if(seat == m_Seats.end()) return false;
    return !seat->second.passenger;
}

Unit *Vehicle::GetPassenger(int8 seatId) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);
    if(seat == m_Seats.end()) return NULL;
    return seat->second.passenger;
}

int8 Vehicle::GetNextEmptySeat(int8 seatId, bool next) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);
    if(seat == m_Seats.end()) return -1;
    while(seat->second.passenger || !seat->second.seatInfo->IsUsable())
    {
        if(next)
        {
            ++seat;
            if(seat == m_Seats.end())
                seat = m_Seats.begin();
        }
        else
        {
            if(seat == m_Seats.begin())
                seat = m_Seats.end();
            --seat;
        }
        if(seat->first == seatId)
            return -1; // no available seat
    }
    return seat->first;
}

void Vehicle::InstallAccessory(uint32 entry, int8 seatId, bool minion)
{
    if(Unit *passenger = GetPassenger(seatId))
    {
        // already installed
        if(passenger->GetEntry() == entry)
        {
            ASSERT(passenger->GetTypeId() == TYPEID_UNIT);
/*            if(me->GetTypeId() == TYPEID_UNIT && ((Creature*)me)->IsInEvadeMode() && ((Creature*)passenger)->IsAIEnabled)
                ((Creature*)passenger)->AI()->EnterEvadeMode();*/
            return;
        }
        passenger->ExitVehicle(); // this should not happen
    }

    //TODO: accessory should be minion
    if(Creature *accessory = me->SummonCreature(entry, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
    {
        if(minion)
            accessory->AddUnitTypeMask(UNIT_MASK_ACCESSORY);
        accessory->EnterVehicle(this, seatId);
        // This is not good, we have to send update twice
		WorldPacket data;
		accessory->BuildHeartBeatMsg(&data);
		accessory->SendMessageToSet(&data, false);
    }
}

bool Vehicle::AddPassenger(Unit *unit, int8 seatId)
{
    if(unit->GetVehicle() != this)
        return false;

    SeatMap::iterator seat;
    if(seatId < 0) // no specific seat requirement
    {
        for (seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
            if(!seat->second.passenger && seat->second.seatInfo->IsUsable())
                break;

        if(seat == m_Seats.end()) // no available seat
            return false;
    }
    else
    {
        seat = m_Seats.find(seatId);
        if(seat == m_Seats.end())
            return false;

        if(seat->second.passenger)
            seat->second.passenger->ExitVehicle();

        ASSERT(!seat->second.passenger);
    }

    sLog.outDebug("Unit %s enter vehicle entry %u id %u dbguid %u seat %d", unit->GetName(), me->GetEntry(), m_vehicleInfo->m_ID, me->GetGUIDLow(), (int32)seat->first);

    seat->second.passenger = unit;
	
    if(seat->second.seatInfo->IsUsable())
    {
        ASSERT(m_usableSeatNum);
        --m_usableSeatNum;
        if(!m_usableSeatNum)
        {
            if (me->GetTypeId() == TYPEID_PLAYER)
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
            else
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    if(seat->second.seatInfo->m_flags && !(seat->second.seatInfo->m_flags & 0x400))
        unit->addUnitState(UNIT_STAT_ON_VEHICLE);

    unit->m_movementInfo.AddMovementFlag(MOVEFLAG_ONTRANSPORT);
	unit->m_movementInfo.AddMovementFlag(MOVEFLAG_FLY_UNK1);
	

    VehicleSeatEntry const *veSeat = seat->second.seatInfo;
	unit->m_movementInfo.SetTransportData(me->GetGUID(),veSeat->m_attachmentOffsetX,
		veSeat->m_attachmentOffsetY, veSeat->m_attachmentOffsetZ, 0, 0, seat->first);

	unit->m_SeatData.OffsetX = (veSeat->m_attachmentOffsetX);      // transport offsetX
    unit->m_SeatData.OffsetY = (veSeat->m_attachmentOffsetY);      // transport offsetY
    unit->m_SeatData.OffsetZ = (veSeat->m_attachmentOffsetZ);      // transport offsetZ
    unit->m_SeatData.Orientation = veSeat->m_passengerYaw;                                                                    // NOTE : needs confirmation
//    unit->m_SeatData.c_time = me->GetCreationTime();
    unit->m_SeatData.dbc_seat = veSeat->m_ID;
    unit->m_SeatData.seat = seatId;
    unit->m_SeatData.s_flags = sObjectMgr.GetSeatFlags(veSeat->m_ID);
//    unit->m_SeatData.v_flags = me->GetVehicleFlags();

    if(unit->GetTypeId() == TYPEID_PLAYER && seat->first == 0 && seat->second.seatInfo->m_flags & 0x800) // not right
	{
		unit->SendMonsterMoveTransport(me);

		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

		me->setFaction(((Player*)unit)->getFaction());
		
		me->SetCharmerGUID(unit->GetGUID());

		unit->SetCharm(me);
		
		((Player*)unit)->SetClientControl(me, 1);
		((Player*)unit)->SetMover(me);


		if (((Player*)unit)->isAFK())
            ((Player*)unit)->ToggleAFK();
		
       ((Player*)unit)->SetMoverInQueve(me);
        
		if(((Player*)unit)->GetGroup())
           ((Player*)unit)->SetGroupUpdateFlag(GROUP_UPDATE_VEHICLE);

		//((Player*)unit)->SetFarSightGUID(me->GetGUID());

		((Player*)unit)->VehicleSpellInitialize();
	}

    if(me->IsInWorld())
    {
        if(me->GetTypeId() == TYPEID_UNIT)
        {
            /*if(((Creature*)me)->IsAIEnabled)
                ((Creature*)me)->AI()->PassengerBoarded(unit, seat->first, true);*/

            // update all passenger's positions
            RelocatePassengers(me->GetMap());
        }
    }

    return true;
}

void Vehicle::RemovePassenger(Unit *unit)
{
    if(unit->GetVehicle() != this)
        return;

    SeatMap::iterator seat;
    for (seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
        if(seat->second.passenger == unit)
            break;

    ASSERT(seat != m_Seats.end());

    sLog.outDebug("Unit %s exit vehicle entry %u id %u dbguid %u seat %d", unit->GetName(), me->GetEntry(), m_vehicleInfo->m_ID, me->GetGUIDLow(), (int32)seat->first);

    seat->second.passenger = NULL;
    if(seat->second.seatInfo->IsUsable())
    {
        if(!m_usableSeatNum)
        {
            if (me->GetTypeId() == TYPEID_PLAYER)
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
            else
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
        ++m_usableSeatNum;
    }

	me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

	unit->clearUnitState(UNIT_STAT_ON_VEHICLE);

	if(unit->GetTypeId() == TYPEID_PLAYER)
    {
		((Player*)unit)->SetCharm(NULL);
		((Player*)unit)->SetFarSightGUID(0);
		((Player*)unit)->SetClientControl(me, 0);
		((Player*)unit)->SetMover(NULL);

		((Player*)unit)->RemovePetActionBar();
		if(((Player*)unit)->GetGroup())
			((Player*)unit)->SetGroupUpdateFlag(GROUP_UPDATE_VEHICLE);
	}


    //SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

    /*if(unit->GetTypeId() == TYPEID_PLAYER && seat->first == 0 && seat->second.seatInfo->m_flags & 0x800)
        me->RemoveCharmedBy(unit);*/

    /*if(me->GetTypeId() == TYPEID_UNIT && ((Creature*)me)->IsAIEnabled)
        ((Creature*)me)->AI()->PassengerBoarded(unit, seat->first, false);*/

    // only for flyable vehicles?
    //CastSpell(this, 45472, true);                           // Parachute
}

void Vehicle::RelocatePassengers(Map* map)
{
    // not sure that absolute position calculation is correct, it must depend on vehicle orientation and pitch angle
    for (SeatMap::const_iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
        if (Unit *passenger = itr->second.passenger)
        {
			float px = me->GetPositionX() + passenger->m_SeatData.OffsetX;
            float py = me->GetPositionY() + passenger->m_SeatData.OffsetY;
            float pz = me->GetPositionZ() + passenger->m_SeatData.OffsetZ;
            float po = me->GetOrientation() + passenger->m_SeatData.Orientation;
            passenger->SetPosition(px, py, pz, po);
        }
}
void Vehicle::Dismiss()
{
    Uninstall();
    SendObjectDeSpawnAnim(GetGUID());
    CombatStop();
    AddObjectToRemoveList();
}
