/* LibDevFS by Frost Sapphire Studios */


#include "precompiled.h"
#include "utgarde_keep.h"


enum Spells
{
	SPELL_FROST_TOMB_INVOC	=	42714,
	SPELL_FROST_TOMB_CHAN	=	48400,
	SPELL_SHADOW_BOLT		=	43667,
	SPELL_SHADOW_BOLT_H		=	59389
};

enum Npcs
{
	NPC_FROST_TOMB			= 23965,
	NPC_SKELETON			= 31635,
};

/*######
## boss_keleseth
######*/

struct MANGOS_DLL_DECL boss_kelesethAI : public LibDevFSAI
{
    boss_kelesethAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(m_difficulty ? SPELL_SHADOW_BOLT_H : SPELL_SHADOW_BOLT,1000,6000,1000);
    }

	uint32	frost_tomb_Timer;
	uint32	frost_tomb_verif_Timer;
	uint32	skeleton_Timer;
	uint64 cr[6];
	bool skeleton_pop;
	uint64 tombGUID;

	bool Achievement;

    void Reset()	
    {
		ResetTimers();
		CleanMyAdds();
		SetInstanceData(TYPE_KELESETH,NOT_STARTED);
		tombGUID = 0;
		for(uint8 i=0;i<6;i++)
			cr[i] = 0;
		frost_tomb_Timer = 12000;
		frost_tomb_verif_Timer = 1000;
		skeleton_Timer = 18000;
		skeleton_pop = false;
		Achievement = true;
    }

    void Aggro(Unit* pWho)
    {
		Yell(13221,"Votre sang est à moi !");
		SetInstanceData(TYPE_KELESETH,IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
		Say(13225,"Je rejoins... la nuit.");
		if(Creature* tomb = GetGuidCreature(tombGUID))
			tomb->RemoveFromWorld();

		if(Achievement && m_difficulty)
			CompleteAchievementForGroup(1919);
		SetInstanceData(TYPE_KELESETH,DONE);
		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {
		Yell(13223,"Les ténèbres, attendent.");
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(frost_tomb_Timer <= diff)
		{
			frost_tomb_Timer = 30000;
			Yell(13222,"Pas si vite !");
			me->CastStop();

			if(Unit* targettomb = SelectUnit(SELECT_TARGET_RANDOM,0))
			{
				if(Creature* tomb = CallCreature(NPC_FROST_TOMB,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,targettomb->GetPositionX(),targettomb->GetPositionY(),targettomb->GetPositionZ()))
				{
					tombGUID = tomb->GetGUID();
					if(targettomb)
					{
						std::string txt = "Keleseth lance Tombeau de Givre sur ";
						txt += std::string(targettomb->GetName());
						BossEmote(0,txt);
						tomb->CastSpell(targettomb,SPELL_FROST_TOMB_CHAN,false);
					}
					ModifyAuraStack(48400,1,targettomb,tomb);
				}
			}
		}
		else
			frost_tomb_Timer -= diff;

		if(frost_tomb_verif_Timer <= diff)
		{
			if(Creature* tomb = GetGuidCreature(tombGUID))
			{
				if(!tomb->isAlive())
				{
					Achievement = false;
					Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();

					if (!lPlayers.isEmpty())
					{
						for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
						{
							if (Player* pPlayer = itr->getSource())
								if(pPlayer->isAlive() && pPlayer->HasAura(48400))
									pPlayer->RemoveAurasDueToSpell(48400);
						}
					}
				}
			}
			frost_tomb_verif_Timer = 1000;
		}
		else
			frost_tomb_verif_Timer -= diff;

		if(m_difficulty && skeleton_Timer <= diff)
		{
			if(!skeleton_pop)
			{
				Yell(13224,"Aranal, lidel ! Leur destin vous appartiendra.");
				for(int i=0;i<6;i++)
				{
					Creature* pSummon = CallCreature(NPC_SKELETON,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,185.209f,206.089f,41.015f);
					cr[i] = pSummon->GetGUID();
				}
				skeleton_Timer = 2000;
				skeleton_pop = true;
			}
			else
			{
				Creature* sk[6];
				for(uint8 i=0;i<6;i++)
					sk[i] = GetGuidCreature(cr[i]);
				if(sk[0] && sk[1] && sk[2] && sk[3] && sk[4] && sk[5])
				{
					if(!sk[0]->isAlive() && !sk[1]->isAlive() && !sk[2]->isAlive() && !sk[3]->isAlive() 
						&& !sk[4]->isAlive() && !sk[5]->isAlive())
					{
						for(uint8 i=0;i<6;i++)
						{
							sk[i]->Respawn();
							Speak(CHAT_TYPE_TEXT_EMOTE,0,"Squelette Vrykul se relève",sk[i]);
						}
						Yell(13224,"Aranal, lidel ! Leur destin vous appartiendra.");
					}
				}
			}
		}
		else
			skeleton_Timer -= diff;

		UpdateEvent(diff);

		DoMeleeAttackIfReady();

    }
};

CreatureAI* GetAI_boss_keleseth(Creature* pCreature)
{
    return new boss_kelesethAI(pCreature);
}

void AddSC_boss_keleseth()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_keleseth";
    newscript->GetAI = &GetAI_boss_keleseth;
    newscript->RegisterSelf();
}

