#include "LFGMgr.h"
#include "Player.h"
#include "WorldPacket.h"
#include "Policies/SingletonImp.h"
#include "Common.h"
#include "SharedDefines.h"
#include "Group.h"
#include "ObjectMgr.h"

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
	m_TankSet.clear();
	m_HealSet.clear();
	m_DpsSet.clear();
	m_MasterSet.clear();
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
    // Initialize dungeonMap
    m_DungeonsMap[LFG_ALL_DUNGEONS] = GetAllDungeons();
    /*m_DungeonsMap[LFG_RANDOM_CLASSIC] = GetDungeonsByRandom(LFG_RANDOM_CLASSIC);
    m_DungeonsMap[LFG_RANDOM_BC_NORMAL] = GetDungeonsByRandom(LFG_RANDOM_BC_NORMAL);
    m_DungeonsMap[LFG_RANDOM_BC_HEROIC] = GetDungeonsByRandom(LFG_RANDOM_BC_HEROIC);
    m_DungeonsMap[LFG_RANDOM_LK_NORMAL] = GetDungeonsByRandom(LFG_RANDOM_LK_NORMAL);
    m_DungeonsMap[LFG_RANDOM_LK_HEROIC] = GetDungeonsByRandom(LFG_RANDOM_LK_HEROIC);*/

	middleTime = 600;  
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
	m_TankSet.erase(plr);
	m_DpsSet.erase(plr);
	m_HealSet.erase(plr);
	m_MasterSet.erase(plr);
}

void LFGMgr::AddPlayerToRandomQueue(Player* plr)
{
	int8 plrRole = plr->m_lookingForGroup.roles;
	plr->m_lookingForGroup.waited = 0;
	if(plrRole & ROLE_MASTER) 
		m_MasterSet.insert(plr);
	if(plrRole & ROLE_TANK) 
		m_TankSet.insert(plr);
	if(plrRole & ROLE_HEAL) 
		m_HealSet.insert(plr);
	if(plrRole & ROLE_DPS) 
		m_DpsSet.insert(plr);
	TryToFormGroup(plr);
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

bool LFGMgr::TryToFormGroup(Player* plr)
{
	Player* Tank,*Heal,*dps[MAX_DPS];

	uint32 plrRole = plr->m_lookingForGroup.roles;
	if(plrRole & ROLE_TANK) 
		Tank = plr;
	else if(plrRole & ROLE_HEAL) 
		Heal = plr;
	else 
		dps[0] = plr;

	if(!Tank && !m_TankSet.empty())
	{
		PlayerSet::const_iterator itr = m_TankSet.begin();
		Tank = *itr;
	}

	if(!Heal && !m_HealSet.empty())
	{
		PlayerSet::const_iterator itr = m_HealSet.begin();
		Heal = *itr;
	}

	if(!m_DpsSet.empty())
	{
		uint8 slot = 0;
		if(dps[0])
			slot = 1;
		PlayerSet::const_iterator itr = m_DpsSet.begin();
		while(itr != m_DpsSet.end() && slot < MAX_DPS)
		{
			dps[slot] = *itr;
			slot++;
			++itr;
		}
	}

	if(Tank && Heal && dps[0] && dps[1] && dps[2])
	{
		uint32 dungeon = GenerateRandomDungeon();
		if(!dungeon)
			return false;
		// TODO: group ready
	}

	return true;
}

void LFGMgr::Update(uint32 diff)
{
	for(PlayerSet::iterator itr = m_DpsSet.begin(); itr != m_DpsSet.end(); ++itr)
	{
		(*itr)->m_lookingForGroup.waited += diff;
		if((*itr)->m_lookingForGroup.waited >= 3000000)
			(*itr)->m_lookingForGroup.waited = 3000000;
		SendLfgQueueStatusUpdate(*itr);
	}

	for(PlayerSet::iterator itr = m_TankSet.begin();itr != m_TankSet.end();++itr)
	{
		(*itr)->m_lookingForGroup.waited += diff;
		if((*itr)->m_lookingForGroup.waited >= 3000000)
			(*itr)->m_lookingForGroup.waited = 3000000;
		SendLfgQueueStatusUpdate(*itr);
	}

	for(PlayerSet::iterator itr = m_HealSet.begin();itr != m_HealSet.end();++itr)
	{
		(*itr)->m_lookingForGroup.waited += diff;
		if((*itr)->m_lookingForGroup.waited >= 9000000)
			(*itr)->m_lookingForGroup.waited = 9000000;
		SendLfgQueueStatusUpdate(*itr);
	}
}

void LFGMgr::SendLfgQueueStatusUpdate(Player *plr)
{
	WorldPacket data(SMSG_LFG_QUEUE_STATUS,4+4+4+4+4+4+1+1+1+4);
	data << uint32(1);
	data << uint32(middleTime); // temps d'attente moyen
	data << uint32(plr->m_lookingForGroup.waited); // temps d'attente
	data << uint32(0);
	data << uint32(0);
	data << uint32(0);
	data << uint8(0); // needed tanks
	data << uint8(0); // needed heals
	data << uint8(0); // needed dps
	data << uint32(plr->m_lookingForGroup.waited); // repet temps d'attente
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

void LFGMgr::SendLfgProposalUpdate(Player* plr)
{
	WorldPacket data(SMSG_LFG_PROPOSAL_UPDATE,4+1+4+4+1+1+(4+1+1+1+1+1));
	data << uint32(LFG_RANDOM_LK_HEROIC); // dungeon type
	data << uint8(1); // state
	data << uint32(1); //
	data << uint32(2);
	data << uint8(0);
	data << uint8(1);
	for(uint8 i=0;i<1;i++)
	{
		data << uint32(ROLE_TANK); // role
		data << uint8(0); // if its self
		data << uint8(0); // if in dungeon
		data << uint8(0);
		data << uint8(0);
		data << uint8(0);
	}
	data << uint32(ROLE_DPS);
	data << uint8(1);
	data << uint8(0);
	data << uint8(0);
	data << uint8(0);
	data << uint8(0);
	data << uint32(ROLE_DPS);
	data << uint8(0);
	data << uint8(1);
	data << uint8(0);
	data << uint8(0);
	data << uint8(0);
	data << uint32(ROLE_DPS);
	data << uint8(0);
	data << uint8(0);
	data << uint8(1);
	data << uint8(0);
	data << uint8(0);
	data << uint32(ROLE_HEAL);
	data << uint8(0);
	data << uint8(0);
	data << uint8(0);
	data << uint8(1);
	data << uint8(0);
	plr->GetSession()->SendPacket(&data);
}