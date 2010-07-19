#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"
#include "Log.h"
#include "Cluster.h"
#include "CORBAThread.h"
#include "SystemConfig.h"
#include "revision.h"
#include "revision_nr.h"
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include <ace/Version.h>

#ifdef WIN32
#include "ServiceWin32.h"
char serviceName[] = "BasicClusterFX";
char serviceLongName[] = "MaNGOSFX loot service";
char serviceDescription[] = "Massive Network Game Object Server";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#endif

DatabaseType WorldDatabase;                                 ///< Accessor to the world database
DatabaseType CharacterDatabase;                             ///< Accessor to the character database
DatabaseType loginDatabase;                                 ///< Accessor to the realm/login database
DatabaseType LocalisationDatabase;

/// Print out the usage string for this program on the console.
void usage(const char *prog)
{
    sLog.outString("Usage: \n %s [<options>]\n"
        "    --version                print version and exist\n\r"
        "    -c config_file           use config_file as configuration file\n\r"
        #ifdef WIN32
        "    Running as service functions:\n\r"
        "    --service                run as service\n\r"
        "    -s install               install service\n\r"
        "    -s uninstall             uninstall service\n\r"
        #endif
        ,prog);
}

/// Launch the mangos server
extern int main(int argc, char **argv)
{
	// - Construct Memory Manager Instance
    MaNGOS::Singleton<MemoryManager>::Instance();

    //char *leak = new char[1000];                          // test leak detection

    ///- Command line parsing to get the configuration file name
    char const* cfg_file = _MANGOSD_CONFIG;
    int c=1;
	while( c < argc )
    {
        if( strcmp(argv[c],"-c") == 0)
        {
            if( ++c >= argc )
            {
                sLog.outError("Runtime-Error: -c option requires an input argument");
                usage(argv[0]);
                return 1;
            }
            else
                cfg_file = argv[c];
        }

        if( strcmp(argv[c],"--version") == 0)
        {
            printf("%s\n", _FULLVERSION(REVISION_DATE,REVISION_TIME,REVISION_NR,REVISION_ID));
            return 0;
        }

        #ifdef WIN32
        ////////////
        //Services//
        ////////////
        if( strcmp(argv[c],"-s") == 0)
        {
            if( ++c >= argc )
            {
                sLog.outError("Runtime-Error: -s option requires an input argument");
                usage(argv[0]);
                Log::WaitBeforeContinueIfNeed();
                return 1;
            }
            if( strcmp(argv[c],"install") == 0)
            {
                if (WinServiceInstall())
                    sLog.outString("Installing service");
                return 1;
            }
            else if( strcmp(argv[c],"uninstall") == 0)
            {
                if(WinServiceUninstall())
                    sLog.outString("Uninstalling service");
                return 1;
            }
            else
            {
                sLog.outError("Runtime-Error: unsupported option %s",argv[c]);
                usage(argv[0]);
                Log::WaitBeforeContinueIfNeed();
                return 1;
            }
        }
        if( strcmp(argv[c],"--service") == 0)
        {
            WinServiceRun();
        }
        ////
        #endif
        ++c;
    }

    if (!sConfig.SetSource(cfg_file))
    {
        sLog.outError("Could not find configuration file %s.", cfg_file);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

	    sLog.outString( "%s [world-daemon]", _FULLVERSION(REVISION_DATE,REVISION_TIME,REVISION_NR,REVISION_ID) );
    sLog.outString( "<Ctrl-C> to stop.\n\n" );
	sLog.outTitle( "-----------------------------------------------------------------------------" );
	sLog.outTitle( "    ////// ////// //////   ////// //     //  // /////// ////// ////// ////// " );
	sLog.outTitle( "   //     //     //       //     //     //  // //        //   //     //   // " );
	sLog.outTitle( "  ////   ////// //////   //     //     //  // //////    //   ////   //////   " );
	sLog.outTitle( " //         //     //   //     //     //  //     //    //   //     //   //   " );
	sLog.outTitle( "//     ////// //////   ////// ////// ////// //////    //   ////// //   //    " );
	sLog.outTitle( "-----------------------------------------------------------------------------" );
	sLog.outTitle( "Frost Sapphire Studios Quest Cluster");
    sLog.outString("Using configuration file %s.", cfg_file);

    sLog.outDetail("%s (Library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
    if (SSLeay() < 0x009080bfL )
    {
        sLog.outDetail("WARNING: Outdated version of OpenSSL lib. Logins to server may not work!");
        sLog.outDetail("WARNING: Minimal required version [OpenSSL 0.9.8k]");
    }

    sLog.outDetail("Using ACE: %s", ACE_VERSION);

    ///- and run the 'Master'
    /// \todo Why do we need this 'Master'? Can't all of this be in the Main as for Realmd?
	
	 return sCluster.Run(argc,argv); // TODO

    // at sMaster return function exist with codes
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart LootClusterFX
}