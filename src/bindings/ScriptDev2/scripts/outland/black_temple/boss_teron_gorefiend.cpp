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
SDName: Boss_Teron_Gorefiend
SD%Complete: 60
SDComment: Requires Mind Control support for Ghosts.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "black_temple.h"

//Speech'n'sound
#define SAY_INTRO                       -1564037
#define SAY_AGGRO                       -1564038
#define SAY_SLAY1                       -1564039
#define SAY_SLAY2                       -1564040
#define SAY_SPELL1                      -1564041
#define SAY_SPELL2                      -1564042
#define SAY_SPECIAL1                    -1564043
#define SAY_SPECIAL2                    -1564044
#define SAY_ENRAGE                      -1564045
#define SAY_DEATH                       -1564046

//Spells
#define SPELL_INCINERATE                40239
#define SPELL_CRUSHING_SHADOWS          40243
#define SPELL_SHADOWBOLT                40185
#define SPELL_PASSIVE_SHADOWFORM        40326
#define SPELL_SHADOW_OF_DEATH           40251
#define SPELL_BERSERK                   45078

#define SPELL_ATROPHY                   40327               // Shadowy Constructs use this when they get within melee range of a player

#define CREATURE_DOOM_BLOSSOM           23123
#define CREATURE_SHADOWY_CONSTRUCT      23111

struct MANGOS_DLL_DECL mob_doom_blossomAI : public ScriptedAI
{
    mob_doom_blossomAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 CheckTeronTimer;
    uint32 ShadowBoltTimer;
    uint64 TeronGUID;

    void Reset()
    {
        CheckTeronTimer = 5000;
        ShadowBoltTimer = 12000;
        TeronGUID = 0;
    }

    void AttackStart(Unit* who) { }
    void MoveInLineOfSight(Unit* who) { }

    void UpdateAI(const uint32 diff)
    {
        if (CheckTeronTimer < diff)
        {
            if (TeronGUID)
            {
                me->SetInCombatWithZone();

                Creature* Teron = ((Creature*)Unit::GetUnit((*me), TeronGUID));
                if ((Teron) && (!Teron->isAlive() || Teron->IsInEvadeMode()))
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            else
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

            CheckTeronTimer = 5000;
        }else CheckTeronTimer -= diff;

        if (!me->getVictim() || !me->SelectHostileTarget())
            return;

        if (ShadowBoltTimer < diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_SHADOWBOLT);
            ShadowBoltTimer = 10000;
        }else ShadowBoltTimer -= diff;
    }

    void SetTeronGUID(uint64 guid){ TeronGUID = guid; }
};

struct MANGOS_DLL_DECL mob_shadowy_constructAI : public ScriptedAI
{
    mob_shadowy_constructAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint64 GhostGUID;
    uint64 TeronGUID;

    uint32 CheckPlayerTimer;
    uint32 CheckTeronTimer;

    void Reset()
    {
        GhostGUID = 0;
        TeronGUID = 0;

        CheckPlayerTimer = 2000;
        CheckTeronTimer = 5000;
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || (!who->isAlive()) || (who->GetGUID() == GhostGUID))
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

/* Comment it out for now. NOTE TO FUTURE DEV: UNCOMMENT THIS OUT ONLY AFTER MIND CONTROL IS IMPLEMENTED
    void DamageTaken(Unit* done_by, uint32 &damage)
    {
        if (done_by->GetGUID() != GhostGUID)
        damage = 0;                                         // Only the ghost can deal damage.
    }
 */

    void CheckPlayers()
    {
       ThreatList const& m_threatlist = me->getThreatManager().getThreatList();
        if (m_threatlist.empty())
            return;                                         // No threat list. Don't continue.

        ThreatList::const_iterator itr = m_threatlist.begin();
        std::list<Unit*> targets;
        for(; itr != m_threatlist.end(); ++itr)
        {
            Unit* pUnit = Unit::GetUnit((*me), (*itr)->getUnitGuid());
            if (pUnit && pUnit->isAlive())
                targets.push_back(pUnit);
        }

        targets.sort(ObjectDistanceOrder(me));
        Unit* target = targets.front();
        if (target && me->IsWithinDistInMap(target, me->GetAttackDistance(target)))
        {
            DoCast(target, SPELL_ATROPHY);
            me->AI()->AttackStart(target);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CheckPlayerTimer < diff)
        {
            CheckPlayers();
            CheckPlayerTimer = 3000;
        }else CheckPlayerTimer -= diff;

        if (CheckTeronTimer < diff)
        {
            Creature* Teron = ((Creature*)Unit::GetUnit((*me), TeronGUID));
            if (!Teron || !Teron->isAlive() || Teron->IsInEvadeMode())
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

            CheckTeronTimer = 5000;
        }else CheckTeronTimer -= diff;
    }
};

struct MANGOS_DLL_DECL boss_teron_gorefiendAI : public ScriptedAI
{
    boss_teron_gorefiendAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    uint32 IncinerateTimer;
    uint32 SummonDoomBlossomTimer;
    uint32 EnrageTimer;
    uint32 CrushingShadowsTimer;
    uint32 ShadowOfDeathTimer;
    uint32 SummonShadowsTimer;
    uint32 RandomYellTimer;
    uint32 AggroTimer;

    uint64 AggroTargetGUID;
    uint64 GhostGUID;                                       // Player that gets killed by Shadow of Death and gets turned into a ghost

    bool Intro;

    void Reset()
    {
        IncinerateTimer = urand(20000, 30000);
        SummonDoomBlossomTimer = 12000;
        EnrageTimer = 600000;
        CrushingShadowsTimer = 22000;
        SummonShadowsTimer = 60000;
        RandomYellTimer = 50000;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        // Start off unattackable so that the intro is done properly
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        AggroTimer = 20000;
        AggroTargetGUID = 0;
        Intro = false;
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_GOREFIEND, NOT_STARTED);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!Intro && pWho->GetTypeId() == TYPEID_PLAYER && pWho->isTargetableForAttack() &&
            me->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(me))
        {
            if (me->IsWithinDistInMap(pWho, VISIBLE_RANGE) && me->IsWithinLOSInMap(pWho))
            {
                if (pInstance)
                    pInstance->SetData(TYPE_GOREFIEND, IN_PROGRESS);

                me->GetMotionMaster()->Clear(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                DoScriptText(SAY_INTRO, me);

                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                AggroTargetGUID = pWho->GetGUID();
                Intro = true;
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY1 : SAY_SLAY2, me);
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(TYPE_GOREFIEND, DONE);

        DoScriptText(SAY_DEATH, me);
    }

    float CalculateRandomLocation(float Loc, uint32 radius)
    {
        float coord = Loc;
        switch(urand(0, 1))
        {
            case 0:
                coord += rand()%radius;
                break;
            case 1:
                coord -= rand()%radius;
                break;
        }
        return coord;
    }

    void SetThreatList(Creature* Blossom)
    {
        if (!Blossom)
            return;

       ThreatList const& m_threatlist = me->getThreatManager().getThreatList();
        ThreatList::const_iterator i = m_threatlist.begin();
        for(i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
        {
            Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
            if (pUnit && pUnit->isAlive())
            {
                float threat = me->getThreatManager().getThreat(pUnit);
                Blossom->AddThreat(pUnit, threat);
            }
        }
    }

    void MindControlGhost()
    {
        /************************************************************************/
        /** NOTE FOR FUTURE DEVELOPER: PROPERLY IMPLEMENT THE GHOST PORTION *****/
        /**  ONLY AFTER MaNGOS FULLY IMPLEMENTS MIND CONTROL ABILITIES      *****/
        /**   THE CURRENT CODE IN THIS FUNCTION IS ONLY THE BEGINNING OF    *****/
        /**    WHAT IS FULLY NECESSARY FOR GOREFIEND TO BE 100% COMPLETE    *****/
        /************************************************************************/

        Unit* Ghost = NULL;
        if (GhostGUID)
            Ghost = Unit::GetUnit((*me), GhostGUID);
        if (Ghost && Ghost->isAlive() && Ghost->HasAura(SPELL_SHADOW_OF_DEATH, 0))
        {
            /*float x,y,z;
            Ghost->GetPosition(x,y,z);
            Creature* control = me->SummonCreature(CREATURE_GHOST, x, y, z, 0, TEMPSUMMON_TIMED_DESAWN, 30000);
            if (control)
            {
                ((Player*)Ghost)->Possess(control);
                Ghost->DealDamage(Ghost, Ghost->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL,
            false);
            }*/
            for(uint8 i = 0; i < 4; ++i)
            {
                Creature* Construct = NULL;
                float X = CalculateRandomLocation(Ghost->GetPositionX(), 10);
                float Y = CalculateRandomLocation(Ghost->GetPositionY(), 10);

                Construct = me->SummonCreature(CREATURE_SHADOWY_CONSTRUCT, X, Y, Ghost->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
                if (Construct)
                {
                    Construct->CastSpell(Construct, SPELL_PASSIVE_SHADOWFORM, true);

                    SetThreatList(Construct);               // Use same function as Doom Blossom to set Threat List.
                    ((mob_shadowy_constructAI*)Construct->AI())->GhostGUID = GhostGUID;

                    Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1);
                    if (!target)                            // someone's trying to solo.
                        target = me->getVictim();

                    if (target)
                        Construct->GetMotionMaster()->MoveChase(target);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (Intro)
        {
            if (AggroTimer < diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                DoScriptText(SAY_AGGRO, me);

                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                Intro = false;
                if (AggroTargetGUID)
                {
                    Unit* pUnit = Unit::GetUnit((*me), AggroTargetGUID);
                    if (pUnit)
                        AttackStart(pUnit);

                    me->SetInCombatWithZone();
                }else EnterEvadeMode();
            }else AggroTimer -= diff;
        }

        if (CanDoSomething() || Intro)
            return;

        if (SummonShadowsTimer < diff)
        {
            //MindControlGhost();

            for(uint8 i = 0; i < 2; ++i)
            {
                Creature* Shadow = NULL;
                float X = CalculateRandomLocation(me->GetPositionX(), 10);

                Shadow = me->SummonCreature(CREATURE_SHADOWY_CONSTRUCT, X, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 0);
                if (Shadow)
                {
                    Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1);
                    if (!target)
                        target = me->getVictim();

                    if (target)
                        Shadow->AI()->AttackStart(target);
                }
            }
            SummonShadowsTimer = 60000;
        }else SummonShadowsTimer -= diff;

        if (SummonDoomBlossomTimer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                float X = CalculateRandomLocation(target->GetPositionX(), 20);
                float Y = CalculateRandomLocation(target->GetPositionY(), 20);

                Creature* DoomBlossom = me->SummonCreature(CREATURE_DOOM_BLOSSOM, X, Y, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
                if (DoomBlossom)
                {
                    DoomBlossom->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    DoomBlossom->setFaction(me->getFaction());
                    DoomBlossom->AddThreat(target, 1.0f);
                    ((mob_doom_blossomAI*)DoomBlossom->AI())->SetTeronGUID(me->GetGUID());

                    SetThreatList(DoomBlossom);
                    SummonDoomBlossomTimer = 35000;
                }
            }
        }else SummonDoomBlossomTimer -= diff;

        if (IncinerateTimer < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1);
            if (!target)
                target = me->getVictim();

            if (target)
            {
                DoScriptText(urand(0, 1) ? SAY_SPECIAL1 : SAY_SPECIAL2, me);
                DoCast(target, SPELL_INCINERATE);
                IncinerateTimer = urand(20000, 50000);
            }
        }else IncinerateTimer -= diff;

        if (CrushingShadowsTimer < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (target && target->isAlive())
                DoCast(target, SPELL_CRUSHING_SHADOWS);

            CrushingShadowsTimer = urand(10000, 26000);
        }else CrushingShadowsTimer -= diff;

        /*** NOTE FOR FUTURE DEV: UNCOMMENT BELOW ONLY IF MIND CONTROL IS FULLY IMPLEMENTED **/
        /*if (ShadowOfDeathTimer < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1);

            if (!target)
               target = me->getVictim();

            if (target && target->isAlive() && target->GetTypeId() == TYPEID_PLAYER)
            {
                DoCast(target, SPELL_SHADOW_OF_DEATH);
                GhostGUID = target->GetGUID();
                ShadowOfDeathTimer = 30000;
                SummonShadowsTimer = 53000; // Make it VERY close but slightly less so that we can check if the aura is still on the pPlayer
            }
        }else ShadowOfDeathTimer -= diff;*/

        if (RandomYellTimer < diff)
        {
            DoScriptText(urand(0, 1) ? SAY_SPELL1 : SAY_SPELL2, me);
            RandomYellTimer = urand(50000, 100000);
        }else RandomYellTimer -= diff;

        if (!me->HasAura(SPELL_BERSERK, 0))
        {
            if (EnrageTimer < diff)
            {
                DoCastMe( SPELL_BERSERK);
                DoScriptText(SAY_ENRAGE, me);
            }else EnrageTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_doom_blossom(Creature* pCreature)
{
    return new mob_doom_blossomAI(pCreature);
}

CreatureAI* GetAI_mob_shadowy_construct(Creature* pCreature)
{
    return new mob_shadowy_constructAI(pCreature);
}

CreatureAI* GetAI_boss_teron_gorefiend(Creature* pCreature)
{
    return new boss_teron_gorefiendAI(pCreature);
}

void AddSC_boss_teron_gorefiend()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "mob_doom_blossom";
    newscript->GetAI = &GetAI_mob_doom_blossom;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowy_construct";
    newscript->GetAI = &GetAI_mob_shadowy_construct;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_teron_gorefiend";
    newscript->GetAI = &GetAI_boss_teron_gorefiend;
    newscript->RegisterSelf();
}
