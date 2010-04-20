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
SDName: Boss_Patchwerk
SD%Complete: 80
SDComment: Some issues with hateful strike inturrupting the melee swing timer.
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"

#define SAY_AGGRO1              -1533017
#define SAY_AGGRO2              -1533018
#define SAY_SLAY                -1533019
#define SAY_DEATH               -1533020

#define EMOTE_BERSERK           -1533021
#define EMOTE_ENRAGE            -1533022

#define SPELL_HATEFULSTRIKE     28308
#define H_SPELL_HATEFULSTRIKE   59192
#define SPELL_ENRAGE            28131
#define SPELL_BERSERK           26662
#define SPELL_SLIMEBOLT         32309

struct MANGOS_DLL_DECL boss_patchwerkAI : public ScriptedAI
{
    boss_patchwerkAI(Creature* pCreature) : ScriptedAI(pCreature) {
		Reset();
		m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
	}

	bool m_bIsHeroic;
    uint32 HatefullStrike_Timer;
    uint32 Enrage_Timer;
    uint32 Slimebolt_Timer;
    bool Enraged;

	MobEventTasks Tasks;
    void Reset()
    {
		Tasks.SetObjects(this,me);
        HatefullStrike_Timer = 1200;                        //1.2 seconds
        Enrage_Timer = 420000;                              //7 minutes 420,000
        Slimebolt_Timer = 450000;                           //7.5 minutes 450,000
        Enraged = false;
    }

    void KilledUnit(Unit* Victim)
    {
        if (urand(0, 4))
            return;

        DoScriptText(SAY_SLAY, me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME);
    }

    void Aggro(Unit *who)
    {
        if (urand(0, 1))
            DoScriptText(SAY_AGGRO1, me);
        else
            DoScriptText(SAY_AGGRO2, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        //HatefullStrike_Timer
        if (HatefullStrike_Timer < diff)
        {
            //Cast Hateful strike on the player with the highest
            //amount of HP within melee distance
            uint32 MostHP = 0;
            Unit* pMostHPTarget = NULL;
            Unit* pTemp = NULL;
            ThreatList::const_iterator i = me->getThreatManager().getThreatList().begin();

            for (i = me->getThreatManager().getThreatList().begin(); i!=me->getThreatManager().getThreatList().end(); ++i)
            {
                pTemp = Unit::GetUnit((*me),(*i)->getUnitGuid());
                if (pTemp && pTemp->isAlive() && pTemp->GetHealth() > MostHP && me->IsWithinDist(pTemp, 5.0f, false))
                {
                    MostHP = pTemp->GetHealth();
                    pMostHPTarget = pTemp;
                }
            }

            if (pMostHPTarget)
				DoCast(pMostHPTarget, (m_bIsHeroic) ? H_SPELL_HATEFULSTRIKE : SPELL_HATEFULSTRIKE);

            HatefullStrike_Timer = 1200;
        }else HatefullStrike_Timer -= diff;

        //Enrage_Timer
        if (Enrage_Timer < diff)
        {
            DoCastMe( SPELL_BERSERK);
            DoScriptText(EMOTE_BERSERK, me);
            Enrage_Timer = 300000;
        }else Enrage_Timer -= diff;

        //Slimebolt_Timer
        if (Slimebolt_Timer < diff)
        {
            DoCastVictim(SPELL_SLIMEBOLT);
            Slimebolt_Timer = 5000;
        }
		else 
			Slimebolt_Timer -= diff;

        //Enrage if not already enraged and below 5%
        if (!Enraged && (me->GetHealth()*100 / me->GetMaxHealth()) < 5)
        {
            DoCastMe(SPELL_ENRAGE);
            DoScriptText(EMOTE_ENRAGE,NULL);
            Enraged = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_patchwerk(Creature* pCreature)
{
    return new boss_patchwerkAI(pCreature);
}

void AddSC_boss_patchwerk()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_patchwerk";
    newscript->GetAI = &GetAI_boss_patchwerk;
    newscript->RegisterSelf();
}
