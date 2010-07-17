/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

//return closest creature alive in grid, with range from pSource
Creature* GetClosestCreatureWithEntry(WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    Creature* pCreature = NULL;

    CellPair pair(MaNGOS::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*pSource, uiEntry, true, fMaxSearchRange);
    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pSource, pCreature, creature_check);

    TypeContainerVisitor<MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck>, GridTypeMapContainer> creature_searcher(searcher);

    cell.Visit(pair, creature_searcher,*(pSource->GetMap()));

    return pCreature;
}

void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& lList , WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    CellPair pair(MaNGOS::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    AllGameObjectsWithEntryInRange check(pSource, uiEntry, fMaxSearchRange);
    MaNGOS::GameObjectListSearcher<AllGameObjectsWithEntryInRange> searcher(pSource, lList, check);
    TypeContainerVisitor<MaNGOS::GameObjectListSearcher<AllGameObjectsWithEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(pSource->GetMap()));
}

void GetCreatureListWithEntryInGrid(std::list<Creature*>& lList, WorldObject* pSource, uint32 uiEntry, float fMaxSearchRange)
{
    CellPair pair(MaNGOS::ComputeCellPair(pSource->GetPositionX(), pSource->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    AllCreaturesOfEntryInRange check(pSource, uiEntry, fMaxSearchRange);
    MaNGOS::CreatureListSearcher<AllCreaturesOfEntryInRange> searcher(pSource, lList, check);
    TypeContainerVisitor<MaNGOS::CreatureListSearcher<AllCreaturesOfEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(pSource->GetMap()));
}
