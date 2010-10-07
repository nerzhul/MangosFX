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
	WorldPacket buffer(0);
	uint32 data;

	Packet >> data;
	buffer << uint32(data);
	Packet >> data;
	buffer << uint32(data);

	buffer >> uint64(Data);
    return Packet;
}

#endif