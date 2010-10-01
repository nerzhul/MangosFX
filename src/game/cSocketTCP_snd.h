#ifndef __C_SOCKET_TCP_SND_H_
#define __C_SOCKET_TCP_SND_H_

#include <Common.h>

class cSocketTCP_snd : public ACE_Based::Runnable
{
	public:
		cSocketTCP_snd();
		~cSocketTCP_snd();
		void run();
};

#endif