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
SDName: Boss_Gluth
SD%Complete: 70
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"

enum
{
    EMOTE_ZOMBIE                    = -1533119,

    SPELL_MORTALWOUND               = 25646,
    SPELL_DECIMATE                  = 28374,
    SPELL_ENRAGE                    = 28371,
    SPELL_ENRAGE_H                  = 54427,
    SPELL_BERSERK                   = 26662,

    NPC_ZOMBIE_CHOW                 = 16360,
};

const static float Spawns[9][3] = 
{
	{3269.590,-3161.287,297.423},
	{3277.797,-3170.352,297.423},
	{3267.049,-3172.820,297.423},
	{3252.157,-3132.135,297.423},
	{3259.990,-3126.590,297.423},
	{3259.815,-3137.576,297.423},
	{3308.030,-3132.135,297.423},
	{3303.046,-3180.682,297.423},
	{3313.283,-3180.766,297.423},
};


struct MANGOS_DLL_DECL boss_gluthAI : public ScriptedAI
{
    boss_gluthAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    uint32 Check_Timer;
    uint32 Summon_Timer;
	uint32 Decimate_Timer;

	MobEventTasks Tasks;

	std::vector<Creature*> GluthAdds;

	void JustDied(Unit* pKiller)
	{
		for (std::vector<Creature*>::iterator itr = GluthAdds.begin(); itr != GluthAdds.end();++itr)
        {
			Creature* tmp = *itr;
			if(tmp)
				tmp->ForcedDespawn();
		}
		GluthAdds.clear();
		GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME);
	}

    void Reset()
    {
		for (std::vector<Creature*>::iterator itr = GluthAdds.begin(); itr != GluthAdds.end();++itr)
        {
			Creature* tmp = *itr;
			if(tmp)
				tmp->ForcedDespawn();
		}
		GluthAdds.clear();

		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();

		Tasks.AddEvent(SPELL_BERSERK,480000,60000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_MORTALWOUND,8000,10000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_DECIMATE,105000,105000,0,TARGET_MAIN);
		if(m_bIsHeroic)
			Tasks.AddEvent(SPELL_ENRAGE_H,60000,60000,0,TARGET_ME);
		else
			Tasks.AddEvent(SPELL_ENRAGE,60000,60000,0,TARGET_ME);
        Check_Timer = 1000;
        Summon_Timer = 10000;
		Decimate_Timer = 105000;

    }

    void UpdateAI(const uint32 diff)
    {
        if (CanDoSomething())
            return;

		if(!GluthAdds.empty())
		{
			if(Check_Timer <= diff)
			{
				for (std::vector<Creature*>::iterator itr = GluthAdds.begin(); itr != GluthAdds.end();++itr)
				{
					if((*itr))
					{
						if((*itr)->isAlive() && (*itr)->GetDistance2d(me->GetPositionX(),me->GetPositionY()) < 7.0f)
						{
							Tasks.Kill((*itr));
							me->SetHealth(me->GetHealth() + me->GetMaxHealth() * 0.04);
						}
					}
				}
				Check_Timer = 1000;
			}
			else
				Check_Timer -= diff;
		}

		if(Decimate_Timer <= diff)
		{
			for (std::vector<Creature*>::iterator itr = GluthAdds.begin(); itr != GluthAdds.end();++itr)
			{
				if((*itr))
				{
					if((*itr)->isAlive())
					{
						if((*itr)->GetHealth() > (*itr)->GetMaxHealth() * 0.05)
							(*itr)->SetHealth((*itr)->GetMaxHealth() * 0.05);
					}
				}
			}

			Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
			if (!lPlayers.isEmpty())
			{
				for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				{
					if (Player* pPlayer = itr->getSource())
						if(pPlayer->isAlive())
						{
							if(pPlayer->GetHealth() > pPlayer->GetMaxHealth() * 0.05)
								pPlayer->SetHealth(pPlayer->GetMaxHealth() * 0.05);
						}
				}
			}
			Decimate_Timer = 105000;
		}
		else
			Decimate_Timer -= diff;
        //Summon_Timer
        if (Summon_Timer <= diff)
        {
			float tmpx,tmpy,tmpz;
			uint16 tmprand = urand(0,8);
			tmpx = Spawns[tmprand][0];
			tmpy = Spawns[tmprand][1];
			tmpz = Spawns[tmprand][2];

            if (Creature* pZombie = me->SummonCreature(NPC_ZOMBIE_CHOW,tmpx,tmpy,tmpz,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,600000))
            {
				GluthAdds.push_back(pZombie);
				pZombie->SetRespawnDelay(RESPAWN_ONE_DAY);
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    pZombie->AddThreat(pTarget,0.0f);
            }

            if (m_bIsHeroic)
            {
                if (Creature* pZombie = me->SummonCreature(NPC_ZOMBIE_CHOW,tmpx,tmpy,tmpz,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,600000))
                {
					GluthAdds.push_back(pZombie);
					pZombie->SetRespawnDelay(RESPAWN_ONE_DAY);
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                        pZombie->AddThreat(pTarget,0.0f);
                }
            }

            Summon_Timer = 10000;
        } 
		else 
			Summon_Timer -= diff;
		
		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gluth(Creature* pCreature)
{
    return new boss_gluthAI(pCreature);
}

void AddSC_boss_gluth()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_gluth";
    newscript->GetAI = &GetAI_boss_gluth;
    newscript->RegisterSelf();
}
