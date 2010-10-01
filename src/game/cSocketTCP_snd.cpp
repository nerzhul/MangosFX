#include <Policies/SingletonImp.h>
#include <SFML/Network.hpp>
#include "World.h"
#include "cSocketTCP_snd.h"

using namespace sf;

cSocketTCP_snd::cSocketTCP_snd()
{
}

cSocketTCP_snd::~cSocketTCP_snd()
{
}

void cSocketTCP_snd::run()
{
	while(!World::IsStopped())
	{
		ACE_Based::Thread::Sleep(50);
	}
}