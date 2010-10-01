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

void cSocketTCP_snd::InitConnect(std::string addr, uint16 port)
{
}

bool cSocketTCP_snd::Connect()
{

	return true;
}

void cSocketTCP_snd::SendPacket(WorldPacket* data)
{

}