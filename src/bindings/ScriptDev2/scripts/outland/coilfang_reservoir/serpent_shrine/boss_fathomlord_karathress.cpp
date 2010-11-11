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
SDName: Boss_Fathomlord_Karathress
SD%Complete: 60
SDComment: Missing Multishot, Totems, Windfury, Whirlwind
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "precompiled.h"
#include "serpent_shrine.h"

enum
{
    SAY_AGGRO                       = -1548021,
    SAY_GAIN_BLESSING               = -1548022,
    SAY_GAIN_ABILITY1               = -1548023,
    SAY_GAIN_ABILITY2               = -1548024,
    SAY_GAIN_ABILITY3               = -1548025,
    SAY_SLAY1                       = -1548026,
    SAY_SLAY2                       = -1548027,
    SAY_SLAY3                       = -1548028,
    SAY_DEATH                       = -1548029,

    //Karathress spells
    SPELL_CATACLYSMIC_BOLT          = 38441,
    SPELL_POWER_OF_SHARKKIS         = 38455,
    SPELL_POWER_OF_TIDALVESS        = 38452,
    SPELL_POWER_OF_CARIBDIS         = 38451,
    SPELL_ENRAGE                    = 24318,
    SPELL_SEAR_NOVA                 = 38445,
    SPELL_BLESSING_OF_THE_TIDES     = 38449,

    //Sharkkis spells
    SPELL_LEECHING_THROW            = 29436,
    SPELL_THE_BEAST_WITHIN          = 38373,
    SPELL_HURL_TRIDENT              = 38374,
    SPELL_MULTI_TOSS                = 38366,
    SPELL_SUMMON_FATHOM_LURKER      = 38433,
    SPELL_SUMMON_FATHOM_SPOREBAT    = 38431,

    //Tidalvess spells
    SPELL_FROST_SHOCK               = 38234,
    SPELL_SPITFIRE_TOTEM            = 38236,
    SPELL_POISON_CLEANSING_TOTEM    = 38306,
    SPELL_EARTHBIND_TOTEM           = 38304,
    SPELL_WINDFURY_WEAPON           = 32911,                // triggers spell 32912 (Windfury)

    //Caribdis Spells
    SPELL_WATER_BOLT_VOLLEY         = 38335,
    SPELL_TIDAL_SURGE               = 38353,                // triggers 38357
    SPELL_HEAL                      = 38330,
    SPELL_SUMMON_CYCLONE            = 38337,                // summons creature 22104 which uses spell 29538

    MAX_ADVISORS                    = 3,

    NPC_SEER_OLUM                   = 22820
};

// position for Seer Olum
const float afCoords_Olum[] = {446.78f, -542.76f, -7.54773f, 0.401581f};

//Fathom-Lord Karathress AI
struct MANGOS_DLL_DECL boss_fathomlord_karathressAI : public ScriptedAI
{
    boss_fathomlord_karathressAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        memset(&m_auiAdvisorsGUID, 0, sizeof(m_auiAdvisorsGUID));
        Reset();
    }

    ScriptedInstance* pInstance;

    // timers
    uint32 m_uiCataclysmicBolt_Timer;
    uint32 m_uiEnrage_Timer;

    bool   m_bBlessingOfTides_MobsChecked;

    uint64 m_auiAdvisorsGUID[MAX_ADVISORS];                     // the GUIDs from the advisors

    void Reset()
    {
        m_uiCataclysmicBolt_Timer      = 10000;
        m_uiEnrage_Timer               = 600000;
        m_bBlessingOfTides_MobsChecked = false;

        for(uint8 i = 0; i < MAX_ADVISORS; ++i)
        {
            if (Creature* pAdvisor = (Creature*)Unit::GetUnit(*me, m_auiAdvisorsGUID[i]))
            {
                if (pAdvisor->getVictim())
                    pAdvisor->AI()->EnterEvadeMode();
                else if (!pAdvisor->isAlive())
                    pAdvisor->Respawn();
            }
        }

        if (pInstance)
            pInstance->SetData(TYPE_KARATHRESS_EVENT, NOT_STARTED);
    }

    // select the spell and the text based on the advisor which died
    void EventAdvisorDeath(uint8 uiAdvisor)
    {
        if(!me->isAlive())
            return;

        int32  iSayGainAbility = 0;
        uint32 uiSpell = 0;

        switch(uiAdvisor)
        {
            case DATA_SHARKKIS:
                iSayGainAbility = SAY_GAIN_ABILITY1;
                uiSpell = SPELL_POWER_OF_SHARKKIS;
                break;
            case DATA_TIDALVESS:
                iSayGainAbility = SAY_GAIN_ABILITY2;
                uiSpell = SPELL_POWER_OF_TIDALVESS;
                break;
            case DATA_CARIBDIS:
                iSayGainAbility = SAY_GAIN_ABILITY3;
                uiSpell = SPELL_POWER_OF_CARIBDIS;
                break;
            default:
                error_log("SD2: invalid advisor (id %u) for karathress!", uiAdvisor);
                break;
        }

        DoScriptText(iSayGainAbility, me);
        DoCastMe( uiSpell);
    }

    void GetAdvisors()
    {
        if (!pInstance)
            return;

        m_auiAdvisorsGUID[0] = pInstance->GetData64(DATA_SHARKKIS);
        m_auiAdvisorsGUID[1] = pInstance->GetData64(DATA_TIDALVESS);
        m_auiAdvisorsGUID[2] = pInstance->GetData64(DATA_CARIBDIS);
    }

    void Aggro(Unit* pWho)
    {
        if (!pInstance)
            return;

        GetAdvisors();

        DoScriptText(SAY_AGGRO, me);

        pInstance->SetData64(DATA_KARATHRESS_STARTER, pWho->GetGUID());
        pInstance->SetData(TYPE_KARATHRESS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY1, me); break;
            case 1: DoScriptText(SAY_SLAY2, me); break;
            case 2: DoScriptText(SAY_SLAY3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_KARATHRESS_EVENT, DONE);

        //support for quest 10944
        me->SummonCreature(NPC_SEER_OLUM, afCoords_Olum[0], afCoords_Olum[1], afCoords_Olum[2], afCoords_Olum[3], TEMPSUMMON_TIMED_DESPAWN, 3600000);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
        {
            //check if the event is started
            if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == IN_PROGRESS)
            {
                if (Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_KARATHRESS_STARTER)))
                {
                    AttackStart(pTarget);
                    GetAdvisors();
                }
            }
            return;
        }

        //someone evaded!
        if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == NOT_STARTED)
        {
            EnterEvadeMode();
            return;
        }

        //m_uiCataclysmicBolt_Timer
        if (m_uiCataclysmicBolt_Timer < diff)
        {
            //select a random unit other than the main tank
            Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);

            //if there aren't other units, cast on the tank
            if (!pTarget)
                pTarget = me->getVictim();

            me->CastSpell(pTarget, SPELL_CATACLYSMIC_BOLT, false);

            m_uiCataclysmicBolt_Timer = 10000;
        }else m_uiCataclysmicBolt_Timer -= diff;

        //hp under 75%
        if (!m_bBlessingOfTides_MobsChecked && ((me->GetHealth()*100 / me->GetMaxHealth())  <= 75))
        {
            for(uint8 i = 0; i < MAX_ADVISORS; ++i)
            {
                if (Unit* pAdvisor = Unit::GetUnit(*me, m_auiAdvisorsGUID[i]))
                {
                    //stack max three times (one for each alive)
                    if (pAdvisor->isAlive())
                    {
                        pAdvisor->InterruptNonMeleeSpells(false);
                        pAdvisor->CastSpell(me, SPELL_BLESSING_OF_THE_TIDES, false);
                    }
                }
            }

            //yell if we now have the aura
            if (me->HasAura(SPELL_BLESSING_OF_THE_TIDES))
                DoScriptText(SAY_GAIN_BLESSING, me);

            m_bBlessingOfTides_MobsChecked = true;
        }

        //m_uiEnrage_Timer
        if (m_uiEnrage_Timer < diff)
        {
            DoCastMe( SPELL_ENRAGE);
            m_uiEnrage_Timer = 90000;
        }else m_uiEnrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

// Base AI for every advisor
struct MANGOS_DLL_DECL Advisor_Base_AI : public ScriptedAI
{
    Advisor_Base_AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }
    protected:
    uint8 m_uiAdvisor;

    public:
    ScriptedInstance* pInstance;

    void JustReachedHome()
    {
        if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == IN_PROGRESS)
            pInstance->SetData(TYPE_KARATHRESS_EVENT, NOT_STARTED);
    }

    void Aggro(Unit *pWho)
    {
        if (!pInstance)
            return;

        if (pInstance->GetData(TYPE_KARATHRESS_EVENT) == NOT_STARTED)
            pInstance->SetData(TYPE_KARATHRESS_EVENT, IN_PROGRESS);

        pInstance->SetData64(DATA_KARATHRESS_STARTER, pWho->GetGUID());
    }

    void JustDied(Unit* pVictim)
    {
        if (!pInstance)
            return;

        if (Creature* pKarathress = (Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_KARATHRESS)))
            ((boss_fathomlord_karathressAI*)pKarathress->AI())->EventAdvisorDeath(m_uiAdvisor);
    }
};

//Fathom-Guard Sharkkis AI
struct MANGOS_DLL_DECL boss_fathomguard_sharkkisAI : public Advisor_Base_AI
{
    boss_fathomguard_sharkkisAI(Creature* pCreature) : Advisor_Base_AI(pCreature)
    {
        Reset();
        m_uiAdvisor = DATA_SHARKKIS;
    }

    // timers
    uint32 m_uiHurlTrident_Timer;
    uint32 m_uiLeechingThrow_Timer;
    uint32 m_uiTheBeastWithin_Timer;
    uint32 m_uiPet_Timer;

    bool m_bIsPetCheckNeeded;

    void Reset()
    {
        m_uiHurlTrident_Timer    = 2500;
        m_uiLeechingThrow_Timer  = 20000;
        m_uiTheBeastWithin_Timer = 30000;
        m_uiPet_Timer            = 10000;

        m_bIsPetCheckNeeded      = true;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (me->Attack(pWho, false))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);

            //using larger distance, since hunter type
            me->GetMotionMaster()->MoveChase(pWho, 15.0f);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->isPet())
        {
            m_uiPet_Timer = 10000;
            m_bIsPetCheckNeeded = false;
        }
    }

    void SummonedCreatureDespawn(Creature* pDespawned)
    {
        if (pDespawned->isPet())
            m_bIsPetCheckNeeded = true;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
        {
            //check if the event is started
            if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == IN_PROGRESS)
            {
                if (Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_KARATHRESS_STARTER)))
                    AttackStart(pTarget);
            }
            return;
        }

        //someone evaded!
        if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == NOT_STARTED)
        {
            EnterEvadeMode();
            return;
        }

        //after 10 seconds: spawn pet if not exist
        if (m_bIsPetCheckNeeded)
        {
            if (m_uiPet_Timer < diff)
            {
                if (!me->GetPet())
                    DoCastMe( urand(0,1) ? SPELL_SUMMON_FATHOM_LURKER : SPELL_SUMMON_FATHOM_SPOREBAT);
            }
            else
                m_uiPet_Timer -= diff;
        }

        //m_uiHurlTrident_Timer
        if (m_uiHurlTrident_Timer < diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
            {
                if (!me->IsWithinDist(pTarget,ATTACK_DISTANCE))
                    DoCast(pTarget, SPELL_HURL_TRIDENT);
            }

            m_uiHurlTrident_Timer = 5000;
        }else m_uiHurlTrident_Timer -= diff;

        //m_uiLeechingThrow_Timer
        if (m_uiLeechingThrow_Timer < diff)
        {
            DoCastVictim( SPELL_LEECHING_THROW);
            m_uiLeechingThrow_Timer = 20000;
        }else m_uiLeechingThrow_Timer -= diff;

        //m_uiTheBeastWithin_Timer
        if (m_uiTheBeastWithin_Timer < diff)
        {
            DoCastMe( SPELL_THE_BEAST_WITHIN);
            m_uiTheBeastWithin_Timer = 30000;
        }else m_uiTheBeastWithin_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Fathom-Guard Tidalvess AI
struct MANGOS_DLL_DECL boss_fathomguard_tidalvessAI : public Advisor_Base_AI
{
    boss_fathomguard_tidalvessAI(Creature* pCreature) : Advisor_Base_AI(pCreature)
    {
        Reset();
        m_uiAdvisor = DATA_TIDALVESS;
    }

    // timers
    uint32 m_uiFrostShock_Timer;

    void Reset()
    {
        m_uiFrostShock_Timer = 25000;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
        {
            //check if the event is started
            if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == IN_PROGRESS)
            {
                if (Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_KARATHRESS_STARTER)))
                    AttackStart(pTarget);
            }
            return;
        }

        //someone evaded!
        if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == NOT_STARTED)
        {
            EnterEvadeMode();
            return;
        }

        //m_uiFrostShock_Timer
        if (m_uiFrostShock_Timer < diff)
        {
            DoCastVictim( SPELL_FROST_SHOCK);
            m_uiFrostShock_Timer = urand(25000, 30000);
        }else m_uiFrostShock_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Fathom-Guard Caribdis AI
struct MANGOS_DLL_DECL boss_fathomguard_caribdisAI : public Advisor_Base_AI
{
    boss_fathomguard_caribdisAI(Creature* pCreature) : Advisor_Base_AI(pCreature)
    {
        Reset();
        m_uiAdvisor = DATA_CARIBDIS;
    }

    // timers
    uint32 m_uiWaterBoltVolley_Timer;
    uint32 m_uiTidalSurge_Timer;
    uint32 m_uiHeal_Timer;

    void Reset()
    {
        m_uiWaterBoltVolley_Timer = 35000;
        m_uiTidalSurge_Timer      = urand(15000, 20000);
        m_uiHeal_Timer            = 55000;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
        {
            //check if the event is started
            if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == IN_PROGRESS)
            {
                if (Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_KARATHRESS_STARTER)))
                    AttackStart(pTarget);
            }
            return;
        }

        //someone evaded!
        if (pInstance && pInstance->GetData(TYPE_KARATHRESS_EVENT) == NOT_STARTED)
        {
            EnterEvadeMode();
            return;
        }

        //m_uiWaterBoltVolley_Timer
        if (m_uiWaterBoltVolley_Timer < diff)
        {
            DoCastVictim( SPELL_WATER_BOLT_VOLLEY);
            m_uiWaterBoltVolley_Timer = 30000;
        }else m_uiWaterBoltVolley_Timer -= diff;

        //m_uiTidalSurge_Timer
        if (m_uiTidalSurge_Timer < diff)
        {
            // the victim has to cast it on himself because in the spell.dbc the EffectImplicitTargetA1 is 1 (TARGET_SELF)
            me->getVictim()->CastSpell(me->getVictim(), SPELL_TIDAL_SURGE, true);
            m_uiTidalSurge_Timer = urand(15000, 20000);
        }else m_uiTidalSurge_Timer -= diff;

        //m_uiHeal_Timer
        if (m_uiHeal_Timer < diff)
        {
            // It can be cast on any of the mobs
            Unit* pUnit = NULL;

            if (pInstance)
            {
                switch(urand(0, 3))
                {
                    case 0: pUnit = Unit::GetUnit((*me), pInstance->GetData64(DATA_KARATHRESS)); break;
                    case 1: pUnit = Unit::GetUnit((*me), pInstance->GetData64(DATA_SHARKKIS)); break;
                    case 2: pUnit = Unit::GetUnit((*me), pInstance->GetData64(DATA_TIDALVESS)); break;
                    case 3: pUnit = me; break;
                }
            }
            else
                pUnit = me;

            if (pUnit && pUnit->isAlive())
                DoCast(pUnit, SPELL_HEAL);

            m_uiHeal_Timer = 60000;
        }else m_uiHeal_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_fathomlord_karathress(Creature* pCreature)
{
    return new boss_fathomlord_karathressAI(pCreature);
}

CreatureAI* GetAI_boss_fathomguard_sharkkis(Creature* pCreature)
{
    return new boss_fathomguard_sharkkisAI(pCreature);
}

CreatureAI* GetAI_boss_fathomguard_tidalvess(Creature* pCreature)
{
    return new boss_fathomguard_tidalvessAI(pCreature);
}

CreatureAI* GetAI_boss_fathomguard_caribdis(Creature* pCreature)
{
    return new boss_fathomguard_caribdisAI(pCreature);
}

void AddSC_boss_fathomlord_karathress()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_fathomlord_karathress";
    newscript->GetAI = &GetAI_boss_fathomlord_karathress;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_fathomguard_sharkkis";
    newscript->GetAI = &GetAI_boss_fathomguard_sharkkis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_fathomguard_tidalvess";
    newscript->GetAI = &GetAI_boss_fathomguard_tidalvess;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_fathomguard_caribdis";
    newscript->GetAI = &GetAI_boss_fathomguard_caribdis;
    newscript->RegisterSelf();
}
