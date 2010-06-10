#include "Cluster.h"
#include <WorldPacket.h>
#include <GameObject.h>
#include <Player.h>
#include <Chat.h>

// Compile fix
uint32 realmID = 1;

/// Close RA connection
bool ChatHandler::HandleQuitCommand(const char* /*args*/)
{
    // processed in RASocket
    // TODO : close server
    return true;
}

/// Exit the realm
bool ChatHandler::HandleServerExitCommand(const char* /*args*/)
{
    // TODO : Close server
    return true;
}

bool ChatHandler::HandleServerSetLogLevelCommand(char const* /*args*/)
{
	// TODO : modify loglevel
	return true;
}

bool ChatHandler::HandleCharacterEraseCommand(char const * /*args*/)
{
	// Not implemented in cluster
	return true;
}

bool ChatHandler::HandleAccountOnlineListCommand(char const * /*args*/)
{
	// Not implemented in cluster
	return true;
}

bool ChatHandler::HandleAccountDeleteCommand(char const * /*args*/)
{
	// Not implemented in cluster
	return true;
}

bool ChatHandler::HandleAccountCreateCommand(char const * /*args*/)
{
	// Not implemented in cluster
	return true;
}