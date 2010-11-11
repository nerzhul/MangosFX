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
SDName: Boss Loken
SD%Complete: 60%
SDComment: Missing intro. Remove hack of Pulsing Shockwave when core supports. Aura is not working (59414)
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "halls_of_lightning.h"
#include "Map.h"

enum
{
    SAY_AGGRO                           = -1602018,
    SAY_INTRO_1                         = -1602019,
    SAY_INTRO_2                         = -1602020,
    SAY_SLAY_1                          = -1602021,
    SAY_SLAY_2                          = -1602022,
    SAY_SLAY_3                          = -1602023,
    SAY_DEATH                           = -1602024,
    SAY_NOVA_1                          = -1602025,
    SAY_NOVA_2                          = -1602026,
    SAY_NOVA_3                          = -1602027,
    SAY_75HEALTH                        = -1602028,
    SAY_50HEALTH                        = -1602029,
    SAY_25HEALTH                        = -1602030,
    EMOTE_NOVA                          = -1602031,

    SPELL_ARC_LIGHTNING                 = 52921,
    SPELL_LIGHTNING_NOVA_N              = 52960,
    SPELL_LIGHTNING_NOVA_H              = 59835,

    SPELL_PULSING_SHOCKWAVE_N           = 52961,
    SPELL_PULSING_SHOCKWAVE_H           = 59836,
    SPELL_PULSING_SHOCKWAVE_AURA        = 59414
};

class MANGOS_DLL_DECL BugAura : public Aura
{
    public:
        BugAura(SpellEntry *spell, uint32 eff, int32 *bp, Unit *target, Unit *caster) : Aura(spell, eff, bp, target, caster, NULL)
            {}
};

/*######
## Boss Loken
######*/

struct MANGOS_DLL_DECL boss_lokenAI : public ScriptedAI
{
    boss_lokenAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;

    bool m_bIsHeroic;
    bool m_bIsAura;

    uint32 m_uiArcLightning_Timer;
    uint32 m_uiLightningNova_Timer;
    uint32 m_uiPulsingShockwave_Timer;
    uint32 m_uiResumePulsingShockwave_Timer;

    uint32 m_uiHealthAmountModifier;

    void Reset()
    {
        m_bIsAura = false;

        m_uiArcLightning_Timer = 15000;
        m_uiLightningNova_Timer = 20000;
        m_uiPulsingShockwave_Timer = 2000;
        m_uiResumePulsingShockwave_Timer = 15000;

        m_uiHealthAmountModifier = 1;

        if (pInstance)
            pInstance->SetData(TYPE_LOKEN, NOT_STARTED);

		Map::PlayerList const &PlayerList =  me->GetMap()->GetPlayers();
		for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
				i->getSource()->RemoveAurasDueToSpell(SPELL_PULSING_SHOCKWAVE_AURA);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);

        if (pInstance)
            pInstance->SetData(TYPE_LOKEN, IN_PROGRESS);

		Map::PlayerList const &PlayerList =  me->GetMap()->GetPlayers();
		for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
			{
				i->getSource()->AddAura(new BugAura((SpellEntry*)GetSpellStore()->LookupEntry(SPELL_PULSING_SHOCKWAVE_AURA), 0, NULL, i->getSource(),i->getSource()));
				i->getSource()->AddAura(new BugAura((SpellEntry*)GetSpellStore()->LookupEntry(SPELL_PULSING_SHOCKWAVE_AURA), 1, NULL, i->getSource(),i->getSource()));
				i->getSource()->AddAura(new BugAura((SpellEntry*)GetSpellStore()->LookupEntry(SPELL_PULSING_SHOCKWAVE_AURA), 2, NULL, i->getSource(),i->getSource()));
			}
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		Map::PlayerList const &PlayerList =  me->GetMap()->GetPlayers();
		for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
				i->getSource()->RemoveAurasDueToSpell(SPELL_PULSING_SHOCKWAVE_AURA);

        if (pInstance)
            pInstance->SetData(TYPE_LOKEN, DONE);
		GiveRandomReward();
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, me);break;
            case 1: DoScriptText(SAY_SLAY_2, me);break;
            case 2: DoScriptText(SAY_SLAY_3, me);break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        if (m_bIsAura)
        {
            // workaround for PULSING_SHOCKWAVE
            /*if (m_uiPulsingShockwave_Timer < diff)
            {
                Map *map = me->GetMap();
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (PlayerList.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
                        {
                            int32 dmg;
                            float m_fDist = me->GetDistance(i->getSource());

                            if (m_fDist <= 1.0f) // Less than 1 yard
                                dmg = (m_bIsHeroic ? 850 : 800); // need to correct damage
                            else // Further from 1 yard
                                dmg = round((m_bIsHeroic ? 250 : 200) * m_fDist) + (m_bIsHeroic ? 850 : 800); // need to correct damage

                            me->CastCustomSpell(i->getSource(), (m_bIsHeroic ? 59837 : 52942), &dmg, 0, 0, false);
                        }
                }
                m_uiPulsingShockwave_Timer = 2000;
            }else m_uiPulsingShockwave_Timer -= diff;*/
        }
        else
        {
            if (m_uiResumePulsingShockwave_Timer < diff)
            {
                //breaks at movement, can we assume when it's time, this spell is casted and also must stop movement?
                //me->CastSpell(me, SPELL_PULSING_SHOCKWAVE_AURA, true);

                DoCastMe( m_bIsHeroic ? SPELL_PULSING_SHOCKWAVE_H : SPELL_PULSING_SHOCKWAVE_N); // need core support
                m_bIsAura = true;
                m_uiResumePulsingShockwave_Timer = 2000;
            }
            else
                m_uiResumePulsingShockwave_Timer -= diff;
        }

        if (m_uiArcLightning_Timer < diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_ARC_LIGHTNING);

            m_uiArcLightning_Timer = urand(15000, 16000);
        }
        else
            m_uiArcLightning_Timer -= diff;

        if (m_uiLightningNova_Timer < diff)
        {
            switch(urand(0, 2))
            {
                case 0: DoScriptText(SAY_NOVA_1, me);break;
                case 1: DoScriptText(SAY_NOVA_2, me);break;
                case 2: DoScriptText(SAY_NOVA_3, me);break;
            }

            DoCastMe( m_bIsHeroic ? SPELL_LIGHTNING_NOVA_H : SPELL_LIGHTNING_NOVA_N);

            m_bIsAura = false;
            m_uiResumePulsingShockwave_Timer = (m_bIsHeroic ? 4000 : 5000); // Pause Pulsing Shockwave aura
            m_uiLightningNova_Timer = urand(20000, 21000);
        }
        else
            m_uiLightningNova_Timer -= diff;

        // Health check
        if ((me->GetHealth()*100 / me->GetMaxHealth()) < (100-(25*m_uiHealthAmountModifier)))
        {
            switch(m_uiHealthAmountModifier)
            {
                case 1: DoScriptText(SAY_75HEALTH, me); break;
                case 2: DoScriptText(SAY_50HEALTH, me); break;
                case 3: DoScriptText(SAY_25HEALTH, me); break;
            }

            ++m_uiHealthAmountModifier;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_loken(Creature* pCreature)
{
    return new boss_lokenAI(pCreature);
}

void AddSC_boss_loken()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_loken";
    newscript->GetAI = &GetAI_boss_loken;
    newscript->RegisterSelf();
}
