/* LibDevFS by Frost Sapphire Studios */


#include "precompiled.h"
#include "utgarde_keep.h"


#define SPELL_FROST_TOMB_INVOC	42714
#define SPELL_FROST_TOMB_CHAN	48400

enum
{
    SAY_AGGRO               = -1574000,
    SAY_FROSTTOMB           = -1574001,
    SAY_SKELETONS           = -1574002,
    SAY_KILL                = -1574003,
    SAY_DEATH               = -1574004,
	EMOTE_TOMB				= -1616000,
};

// mobs
enum
{
	NPC_FROST_TOMB			= 23965,
	NPC_SKELETON			= 31635,
};

/*######
## boss_keleseth
######*/

struct MANGOS_DLL_DECL boss_kelesethAI : public ScriptedAI
{
	uint32	frost_tomb_Timer;
	uint32	frost_tomb_verif_Timer;
	Creature* tomb;
	uint32	skeleton_Timer;
	Creature* cr[6];
	bool skeleton_pop;
	Unit* target, *targettomb;

    boss_kelesethAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()	
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(59389,1000,6000,1000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(43667,1000,6000,1000,TARGET_MAIN);
		}
		frost_tomb_Timer = 12000;
		frost_tomb_verif_Timer = 1000;
		tomb = NULL;
		skeleton_Timer = 18000;
		skeleton_pop = false;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void JustDied(Unit* pKiller)
    {
		DoScriptText(SAY_DEATH, me);
		if(tomb)
			tomb->RemoveFromWorld();

		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {		
        DoScriptText(SAY_KILL, me);
    }

    void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;

		if(frost_tomb_Timer <= diff)
		{
			frost_tomb_Timer = 30000;
			DoScriptText(SAY_FROSTTOMB, me);
			me->CastStop();

			targettomb = SelectUnit(SELECT_TARGET_RANDOM,0);			
			if(tomb = Tasks.CallCreature(NPC_FROST_TOMB,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,targettomb->GetPositionX(),targettomb->GetPositionY(),targettomb->GetPositionZ()))
			{
				DoScriptText(EMOTE_TOMB,me);
				if(targettomb)
					tomb->CastSpell(targettomb,SPELL_FROST_TOMB_CHAN,false);
				SetAuraStack(48400,1,targettomb,tomb);
			}
		}
		else
			frost_tomb_Timer -= diff;

		/*if(frost_tomb_verif_Timer <= diff)
		{
			if(tomb && !tomb->isAlive())
			{
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
			frost_tomb_verif_Timer = 1000;
		}
		else
			frost_tomb_verif_Timer -= diff;*/

		if(m_bIsHeroic && skeleton_Timer <= diff)
		{
			if(!skeleton_pop)
			{
				DoScriptText(SAY_SKELETONS, me);
				
				for(int i=0;i<6;i++)
					cr[i] = Tasks.CallCreature(NPC_SKELETON,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,185.209f,206.089f,41.015f);
				skeleton_Timer = 2000;
				skeleton_pop = true;
			}
			else
			{
				if(cr[0] && !cr[0]->isAlive() && cr[1] && !cr[1]->isAlive() && cr[2] && !cr[2]->isAlive() &&
					cr[3] && !cr[3]->isAlive() && cr[4] && !cr[4]->isAlive() && cr[5] && !cr[5]->isAlive())
				{
					for(short i=0;i<6;i++)
					{
						if(cr[i])
						{
							cr[i]->Respawn();
							Speak(CHAT_TYPE_TEXT_EMOTE,0,"Squelette Vrykul se relève",cr[i]);
						}
					}
					DoScriptText(SAY_SKELETONS, me);
				}
			}
		}
		else
			skeleton_Timer -= diff;

		Tasks.UpdateEvent(diff);

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

