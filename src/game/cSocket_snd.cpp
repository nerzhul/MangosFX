#include <Policies/SingletonImp.h>
#include <SFML/Network.hpp>
#include "World.h"
#include "cSocket_snd.h"

cSocket_snd::cSocket_snd()
{
}

cSocket_snd::~cSocket_snd()
{
}

void cSocket_snd::run()
{
	while(!World::IsStopped())
	{
		ACE_Based::Thread::Sleep(50);
	}
}