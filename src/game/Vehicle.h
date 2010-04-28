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

#ifndef MANGOSSERVER_VEHICLE_H
#define MANGOSSERVER_VEHICLE_H

#include "ObjectDefines.h"
#include "Creature.h"

struct VehicleEntry;
struct VehicleSeatEntry;
class Unit;

enum PowerType
{
    POWER_STEAM     = 61,
    POWER_PYRITE    = 41,
};

#define MAX_SEAT 8

struct VehicleSeat
{
    explicit VehicleSeat(VehicleSeatEntry const *_seatInfo) : seatInfo(_seatInfo), passenger(NULL) {}
    explicit VehicleSeat() : passenger(NULL) {}
    VehicleSeatEntry const *seatInfo;
    uint8 flags;
    Unit* passenger;
    uint32 vs_flags;
};

enum VehicleSeatFlags
{
    SEAT_FREE           = 0x01,                             // free seat
    SEAT_FULL           = 0x02,                             // seat occupied by player/creature
    // special cases
    SEAT_VEHICLE_FREE   = 0x04,                             // seat occupied by vehicle, but that vehicle is free
    SEAT_VEHICLE_FULL   = 0x08                              // seat occupied by vehicle and that vehicle is full too
};

typedef std::map<int8, VehicleSeat> SeatMap;

class MANGOS_DLL_DECL Vehicle : public Creature
{
    public:
        explicit Vehicle(Unit *unit, VehicleEntry const *vehInfo);
        virtual ~Vehicle();

        uint32 GetVehicleId() { return m_vehicleId; }
        void SetVehicleId(uint32 vehicleid) { m_vehicleId = vehicleid; }

        void Install();
        void Uninstall();
        void Reset();
        void Die();
        void InstallAllAccessories();
		void RelocatePassengers(Map* map);

        Unit *GetBase() const { return me; }
        VehicleEntry const *GetVehicleInfo() { return m_vehicleInfo; }

        bool HasEmptySeat(int8 seatId) const;
        Unit *GetPassenger(int8 seatId) const;
        int8 GetNextEmptySeat(int8 seatId, bool next) const;
        bool AddPassenger(Unit *passenger, int8 seatId = -1);
        void RemovePassenger(Unit *passenger);
        void RemoveAllPassengers();
        void Dismiss();

        SeatMap m_Seats;
		void InstallAccessory(uint32 entry, int8 seatId, bool minion = true);
		void ChangeSeatFlag(uint8 seat, uint8 flag);
		
		int8 GetTotalSeatsCount() { return m_Seats.size(); }
		int8 GetEmptySeatsCount(bool force = true);
		void EmptySeatsCountChanged();
		int8 GetNextEmptySeatNum(int8 seatId, bool next) const;

    protected:
        uint32 m_vehicleId;
        Unit *me;
        VehicleEntry const *m_vehicleInfo;
        uint32 m_usableSeatNum;

        
    private:
		void InitSeats();
        void SaveToDB(uint32, uint8)                        // overwrited of Creature::SaveToDB     - don't must be called
        {
            ASSERT(false);
        }
        void DeleteFromDB()                                 // overwrited of Creature::DeleteFromDB - don't must be called
        {
            ASSERT(false);
        }
};
#endif
