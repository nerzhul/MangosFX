/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"
#include "DBCStores.h"
#include "AchievementMgr.h"

//Optional uiWithRestoreTime. If not defined, autoCloseTime will be used (if not 0 by default in *_template)
void ScriptedInstance::DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime, bool bUseAlternativeState)
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
            error_log("SD2: Script call DoUseDoorOrButton, but gameobject entry %u is type %u.",pGo->GetEntry(),pGo->GetGoType());
    }
}

void ScriptedInstance::OpenDoor(uint64 guid)
{
	GameObject* pGo = instance->GetGameObject(guid);
	if(pGo)
	{
		if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR || pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
		{
			if (pGo->getLootState() == GO_READY)
                pGo->UseDoorOrButton();
		}
	}
}

void ScriptedInstance::CloseDoor(uint64 guid)
{
	GameObject* pGo = instance->GetGameObject(guid);
	if(pGo)
	{
		if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR || pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
		{
			if (pGo->getLootState() == GO_ACTIVATED)
                pGo->ResetDoorOrButton();
		}
	}
}

void ScriptedInstance::DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn)
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

void ScriptedInstance::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
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
        debug_log("SD2: DoUpdateWorldState attempt send data but no players in map.");
}

void ScriptedInstance::DoSpeak(Unit* pwho, uint32 soundid, std::string text, uint8 type)
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

void ScriptedInstance::AutoFreeze(Creature* cr)
{
	if(!cr)
		return;
	cr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
	cr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
	cr->CastSpell(cr,66830,false);
}

Player* ScriptedInstance::GetClosestPlayer(Unit* u, float maxRange)
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