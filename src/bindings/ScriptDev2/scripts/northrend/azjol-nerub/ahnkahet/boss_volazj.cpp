/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Volazj
SD%Complete: 20%
SDComment:
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"

//TODO: fill in texts in database. Also need to add text for whisper.
enum
{
    SAY_AGGRO                       = -1619033,
    SAY_INSANITY                    = -1619034,
    SAY_SLAY_1                      = -1619035,
    SAY_SLAY_2                      = -1619036,
    SAY_SLAY_3                      = -1619037,
    SAY_DEATH_1                     = -1619038,
    SAY_DEATH_2                     = -1619039,
	SPELL_FOUET_N					= 57941,
	SPELL_FOUET_H					= 59974,
	SPELL_SHADOW_BOLT_N				= 57942,
	SPELL_SHADOW_BOLT_H				= 59975,
	SPELL_SHIVER_N					= 57949,
	SPELL_SHIVER_H					= 59978,
	SPELL_INSANE					= 57496,
	SPELL_INSANE_16					= 57508,
	SPELL_INSANE_32					= 57509,
	SPELL_INSANE_64					= 57510,
	SPELL_INSANE_128				= 57511,
	SPELL_INSANE_256				= 57512,
	NPC_CLONE						= 30625,
};

/*######
## boss_volazj
######*/

struct MANGOS_DLL_DECL boss_volazjAI : public ScriptedAI
{
    boss_volazjAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;
	uint32 phasing_Timer;
	bool phasing;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_FOUET_H,12000,15000,1000);
			Tasks.AddEvent(SPELL_SHADOW_BOLT_H,20000,15000,3000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_SHIVER_H,7000,12000,1500);
		}
		else
		{
			Tasks.AddEvent(SPELL_FOUET_N,12000,15000,1000);
			Tasks.AddEvent(SPELL_SHADOW_BOLT_N,20000,15000,3000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_SHIVER_N,7000,12000,1500);
		}
		phasing_Timer = 60000;
		phasing = false;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SLAY_3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(urand(0, 1) ? SAY_DEATH_1 : SAY_DEATH_2, me);
		Tasks.GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;


		if(phasing_Timer <= uiDiff)
		{
			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
			{
				uint8 i = 0;
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				{
					if (Player* pPlayer = itr->getSource())
					{
						if(phasing)
						{
							DoCast(pPlayer,SPELL_INSANE_16+i);
							switch(i)
							{
								case 0:
									for(uint8 i=0;i<4;i++)
										if(Creature* tmp = me->SummonCreature(NPC_CLONE,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,100000))
											tmp->SetPhaseMask(16,true);
								case 1:
									for(uint8 i=0;i<4;i++)
										if(Creature* tmp = me->SummonCreature(NPC_CLONE,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,100000))
											tmp->SetPhaseMask(32,true);
								case 2:
									for(uint8 i=0;i<4;i++)
										if(Creature* tmp = me->SummonCreature(NPC_CLONE,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,100000))
											tmp->SetPhaseMask(64,true);
								case 3:
									for(uint8 i=0;i<4;i++)
										if(Creature* tmp = me->SummonCreature(NPC_CLONE,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,100000))
											tmp->SetPhaseMask(128,true);
								default:
									break;
							}
							i++;
						}
						else
						{
							pPlayer->RemoveAurasDueToSpell(SPELL_INSANE_16);
							pPlayer->RemoveAurasDueToSpell(SPELL_INSANE_32);
							pPlayer->RemoveAurasDueToSpell(SPELL_INSANE_64);
							pPlayer->RemoveAurasDueToSpell(SPELL_INSANE_128);
						}
					}
				}
			}

			phasing_Timer = 60000;
		}
		else
			phasing_Timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_volazj(Creature* pCreature)
{
    return new boss_volazjAI(pCreature);
}

void AddSC_boss_volazj()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_volazj";
    newscript->GetAI = &GetAI_boss_volazj;
    newscript->RegisterSelf();
}
