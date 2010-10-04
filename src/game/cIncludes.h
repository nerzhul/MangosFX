#ifndef __C_INCLUDES_H_
#define __C_INCLUDES_H_

#include <SFML/Network.hpp>
#include "cClusterSession.h"
#include "cSocketTCP.h"
using namespace sf;

enum ClusterType
{
	C_NULL	=	0x00,
	C_LOOT	=	0x01,
	C_BG	=	0x02,
	
	C_ALL	=	0xFF,
};

#endif