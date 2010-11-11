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
SDName: boss_kri, boss_yauj, boss_vem : The Bug Trio
SD%Complete: 100
SDComment:
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"

#define SPELL_CLEAVE        26350
#define SPELL_TOXIC_VOLLEY  25812
#define SPELL_POISON_CLOUD  38718                           //Only Spell with right dmg.
#define SPELL_ENRAGE        34624                           //Changed cause 25790 is casted on gamers too. Same prob with old explosion of twin emperors.

#define SPELL_CHARGE        26561
#define SPELL_KNOCKBACK     26027

#define SPELL_HEAL      25807
#define SPELL_FEAR      19408

struct MANGOS_DLL_DECL boss_kriAI : public ScriptedAI
{
    boss_kriAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 Cleave_Timer;
    uint32 ToxicVolley_Timer;
    uint32 Check_Timer;

    bool VemDead;
    bool Death;

    void Reset()
    {
        Cleave_Timer = urand(4000, 8000);
        ToxicVolley_Timer = urand(6000, 12000);
        Check_Timer = 2000;

        VemDead = false;
        Death = false;
    }

    void JustDied(Unit* killer)
    {
        if (pInstance)
        {
            if (pInstance->GetData(DATA_BUG_TRIO_DEATH) < 2)
                                                            // Unlootable if death
                me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            pInstance->SetData(DATA_BUG_TRIO_DEATH, 1);
        }
    }
    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //Cleave_Timer
        if (Cleave_Timer < diff)
        {
            DoCastVictim(SPELL_CLEAVE);
            Cleave_Timer = urand(5000, 12000);
        }else Cleave_Timer -= diff;

        //ToxicVolley_Timer
        if (ToxicVolley_Timer < diff)
        {
            DoCastVictim(SPELL_TOXIC_VOLLEY);
            ToxicVolley_Timer = urand(10000, 15000);
        }else ToxicVolley_Timer -= diff;

        if (me->GetHealth() <= me->GetMaxHealth() * 0.05 && !Death)
        {
            DoCastVictim(SPELL_POISON_CLOUD);
            Death = true;
        }

        if (!VemDead)
        {
            //Checking if Vem is dead. If yes we will enrage.
            if (Check_Timer < diff)
            {
                if (pInstance && pInstance->GetData(TYPE_VEM) == DONE)
                {
                    DoCastMe( SPELL_ENRAGE);
                    VemDead = true;
                }
                Check_Timer = 2000;
            }else Check_Timer -=diff;
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_vemAI : public ScriptedAI
{
    boss_vemAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 Charge_Timer;
    uint32 KnockBack_Timer;
    uint32 Enrage_Timer;

    bool Enraged;

    void Reset()
    {
        Charge_Timer = urand(15000, 27000);
        KnockBack_Timer = urand(8000, 20000);
        Enrage_Timer = 120000;

        Enraged = false;
    }

    void JustDied(Unit* Killer)
    {
        if (pInstance)
        {
            pInstance->SetData(TYPE_VEM, DONE);

            // Unlootable if death
            if (pInstance->GetData(DATA_BUG_TRIO_DEATH) < 2)
                me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            pInstance->SetData(DATA_BUG_TRIO_DEATH, 1);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //Charge_Timer
        if (Charge_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, SPELL_CHARGE);

            Charge_Timer = urand(8000, 16000);
        }else Charge_Timer -= diff;

        //KnockBack_Timer
        if (KnockBack_Timer < diff)
        {
            DoCastVictim(SPELL_KNOCKBACK);
            if (me->getThreatManager().getThreat(me->getVictim()))
                me->getThreatManager().modifyThreatPercent(me->getVictim(),-80);
            KnockBack_Timer = urand(15000, 25000);
        }else KnockBack_Timer -= diff;

        //Enrage_Timer
        if (!Enraged && Enrage_Timer < diff)
        {
            DoCastMe(SPELL_ENRAGE);
            Enraged = true;
        }else Charge_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_yaujAI : public ScriptedAI
{
    boss_yaujAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 Heal_Timer;
    uint32 Fear_Timer;
    uint32 Check_Timer;

    bool VemDead;

    void Reset()
    {
        Heal_Timer = urand(25000, 40000);
        Fear_Timer = urand(12000, 24000);
        Check_Timer = 2000;

        VemDead = false;
    }

    void JustDied(Unit* Killer)
    {
        if (pInstance)
        {
            if (pInstance->GetData(DATA_BUG_TRIO_DEATH) < 2)
                                                            // Unlootable if death
                me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            pInstance->SetData(DATA_BUG_TRIO_DEATH, 1);
        }

        for(int i = 0; i < 10; ++i)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0);
            Creature* Summoned = me->SummonCreature(15621,me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(),0,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,90000);
            if (Summoned)
                ((CreatureAI*)Summoned->AI())->AttackStart(target);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        //Fear_Timer
        if (Fear_Timer < diff)
        {
            DoCastVictim(SPELL_FEAR);
            DoResetThreat();
            Fear_Timer = 20000;
        }else Fear_Timer -= diff;

        //Casting Heal to other twins or herself.
        if (Heal_Timer < diff)
        {
            if (pInstance)
            {
                Unit *pKri = Unit::GetUnit((*me), pInstance->GetData64(DATA_KRI));
                Unit *pVem = Unit::GetUnit((*me), pInstance->GetData64(DATA_VEM));

                switch(urand(0, 2))
                {
                    case 0:
                        if (pKri)
                            DoCast(pKri, SPELL_HEAL);
                        break;
                    case 1:
                        if (pVem)
                            DoCast(pVem, SPELL_HEAL);
                        break;
                    case 2:
                        DoCastMe( SPELL_HEAL);
                        break;
                }
            }

            Heal_Timer = urand(15000, 30000);
        }else Heal_Timer -= diff;

        //Checking if Vem is dead. If yes we will enrage.
        if (Check_Timer < diff)
        {
            if (!VemDead)
            {
                if (pInstance)
                {
                    if (pInstance->GetData(TYPE_VEM) == DONE)
                    {
                        DoCastMe( SPELL_ENRAGE);
                        VemDead = true;
                    }
                }
            }
            Check_Timer = 2000;
        }else Check_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_yauj(Creature* pCreature)
{
    return new boss_yaujAI(pCreature);
}

CreatureAI* GetAI_boss_vem(Creature* pCreature)
{
    return new boss_vemAI(pCreature);
}

CreatureAI* GetAI_boss_kri(Creature* pCreature)
{
    return new boss_kriAI(pCreature);
}

void AddSC_bug_trio()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kri";
    newscript->GetAI = &GetAI_boss_kri;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_vem";
    newscript->GetAI = &GetAI_boss_vem;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_yauj";
    newscript->GetAI = &GetAI_boss_yauj;
    newscript->RegisterSelf();
}
