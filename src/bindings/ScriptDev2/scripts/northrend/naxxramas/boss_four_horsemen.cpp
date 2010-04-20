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
SDName: Boss_Four_Horsemen
SD%Complete: 75
SDComment: Lady Blaumeux, Thane Korthazz, Sir Zeliek, Baron Rivendare
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//all horsemen
#define SPELL_SHIELDWALL            29061
#define SPELL_BESERK                26662

//lady blaumeux
#define SAY_BLAU_AGGRO              -1533044
#define SAY_BLAU_TAUNT1             -1533045
#define SAY_BLAU_TAUNT2             -1533046
#define SAY_BLAU_TAUNT3             -1533047
#define SAY_BLAU_SPECIAL            -1533048
#define SAY_BLAU_SLAY               -1533049
#define SAY_BLAU_DEATH              -1533050

#define SPELL_MARK_OF_BLAUMEUX      28833
#define SPELL_UNYILDING_PAIN        57381
#define SPELL_VOIDZONE              28863
#define H_SPELL_VOIDZONE            57463
#define SPELL_SHADOW_BOLT           57374
#define H_SPELL_SHADOW_BOLT         57464

#define C_SPIRIT_OF_BLAUMEUX        16776

struct MANGOS_DLL_DECL boss_lady_blaumeuxAI : public ScriptedAI
{
    boss_lady_blaumeuxAI(Creature* pCreature) : ScriptedAI(pCreature) {
		Reset();
		m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
	}

	bool m_bIsHeroic;
    MobEventTasks Tasks;
    bool ShieldWall1;
    bool ShieldWall2;
	uint32 MarkTimer;

    void Reset()
    {
		Tasks.SetObjects(this,me);

		MarkTimer = 20000;
		//Tasks.AddEvent(SPELL_MARK_OF_BLAUMEUX,20000,12000,0,TARGET_MAIN,0,0,true);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(H_SPELL_SHADOW_BOLT,1500,2000,0,TARGET_NEAR);
			Tasks.AddEvent(H_SPELL_VOIDZONE,12000,12000,0,TARGET_MAIN,0,0,true);
		}
		else
		{
			Tasks.AddEvent(SPELL_SHADOW_BOLT,1500,2000,0,TARGET_NEAR);
			Tasks.AddEvent(SPELL_VOIDZONE,12000,12000,0,TARGET_MAIN,0,0,true);
		}
        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_BLAU_AGGRO, me);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_BLAU_SLAY, me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_BLAU_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (me->GetHealth()*100 / me->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (me->GetHealth()*100 / me->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

		if(MarkTimer <= diff)
		{
			if(me->getVictim())
			{
				int32 dmg = 0;
				if(me->getVictim()->HasAura(SPELL_MARK_OF_BLAUMEUX))
					dmg = me->getVictim()->GetAura(SPELL_MARK_OF_BLAUMEUX,0)->GetStackAmount() * 750;
				me->CastCustomSpell(me->getVictim(),SPELL_MARK_OF_BLAUMEUX,&dmg,0,0,false);
			}
			MarkTimer = 12000;
		}
		else
			MarkTimer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lady_blaumeux(Creature* pCreature)
{
    return new boss_lady_blaumeuxAI(pCreature);
}

//baron rivendare
#define SAY_RIVE_AGGRO1             -1533065
#define SAY_RIVE_AGGRO2             -1533066
#define SAY_RIVE_AGGRO3             -1533067
#define SAY_RIVE_SLAY1              -1533068
#define SAY_RIVE_SLAY2              -1533069
#define SAY_RIVE_SPECIAL            -1533070
#define SAY_RIVE_TAUNT1             -1533071
#define SAY_RIVE_TAUNT2             -1533072
#define SAY_RIVE_TAUNT3             -1533073
#define SAY_RIVE_DEATH              -1533074

#define SPELL_MARK_OF_RIVENDARE     28834
#define SPELL_UNHOLY_SHADOW         28882
#define H_SPELL_UNHOLY_SHADOW       57369

#define C_SPIRIT_OF_RIVENDARE       0                       //creature entry not known yet

struct MANGOS_DLL_DECL boss_rivendare_naxxAI : public ScriptedAI
{
    boss_rivendare_naxxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    uint32 UnholyShadow_Timer;
    bool ShieldWall1;
    bool ShieldWall2;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);

		Tasks.AddEvent(SPELL_MARK_OF_RIVENDARE,20000,15000,0,TARGET_MAIN,0,0,true);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(H_SPELL_UNHOLY_SHADOW,15000,15000,0,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_UNHOLY_SHADOW,15000,15000,0,TARGET_MAIN);
		}

        UnholyShadow_Timer = 15000;
        ShieldWall1 = true;
        ShieldWall2 = true;

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, NOT_STARTED);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_KORTHAZZ))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_BLAUMEUX))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_ZELIEK))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
        }
    }

    void Aggro(Unit *who)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_RIVE_AGGRO1, me); break;
            case 1: DoScriptText(SAY_RIVE_AGGRO2, me); break;
            case 2: DoScriptText(SAY_RIVE_AGGRO3, me); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* Victim)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_RIVE_SLAY1, me); break;
            case 1: DoScriptText(SAY_RIVE_SLAY2, me); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_RIVE_DEATH, me);

        if (m_pInstance)
        {
            bool HorsemenDead = true;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_KORTHAZZ))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_BLAUMEUX))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_ZELIEK))))
                if (pTemp->isAlive())
                    HorsemenDead = false;

            if (HorsemenDead)
                m_pInstance->SetData(TYPE_FOUR_HORSEMEN, DONE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (me->GetHealth()*100 / me->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (me->GetHealth()*100 / me->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rivendare_naxx(Creature* pCreature)
{
    return new boss_rivendare_naxxAI(pCreature);
}

//thane korthazz
#define SAY_KORT_AGGRO              -1533051
#define SAY_KORT_TAUNT1             -1533052
#define SAY_KORT_TAUNT2             -1533053
#define SAY_KORT_TAUNT3             -1533054
#define SAY_KORT_SPECIAL            -1533055
#define SAY_KORT_SLAY               -1533056
#define SAY_KORT_DEATH              -1533057

#define SPELL_MARK_OF_KORTHAZZ      28832
#define SPELL_METEOR                26558                   // me->getVictim() auto-area spell but with a core problem
#define SPELL_METEOR_H				57467

#define C_SPIRIT_OF_KORTHAZZ        16778

struct MANGOS_DLL_DECL boss_thane_korthazzAI : public ScriptedAI
{
    boss_thane_korthazzAI(Creature* pCreature) : ScriptedAI(pCreature) {
		Reset();
		m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
	}

    uint32 UnholyShadow_Timer;
    bool ShieldWall1;
    bool ShieldWall2;
	bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);

		Tasks.AddEvent(SPELL_MARK_OF_KORTHAZZ,20000,12000,0,TARGET_MAIN,0,0,true);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_METEOR_H,30000,20000,0,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_METEOR,30000,20000,0,TARGET_MAIN);
		}
        UnholyShadow_Timer = 15000;

		ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_KORT_AGGRO, me);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_KORT_SLAY, me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_KORT_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (me->GetHealth()*100 / me->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (me->GetHealth()*100 / me->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thane_korthazz(Creature* pCreature)
{
    return new boss_thane_korthazzAI(pCreature);
}

//sir zeliek
#define SAY_ZELI_AGGRO              -1533058
#define SAY_ZELI_TAUNT1             -1533059
#define SAY_ZELI_TAUNT2             -1533060
#define SAY_ZELI_TAUNT3             -1533061
#define SAY_ZELI_SPECIAL            -1533062
#define SAY_ZELI_SLAY               -1533063
#define SAY_ZELI_DEATH              -1533064

#define SPELL_MARK_OF_ZELIEK        28835
#define SPELL_HOLY_WRATH            28883
#define H_SPELL_HOLY_WRATH          57466
#define SPELL_HOLY_BOLT             57376
#define H_SPELL_HOLY_BOLT           57465

#define C_SPIRIT_OF_ZELIREK         16777

struct MANGOS_DLL_DECL boss_sir_zeliekAI : public ScriptedAI
{
    boss_sir_zeliekAI(Creature* pCreature) : ScriptedAI(pCreature) {
		m_bIsHeroic = !pCreature->GetMap()->IsRegularDifficulty();
		Reset();
	}

	bool m_bIsHeroic;
    bool ShieldWall1;
    bool ShieldWall2;

	MobEventTasks Tasks;

	uint32 Mark_Timer;

    void Reset()
    {
		Tasks.SetObjects(this,me);

		//Tasks.AddEvent(SPELL_MARK_OF_ZELIEK,20000,12000,0,TARGET_MAIN,0,0,true);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(H_SPELL_HOLY_WRATH,12000,12000,0,TARGET_MAIN,0,0,true);
			Tasks.AddEvent(H_SPELL_HOLY_BOLT,1500,2000,0,TARGET_NEAR);
		}
		else
		{
			Tasks.AddEvent(SPELL_HOLY_WRATH,12000,12000,0,TARGET_MAIN,0,0,true);
			Tasks.AddEvent(SPELL_HOLY_BOLT,1500,2000,0,TARGET_NEAR);
		}

		Mark_Timer = 20000;

        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_ZELI_AGGRO, me);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_ZELI_SLAY, me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_ZELI_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (me->GetHealth()*100 / me->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (me->GetHealth()*100 / me->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCastMe(SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

		if(Mark_Timer <= diff)
		{
			me->CastStop();
			DoCastVictim(SPELL_MARK_OF_ZELIEK);
			/*for (ThreatList::const_iterator i = me->getThreatManager().getThreatList().begin();i != me->getThreatManager().getThreatList().end(); ++i)
			{
				Unit* pUnit = NULL;
				pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
				if (!pUnit)
					continue;
				if(pUnit->GetDistance2d(me) < 25.0f)
				{
					me->DealDamage(pUnit, 1000, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_HOLY, NULL, false);
				}
			}*/
			
			Mark_Timer = 20000;
		}
		else
			Mark_Timer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_boss_sir_zeliek(Creature* pCreature)
{
    return new boss_sir_zeliekAI(pCreature);
}

void AddSC_boss_four_horsemen()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lady_blaumeux";
    newscript->GetAI = &GetAI_boss_lady_blaumeux;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_rivendare_naxx";
    newscript->GetAI = &GetAI_boss_rivendare_naxx;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_thane_korthazz";
    newscript->GetAI = &GetAI_boss_thane_korthazz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_sir_zeliek";
    newscript->GetAI = &GetAI_boss_sir_zeliek;
    newscript->RegisterSelf();
}
