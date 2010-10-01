#include <Policies/SingletonImp.h>
#include <SFML/Network.hpp>
#include "World.h"
#include "ClusterSocket.h"

ClusterSocket::ClusterSocket()
{
}

ClusterSocket::~ClusterSocket()
{
}

void ClusterSocket::run()
{
	while(!World::IsStopped())
	{
		ACE_Based::Thread::Sleep(50);
	}
}