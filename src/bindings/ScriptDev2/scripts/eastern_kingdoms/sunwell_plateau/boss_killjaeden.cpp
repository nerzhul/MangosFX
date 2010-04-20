/* Copyright 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
 
/* ScriptData
SDName: boss_kiljaden
SD%Complete: 80%
SDComment: 
SDAuthor: ScrappyDoo
SDTestTeam: Legion Of War (c) Andeeria
EndScriptData */
 
/*
Phase1 50% all mobs need to aggro all t once
Phase2 90% fire bloom not affect 10 yards
Phase3 75%
Shadow Spike 0%
Phase4 100%
Phase5 50%

Kalecgos Event
Drakes Orbs Event
*/

#include "precompiled.h"
#include "sunwell_plateau.h"
 
enum Speeches // (c) Trinitcore 
{
    // These are used throughout Sunwell and Magisters(?). Players can hear this while running through the instances.
    SAY_KJ_OFFCOMBAT1   = -1580066,
    SAY_KJ_OFFCOMBAT2   = -1580067,
    SAY_KJ_OFFCOMBAT3   = -1580068,
    SAY_KJ_OFFCOMBAT4   = -1580069,
    SAY_KJ_OFFCOMBAT5   = -1580070,

    // Encounter speech and sounds
    SAY_KJ_EMERGE       = -1580071, //
    SAY_KJ_SLAY1        = -1580072,
    SAY_KJ_SLAY2        = -1580073,
    SAY_KJ_REFLECTION1  = -1580074, //
    SAY_KJ_REFLECTION2  = -1580075, //
    SAY_KJ_DARKNESS1    = -1580076,
    SAY_KJ_DARKNESS2    = -1580077,
    SAY_KJ_DARKNESS3    = -1580078,
    SAY_KJ_PHASE3       = -1580079,
    SAY_KJ_PHASE4       = -1580080,
    SAY_KJ_PHASE5       = -1580081,
    SAY_KJ_DEATH        = -1580093,
    EMOTE_KJ_DARKNESS   = -1580094,

    /*** Kalecgos - Anveena speech at the beginning of Phase 5; Anveena's sacrifice ***/
    SAY_KALECGOS_AWAKEN     = -1580082,
    SAY_ANVEENA_IMPRISONED  = -1580083,
    SAY_KALECGOS_LETGO      = -1580084,
    SAY_ANVEENA_LOST        = -1580085,
    SAY_KALECGOS_FOCUS      = -1580086,
    SAY_ANVEENA_KALEC       = -1580087,
    SAY_KALECGOS_FATE       = -1580088,
    SAY_ANVEENA_GOODBYE     = -1580089,
    SAY_KALECGOS_GOODBYE    = -1580090,
    SAY_KALECGOS_ENCOURAGE  = -1580091,

    /*** Kalecgos says throughout the fight ***/
    SAY_KALECGOS_JOIN       = -1580092,
    SAY_KALEC_ORB_READY1    = -1580095,
    SAY_KALEC_ORB_READY2    = -1580096,
    SAY_KALEC_ORB_READY3    = -1580097,
    SAY_KALEC_ORB_READY4    = -1580098
};

enum spells
{
    //Kilajden Spells
    AURA_SUNWELL_RADIANCE       = 45769, // NOT WORKING
    SPELL_REBIRTH               = 44200, // Emerge from the Sunwell Epick :D

    SPELL_SOULFLY               = 45442, // 100%
    SPELL_DARKNESS_OF_SOULS     = 46605, // 100% Visual Effect
    SPELL_DARKNESS_EXPLOSION    = 45657, // 100% Damage
    SPELL_DESTROY_DRAKES        = 46707, // ?
    SPELL_FIREBLOOM             = 45641, // NOT WORKING
    SPELL_FIREBLOOM_EFF         = 45642, // 100% Damage
    SPELL_FLAMEDARTS            = 45737, // 100%
    SPELL_LEGION_LIGHTING       = 45664, // 100%

    SPELL_SHADOWSPIKE           = 46680, // NOT WORKING -> WorkArround 99%
    SPELL_SHADOWSPIKE_EXP       = 45885, // 100% damage -> target self cast long range

    SPELL_ARMAGEDDON_EXPLOSION  = 45909, // 100% Damage
    SPELL_ARMAGEDDON_EFFECT     = 24207, // 100% Visual

    SPELL_SINISTER_REFLECTION   = 45892, // NOT WORKING -> WorkArround 99%
    SPELL_SACRIFICE_OF_ANVEENA  = 46474, // 100%

    SPELL_KNOCKBACK             = 0,     // ?

    //Hand of the Deceiver Spells
    SPELL_FELLFIRE_PORTAL       = 46875, // NOT WORKING -> WorkArround 99%
    SPELL_SHADOWBOLT_VOLLEY     = 45770, // 100%
    SPELL_SHADOWINFUSION        = 45772, // 100%
    SPELL_IMPFUSION             = 45779, // 100%

    //Orb Spells
    SPELL_ORB_BOLT              = 45680, // 100%

    //Reflections Spells
    SPELL_PRIEST                = 47077, // 100%
    SPELL_PALADIN               = 37369, // 100% 
    SPELL_WARLOCK               = 46190, // 100%
    SPELL_MAGE                  = 47074, // 100%
    SPELL_ROGUE                 = 45897, // 100%
    SPELL_WARRIOR               = 17207, // 100%
    SPELL_DRUID                 = 47072, // 100%
    SPELL_SHAMAN                = 47071, // 100%
    SPELL_HUNTER                = 48098, // 100%

    //Orbs of DragonFligth
    SPELL_REVITALIZE            = 45027, // 100%
    SPELL_SHIELD_OF_BLUE        = 45848, // 100%
    SPELL_HASTE                 = 45856, // NOT WORKING

    //Other Spells (used by players, etc)
    SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT  = 45839, // Possess the blue dragon from the orb to help the raid.
};

enum ids
{
	ID_KILJADEN		= 25315,
    ID_DECIVER      = 25588,
    ID_SHIELDORB    = 25502,
    ID_SINISTER     = 25708,
    ID_ARMAGEDON    = 25735,
    ID_IMP          = 25598,
    ID_PORTAL       = 25603,
    ID_DRAGON       = 25653,
    ID_SHADOWSPIKE  = 30598,
};
 
float OrbSpawn [4][2] =
{
    {1853.300,588.653},
    {1781.502,659.254},
    {1853.300,588.653},
    {1853.300,588.653},
};

#define GAMEOBJECT_ORB_OF_THE_BLUE_DRAGONFLIGHT 188415

#define GOSSIP_ITEM_1 "cast on me Shield of the Blue Dragon Flight ! Quikly !"
#define GOSSIP_ITEM_2 "cast on me Dragon Breath: Revitalize !"
#define GOSSIP_ITEM_3 "cast on me Dragon Breath: Haste !"
#define GOSSIP_ITEM_4 "cast on me Blink !"
#define GOSSIP_ITEM_5 "Fight with our Enemy !"

//Dragon Orb
bool GO_go_orb_of_the_blue_flight(Player *plr, GameObject* go)
{
    //if(go->GetUInt32Value(GAMEOBJECT_FACTION) == 35){
        //ScriptedInstance* pInstance = ((ScriptedInstance*)go->GetInstanceData());
        //float x,y,z, dx,dy,dz;
        go->SummonCreature(ID_DRAGON, plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 121000);
        plr->CastSpell(plr, SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, true);
        go->SetUInt32Value(GAMEOBJECT_FACTION, 0);
        /*
        Unit* Kalec = ((Creature*)Unit::GetUnit(*plr, pInstance->GetData64(DATA_KALECGOS_KJ)));
        Kalec->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
        go->GetPosition(x,y,z);
        for(uint8 i = 0; i < 4; ++i){
            DynamicObject* Dyn = Kalec->GetDynObject(SPELL_RING_OF_BLUE_FLAMES);
            if(Dyn){
                Dyn->GetPosition(dx,dy,dz);
                if(x == dx && dy == y && dz == z){
                    Dyn->RemoveFromWorld();
                    break;
                }
            }
        }
        */
        go->Refresh();
    //}
    return true;
}


/* --- Kiljaden --- */
struct MANGOS_DLL_DECL boss_kiljadenAI : public Scripted_NoMovementAI
{
    boss_kiljadenAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }
 
    ScriptedInstance* pInstance;

    uint32 OrbTimer;
    bool BoolOrb;

    //FireBloom Targets Table
    uint64 FireBloomTarget[5];
    //Shadow Spike Target
    uint64 ShadowSpikeTarget;

    //Phase2
    uint32 SoulFlyTimer;
    uint32 LegionLightingTimer;
    uint32 FireBloomCheck;
    uint32 FireBloomTimer;
    uint32 FireBloomCount;
    uint32 ShieldOrbTimer;

    //Phase3
    bool Phase3;
    uint32 ShadowSpikeTimer;
    uint32 FlameDartTimer;
    uint32 DarknessOfSoulsTimer;
    uint32 DarknessExplosionTimer;
    uint32 ShadowSpikeEndsTimer;
    uint32 SpikesCount;
    bool ShadowSpikeEnds;

    //Phase4
    bool Phase4;
    bool DarknessOfSoulsCasting;
    uint32 AramageddonTimer;
    
    //Phase5
    bool Phase5;
    bool ArwenaSacrificedSelf;
    uint32 ArwenaSacrificeTimer;

    //Sinister Reflections
    uint64 SinisterGUID[12][2];
    uint32 SinnisterCastTimer;
 
    void Reset()
    {
        BoolOrb = true;

        //Phase2
        SoulFlyTimer        = 1000;
        LegionLightingTimer = 15000;
        FireBloomCheck      = 2000;
        FireBloomTimer      = 30000;
        FireBloomCount      = 10;
        ShieldOrbTimer      = 25000;

        //Phase3
        ShadowSpikeTarget   = 0;
        ShadowSpikeEnds     = false;
        SpikesCount         = 9;

        //Phase5
        ArwenaSacrificedSelf = false;
       
        DarknessOfSoulsCasting  = false;

        Phase3 = false;
        Phase4 = false;
        Phase5 = false;

        me->SetVisibility(VISIBILITY_OFF);
        me->setFaction(35);

        if(!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if(!me->HasAura(AURA_SUNWELL_RADIANCE, 0))
            me->CastSpell(me, AURA_SUNWELL_RADIANCE, true);

        if(me->HasAura(SPELL_SACRIFICE_OF_ANVEENA, 0))
            me->RemoveAurasDueToSpell(SPELL_SACRIFICE_OF_ANVEENA,0);

        /*if(pInstance)
            pInstance->SetData(DATA_DECIVER, NOT_STARTED); */
    }
    
    void Aggro(Unit *who) 
    {
        DoScriptText(SAY_KJ_EMERGE, me);
        me->SetVisibility(VISIBILITY_ON);
        me->CastSpell(me, SPELL_REBIRTH, false);
    }

    void KilledUnit(Unit *Victim) {}

    void Sinister(Player* victim,uint8 i,uint8 k)
    {
        uint32 spell;

        switch(victim->getClass())
        {
            case CLASS_PRIEST:  spell = SPELL_PRIEST; break;
            case CLASS_PALADIN: spell = SPELL_PALADIN; break;
            case CLASS_WARLOCK: spell = SPELL_WARLOCK; break;
            case CLASS_MAGE:    spell = SPELL_MAGE; break;
            case CLASS_ROGUE:   spell = SPELL_ROGUE; break;
            case CLASS_WARRIOR: spell = SPELL_WARRIOR; break;
            case CLASS_DRUID:   spell = SPELL_DRUID; break;
            case CLASS_SHAMAN:  spell = SPELL_SHAMAN; break;
            case CLASS_HUNTER:  spell = SPELL_HUNTER; break;
        }

        for(i=0; i<k; ++i)
        {
            Creature* Sinister = me->SummonCreature(ID_SINISTER, victim->GetPositionX()+rand()%3, victim->GetPositionY()+rand()%3, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            if(Sinister)
            {
                Sinister->AI()->AttackStart(Sinister->getVictim());
                SinisterGUID[i][0] = Sinister->GetGUID();
                SinisterGUID[i][1] = spell;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        /*if(pInstance && pInstance->GetData(DATA_DECIVER) == SPECIAL)
        {
            //DoCast(me->getVictim(), SPELL_REBIRTH, true);

            me->setFaction(14);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            
            pInstance->SetData(DATA_KILJAEDEN_EVENT, IN_PROGRESS);
            pInstance->SetData(DATA_DECIVER, NOT_STARTED); 
        }*/

        if(!CanDoSomething())
               return;

        if(DarknessExplosionTimer < diff && DarknessOfSoulsCasting)
        {
            me->CastSpell(me->getVictim(), SPELL_DARKNESS_EXPLOSION, true);
            DarknessOfSoulsCasting = false;
            DarknessExplosionTimer = 600000;
            DarknessOfSoulsTimer = 45000;
            if(Phase5)
                DarknessOfSoulsTimer = 25000;
        }else DarknessExplosionTimer -= diff;

        if(DarknessOfSoulsCasting)
            return;

        if(DarknessOfSoulsTimer < diff)
        {
            me->CastSpell(me, SPELL_DARKNESS_OF_SOULS, true);
            DarknessOfSoulsCasting = true;
            DarknessExplosionTimer = 8500;
        }else DarknessOfSoulsTimer -= diff;

        if((OrbTimer < diff) && !BoolOrb)
        {
            //uint8 k=1;
            //if(Phase5)
                //k=4;
            //for(uint i=0; i<k; ++i)
            //{
                Creature* ShieldOrb = me->SummonCreature(ID_DRAGON, me->GetPositionX()+15, me->GetPositionY()+15, me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 12000);
            //}
            BoolOrb = true;
        }else OrbTimer -= diff;

        //Phase5
        if(ArwenaSacrificeTimer < diff && !ArwenaSacrificedSelf && Phase5)
        {
            me->CastSpell(me, SPELL_SACRIFICE_OF_ANVEENA, false);
            ArwenaSacrificedSelf = true;
        }else ArwenaSacrificeTimer -= diff;

        //FireBloom Damage WorkArround
        if (FireBloomCheck < diff)
        {
            if(FireBloomCount < 10)
                for(uint8 i=0; i<5; ++i)
                {
                    if(Unit* FireTarget = Unit::GetUnit(*me, FireBloomTarget[i]))
                        FireTarget->CastSpell(FireTarget, SPELL_FIREBLOOM_EFF, true);
                }
            ++FireBloomCount;
            FireBloomCheck = 2000;
        }else FireBloomCheck -= diff;

        //Sinister Reflects Attack
        if(SinnisterCastTimer < diff)
        {
            uint8 SinisterCount = 0;
            if(Phase3)
                SinisterCount = 4;
            if(Phase4)
                SinisterCount = 8;
            if(Phase5)
                SinisterCount = 12;
            for(uint8 i=0; i<SinisterCount; ++i)
                if(Unit* Sinister = Unit::GetUnit((*me), SinisterGUID[i][0]))
                    if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        Sinister->CastSpell(target, SinisterGUID[i][1], true);

            SinnisterCastTimer = 8000;
        }else SinnisterCastTimer -= diff;

        //Shield Orb At Start each phases
        if(ShieldOrbTimer < diff && !Phase5)
        {
            uint8 l=1;
            if(Phase3)
                l=2;
            if(Phase4)
                l=3;
            for(uint8 k=0; k<l; ++k)
            {
                Creature* ShieldOrb = me->SummonCreature(ID_SHIELDORB, OrbSpawn[k][0], OrbSpawn[k][1], me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                if(ShieldOrb)
                    ShieldOrb->AI()->AttackStart(me->getVictim()); 
            }
            ShieldOrbTimer = 50000; 
        }else ShieldOrbTimer -= diff;

        //Phases
        if(((me->GetHealth()*100 / me->GetMaxHealth()) <= 85) && !Phase3)
        {
            DoScriptText(SAY_KJ_REFLECTION1,me);
            if(Unit* victim = SelectUnit(SELECT_TARGET_RANDOM,0)) 
                if (victim && (victim->GetTypeId() == TYPEID_PLAYER))
                    Sinister(((Player*)victim),0,4);

            //Start Timerow  Dochodzacych w 3 Fazie
            SinnisterCastTimer      = 10000;
            //ShadowSpikeTimer        = 30000;
            FlameDartTimer          = 40000;
            DarknessOfSoulsTimer    = 20000;
            ShadowSpikeEnds         = false;
            DarknessOfSoulsCasting  = false;
            Phase3 = true;

            OrbTimer = 35000;
            BoolOrb = false;
        }

        if(((me->GetHealth()*100 / me->GetMaxHealth()) <= 55) && !Phase4)
        {
            //SAY_KJ_REFLECTION2
            if(Unit* victim = SelectUnit(SELECT_TARGET_RANDOM,0)) 
                if (victim && (victim->GetTypeId() == TYPEID_PLAYER))
                    Sinister(((Player*)victim),4,8);

            //Start Timerow  Dochodzacych w 4 Fazie
            AramageddonTimer       = 2000;  //100% ok
            Phase4 = true;

            OrbTimer = 35000;
            BoolOrb = false;
        }

        if(((me->GetHealth()*100 / me->GetMaxHealth()) <= 25) && !Phase5)
        {
            //SAY_KJ_REFLECTION1
            if(Unit* victim = SelectUnit(SELECT_TARGET_RANDOM,0)) 
                if (victim && (victim->GetTypeId() == TYPEID_PLAYER))
                    Sinister(((Player*)victim),8,12);

            //Start Timerow  Dochodzacych w 5 Fazie
            //ShadowSpikeTimer = 1000;
            ArwenaSacrificeTimer = 29000;
            ArwenaSacrificedSelf = false;
            Phase5 = true;

            OrbTimer = 35000;
            BoolOrb = false;
        }

        //Phase3
        if(Phase3)
        {
            if(ShadowSpikeTimer < diff)
            {
                //DoCast(me->getVictim(), SPELL_SHADOWSPIKE); NOT WORKING

                ShadowSpikeEnds         = true;
                ShadowSpikeEndsTimer    = 500;
                SpikesCount             = 0;
                ShadowSpikeTimer        = 90000;
            }else ShadowSpikeTimer -= diff;

            if((ShadowSpikeEndsTimer < diff) && ShadowSpikeEnds)
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                {
                    target->CastSpell(target, SPELL_SHADOWSPIKE_EXP, false);
                }
                ++SpikesCount;
                if(SpikesCount < 9)
                    ShadowSpikeEnds = false;
                ShadowSpikeEndsTimer = 3000;
            }else ShadowSpikeEndsTimer -= diff;

            if(FlameDartTimer < diff)
            {
                DoCast(me->getVictim(), SPELL_FLAMEDARTS);
                FlameDartTimer = 33000;
            }else FlameDartTimer -= diff;
        }

        //Phase4
        if((AramageddonTimer < diff) && Phase4)
        {
            /*
            3 of these will be in flight constantly throughout this phase and they will 
            stop spawning while he is casting Darkness of a Thousand Souls. 
            */

            uint8 h=3;
            if(Phase5)
                h=5;
            if(!Phase5 && DarknessOfSoulsCasting)
                h=0;
            
            for(uint8 i=0; i<h; ++i)
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    Creature* Armagedon = me->SummonCreature(ID_ARMAGEDON, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
            }
            AramageddonTimer = 14000;
        }else AramageddonTimer -= diff;

        //Phase2
        if(LegionLightingTimer < diff)
        {
            DoCast(me->getVictim(), SPELL_LEGION_LIGHTING);
            LegionLightingTimer = 11000;
        }else LegionLightingTimer -= diff;

        if(FireBloomTimer < diff)
        {
            for(uint8 i=0; i<5; ++i)
            {
                Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                FireBloomTarget[i] = target->GetGUID();
                FireBloomCount = 0;
                //DoCast(target, SPELL_FIREBLOOM, true);
            }
            FireBloomTimer = 25000;
        }else FireBloomTimer -= diff;

        if(SoulFlyTimer < diff)
        {
            DoCast(me->getVictim(), SPELL_SOULFLY);
            SoulFlyTimer = 7000;
        }else SoulFlyTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_deceiverAI : public ScriptedAI
{
    mob_deceiverAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }
 
    ScriptedInstance* pInstance;

    uint32 BoltTimer;
    uint32 PortalTimer;
    uint32 ImpTimer;
    uint8 ImpCount;
    float xx;
    float yy;
 
    void Reset()
    {
        PortalTimer = 25000;
        BoltTimer = 10000;
        ImpTimer = 60000;
        ImpCount = 0;
        xx = 0;
        yy = 0;
    }
    
    void Aggro(Unit *who) 
    {

    }

    void JustDied(Unit* Killer) 
    {
        /*if(pInstance && pInstance->GetData(DATA_DECIVER) == NOT_STARTED)
            pInstance->SetData(DATA_DECIVER, IN_PROGRESS);
        else 
            if(pInstance && pInstance->GetData(DATA_DECIVER) == IN_PROGRESS)
                pInstance->SetData(DATA_DECIVER, DONE);
            else
                if(pInstance && pInstance->GetData(DATA_DECIVER) == DONE)
                    pInstance->SetData(DATA_DECIVER, SPECIAL);*/
    }

    void KilledUnit(Unit *Victim) {}
     
    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
                return;

        if(BoltTimer < diff)
        {
            DoCast(me->getVictim(), SPELL_SHADOWBOLT_VOLLEY);
            BoltTimer = 10000;
        }else BoltTimer -= diff;

        if((me->GetHealth()*100 / me->GetMaxHealth()) <= 20)
            if(!me->HasAura(SPELL_SHADOWINFUSION,0))
                me->CastSpell(me, SPELL_SHADOWINFUSION, true);

        if(PortalTimer < diff)
        {
            xx = me->GetPositionX();
            yy = me->GetPositionY();

            Creature* Portal = me->SummonCreature(ID_PORTAL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 20500);
            PortalTimer = 30000;
            ImpTimer = 500;
            ImpCount = 0;
        }else PortalTimer -= diff;

        if(ImpTimer < diff)
        {
            if(ImpCount < 4)
            {
                Creature* Imp = me->SummonCreature(ID_IMP, xx, yy, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                if(Imp)
                    if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        Imp->AI()->AttackStart(target); 
                
            }
            ImpTimer = 5000;
            ++ImpCount;
        }else ImpTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_orbAI : public ScriptedAI
{
    mob_orbAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }
 
    ScriptedInstance* pInstance;
    uint32 SpellTimer;
 
    void Reset()
    {
        SpellTimer = 1000;
        me->setFaction(14);
        me->SetVisibility(VISIBILITY_ON);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }
    void Aggro(Unit *who) {}
    void JustDied(Unit* Killer) {}
    void KilledUnit(Unit *Victim) {}
    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
                return;

        if(SpellTimer < diff)
        {
            for(uint8 i=0; i<3; ++i)
            {
                if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    me->CastSpell(target, SPELL_ORB_BOLT, true);
            }
            SpellTimer = 1000;
        }else SpellTimer -= diff;
    }
};

struct MANGOS_DLL_DECL mob_armagedonAI : public Scripted_NoMovementAI
{
    mob_armagedonAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }
 
    ScriptedInstance* pInstance;

    uint32 ExplosionTimer;
    uint32 EffectTimer;
    bool Explosion;
 
    void Reset()
    {
        me->SetDisplayId(25206);

        ExplosionTimer = 8000;
        EffectTimer = 100;
        Explosion = false;

        me->setFaction(14);
        me->SetVisibility(VISIBILITY_ON);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit *who) {}

    void JustDied(Unit* Killer) {}

    void KilledUnit(Unit *Victim) {}

    void UpdateAI(const uint32 diff)
    {
        if(Explosion)
            return;

        if((ExplosionTimer < diff) && !Explosion)
        {
            me->CastSpell(me, SPELL_ARMAGEDDON_EXPLOSION, true);
            Explosion = true;
        }else ExplosionTimer -= diff;

        if(EffectTimer < diff && !Explosion)
        {
            me->CastSpell(me,SPELL_ARMAGEDDON_EFFECT,false);
            EffectTimer = 1000;
        }else EffectTimer -= diff;
    }
};

// ShadowSpike Trigger
struct MANGOS_DLL_DECL mob_shadow_spikeAI : public Scripted_NoMovementAI
{
    mob_shadow_spikeAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }
 
    ScriptedInstance* pInstance;

    uint32 ExplosionTimer;
    uint32 EffectTimer;
    bool Explosion;
 
    void Reset()
    {
        me->SetDisplayId(25206);

        ExplosionTimer = 4000;
        EffectTimer = 100;
        Explosion = false;

        me->setFaction(14);
        me->SetVisibility(VISIBILITY_ON);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit *who) {}

    void JustDied(Unit* Killer) {}

    void KilledUnit(Unit *Victim) {}

    void UpdateAI(const uint32 diff)
    {
        if(Explosion)
            return;

        if((ExplosionTimer < diff) && !Explosion)
        {
            me->CastSpell(me, SPELL_ARMAGEDDON_EXPLOSION, true);
            Explosion = true;
        }else ExplosionTimer -= diff;

        if(EffectTimer < diff && !Explosion)
        {
            me->CastSpell(me,SPELL_ARMAGEDDON_EFFECT,false);
            EffectTimer = 1000;
        }else EffectTimer -= diff;
    }
};

/* --- Imps AI --- */
struct MANGOS_DLL_DECL mob_killimpAI : public ScriptedAI
{
    mob_killimpAI(Creature *c) : ScriptedAI(c) 
    {
	    pInstance = ((ScriptedInstance*)c->GetInstanceData());
	    Reset(); 
    }

    ScriptedInstance* pInstance; 

    bool Reached;
    uint32 VisibilityTimer ;

    void Reset() 
    {
        Reached = false;
    }
    void Aggro(Unit *who) {}
    void JustDied(Unit* Killer) 
    {
        if(!Reached)
        {
            me->CastSpell(me->getVictim(), SPELL_IMPFUSION, true);
            Reached = true;
        }
    }
    void KilledUnit(Unit *Victim) {}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
                return;

        if( me->isAttackReady() && !me->IsNonMeleeSpellCasted(false))
        {
            if(Reached)
                if(VisibilityTimer < diff)
                {
                    me->SetVisibility(VISIBILITY_OFF);
                    VisibilityTimer = 60000;
                }else VisibilityTimer -= diff;

            //If we are within range melee the target
            if(!Reached)
                if(me->IsWithinDistInMap(me->getVictim(), ATTACK_DISTANCE))
                {
                    me->CastSpell(me->getVictim(), SPELL_IMPFUSION, true);
                    VisibilityTimer = 2000;
                    Reached = true;
                }
        }
    }
};

//Dragon Gossip Menu
//This function is called when the player opens the gossip menubool
bool GossipHello_dragon(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(0, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1, "", 0, true);
    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(0, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2, "", 0, true);
    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(0, GOSSIP_ITEM_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3, "", 0, true);
    pPlayer->ADD_GOSSIP_ITEM_EXTENDED(0, GOSSIP_ITEM_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4, "", 0, true);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);

    pPlayer->PlayerTalkClass->SendGossipMenu(907, pCreature->GetGUID());

    return true;
}
//This function is called when the player clicks an option on the gossip menubool
bool GossipSelect_dragon(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+5)
    {
        //DoScriptText(SAY_NOT_INTERESTED, pCreature);
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

bool GossipSelectWithCode_dragon(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction, const char* sCode)
{
    if (uiSender == GOSSIP_SENDER_MAIN)
    {
        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CastSpell(pPlayer, SPELL_SHIELD_OF_BLUE, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            return true;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CastSpell(pPlayer, SPELL_REVITALIZE, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            return true;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CastSpell(pPlayer, SPELL_HASTE, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            return true;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->TeleportTo(pPlayer->GetMapId(), pPlayer->GetPositionX()+2, pPlayer->GetPositionY()+2, pPlayer->GetPositionZ(), pPlayer->GetOrientation());
            pPlayer->CLOSE_GOSSIP_MENU();
            return true;
        }
    }
    return false;
}


CreatureAI* GetAI_mob_killimp(Creature *_Creature)
{
    return new mob_killimpAI(_Creature);
}

CreatureAI* GetAI_mob_deceiver(Creature *_Creature)
{
    return new mob_deceiverAI(_Creature);
}

CreatureAI* GetAI_mob_armagedon(Creature *_Creature)
{
    return new mob_armagedonAI(_Creature);
}

CreatureAI* GetAI_boss_kiljaden(Creature *_Creature)
{
    return new boss_kiljadenAI(_Creature);
}

CreatureAI* GetAI_mob_orb(Creature *_Creature)
{
    return new mob_orbAI(_Creature);
}

CreatureAI* GetAI_mob_shadow_spike(Creature *_Creature)
{
    return new mob_shadow_spikeAI(_Creature);
}

void AddSC_boss_kiljaden()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_kiljaden";
    newscript->GetAI = &GetAI_boss_kiljaden;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_killimp";
    newscript->GetAI = &GetAI_mob_killimp;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_orb";
    newscript->GetAI = &GetAI_mob_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_armagedon";
    newscript->GetAI = &GetAI_mob_armagedon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_deceiver";
    newscript->GetAI = &GetAI_mob_deceiver;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_shadow_spike";
    newscript->GetAI = &GetAI_mob_shadow_spike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "dragon";
    newscript->pGossipHello = &GossipHello_dragon;
    newscript->pGossipSelect = &GossipSelect_dragon;
    newscript->pGossipSelectWithCode = &GossipSelectWithCode_dragon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_orb_of_the_blue_flight";
    newscript->pGOHello = &GO_go_orb_of_the_blue_flight;
    newscript->RegisterSelf();
}