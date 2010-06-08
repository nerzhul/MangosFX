#include "cObjectMgr.h"
#include <Database/SQLStorage.h>
#include <Database/SQLStorageImpl.h>
#include <Common.h>
#include <DBCStructure.h>
#include <DBCStores.h>
#include <SharedDefines.h>
#include <Policies/SingletonImp.h>

INSTANTIATE_SINGLETON_1(cObjectMgr);

struct SQLItemLoader : public SQLStorageLoaderBase<SQLItemLoader>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char *src, D &dst)
    {
        dst = D(sClusterObjectMgr.GetScriptId(src));
    }
};

void cObjectMgr::LoadItemPrototypes()
{
    SQLItemLoader loader;
    loader.Load(sItemStorage);
    sLog.outString( ">> Loaded %u item prototypes", sItemStorage.RecordCount );
    sLog.outString();

    // check some dbc referenced items (avoid duplicate reports)
    std::set<uint32> notFoundOutfit;
    for (uint32 i = 1; i < sCharStartOutfitStore.GetNumRows(); ++i)
    {
        CharStartOutfitEntry const* entry = sCharStartOutfitStore.LookupEntry(i);
        if (!entry)
            continue;

        for(int j = 0; j < MAX_OUTFIT_ITEMS; ++j)
        {
            if (entry->ItemId[j] <= 0)
                continue;

            uint32 item_id = entry->ItemId[j];

            if (!GetItemPrototype(item_id))
                    notFoundOutfit.insert(item_id);
        }
    }

    for(std::set<uint32>::const_iterator itr = notFoundOutfit.begin(); itr != notFoundOutfit.end(); ++itr)
        sLog.outErrorDb("Item (Entry: %u) not exist in `item_template` but referenced in `CharStartOutfit.dbc`", *itr);
}

struct SQLCreatureLoader : public SQLStorageLoaderBase<SQLCreatureLoader>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char *src, D &dst)
    {
        dst = D(sClusterObjectMgr.GetScriptId(src));
    }
};

void cObjectMgr::LoadCreatureTemplates()
{
    SQLCreatureLoader loader;
    loader.Load(sCreatureStorage);

    sLog.outString( ">> Loaded %u creature definitions", sCreatureStorage.RecordCount );
    sLog.outString();
}

struct SQLGameObjectLoader : public SQLStorageLoaderBase<SQLGameObjectLoader>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char *src, D &dst)
    {
        dst = D(sClusterObjectMgr.GetScriptId(src));
    }
};

void cObjectMgr::LoadGameobjectInfo()
{
    SQLGameObjectLoader loader;
    loader.Load(sGOStorage);

    sLog.outString( ">> Loaded %u game object templates", sGOStorage.RecordCount );
    sLog.outString();
}


void cObjectMgr::LoadQuests()
{
    // For reload case
    for(QuestMap::const_iterator itr=mQuestTemplates.begin(); itr != mQuestTemplates.end(); ++itr)
        delete itr->second;
    mQuestTemplates.clear();

    QueryResult *result = WorldDatabase.Query("SELECT entry, Method, ZoneOrSort, SkillOrClass, MinLevel, QuestLevel, Type, RequiredRaces, RequiredSkillValue,"
    //   9                    10                 11                     12                   13                     14                   15                16
        "RepObjectiveFaction, RepObjectiveValue, RequiredMinRepFaction, RequiredMinRepValue, RequiredMaxRepFaction, RequiredMaxRepValue, SuggestedPlayers, LimitTime,"
    //   17          18            19           20            21            22           23           24              25
        "QuestFlags, SpecialFlags, CharTitleId, PlayersSlain, BonusTalents, PrevQuestId, NextQuestId, ExclusiveGroup, NextQuestInChain,"
    //   26        27         28            29
        "RewXPId, SrcItemId, SrcItemCount, SrcSpell,"
    //   30     31       32          33               34                35       36             37              38              39              40
        "Title, Details, Objectives, OfferRewardText, RequestItemsText, EndText, CompletedText, ObjectiveText1, ObjectiveText2, ObjectiveText3, ObjectiveText4,"
    //   41          42          43          44          45          46          47             48             49             50             51             52
        "ReqItemId1, ReqItemId2, ReqItemId3, ReqItemId4, ReqItemId5, ReqItemId6, ReqItemCount1, ReqItemCount2, ReqItemCount3, ReqItemCount4, ReqItemCount5, ReqItemCount6,"
    //   53            54            55            56            57               58               59               60
        "ReqSourceId1, ReqSourceId2, ReqSourceId3, ReqSourceId4, ReqSourceCount1, ReqSourceCount2, ReqSourceCount3, ReqSourceCount4,"
    //   61                  62                  63                  64                  65                     66                     67                     68
        "ReqCreatureOrGOId1, ReqCreatureOrGOId2, ReqCreatureOrGOId3, ReqCreatureOrGOId4, ReqCreatureOrGOCount1, ReqCreatureOrGOCount2, ReqCreatureOrGOCount3, ReqCreatureOrGOCount4,"
    //   69             70             71             72
        "ReqSpellCast1, ReqSpellCast2, ReqSpellCast3, ReqSpellCast4,"
    //   73                74                75                76                77                78
        "RewChoiceItemId1, RewChoiceItemId2, RewChoiceItemId3, RewChoiceItemId4, RewChoiceItemId5, RewChoiceItemId6,"
    //   79                   80                   81                   82                   83                   84
        "RewChoiceItemCount1, RewChoiceItemCount2, RewChoiceItemCount3, RewChoiceItemCount4, RewChoiceItemCount5, RewChoiceItemCount6,"
    //   85          86          87          88          89             90             91             92
        "RewItemId1, RewItemId2, RewItemId3, RewItemId4, RewItemCount1, RewItemCount2, RewItemCount3, RewItemCount4,"
    //   93              94              95              96              97              98            99            100           101           102
        "RewRepFaction1, RewRepFaction2, RewRepFaction3, RewRepFaction4, RewRepFaction5, RewRepValue1, RewRepValue2, RewRepValue3, RewRepValue4, RewRepValue5,"
    //   103               104                 105            106               107       108
        "RewHonorAddition, RewHonorMultiplier, RewOrReqMoney, RewMoneyMaxLevel, RewSpell, RewSpellCast,"
    //   109                110               111         112     113     114
        "RewMailTemplateId, RewMailDelaySecs, PointMapId, PointX, PointY, PointOpt,"
    //   115            116            117            118            119                 120                 121                 122
        "DetailsEmote1, DetailsEmote2, DetailsEmote3, DetailsEmote4, DetailsEmoteDelay1, DetailsEmoteDelay2, DetailsEmoteDelay3, DetailsEmoteDelay4,"
    //   123              124            125                126                127                128
        "IncompleteEmote, CompleteEmote, OfferRewardEmote1, OfferRewardEmote2, OfferRewardEmote3, OfferRewardEmote4,"
    //   129                     130                     131                     132
        "OfferRewardEmoteDelay1, OfferRewardEmoteDelay2, OfferRewardEmoteDelay3, OfferRewardEmoteDelay4,"
    //   133          134
        "StartScript, CompleteScript"
        " FROM quest_template");
    if(result == NULL)
    {
        barGoLink bar( 1 );
        bar.step();

        sLog.outString();
        sLog.outString( ">> Loaded 0 quests definitions" );
        sLog.outErrorDb("`quest_template` table is empty!");
        return;
    }

    barGoLink bar( result->GetRowCount() );
    do
    {
        bar.step();
        Field *fields = result->Fetch();

        Quest * newQuest = new Quest(fields);
        mQuestTemplates[newQuest->GetQuestId()] = newQuest;
    } while( result->NextRow() );

    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %lu quests definitions", (unsigned long)mQuestTemplates.size() );
}

uint32 cObjectMgr::GetScriptId(const char *name)
{
    // use binary search to find the script name in the sorted vector
    // assume "" is the first element
    if(!name) return 0;
    ScriptNameMap::const_iterator itr =
        std::lower_bound(m_scriptNames.begin(), m_scriptNames.end(), name);
    if(itr == m_scriptNames.end() || *itr != name) return 0;
    return itr - m_scriptNames.begin();
}