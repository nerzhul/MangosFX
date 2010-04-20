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
SDName: Boss_Razuvious
SD%Complete: 50
SDComment: Missing adds and event is impossible without Mind Control
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//Razuvious - NO TEXT sound only
//8852 aggro01 - Hah hah, I'm just getting warmed up!
//8853 aggro02 Stand and fight!
//8854 aggro03 Show me what you've got!
//8861 slay1 - You should've stayed home!
//8863 slay2-
//8858 cmmnd3 - You disappoint me, students!
//8855 cmmnd1 - Do as I taught you!
//8856 cmmnd2 - Show them no mercy!
//8859 cmmnd4 - The time for practice is over! Show me what you've learned!
//8861 Sweep the leg! Do you have a problem with that?
//8860 death - An honorable... death...
//8947 - Aggro Mixed? - ?

#define SOUND_AGGRO1    8852
#define SOUND_AGGRO2    8853
#define SOUND_AGGRO3    8854
#define SOUND_SLAY1     8861
#define SOUND_SLAY2     8863
#define SOUND_COMMND1   8855
#define SOUND_COMMND2   8856
#define SOUND_COMMND3   8858
#define SOUND_COMMND4   8859
#define SOUND_COMMND5   8861
#define SOUND_DEATH     8860
#define SOUND_AGGROMIX  8847

#define SPELL_UNBALANCING_STRIKE     55470
#define SPELL_DISRUPTING_SHOUT       55543
#define SPELL_DISRUPTING_SHOUT_H     29107
#define SPELL_HOPELESS               29125
#define SPELL_JAGGED_KNIFE           55550

#define NPC_DEATH_KNIGHT_UNDERSTUDY  16803

struct MANGOS_DLL_DECL boss_razuviousAI : public ScriptedAI
{
    boss_razuviousAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    std::list<uint64> DeathKnightList;

    uint32 CommandSound_Timer;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_UNBALANCING_STRIKE,30000,30000,0,TARGET_MAIN);
		if(m_bIsHeroic)
			Tasks.AddEvent(SPELL_DISRUPTING_SHOUT_H,25000,25000,0,TARGET_MAIN);
		else
			Tasks.AddEvent(SPELL_DISRUPTING_SHOUT,25000,25000,0,TARGET_MAIN);

        CommandSound_Timer = 40000;                         //40 seconds

        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZUVIOUS, NOT_STARTED);
    }

    void KilledUnit(Unit* Victim)
    {
        if (rand()%3)
            return;

        switch (rand()%2)
        {
            case 0:
                DoPlaySoundToSet(me, SOUND_SLAY1);
                break;
            case 1:
                DoPlaySoundToSet(me, SOUND_SLAY2);
                break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoPlaySoundToSet(me, SOUND_DEATH);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZUVIOUS, DONE);

        if (!DeathKnightList.empty())
        {
            for(std::list<uint64>::iterator itr = DeathKnightList.begin(); itr != DeathKnightList.end(); ++itr)
            {
                Creature* pDeathKnight = NULL;
                pDeathKnight = ((Creature*)Unit::GetUnit(*me, *itr));

                if (pDeathKnight)
                    if (pDeathKnight->isAlive())
                        pDeathKnight->CastSpell(pDeathKnight, SPELL_HOPELESS, true);
            }
        }
		Tasks.GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME);
    }

    void Aggro(Unit *who)
    {
        switch (rand()%3)
        {
            case 0:
                DoPlaySoundToSet(me, SOUND_AGGRO1);
                break;
            case 1:
                DoPlaySoundToSet(me, SOUND_AGGRO2);
                break;
            case 2:
                DoPlaySoundToSet(me, SOUND_AGGRO3);
                break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZUVIOUS, IN_PROGRESS);

        FindDeathKnight();

        if (!DeathKnightList.empty())
        {
            for(std::list<uint64>::iterator itr = DeathKnightList.begin(); itr != DeathKnightList.end(); ++itr)
            {
                if (Creature* pDeathKnight = ((Creature*)Unit::GetUnit(*me, *itr)))
                {
                    if (pDeathKnight->isDead())
                    {
                        pDeathKnight->RemoveCorpse();
                        pDeathKnight->Respawn();
                    }

                    pDeathKnight->AI()->AttackStart(who);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;
		
		Tasks.UpdateEvent(diff);

        //CommandSound_Timer
        if (CommandSound_Timer < diff)
        {
            switch (rand()%5)
            {
                case 0:
                    DoPlaySoundToSet(me, SOUND_COMMND1);
                    break;
                case 1:
                    DoPlaySoundToSet(me, SOUND_COMMND2);
                    break;
                case 2:
                    DoPlaySoundToSet(me, SOUND_COMMND3);
                    break;
                case 3:
                    DoPlaySoundToSet(me, SOUND_COMMND4);
                    break;
                case 4:
                    DoPlaySoundToSet(me, SOUND_COMMND5);
                    break;
            }

            CommandSound_Timer = 40000;
        }
		else 
			CommandSound_Timer -= diff;

        DoMeleeAttackIfReady();
    }

    void FindDeathKnight()
    {
        std::list<Creature*> DeathKnight;
        GetCreatureListWithEntryInGrid(DeathKnight, me, NPC_DEATH_KNIGHT_UNDERSTUDY, 50.0f);

        if (!DeathKnight.empty())
        {
            DeathKnightList.clear();

            for(std::list<Creature*>::iterator itr = DeathKnight.begin(); itr != DeathKnight.end(); ++itr)
                DeathKnightList.push_back((*itr)->GetGUID());
        }
    }

};

CreatureAI* GetAI_boss_razuvious(Creature* pCreature)
{
    return new boss_razuviousAI(pCreature);
}

void AddSC_boss_razuvious()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_razuvious";
    newscript->GetAI = &GetAI_boss_razuvious;
    newscript->RegisterSelf();
}
