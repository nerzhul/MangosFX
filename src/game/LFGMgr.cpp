#include "LFGMgr.h"
#include "Player.h"
#include "WorldPacket.h"
#include "Policies/SingletonImp.h"
#include "Common.h"
#include "SharedDefines.h"
#include "Group.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Chat.h"

INSTANTIATE_SINGLETON_1(LFGMgr);

LFGMgr::LFGMgr()
{
}

LFGMgr::~LFGMgr()
{
	for (LfgRewardList::iterator it = m_RewardList.begin(); it != m_RewardList.end(); ++it)
        delete *it;
    m_RewardList.clear();

    for (LfgRewardList::iterator it = m_RewardDoneList.begin(); it != m_RewardDoneList.end(); ++it)
        delete *it;
    m_RewardDoneList.clear();

    for (LfgDungeonMap::iterator it = m_DungeonsMap.begin(); it != m_DungeonsMap.end(); ++it)
    {
        it->second->clear();
        delete it->second;
    }
    m_DungeonsMap.clear();
	m_LFGGroupList[0].clear();
	m_LFGGroupList[1].clear();
}

void LFGMgr::InitLFG()
{
	// Fill reward data
    LfgReward *reward;
    for (uint8 i = 0; i <= LFG_REWARD_DATA_SIZE; ++i)
    {
        reward = new LfgReward();
        reward->strangers = 0;
        reward->baseXP = RewardDungeonData[i][0];
        reward->baseMoney = RewardDungeonData[i][1];
        reward->variableMoney = 0;
        reward->variableXP = 0;
        reward->itemId = RewardDungeonData[i][2];
        reward->displayId = RewardDungeonData[i][3];
        reward->stackCount = RewardDungeonData[i][4];
        m_RewardList.push_back(reward);
    }

    for (uint8 i = 0; i < LFG_REWARD_DATA_SIZE; ++i)
    {
        reward = new LfgReward();
        reward->strangers = 0;
        reward->baseXP = RewardDungeonDoneData[i][0];
        reward->baseMoney = RewardDungeonDoneData[i][1];
        reward->variableMoney = 0;
        reward->variableXP = 0;
        reward->itemId = RewardDungeonDoneData[i][2];
        reward->displayId = RewardDungeonDoneData[i][3];
        reward->stackCount = RewardDungeonDoneData[i][4];
        m_RewardDoneList.push_back(reward);
    }

	m_LFGGroupList[0].clear();
	m_LFGGroupList[1].clear();

    // Initialize dungeonMap
    m_DungeonsMap[LFG_ALL_DUNGEONS] = GetAllDungeons();
    /*m_DungeonsMap[LFG_RANDOM_CLASSIC] = GetDungeonsByRandom(LFG_RANDOM_CLASSIC);
    m_DungeonsMap[LFG_RANDOM_BC_NORMAL] = GetDungeonsByRandom(LFG_RANDOM_BC_NORMAL);
    m_DungeonsMap[LFG_RANDOM_BC_HEROIC] = GetDungeonsByRandom(LFG_RANDOM_BC_HEROIC);
    m_DungeonsMap[LFG_RANDOM_LK_NORMAL] = GetDungeonsByRandom(LFG_RANDOM_LK_NORMAL);
    m_DungeonsMap[LFG_RANDOM_LK_HEROIC] = GetDungeonsByRandom(LFG_RANDOM_LK_HEROIC);*/

	middleTime = 600;
	Update_Timer = 1000;
}

void LFGMgr::SendLfgPartyInfo(Player *plr)
{
 	if (LfgLockStatusMap *lockMap = GetPartyLockStatusDungeons(plr, m_DungeonsMap[LFG_ALL_DUNGEONS]))
	{
		uint32 size = 0;
		for (LfgLockStatusMap::const_iterator it = lockMap->begin(); it != lockMap->end(); ++it)
			size += 8 + 4 + it->second->size() * (4 + 4);
		
		sLog.outDebug("SMSG_LFG_PARTY_INFO");
		WorldPacket data(SMSG_LFG_PARTY_INFO, 1 + size);
		BuildPartyLockDungeonBlock(data, lockMap);
		plr->GetSession()->SendPacket(&data);
	}
}

void LFGMgr::SendLfgPlayerInfo(Player *plr)
{
	uint32 rsize = 0;
    uint32 lsize = 0;
    LfgDungeonSet *randomlist = GetRandomDungeons(plr->getLevel(), plr->GetSession()->Expansion());
    LfgLockStatusSet *lockSet = GetPlayerLockStatusDungeons(plr, m_DungeonsMap[LFG_ALL_DUNGEONS]);
    if (randomlist)
        rsize = randomlist->size();
    if (lockSet)
        lsize = lockSet->size();

    sLog.outDebug("SMSG_LFG_PLAYER_INFO");
    WorldPacket data(SMSG_LFG_PLAYER_INFO, 1 + rsize * (4 + 1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4) + 4 + lsize * (4 + 4));
    if (!randomlist)
    {
        data << uint8(0);
    }
    else
    {
        data << uint8(randomlist->size());                  // Random Dungeon count
        for (LfgDungeonSet::iterator it = randomlist->begin(); it != randomlist->end(); ++it)
        {
            data << uint32(*it);                            // Entry
            BuildRewardBlock(data, *it, plr);
        }
        randomlist->clear();
        delete randomlist;
    }
    BuildPlayerLockDungeonBlock(data, lockSet);
    plr->GetSession()->SendPacket(&data);
}

void LFGMgr::BuildAvailableRandomDungeonList(WorldPacket &data, Player *plr)
{
}

void LFGMgr::BuildPartyLockDungeonBlock(WorldPacket &data, LfgLockStatusMap *lockMap)
{
	assert(lockMap);

    data << uint8(lockMap->size());

    LfgLockStatusSet *lockSet;
    uint64 guid;
    for (LfgLockStatusMap::const_iterator it = lockMap->begin(); it != lockMap->end(); ++it)
    {
        guid = it->first;
        lockSet = it->second;
        if (!lockSet)
            continue;

        data << uint64(guid);                               // Player guid
        BuildPlayerLockDungeonBlock(data, lockSet);
    }
    lockMap->clear();
    delete lockMap;
}

void LFGMgr::BuildPlayerLockDungeonBlock(WorldPacket &data, LfgLockStatusSet *lockSet)
{
	assert(lockSet);
    data << uint32(lockSet->size());                        // Size of lock dungeons
    for (LfgLockStatusSet::iterator it = lockSet->begin(); it != lockSet->end(); ++it)
    {
        data << uint32((*it)->dungeon);                     // Dungeon entry + type
        data << uint32((*it)->lockstatus);                  // Lock status
        delete (*it);
    }
    lockSet->clear();
    delete lockSet;
}

void LFGMgr::BuildRewardBlock(WorldPacket &data, uint32 dungeon, Player *plr)
{
	bool done = plr->m_lookingForGroup.isDungeonDone(dungeon);
    LfgReward *reward = GetRandomDungeonReward(dungeon, done, plr->getLevel());

    if (!reward)
        return;

    data << uint8(done);
    if (data.GetOpcode() == SMSG_LFG_PLAYER_REWARD)
        data << uint32(reward->strangers);
    data << uint32(reward->baseMoney);
    data << uint32(reward->baseXP);
    data << uint32(reward->variableMoney);
    data << uint32(reward->variableXP);
    data << uint8(reward->itemId != 0);
    if (reward->itemId)
    {
        data << uint32(reward->itemId);
        data << uint32(reward->displayId);
        data << uint32(reward->stackCount);
    }
}

LfgDungeonSet* LFGMgr::GetAllDungeons()
{
	LfgDungeonSet *dungeons = new LfgDungeonSet();
    LFGDungeonEntry const *dungeon;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon || dungeon->type == LFG_TYPE_ZONE)
            continue;
        dungeons->insert(dungeon->ID);
    }
    if (!dungeons->size())
    {
        delete dungeons;
        return NULL;
    }
    else
        return dungeons;
}

LfgDungeonSet* LFGMgr::GetDungeonsByRandom(uint32 randomdungeon)
{
	LFGDungeonEntry const *dungeon = sLFGDungeonStore.LookupEntry(randomdungeon);
    if (!dungeon)
        return NULL;

    uint32 grouptype = dungeon->grouptype;
    LfgDungeonSet *random = new LfgDungeonSet();
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        dungeon = sLFGDungeonStore.LookupEntry(i);
        if (!dungeon || dungeon->type == LFG_TYPE_RANDOM || dungeon->grouptype != grouptype)
            continue;
        random->insert(dungeon->ID);
    }
    if (!random->size())
    {
        delete random;
        return NULL;
    }
    else
        return random;
}

LfgLockStatusMap* LFGMgr::GetPartyLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons)
{
	assert(plr);
    assert(dungeons);
    Group *grp = plr->GetGroup();
    if (!grp)
        return NULL;

    Player *plrg;
    LfgLockStatusMap *dungeonMap = new LfgLockStatusMap();
    for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        plrg = itr->getSource();
        if (!plrg || plrg == plr)
            continue;
        (*dungeonMap)[plrg->GetGUID()] = GetPlayerLockStatusDungeons(plrg, dungeons);
    }
    return dungeonMap;
}

LfgLockStatusSet* LFGMgr::GetPlayerLockStatusDungeons(Player *plr, LfgDungeonSet *dungeons)
{
	LfgLockStatusSet *list = new LfgLockStatusSet();
    LfgLockStatus *lockstatus = NULL;
    LFGDungeonEntry const *dungeon;
    LfgLockStatusType locktype;
    uint8 level = plr->getLevel();
    uint8 expansion = plr->GetSession()->Expansion();

    for (LfgDungeonSet::const_iterator it = dungeons->begin(); it != dungeons->end(); ++it)
    {
        dungeon = sLFGDungeonStore.LookupEntry(*it);
        assert(dungeon); // Will never happen - We provide a list from sLFGDungeonStore

        locktype = LFG_LOCKSTATUS_OK;
        if (dungeon->expansion > expansion)
            locktype = LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION;
        else if (dungeon->minlevel > level)
            locktype = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (dungeon->maxlevel < level)
            locktype = LFG_LOCKSTATUS_TOO_HIGH_LEVEL;
        /* TODO - Use these types when needed...
        else if ()
            locktype = LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE;
        else if ()
            locktype = LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE;
        else if () // Locked due to WG, closed by GM, done daily, etc
            locktype = LFG_LOCKSTATUS_RAID_LOCKED;
        else if ()
            locktype = LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL;
        else if ()
            locktype = LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL;
        else if () // Need list of instances and needed quest to enter
            locktype = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED;
        else if () // Need list of instances and needed key to enter
            locktype = LFG_LOCKSTATUS_MISSING_ITEM;
        else if () // Need list of instances and needed season to open
            locktype = LFG_LOCKSTATUS_NOT_IN_SEASON;
        */

        if (locktype != LFG_LOCKSTATUS_OK)
        {
            lockstatus = new LfgLockStatus();
            lockstatus->dungeon = dungeon->Entry();
            lockstatus->lockstatus = locktype;
            list->insert(lockstatus);
        }
    }
    return list;
}

LfgReward* LFGMgr::GetRandomDungeonReward(uint32 dungeon, bool done, uint8 level)
{
	uint8 index = 0;
    switch((dungeon & 0x00FFFFFF))                          // Get dungeon id from dungeon entry
    {
    case LFG_RANDOM_CLASSIC:
        if (level < 15)
            index = LFG_REWARD_LEVEL0;
        else if (level < 24)
            index = LFG_REWARD_LEVEL1;
        else if (level < 35)
            index = LFG_REWARD_LEVEL2;
        else if (level < 46)
            index = LFG_REWARD_LEVEL3;
        else if (level < 56)
            index = LFG_REWARD_LEVEL4;
        else
            index = LFG_REWARD_LEVEL5;
        break;
    case LFG_RANDOM_BC_NORMAL:
            index = LFG_REWARD_BC_NORMAL;
        break;
    case LFG_RANDOM_BC_HEROIC:
            index = LFG_REWARD_BC_HEROIC;
        break;
    case LFG_RANDOM_LK_NORMAL:
        index = level == 80 ? LFG_REWARD_LK_NORMAL80 : LFG_REWARD_LK_NORMAL;
        break;
    case LFG_RANDOM_LK_HEROIC:
        index = LFG_REWARD_LK_HEROIC;
        break;
    default:                                                // This should never happen!
        done = false;
        index = LFG_REWARD_LEVEL0;
        sLog.outError("LFGMgr::GetRandomDungeonReward: Dungeon %u is not random dungeon!", dungeon);
        break;
    }
    return done ? m_RewardDoneList.at(index) : m_RewardList.at(index);
}

LfgDungeonSet* LFGMgr::GetRandomDungeons(uint8 level, uint8 expansion)
{
	LfgDungeonSet *list = new LfgDungeonSet();
    LFGDungeonEntry const *dungeon;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        dungeon = sLFGDungeonStore.LookupEntry(i);
        if (dungeon && dungeon->expansion <= expansion && dungeon->type == LFG_TYPE_RANDOM &&
            dungeon->minlevel <= level && level <= dungeon->maxlevel)
            list->insert(dungeon->Entry());
    }
    return list;
}

void LFGMgr::RemovePlayerFromRandomQueue(Player* plr)
{
	LFGGroup* plrGrp = plr->m_lookingForGroup.group;
	if(!plrGrp)
		return;

	plrGrp->RemovePlayer(plr->GetGUID());
	plrGrp->ResetAnswers();
	plr->m_lookingForGroup.group = NULL;
	plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
}

void LFGMgr::AddPlayerToRandomQueue(Player* plr, LFG_Role role)
{
	plr->m_lookingForGroup.roles = role;
	plr->m_lookingForGroup.waited = 0;

	LFGGroup* grp = SearchGroup(role,plr->GetTeam() == ALLIANCE ? 0 : 1);
	LFG_Role TryiedRole = grp->TryToGiveRole(role);
	if(grp->SetRole(plr->GetGUID(),TryiedRole))
	{
		plr->m_lookingForGroup.group = grp;
		plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_ADDED_TO_QUEUE);
	}
	else
	{
		plr->m_lookingForGroup.roles = 0x00;
		plr->GetSession()->SendLfgUpdatePlayer(LFG_UPDATETYPE_ROLECHECK_FAILED);
	}

	if(grp->IsFull())
		grp->SendLfgProposalUpdate();	
}

LFGGroup* LFGMgr::SearchGroup(LFG_Role role, uint8 team)
{
	LFGGroup* grp = NULL;
	if(m_LFGGroupList[team].empty())
		grp = new LFGGroup();
	else
	{
		for(std::vector<LFGGroup*>::iterator itr = m_LFGGroupList[team].begin(); itr != m_LFGGroupList[team].end(); ++itr)
		{
			if(LFGGroup* tmpGrp = (*itr))
				if(tmpGrp->TryToGiveRole(role) != ROLE_NONE)
				{
					grp = tmpGrp;
					break;
				}
		}
		if(!grp)
			grp = new LFGGroup();
	}

	m_LFGGroupList[team].push_back(grp);
	return grp;
}

uint32 LFGMgr::GenerateRandomDungeon()
{
	LfgDungeonSet* dungeonList = GetRandomDungeons(80,2);
	uint32 _rand = urand(0,dungeonList->size());
	uint32 pos = 0;
	for(LfgDungeonSet::const_iterator itr = dungeonList->begin(); itr != dungeonList->end(); ++itr)
	{
		if(pos == _rand)
			return *itr;
		pos++;
	}
	return 0;
}

void LFGMgr::Update(uint32 diff)
{
	if(Update_Timer <= diff)
	{
		for(uint8 i=0;i<BG_TEAMS_COUNT;i++)
		{
			if(!m_LFGGroupList[i].empty())
			{
				for(std::vector<LFGGroup*>::iterator itr = m_LFGGroupList[i].begin(); itr != m_LFGGroupList[i].end(); ++itr)
				{
					if(LFGGroup* tmpGrp = (*itr))
					{
						if(!tmpGrp->IsFull())
						{
							if(Player* plr = tmpGrp->GetPlayerByRole(ROLE_TANK))
							{
								plr->m_lookingForGroup.waited += 15;
								if(plr->m_lookingForGroup.waited > 7200)
									plr->m_lookingForGroup.waited = 7200;
								SendLfgQueueStatusUpdate(plr,tmpGrp); // not sure
							}

							if(Player* plr = tmpGrp->GetPlayerByRole(ROLE_HEAL))
							{
								plr->m_lookingForGroup.waited += 15;
								if(plr->m_lookingForGroup.waited > 7200)
									plr->m_lookingForGroup.waited = 7200;
								SendLfgQueueStatusUpdate(plr,tmpGrp); // not sure
							}

							if(Player* plr = tmpGrp->GetPlayerByRole(ROLE_DPS))
							{
								plr->m_lookingForGroup.waited += 15;
								if(plr->m_lookingForGroup.waited > 7200)
									plr->m_lookingForGroup.waited = 7200;
								SendLfgQueueStatusUpdate(plr,tmpGrp); // not sure
							}

							if(Player* plr = tmpGrp->GetPlayerByRole(ROLE_DPS,1))
							{
								plr->m_lookingForGroup.waited += 15;
								if(plr->m_lookingForGroup.waited > 7200)
									plr->m_lookingForGroup.waited = 7200;
								SendLfgQueueStatusUpdate(plr,tmpGrp); // not sure
							}

							if(Player* plr = tmpGrp->GetPlayerByRole(ROLE_DPS,2))
							{
								plr->m_lookingForGroup.waited += 15;
								if(plr->m_lookingForGroup.waited > 7200)
									plr->m_lookingForGroup.waited = 7200;
								SendLfgQueueStatusUpdate(plr,tmpGrp); // not sure
							}
						}
						else
							tmpGrp->SendLfgProposalUpdate();
								
					}
				}
			}
		}
		Update_Timer = 15000;
	}
	else
		Update_Timer -= diff;
}

void LFGMgr::SendLfgQueueStatusUpdate(Player *plr, LFGGroup* grp)
{
	if(!plr || !grp)
		return;

	WorldPacket data(SMSG_LFG_QUEUE_STATUS,4+4+4+4+4+4+1+1+1+4);
	data << uint32(1);
	data << uint32(middleTime); // temps d'attente moyen
	data << uint32(plr->m_lookingForGroup.waited); // temps d'attente
	data << uint32(0); // tanks wait
	data << uint32(0); // heal wait
	data << uint32(0); // dps wait
	data << uint8(grp->GetTankNb() ? 0 : 1); // needed tanks
	data << uint8(grp->GetHealNb() ? 0 : 1); // needed heals
	data << uint8(3 - grp->GetDpsNb()); // needed dps
	data << uint32(plr->m_lookingForGroup.waited); // repet temps d'attente
	if(plr->GetSession())
		plr->GetSession()->SendPacket(&data);
}

void LFGMgr::TeleportPlayerToInstance(Player* plr)
{
	error_log("TeleportPlayerToInstance non implante");
	/*
	if player in instance => teleport to instance
	else teleport to last save outdoor
	*/
}

void LFGMgr::SendLfgRoleCheckResult(Player* plr, bool accept)
{
	sLog.outDebug("SMSG_LFG_QUEUE_STATUS");
	WorldPacket data(SMSG_LFG_ROLE_CHOSEN,8+1+4);
	data << uint64(plr->GetGUID());
	data << uint8(accept);
	data << plr->m_lookingForGroup.roles;
	plr->GetSession()->SendPacket(&data);
	// Party send packet
}

void LFGGroup::SendLfgProposalUpdate()
{
	for(uint8 i=0;i<MAX_GROUP_SIZE;i++)
	{
		Player* plr = GetPlayerBySlot(i);

		if(!plr)
			continue;

		WorldPacket data(SMSG_LFG_PROPOSAL_UPDATE,4+1+4+4+1+1+(4+1+1+1+1+1)*5);
		data << uint32(LFG_RANDOM_LK_HEROIC); // dungeon type
		data << uint8(1); // state
		data << uint32(1); // group id
		data << uint32(0); // boss killed
		data << uint8(0); // silent

		data << uint8(5); // loop size

		data << uint32(ROLE_TANK); // role
		if(plr == GetPlayerBySlot(0))
			data << uint8(1); // if its self
		else
			data << uint8(0);
		data << uint8(0); // if in dungeon
		data << uint8(0); // same group
		data << uint8(groupAnswers[0]); // answer (real)
		data << uint8(1); // accept answer

		data << uint32(ROLE_HEAL); // role
		if(plr == GetPlayerBySlot(1))
			data << uint8(1); // if its self
		else
			data << uint8(0);
		data << uint8(0); // if in dungeon
		data << uint8(0); // same group
		data << uint8(groupAnswers[1]); // answer (real)
		data << uint8(groupAnswers[1]); // accept answer

		data << uint32(ROLE_DPS); // role
		if(plr == GetPlayerBySlot(2))
			data << uint8(1); // if its self
		else
			data << uint8(0);
		data << uint8(0); // if in dungeon
		data << uint8(0); // same group
		data << uint8(groupAnswers[2]); // answer (real)
		data << uint8(groupAnswers[2]); // accept answer

		data << uint32(ROLE_DPS); // role
		if(plr == GetPlayerBySlot(3))
			data << uint8(1); // if its self
		else
			data << uint8(0);
		data << uint8(0); // if in dungeon
		data << uint8(0); // same group
		data << uint8(groupAnswers[3]); // answer (real)
		data << uint8(groupAnswers[3]); // accept answer

		data << uint32(ROLE_DPS); // role
		if(plr == GetPlayerBySlot(4))
			data << uint8(1); // if its self
		else
			data << uint8(0);
		data << uint8(0); // if in dungeon
		data << uint8(0); // same group
		data << uint8(groupAnswers[4]); // answer (real)
		data << uint8(groupAnswers[4]); // accept answer

		plr->GetSession()->SendPacket(&data);
	}

	if(AllAnswer())
	{
		if(AllAccept())
		{
			Group* grp = new Group;
			bool masterIsSet = false;
			uint8 success = 0;
			for(uint8 i=0;i<MAX_GROUP_SIZE;i++)
			{
				Player* plr = GetPlayerBySlot(i);

				if(!plr)
					continue;

				if(Group* plrGrp = plr->GetGroup())
					plr->RemoveFromGroup(plrGrp,plr->GetGUID());

				if(!grp->GetId())
				{
					if(!grp->Create(plr->GetGUID(),plr->GetName()))
					{
						delete grp;
						grp = new Group;
						continue;
					}	
				}
				else
				{
					if(!grp->AddMember(plr->GetGUID(),plr->GetName()))
					{
						RemovePlayer(plr->GetGUID());
						continue;
					}
				}

				uint8 roleToSet = GetRoleBySlot(i);
				if(!masterIsSet && (plr->m_lookingForGroup.roles & ROLE_MASTER))
				{
					roleToSet &= ROLE_MASTER;
					masterIsSet = true;
				}

				plr->m_lookingForGroup.roles = roleToSet;
				success++;
			}

			if(success == MAX_GROUP_SIZE)
			{
				if(!masterIsSet)
					if(Player* Tankptr = ObjectAccessor::FindPlayer(Tank))
						Tankptr->m_lookingForGroup.roles &= ROLE_MASTER;

				grp->SetRandomInstanceGroup();
			}
		}
		else
		{
			for(uint8 i=0;i<MAX_GROUP_SIZE;i++)
			{
				Player* plr = GetPlayerBySlot(i);

				if(!plr)
					continue;

				ChatHandler(plr).SendSysMessage("On dirait que qqun n'a pas accepte");
			}
		}
	}
}

LFGGroup::LFGGroup()
{
	Tank = 0;
	Heal = 0;
	for(uint8 i=0;i<MAX_DPS;i++)
		Dps[i] = 0;
	ResetAnswers();
}

LFGGroup::~LFGGroup()
{
}

void LFGGroup::SetDps(uint64 guid)
{
	for(uint8 i=0;i<MAX_DPS;i++)
		if(Dps[i] == 0)
		{
			Dps[i] = guid;
			return;
		}
}

LFG_Role LFGGroup::TryToGiveRole(LFG_Role role)
{
	if((role & ROLE_TANK) && Tank == 0)
		return ROLE_TANK;

	if((role & ROLE_HEAL) && Heal == 0)
		return ROLE_HEAL;

	if((role & ROLE_DPS))
		for(uint8 i=0;i<MAX_DPS;i++)
			if(Dps[i] == 0)
				return ROLE_DPS;

	return ROLE_NONE;
}

bool LFGGroup::SetRole(uint64 guid, LFG_Role role)
{
	switch(role)
	{
		case ROLE_TANK:
			SetTank(guid);
			break;
		case ROLE_HEAL:
			SetHeal(guid);
			break;
		case ROLE_DPS:
			SetDps(guid);
			break;
		default:
			return false;
	}
	return true;
}

Player* LFGGroup::GetPlayerByRole(LFG_Role role, uint8 place)
{
	if((role == ROLE_HEAL || role == ROLE_TANK) && place > 0)
		return NULL;

	if(place >= MAX_DPS)
		return NULL;

	switch(role)
	{
		case ROLE_TANK:
			return ObjectAccessor::FindPlayer(Tank);
		case ROLE_HEAL:
			return ObjectAccessor::FindPlayer(Heal);
		case ROLE_DPS:
			return ObjectAccessor::FindPlayer(Dps[place]);
	}

	return NULL;
}

uint8 LFGGroup::GetDpsNb()
{
	uint8 nb = 0;
	for(uint8 i=0;i<MAX_DPS;i++)
		if(Dps[i] != 0)
			nb++;

	return nb;
}

void LFGGroup::RemovePlayer(uint64 guid)
{
	if(Tank == guid)
	{
		Tank = 0;
		return;
	}

	if(Heal == guid)
	{
		Heal = 0;
		return;
	}

	for(uint8 i=0;i<MAX_DPS;i++)
		if(Dps[i] == guid)
		{
			Dps[i] = 0;
			return;
		}
}

LFG_Role LFGGroup::GetRoleBySlot(uint8 slot)
{
	if(slot >= MAX_GROUP_SIZE)
		return ROLE_NONE;

	switch(slot)
	{
		case 0:
			return ROLE_TANK;
		case 1:
			return ROLE_HEAL;
		case 2:
		case 3:
		case 4:
			return ROLE_DPS;
	}

	return ROLE_NONE;
}

Player* LFGGroup::GetPlayerBySlot(uint8 slot)
{
	if(slot >= MAX_GROUP_SIZE)
		return NULL;

	switch(slot)
	{
		case 0:
			return ObjectAccessor::FindPlayer(Tank);
		case 1:
			return ObjectAccessor::FindPlayer(Heal);
		case 2:
			return ObjectAccessor::FindPlayer(Dps[0]);
		case 3:
			return ObjectAccessor::FindPlayer(Dps[1]);
		case 4:
			return ObjectAccessor::FindPlayer(Dps[2]);
	}

	return NULL;
}

bool LFGGroup::IsFull()
{
	if(Tank == 0 || Heal == 0)
		return false;

	for(uint8 i=0;i<MAX_DPS;i++)
		if(Dps[i] == 0)
			return false;

	return true;
}

void LFGGroup::ResetAnswers()
{
	for(uint8 i=0;i<MAX_GROUP_SIZE;i++)
		groupAnswers[i] = LFG_ANSW_NONE;
}

bool LFGGroup::AllAccept()
{
	uint8 accept = 0;
	for(uint8 j=0;j<MAX_GROUP_SIZE;j++)
		if(groupAnswers[j] == LFG_ANSW_ACCEPT)
			accept++;

	if(accept == MAX_GROUP_SIZE)
		return true;
	else
		return false;
}

bool LFGGroup::AllAnswer()
{
	for(uint8 j=0;j<MAX_GROUP_SIZE;j++)
		if(groupAnswers[j] == LFG_ANSW_NONE)
			return false;

	return true;
}

uint8 LFGGroup::GetSlotForPlayer(Player* plr)
{
	if(!plr)
		return 5;

	if(Tank == plr->GetGUID())
		return 0;

	if(Heal == plr->GetGUID())
		return 1;

	for(uint8 i=0;i<MAX_DPS;i++)
		if(Dps[i] == plr->GetGUID())
			return (2+i);

	return 5;
}

void LFGGroup::PlayerAccept(Player* plr, bool accept)
{
	uint8 slot = GetSlotForPlayer(plr);
	if(slot == MAX_GROUP_SIZE)
		return;
	
	groupAnswers[slot] = accept ? LFG_ANSW_ACCEPT : LFG_ANSW_DENY;
}