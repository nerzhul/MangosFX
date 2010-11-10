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

#include "InstanceData.h"
#include "Database/DatabaseEnv.h"
#include "Map.h"
#include "Player.h"
#include "GameObject.h"
#include "DBCStores.h"
#include "Log.h"

void InstanceData::SaveToDB()
{
    if(!Save()) return;
    std::string data = Save();
    CharacterDatabase.escape_string(data);
    CharacterDatabase.PExecute("UPDATE instance SET data = '%s' WHERE id = '%d'", data.c_str(), instance->GetInstanceId());
}

bool InstanceData::CheckAchievementCriteriaMeet( uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/ /*= NULL*/, uint32 /*miscvalue1*/ /*= 0*/ )
{
    /*sLog.outError("Achievement system call InstanceData::CheckAchievementCriteriaMeet but instance script for map %u not have implementation for achievement criteria %u",
        instance->GetId(),criteria_id);*/
    return false;
}

bool InstanceData::CheckConditionCriteriaMeet(Player const* /*source*/, uint32 map_id, uint32 instance_condition_id)
{
    /*sLog.outError("Condition system call InstanceData::CheckConditionCriteriaMeet but instance script for map %u not have implementation for player condition criteria with internal id %u for map %u",
        instance->GetId(), instance_condition_id, map_id);*/
    return false;
}

void InstanceData::CompleteAchievementForGroup(uint32 AchId)
{
	AchievementEntry const* pAE = GetAchievementStore()->LookupEntry(AchId);
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!pAE)
    {
        sLog.outError("LibDevFS: DoCompleteAchievement called for not existing achievement %u", AchId);
        return;
    }

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player *pPlayer = i->getSource())
				pPlayer->GetAchievementMgr().DoCompleteAchivement(pAE);
}

void InstanceData::CompleteAchievementForPlayer(Player* plr, uint32 AchId)
{
	if(!plr)
		return;

	AchievementEntry const* pAE = GetAchievementStore()->LookupEntry(AchId);
	if (!pAE)
    {
        sLog.outError("LibDevFS: DoCompleteAchievement called for not existing achievement %u", AchId);
        return;
    }
	
	plr->GetAchievementMgr().DoCompleteAchivement(pAE);
}

void InstanceData::OpenDoor(uint64 guid)
{
	if(GameObject* pGo = instance->GetGameObject(guid))
	{
		if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR || pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
		{
			if (pGo->getLootState() == GO_READY)
                pGo->UseDoorOrButton();
		}
	}
}

void InstanceData::CloseDoor(uint64 guid)
{
	if(GameObject* pGo = instance->GetGameObject(guid))
	{
		if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR || pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
		{
			if (pGo->getLootState() == GO_ACTIVATED)
                pGo->ResetDoorOrButton();
		}
	}
}