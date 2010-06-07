#include "ClusterLoot.h"
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
	///- Init new SQL thread for the world database
    WorldDatabase.ThreadStart();                                // let thread do safe mySQL requests (one connection call enough)
}

void ClusterLoot::SetInitialSettings()
{
}