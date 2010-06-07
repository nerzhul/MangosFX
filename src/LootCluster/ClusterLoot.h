#ifndef _CLUSTERLOOT_H
#define _CLUSTERLOOT_H

#include "Common.h"
#include "Policies/Singleton.h"

enum ShutdownExitCode
{
    SHUTDOWN_EXIT_CODE = 0,
    ERROR_EXIT_CODE    = 1,
    RESTART_EXIT_CODE  = 2,
};

class ClusterLoot : public ACE_Based::Runnable
{
	public:
		ClusterLoot();
		~ClusterLoot();
		void run();
		void SetInitialSettings();
		static void StopNow(uint8 exitcode) { m_stopEvent = true; m_ExitCode = exitcode; }

	private:
		static volatile bool m_stopEvent;
		static uint8 m_ExitCode;
};
#define sClusterLoot MaNGOS::Singleton<ClusterLoot>::Instance()
#endif