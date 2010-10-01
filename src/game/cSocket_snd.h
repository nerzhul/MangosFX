#ifndef __C_SOCKET_SND_H_
#define __C_SOCKET_SND_H_

#include <Common.h>

class cSocket_snd : public ACE_Based::Runnable
{
	public:
		cSocket_snd();
		~cSocket_snd();
		void run();
};

#endif