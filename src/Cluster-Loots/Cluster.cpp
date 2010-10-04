#include <Log.h>
#include <Timer.h>
#include "revision_sql.h"
#include <Config/Config.h>
#include <Policies/SingletonImp.h>
#include <Database/DatabaseEnv.h>
#include <ace/OS_NS_signal.h>
#include <ace/TP_Reactor.h>
#include <ace/Dev_Poll_Reactor.h>

#include "Cluster.h"
#include "ClusterSpec.h"
#include "TCPListener.h"

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

    ///- Launch Cluster thread
    ACE_Based::Thread cluster_thread(new ClusterLoot);
    cluster_thread.setPriority(ACE_Based::Highest);

	ACE_Based::Thread sockListener_thread(new TCPListener(sConfig.GetIntDefault("LootClusterPort",3695)));
	sockListener_thread.setPriority(ACE_Based::Highest);

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
    return true;
}

/// Handle termination signals
void Cluster::_OnSignal(int s)
{
    switch (s)
    {
        case SIGINT:
			ClusterLoot::StopNOW();
            break;
        case SIGTERM:
        #ifdef _WIN32
        case SIGBREAK:
        #endif
            ClusterLoot::StopNOW();
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