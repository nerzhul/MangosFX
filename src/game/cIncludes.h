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
	sf::Uint32 data=0, data2=0;
	Packet >> data >> data2;
	
	Data = data2*pow(2.0f,32.0f)+data;
    return Packet;
}

static uint64 readUint64(sf::Packet Packet, uint32 pos)
{
	uint64 value = 0;
	value += uint8(Packet.GetData()[pos+3]);
	value += uint8(Packet.GetData()[pos+2])*256;
	value += uint8(Packet.GetData()[pos+1])*65536;
	value += uint8(Packet.GetData()[pos+0])*16777216;
	value += uint8(Packet.GetData()[pos+7])*4294967296;
	value += uint8(Packet.GetData()[pos+6])*1099511627776;
	value += uint8(Packet.GetData()[pos+5])*281474976710656;
	value += uint8(Packet.GetData()[pos+4])*72057594037927936;
	return value;
}

#endif