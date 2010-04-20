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
SDName: Boss_Grobbulus
SDAuthor: ckegg
SD%Complete: 0
SDComment: Place holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

#define SPELL_BOMBARD_SLIME         28280

#define SPELL_MUTATING_INJECTION    28169
#define SPELL_SLIME_SPRAY           28157
#define H_SPELL_SLIME_SPRAY         54364
#define SPELL_BERSERK               26662

#define MOB_FALLOUT_SLIME   16290

struct MANGOS_DLL_DECL boss_grobbulusAI : public ScriptedAI
{
    boss_grobbulusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_BOMBARD_SLIME,10000,20000,0,TARGET_ME);
		Tasks.AddEvent(SPELL_MUTATING_INJECTION,20000,20000,0,TARGET_RANDOM);
		Tasks.AddEvent(SPELL_BERSERK,300000,60000,0,TARGET_ME);

		if(m_bIsHeroic)
			Tasks.AddEvent(H_SPELL_SLIME_SPRAY,20000,20000,15000,TARGET_MAIN);
		else
			Tasks.AddEvent(SPELL_SLIME_SPRAY,20000,20000,15000,TARGET_MAIN);
	
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, NOT_STARTED);
    }

    void JustDied(Unit* Killer)
    {
		Tasks.CleanMyAdds();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, DONE);
		GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME);
    }

    void Aggro(Unit *who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GROBBULUS, IN_PROGRESS);
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_SLIME_SPRAY || spell->Id == H_SPELL_SLIME_SPRAY)
        {
			if(target)
				Tasks.CallCreature(MOB_FALLOUT_SLIME,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_grobbulus_poison_cloudAI : public Scripted_NoMovementAI
{
    npc_grobbulus_poison_cloudAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint32 Cloud_Timer;

    void Reset()
    {
        Cloud_Timer = 1000;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (Cloud_Timer < diff)
        {
            DoCastMe(59116);
            Cloud_Timer = 10000;
        }else Cloud_Timer -= diff;
    }
};


struct MANGOS_DLL_DECL mob_grobbulusnueeAI : public ScriptedAI
{
    mob_grobbulusnueeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

	void Reset()
	{}

	void UpdateAI(const uint32 uiDiff)
	{
		if(!me->HasAura(28156))
			DoCastMe(28156);
	}
};

CreatureAI* GetAI_mob_grobbulusnueeAI(Creature* pCreature)
{
    return new mob_grobbulusnueeAI(pCreature);

}
CreatureAI* GetAI_boss_grobbulus(Creature* pCreature)
{
    return new boss_grobbulusAI(pCreature);
}

CreatureAI* GetAI_npc_grobbulus_poison_cloud(Creature* pCreature)
{
    return new npc_grobbulus_poison_cloudAI(pCreature);
}

void AddSC_boss_grobbulus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_grobbulus";
    newscript->GetAI = &GetAI_boss_grobbulus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_grobbulus_poison_cloud";
    newscript->GetAI = &GetAI_npc_grobbulus_poison_cloud;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_grobbulus_nuee";
    newscript->GetAI = &GetAI_mob_grobbulusnueeAI;
    newscript->RegisterSelf();

}
