#include "ClusterLoot.h"
#include "Timer.h"
#include "Database/DatabaseEnv.h"
#include "Policies/SingletonImp.h"

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
    LoadDBCStores(m_dataPath);
    DetectDBCLang();

	// For other clusters, modify loaded tables there

	sLog.outString( "Loading Items..." );                   // must be after LoadRandomEnchantmentsTable and LoadPageTexts
    sObjectMgr.LoadItemPrototypes();

	sLog.outString( "Loading Creature templates..." );
    sObjectMgr.LoadCreatureTemplates();

	sLog.outString( "Loading Gameobject Data..." );
    sObjectMgr.LoadGameobjects();

	sLog.outString( "Loading Loot Tables..." );
    sLog.outString();
    LoadLootTables();
    sLog.outString( ">>> Loot Tables loaded" );
    sLog.outString();

	sLog.outString( "WORLD: ClusterLoot initialized" );

	uint32 uStartInterval = getMSTimeDiff(uStartTime, getMSTime());
	sLog.outError( "CLUSTER STARTUP TIME: %i minutes %i seconds", uStartInterval / 60000, (uStartInterval % 60000) / 1000 );
}

void ClusterLoot::Wait()
{
	while(!MustStop())
	{
		Sleep(1000);
	}
}