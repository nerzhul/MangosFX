#ifndef __PLAYERBOT_H_
#define __PLAYERBOT_H_

#include "Player.h"

class PlayerBot// : public Player
{
	public:
		explicit PlayerBot (WorldSession *session);
		~PlayerBot();
		        
        void Stay();
		void Update(uint32 diff);

		Player* GetPlayer() { return bot; }
		void SetPlayer(Player* plr) { bot = plr; }

	private:
		Player* bot;

};

#endif