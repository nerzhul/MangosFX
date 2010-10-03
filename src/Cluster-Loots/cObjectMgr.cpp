#include "cObjectMgr.h"
#include <Database/SQLStorage.h>
#include <Database/SQLStorageImpl.h>
#include <Common.h>
#include <ObjectMgr.h>
#include <DBCStructure.h>
#include <DBCStores.h>
#include <SharedDefines.h>
#include <Policies/SingletonImp.h>

INSTANTIATE_SINGLETON_1(cObjectMgr);

void cObjectMgr::LoadLoots()
{
	for(uint8 i=0;i<MAX_LOOT_TABLE;i++)
		LoadLootTable(LootTable(i));
}

void cObjectMgr::LoadLootTable(LootTable lt)
{
	m_LootTemplates[lt].clear();

	sLog.outString();
	sLog.outString("Loading %s table...",TableName[lt]);
	LootTemplateMap::const_iterator tab;
	uint32 count = 0;

	QueryResult *result = WorldDatabase.PQuery("SELECT entry, item, ChanceOrQuestChance, groupid, mincountOrRef, maxcount, lootcondition, condition_value1, condition_value2 FROM %s",TableName[lt]);

    if (result)
    {
        do
        {
            Field *fields = result->Fetch();

            uint32 entry               = fields[0].GetUInt32();
            uint32 item                = fields[1].GetUInt32();
            float  chanceOrQuestChance = fields[2].GetFloat();
            uint8  group               = fields[3].GetUInt8();
            int32  mincountOrRef       = fields[4].GetInt32();
            uint32 maxcount            = fields[5].GetUInt32();
            ConditionType condition    = (ConditionType)fields[6].GetUInt8();
            uint32 cond_value1         = fields[7].GetUInt32();
            uint32 cond_value2         = fields[8].GetUInt32();

            if(maxcount > std::numeric_limits<uint8>::max())
            {
                sLog.outErrorDb("Table '%s' entry %d item %d: maxcount value (%u) to large. must be less %u - skipped", TableName[lt], entry, item, maxcount,std::numeric_limits<uint8>::max());
                continue;                                   // error already printed to log/console.
            }

            // (condition + cond_value1/2) are converted into single conditionId
            uint16 conditionId = sObjectMgr.GetConditionId(condition, cond_value1, cond_value2);

			LootStoreItem storeitem = LootStoreItem(item, chanceOrQuestChance, group, conditionId, mincountOrRef, maxcount);

			// Looking for the template of the entry
                                                            // often entries are put together
            if (m_LootTemplates[lt].empty() || tab->first != entry)
            {
                // Searching the template (in case template Id changed)
                tab = m_LootTemplates[lt].find(entry);
                if ( tab == m_LootTemplates[lt].end() )
                {
                    std::pair< LootTemplateMap::iterator, bool > pr = m_LootTemplates[lt].insert(LootTemplateMap::value_type(entry, new LootTemplate));
                    tab = pr.first;
                }
            }
            // else is empty - template Id and iter are the same
            // finally iter refers to already existed or just created <entry, LootTemplate>

            // Adds current row to the template
            tab->second->AddEntry(storeitem);
            ++count;

        } while (result->NextRow());

        delete result;
        sLog.outString( ">> Loaded %u loot definitions (%lu templates)", count, (unsigned long)m_LootTemplates[lt].size());
    }
    else
        sLog.outErrorDb( ">> Loaded 0 loot definitions. DB table `%s` is empty.",TableName[lt]);
}