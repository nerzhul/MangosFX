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

//Optional uiWithRestoreTime. If not defined, autoCloseTime will be used (if not 0 by default in *_template)
void InstanceData::DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime, bool bUseAlternativeState)
{
    if (!uiGuid)
        return;

    GameObject* pGo = instance->GetGameObject(uiGuid);

    if (pGo)
    {
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR || pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (pGo->getLootState() == GO_READY)
                pGo->UseDoorOrButton(uiWithRestoreTime,bUseAlternativeState);
            else if (pGo->getLootState() == GO_ACTIVATED)
                pGo->ResetDoorOrButton();
        }
        else
            error_log("LibDevFS: Script call DoUseDoorOrButton, but gameobject entry %u is type %u.",pGo->GetEntry(),pGo->GetGoType());
    }
}

void InstanceData::DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn)
{
    if (GameObject* pGo = instance->GetGameObject(uiGuid))
    {
        //not expect any of these should ever be handled
        if (pGo->GetGoType()==GAMEOBJECT_TYPE_FISHINGNODE || pGo->GetGoType()==GAMEOBJECT_TYPE_DOOR ||
            pGo->GetGoType()==GAMEOBJECT_TYPE_BUTTON || pGo->GetGoType()==GAMEOBJECT_TYPE_TRAP)
            return;

        if (pGo->isSpawned())
            return;

        pGo->SetRespawnTime(uiTimeToDespawn);
    }
}

void InstanceData::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (!lPlayers.isEmpty())
    {
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
                pPlayer->SendUpdateWorldState(uiStateId, uiStateData);
        }
    }
    else
        debug_log("LibDevFS: DoUpdateWorldState attempt send data but no players in map.");
}

void InstanceData::DoSpeak(Unit* pwho, uint32 soundid, std::string text, uint8 type)
{
	if(!pwho)
		return;

	if(soundid > 0 && GetSoundEntriesStore()->LookupEntry(soundid))
		pwho->PlayDirectSound(soundid);

	switch(type)
    {
        case CHAT_TYPE_SAY:
			pwho->MonsterSay(text.c_str(), 0, pwho ? pwho->GetGUID() : 0);
            break;
        case CHAT_TYPE_YELL:
            pwho->MonsterYell(text.c_str(), 0, pwho ? pwho->GetGUID() : 0);
            break;
        case CHAT_TYPE_TEXT_EMOTE:
            pwho->MonsterTextEmote(text.c_str(), pwho ? pwho->GetGUID() : 0);
            break;
        case CHAT_TYPE_BOSS_EMOTE:
            pwho->MonsterTextEmote(text.c_str(), pwho ? pwho->GetGUID() : 0, true);
            break;
	}
}

void InstanceData::AutoFreeze(Creature* cr)
{
	if(!cr)
		return;
	cr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
	cr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
	cr->CastSpell(cr,66830,false);
}

Player* InstanceData::GetClosestPlayer(Unit* u, float maxRange)
{
	float minDist = 0.0f;
	Player* nearestPlayer = NULL;
	Map::PlayerList const& lPlayers = u->GetMap()->GetPlayers();
	for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
		if(Player* plr = itr->getSource())
			if(plr != u && (!nearestPlayer || minDist > plr->GetDistance2d(u)))
			{
				nearestPlayer = plr;
				minDist = plr->GetDistance2d(u);
			}

	return nearestPlayer;
}