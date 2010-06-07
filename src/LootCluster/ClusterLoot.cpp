#include "ClusterLoot.h"
#include <Timer.h>
#include <Config/ConfigEnv.h>
#include <Database/DatabaseEnv.h>
#include <Policies/SingletonImp.h>
#include <ObjectMgr.h>
#include <GameEventMgr.h>
#include <PoolManager.h>

INSTANTIATE_SINGLETON_1( ClusterLoot );
volatile bool ClusterLoot::m_stopEvent = false;
uint8 ClusterLoot::m_ExitCode = SHUTDOWN_EXIT_CODE;

ClusterLoot::ClusterLoot()
{
}

ClusterLoot::~ClusterLoot()
{
}

void ClusterLoot::run()
{
	// Init
	///- Init new SQL thread for the world database
    WorldDatabase.ThreadStart();                                // let thread do safe mySQL requests (one connection call enough)
	uint32 realCurrTime = 0;
    uint32 realPrevTime = getMSTime();
	uint32 prevSleepTime = 0;                               // used for balanced full tick time length near WORLD_SLEEP_CONST

	while(!MustStop())
	{
		// Updates
		realCurrTime = getMSTime();
        uint32 diff = getMSTimeDiff(realPrevTime,realCurrTime);

		// Main update there if need
		realPrevTime = realCurrTime;
		if (diff <=  CLUSTER_SLEEP_CONST+prevSleepTime)
        {
            prevSleepTime = CLUSTER_SLEEP_CONST+prevSleepTime-diff;
            ACE_Based::Thread::Sleep(prevSleepTime);
        }
        else
            prevSleepTime = 0;
	}
	
	// Exit Cleanup there

	///- End the database thread
    WorldDatabase.ThreadEnd();                                  // free mySQL thread resources
}

void ClusterLoot::SetInitialSettings()
{
	///- Time server startup
    uint32 uStartTime = getMSTime();
	
	///- Initialize config settings
    LoadConfigSettings();

	///- Load the DBC files
    sLog.outString("Initialize data stores...");
	LoadDBCStores("D:/Frost Sapphire/");
    DetectDBCLang();

	// For other clusters, modify loaded tables there

	sLog.outString( "Loading Items..." );                   // must be after LoadRandomEnchantmentsTable and LoadPageTexts
    sObjectMgr.LoadItemPrototypes(true);

	sLog.outString( "Loading Creature templates..." );
    sObjectMgr.LoadCreatureTemplates(true);

	sLog.outString( "Loading Game Object Templates..." );   // must be after LoadPageTexts
    sObjectMgr.LoadGameobjectInfo(true);

	sLog.outString( "Loading Quests..." );
    sObjectMgr.LoadQuests(true);                                    // must be loaded after DBCs, creature_template, item_template, gameobject tables

	sLog.outString( "Loading Objects Pooling Data...");
    sPoolMgr.LoadFromDB(true);

	sLog.outString( "Loading Game Event Data...");
    sLog.outString();
    sGameEventMgr.LoadFromDB(true);
    sLog.outString( ">>> Game Event Data loaded" );
    sLog.outString();

	sLog.outString( "Loading Loot Tables..." );
    sLog.outString();
    LoadLootTables();
    sLog.outString( ">>> Loot Tables loaded" );
    sLog.outString();

	sLog.outString( "CLUSTER: ClusterLoot initialized" );

	uint32 uStartInterval = getMSTimeDiff(uStartTime, getMSTime());
	sLog.outBasic( "CLUSTER STARTUP TIME: %i minutes %i seconds", uStartInterval / 60000, (uStartInterval % 60000) / 1000 );
}

void ClusterLoot::DetectDBCLang()
{
    uint32 m_lang_confid = sConfig.GetIntDefault("DBC.Locale", 255);

    if(m_lang_confid != 255 && m_lang_confid >= MAX_LOCALE)
    {
        sLog.outError("Incorrect DBC.Locale! Must be >= 0 and < %d (set to 0)",MAX_LOCALE);
        m_lang_confid = LOCALE_enUS;
    }

    ChrRacesEntry const* race = sChrRacesStore.LookupEntry(1);

    std::string availableLocalsStr;

    int default_locale = MAX_LOCALE;
    for (int i = MAX_LOCALE-1; i >= 0; --i)
    {
        if ( strlen(race->name[i]) > 0)                     // check by race names
        {
            default_locale = i;
            m_availableDbcLocaleMask |= (1 << i);
            availableLocalsStr += localeNames[i];
            availableLocalsStr += " ";
        }
    }

    if( default_locale != m_lang_confid && m_lang_confid < MAX_LOCALE &&
        (m_availableDbcLocaleMask & (1 << m_lang_confid)) )
    {
        default_locale = m_lang_confid;
    }

    if(default_locale >= MAX_LOCALE)
    {
        sLog.outError("Unable to determine your DBC Locale! (corrupt DBC?)");
        exit(1);
    }

    m_defaultDbcLocale = LocaleConstant(default_locale);

    sLog.outString("Using %s DBC Locale as default. All available DBC locales: %s",localeNames[m_defaultDbcLocale],availableLocalsStr.empty() ? "<none>" : availableLocalsStr.c_str());
    sLog.outString();
}

void ClusterLoot::LoadConfigSettings()
{
}

void ClusterLoot::Wait()
{
	while(!MustStop())
	{
		Sleep(1000);
	}
}