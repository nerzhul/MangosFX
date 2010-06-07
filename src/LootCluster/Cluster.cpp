#include "Cluster.h"
#include "ClusterLoot.h"
#include "Master.h"
#include <Log.h>
#include <Timer.h>
#include "revision_sql.h"
#include <Config/ConfigEnv.h>
#include <Policies/SingletonImp.h>
#include <Database/DatabaseEnv.h>
#include <ace/OS_NS_signal.h>
#include <ace/TP_Reactor.h>
#include <ace/Dev_Poll_Reactor.h>

#ifdef WIN32
#include "ServiceWin32.h"
extern int m_ServiceStatus;
#endif

INSTANTIATE_SINGLETON_1( Cluster );

volatile uint32 Cluster::m_masterLoopCounter = 0;

Cluster::Cluster()
{
}

Cluster::~Cluster()
{
}

/// Main function
int Cluster::Run()
{
    ///- Start the databases
    if (!_StartDB())
    {
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    ///- Initialize the World
    sClusterLoot.SetInitialSettings();

    ///- Catch termination signals
    _HookSignals();

    ///- Launch WorldRunnable thread
    ACE_Based::Thread cluster_thread(new ClusterLoot);
    cluster_thread.setPriority(ACE_Based::Highest);

	///- Handle affinity for multiple processors and process priority on Windows
    #ifdef WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = sConfig.GetIntDefault("UseProcessors", 0);
        if(Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if(GetProcessAffinityMask(hProcess,&appAff,&sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if(!curAff )
                {
                    sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for LootClusterFX. Accessible processors bitmask (hex): %x",Aff,appAff);
                }
                else
                {
                    if(SetProcessAffinityMask(hProcess,curAff))
                        sLog.outString("Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog.outError("Can't set used processors (hex): %x",curAff);
                }
            }
            sLog.outString();
        }

        bool Prio = sConfig.GetBoolDefault("ProcessPriority", false);

//        if(Prio && (m_ServiceStatus == -1)/* need set to default process priority class in service mode*/)
        if(Prio)
        {
            if(SetPriorityClass(hProcess,HIGH_PRIORITY_CLASS))
                sLog.outString("LootClusterFX process priority class set to HIGH");
            else
                sLog.outError("ERROR: Can't set LootClusterFX process priority class.");
            sLog.outString();
        }
    }
    #endif

    uint32 realCurrTime, realPrevTime;
    realCurrTime = realPrevTime = getMSTime();

    ///- Start up freeze catcher thread
    /*ACE_Based::Thread* freeze_thread = NULL;
    if(uint32 freeze_delay = sConfig.GetIntDefault("MaxCoreStuckTime", 0))
    {
        FreezeDetectorRunnable *fdr = new FreezeDetectorRunnable();
        fdr->SetDelayTime(freeze_delay*1000);
        freeze_thread = new ACE_Based::Thread(fdr);
        freeze_thread->setPriority(ACE_Based::Highest);
    }*/

    ///- Launch the world listener socket
    /*uint16 wsport = sWorld.getConfig (CONFIG_PORT_WORLD);
    std::string bind_ip = sConfig.GetStringDefault ("BindIP", "0.0.0.0");

    if (sWorldSocketMgr->StartNetwork (wsport, bind_ip) == -1)
    {
        sLog.outError ("Failed to start network");
        Log::WaitBeforeContinueIfNeed();
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }

    sWorldSocketMgr->Wait ();*/

    ///- Stop freeze protection before shutdown tasks
    /*if (freeze_thread)
    {
        freeze_thread->destroy();
        delete freeze_thread;
    }*/

	sClusterLoot.Wait();

    ///- Remove signal handling before leaving
    _UnhookSignals();

    // when the main thread closes the singletons get unloaded
    // since worldrunnable uses them, it will crash if unloaded after master
    cluster_thread.wait();

    ///- Wait for DB delay threads to end
    CharacterDatabase.HaltDelayThread();
    WorldDatabase.HaltDelayThread();
    loginDatabase.HaltDelayThread();

    sLog.outString( "Halting process..." );

    ///- Exit the process with specified return value
    return 0;
}

/// Initialize connection to the databases
bool Cluster::_StartDB()
{
    ///- Get world database info from configuration file
    std::string dbstring = sConfig.GetStringDefault("WorldDatabaseInfo", "");
    if(dbstring.empty())
    {
        sLog.outError("Database not specified in configuration file");
        return false;
    }
    sLog.outString("World Database: %s", dbstring.c_str());

    ///- Initialise the world database
    if(!WorldDatabase.Initialize(dbstring.c_str()))
    {
        sLog.outError("Cannot connect to world database %s",dbstring.c_str());
        return false;
    }

    if(!WorldDatabase.CheckRequiredField("db_version",REVISION_DB_MANGOS))
    {
        ///- Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }

    dbstring = sConfig.GetStringDefault("CharacterDatabaseInfo", "");
    if(dbstring.empty())
    {
        sLog.outError("Character Database not specified in configuration file");

        ///- Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }
    sLog.outString("Character Database: %s", dbstring.c_str());

    ///- Initialise the Character database
    if(!CharacterDatabase.Initialize(dbstring.c_str()))
    {
        sLog.outError("Cannot connect to Character database %s",dbstring.c_str());

        ///- Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }

    if(!CharacterDatabase.CheckRequiredField("character_db_version",REVISION_DB_CHARACTERS))
    {
        ///- Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }

    ///- Get login database info from configuration file
    dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    if(dbstring.empty())
    {
        sLog.outError("Login database not specified in configuration file");

        ///- Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }

    ///- Initialise the login database
    sLog.outString("Login Database: %s", dbstring.c_str() );
    if(!loginDatabase.Initialize(dbstring.c_str()))
    {
        sLog.outError("Cannot connect to login database %s",dbstring.c_str());

        ///- Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }

    return true;
}

/// Handle termination signals
void Cluster::_OnSignal(int s)
{
    switch (s)
    {
        case SIGINT:
            //World::StopNow(RESTART_EXIT_CODE);
            break;
        case SIGTERM:
        #ifdef _WIN32
        case SIGBREAK:
        #endif
            //World::StopNow(SHUTDOWN_EXIT_CODE);
            break;
    }

    signal(s, _OnSignal);
}

/// Define hook '_OnSignal' for all termination signals
void Cluster::_HookSignals()
{
    signal(SIGINT, _OnSignal);
    signal(SIGTERM, _OnSignal);
    #ifdef _WIN32
    signal(SIGBREAK, _OnSignal);
    #endif
}

/// Unhook the signals before leaving
void Cluster::_UnhookSignals()
{
    signal(SIGINT, 0);
    signal(SIGTERM, 0);
    #ifdef _WIN32
    signal(SIGBREAK, 0);
    #endif
}