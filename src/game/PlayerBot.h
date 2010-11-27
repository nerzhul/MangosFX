#ifndef __PLAYERBOT_H_
#define __PLAYERBOT_H_

#include "Player.h"
#include "WorldSession.h"

class PlayerBot: public Player
{
	public:
		explicit PlayerBot (WorldSession *session) : Player(session) {}
		~PlayerBot() {}

		void Update(uint32 diff) {}
};
#endif