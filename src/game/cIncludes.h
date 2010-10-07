#ifndef __C_INCLUDES_H_
#define __C_INCLUDES_H_

#include <SFML/Network.hpp>
#include <Common.h>
#include <Log.h>
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
	error_log("PacketSize %u",Packet.GetDataSize());
    Packet.Append(&ToWrite, sizeof(ToWrite));
	error_log("PacketSize %u",Packet.GetDataSize());
    return Packet;
}

static sf::Packet& operator >>(sf::Packet& Packet, uint64& Data)
{
	uint32 data=0,data2=0;
	Packet >> data;
	Packet >> data2;
	
	Data = data*pow(2.0f,32.0f)+data2;
    return Packet;
}

#endif