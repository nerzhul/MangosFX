#ifndef _CORBA_THREAD_H
#define _CORBA_THREAD_H

#include <Common.h>
#include <Policies/Singleton.h>
#include "World.h"

#define CLUSTER_SLEEP_CONST 50

class CORBAThread : public ACE_Based::Runnable
{
	public:
		CORBAThread();
		~CORBAThread();
		void run();
		static void Wait();
		static void StopNOW() { m_stopEvent = true; }
	private:
		static bool MustStop() { return m_stopEvent; }

		static volatile bool m_stopEvent;
		static uint8 m_ExitCode;
};

#define sCORBAThread MaNGOS::Singleton<CORBAThread>::Instance()

#endif