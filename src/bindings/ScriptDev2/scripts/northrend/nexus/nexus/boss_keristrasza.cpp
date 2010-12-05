/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"

enum
{
    SAY_AGGRO                   = -1576016,
    SAY_CRYSTAL_NOVA            = -1576017,
    SAY_ENRAGE                  = -1576018,
    SAY_KILL                    = -1576019,
    SAY_DEATH                   = -1576020,
    SPELL_CRYSTALFIRE_BREATH    = 48096,
    SPELL_CRYSTALFIRE_BREATH_H  = 57091,
    SPELL_CRYSTALLIZE           = 48179,
    SPELL_CRYSTAL_CHAINS        = 50997,
    SPELL_CRYSTAL_CHAINS_H      = 57050,
    SPELL_TAIL_SWEEP            = 50155,
    SPELL_INTENSE_COLD          = 48094,
    SPELL_ENRAGE                = 8599
};

/*######
## boss_keristrasza
######*/

struct MANGOS_DLL_DECL boss_keristraszaAI : public LibDevFSAI
{
	boss_keristraszaAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_TAIL_SWEEP,7000,16000,2000);
		if(m_difficulty)
		{
			AddEvent(SPELL_CRYSTAL_CHAINS_H,3000,10000,1000);
			AddEventOnTank(SPELL_CRYSTALFIRE_BREATH_H,5000,10000,2000);
			AddEvent(SPELL_CRYSTALLIZE,15000,14000,2000);
		}
		else
		{
			AddEvent(SPELL_CRYSTAL_CHAINS,3000,10000,1000);
			AddEventOnTank(SPELL_CRYSTALFIRE_BREATH,5000,10000,2000);
		}
    }

	bool achievement;
	uint32 checkAchievement_Timer;

    void Reset()
    {
		ResetTimers();
		checkAchievement_Timer = 500;
		achievement = true;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
		DoCastMe(SPELL_INTENSE_COLD);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
		GiveRandomReward();
		if(achievement)
			CompleteAchievementForGroup(2036);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(m_difficulty)
		{
			if(checkAchievement_Timer <= diff)
			{
				Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
				if (!lPlayers.isEmpty())
					for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
						if(Player* pPlayer = itr->getSource())
							if(Aura* aur = pPlayer->GetAura(48095))
								if(aur->GetStackAmount() > 2)
									achievement = false;

				checkAchievement_Timer = 500;
			}
			else
				checkAchievement_Timer -= diff;
		}

		if(CheckPercentLife(25) && !me->HasAura(8599,0))
			DoCastMe(SPELL_ENRAGE);

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_keristrasza(Creature* pCreature)
{
    return new boss_keristraszaAI(pCreature);
}

void AddSC_boss_keristrasza()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_keristrasza";
    newscript->GetAI = &GetAI_boss_keristrasza;
    newscript->RegisterSelf();
}
