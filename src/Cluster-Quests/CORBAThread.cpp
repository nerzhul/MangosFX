#include <Policies/SingletonImp.h>
#include <omniORB4/CORBA.h>
#include <Log.h>
#include <Timer.h>
#include "CORBAThread.h"

INSTANTIATE_SINGLETON_1( CORBAThread );
volatile bool CORBAThread::m_stopEvent = false;
uint8 CORBAThread::m_ExitCode = 0;

CORBAThread::CORBAThread()
{
}

CORBAThread::~CORBAThread()
{
}

void CORBAThread::run()
{
	uint32 realCurrTime = 0;
    uint32 realPrevTime = getMSTime();
	uint32 prevSleepTime = 0;                               // used for balanced full tick time length near WORLD_SLEEP_CONST

	int argc = 0;
	char** argv = NULL;
	try
	{
		sLog.outBasic("Initializing CORBA...");
		CORBA::ORB_ptr orb = CORBA::ORB_init(argc,argv,"omniORB4");

		sLog.outBasic("Initializing CORBA POA...");
		CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

		/** TODO : activate objects there

		*/

		sLog.outBasic("Activate CORBA POA Manager...");
		PortableServer::POAManager_var pman = poa->the_POAManager();
		pman->activate();

		sLog.outBasic("CORBA Thread launched successfuly !");
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

		sLog.outBasic("Destroy CORBA ORB...");
		orb->destroy();
	}
	catch(CORBA::SystemException& ex)
	{
		sLog.outError("CORBA System Exception : %s",ex._name());
	}
	catch(CORBA::Exception& ex)
	{
		sLog.outError("CORBA Exception : %s",ex._name());
	}
	catch(omniORB::fatalException& ex)
	{
		sLog.outError("omniORB fatalException : file => %s / line => %d / error : %s",ex.file(),ex.line(),ex.errmsg());
	}

	sLog.outBasic("CORBA Thread finished successfuly !");
}

void CORBAThread::Wait()
{
	while(!MustStop())
	{
		//ACE_Based::Thread::Sleep(1000);
	}
}