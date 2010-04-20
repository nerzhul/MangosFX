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
SDName: Boss_Taldaram
SD%Complete: 20%
SDComment:
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO                       = -1619008,
    SAY_VANISH_1                    = -1619009,
    SAY_VANISH_2                    = -1619010,
    SAY_FEED_1                      = -1619011,
    SAY_FEED_2                      = -1619012,
    SAY_SLAY_1                      = -1619013,
    SAY_SLAY_2                      = -1619014,
    SAY_SLAY_3                      = -1619015,
    SAY_DEATH                       = -1619016,
	SPELL_DISPARITION				= 55964,
	SPELL_ETREINTE_H				= 59513,
	SPELL_ETREINTE_N				= 55959,
	SPELL_SPHERE					= 55931,
	SPELL_SANGUINAIRE				= 55968,
};

/*######
## boss_taldaram
######*/

struct MANGOS_DLL_DECL boss_taldaramAI : public ScriptedAI
{
	uint32 disparition_timer;
	uint32 etreinte_timer;
	Unit* target;
	MobEventTasks Tasks;

    boss_taldaramAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_SANGUINAIRE,5000,5000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_SPHERE,20000,20000);
		disparition_timer	= 25000 ;
		etreinte_timer		= disparition_timer + 3000 ;
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
        DoScriptText(SAY_DEATH, me);
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (CanDoSomething())
            return;

		if (disparition_timer < uiDiff)
		{
			switch(urand(0,1))
			{
				case 0: DoScriptText(SAY_VANISH_1, me); break;
				case 1: DoScriptText(SAY_VANISH_2, me); break;
            } 
			me->SetVisibility(VISIBILITY_OFF);
			DoCastMe(66830);
			disparition_timer = urand(33000,35000);
		}
		else
			disparition_timer -= uiDiff;

		if (etreinte_timer <= uiDiff)
		{
			me->RemoveAurasDueToSpell(66830);
			me->SetVisibility(VISIBILITY_ON);
			DoCastRandom((m_bIsHeroic) ? SPELL_ETREINTE_H : SPELL_ETREINTE_N);
		}
		else
			etreinte_timer -= uiDiff;

		
		Tasks.UpdateEvent(uiDiff);


        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_taldaram(Creature* pCreature)
{
    return new boss_taldaramAI(pCreature);
}

/*######
## go_nerubian_device
######*/

bool GOHello_go_nerubian_device(Player* pPlayer, GameObject* pGo)
{
    return false;
}

void AddSC_boss_taldaram()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_taldaram";
    newscript->GetAI = &GetAI_boss_taldaram;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_nerubian_device";
    newscript->pGOHello = &GOHello_go_nerubian_device;
    newscript->RegisterSelf();
}
