#ifndef _CLUSTERBASIC_H
#define _CLUSTERBASIC_H

#include "Common.h"
#include "Policies/Singleton.h"

#define CLUSTER_SLEEP_CONST 50
enum ShutdownExitCode
{
    SHUTDOWN_EXIT_CODE = 0,
    ERROR_EXIT_CODE    = 1,
    RESTART_EXIT_CODE  = 2,
};

class ClusterBasic : public ACE_Based::Runnable
{
	public:
		ClusterBasic();
		~ClusterBasic();
		void run();
		void SetInitialSettings();
		static void Wait();
		static void StopNOW() { m_stopEvent = true; }

	private:
		static bool MustStop() { return m_stopEvent; }
		
		static void LoadConfigSettings();
		void DetectDBCLang();
		LocaleConstant m_defaultDbcLocale;                     // from config for one from loaded DBC locales
        uint32 m_availableDbcLocaleMask;                       // by loaded DBC
		static volatile bool m_stopEvent;
		static uint8 m_ExitCode;
};
#define sClusterBasic MaNGOS::Singleton<ClusterBasic>::Instance()
#endif