#include <Policies/SingletonImp.h>
#include <Log.h>
#include <Timer.h>
#include "Cluster.h"
#include "CORBAThread.h"
#include "FirstCallBack.hh"

INSTANTIATE_SINGLETON_1( CORBAThread );
volatile bool CORBAThread::m_stopEvent = false;
uint8 CORBAThread::m_ExitCode = 0;

class FirstCallBack_i : public POA_FirstCallBack
{
	public:
	  inline FirstCallBack_i() {}
	  virtual ~FirstCallBack_i() {}
	  virtual char* nerzhulCB(const char* mesg);
};


char* FirstCallBack_i::nerzhulCB(const char* mesg)
{
  return CORBA::string_dup(mesg);
}

CORBAThread::CORBAThread()
{
	argtab = NULL;
	argnb = 0;
}

CORBAThread::~CORBAThread()
{
}

void CORBAThread::run()
{
	uint32 realCurrTime = 0;
    uint32 realPrevTime = getMSTime();
	uint32 prevSleepTime = 0;                               // used for balanced full tick time length near WORLD_SLEEP_CONST

	CosNaming::NamingContext_var rootContext;
	try
	{
		sLog.outBasic("Initializing CORBA...");
		CORBA::ORB_ptr orb = CORBA::ORB_init(argnb,argtab,"omniORB4");

		sLog.outBasic("Initializing CORBA POA...");
		CORBA::Object_var obj = orb->resolve_initial_references("NameService");
		rootContext = CosNaming::NamingContext::_narrow(obj);
		PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
		if( CORBA::is_nil(rootContext) ) {
			sLog.outError("RootContext configuration failed !");
		}

		sLog.outBasic("Prepare FirstCallBack object...");
		FirstCallBack_var v = FirstCallBack::_narrow(obj);

		FirstCallBack_i* FCB = new FirstCallBack_i();

		// Activate the object.  This tells the POA that this object is
		// ready to accept requests.
		PortableServer::ObjectId_var FCBid = poa->activate_object(FCB);

		// Obtain a reference to the object.
		FirstCallBack_var FCBref = FCB->_this();

		// Decrement the reference count of the object implementation, so
		// that it will be properly cleaned up when the POA has determined
		// that it is no longer needed.
		FCB->_remove_ref();
		
		/** TODO : activate objects there

		*/

		sLog.outBasic("Activate CORBA POA Manager...");
		PortableServer::POAManager_var pman = poa->the_POAManager();
		pman->activate();

		if(CORBA::is_nil(v))
			sLog.outError("Cannot Initialize FirstCallBack object");
		else
		{
			CORBA::String_var src = (const char*) "Toto";
			CORBA::String_var dest = v->nerzhulCB(src);
			sLog.outError("String recv %s",dest);
		}

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