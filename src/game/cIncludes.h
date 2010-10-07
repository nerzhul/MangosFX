#ifndef __C_INCLUDES_H_
#define __C_INCLUDES_H_

#include <SFML/Network.hpp>
#include <Common.h>
using namespace sf;

enum ClusterType
{
	C_NULL	=	0x00,
	C_LOOT	=	0x01,
	C_BG	=	0x02,
	
	C_ALL	=	0xFF,
};

static sf::Packet& operator <<(sf::Packet& Packet, uint64 Data)
{
    uint64 ToWrite = htonl(Data);
    Packet.Append(&ToWrite, sizeof(ToWrite));
    return Packet;
}

static sf::Packet& operator >>(sf::Packet& Packet, uint64& Data)
{
	uint32 data=0,data2=0;
	Packet >> data;
	Packet >> data2;
	
	Data = data*pow(2,32)+data2;
    return Packet;
}

#endif