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
SDName: Boss Sartharion
SD%Complete: 70%
SDComment: Flame wave, achievement and portal events need to be implemented
SDCategory: Obsidian Sanctum
EndScriptData */

#include "precompiled.h"
#include "obsidian_sanctum.h"

enum
{
    //Sartharion Yell
    SAY_SARTHARION_AGGRO                        = -1615018,
    SAY_SARTHARION_BERSERK                      = -1615019,
    SAY_SARTHARION_BREATH                       = -1615020,
    SAY_SARTHARION_CALL_SHADRON                 = -1615021,
    SAY_SARTHARION_CALL_TENEBRON                = -1615022,
    SAY_SARTHARION_CALL_VESPERON                = -1615023,
    SAY_SARTHARION_DEATH                        = -1615024,
    SAY_SARTHARION_SPECIAL_1                    = -1615025,
    SAY_SARTHARION_SPECIAL_2                    = -1615026,
    SAY_SARTHARION_SPECIAL_3                    = -1615027,
    SAY_SARTHARION_SPECIAL_4                    = -1615028,
    SAY_SARTHARION_SLAY_1                       = -1615029,
    SAY_SARTHARION_SLAY_2                       = -1615030,
    SAY_SARTHARION_SLAY_3                       = -1615031,

    WHISPER_LAVA_CHURN                          = -1615032,

    WHISPER_SHADRON_DICIPLE                     = -1615008,
    WHISPER_VESPERON_DICIPLE                    = -1615041,
    WHISPER_HATCH_EGGS                          = -1615017,
    WHISPER_OPEN_PORTAL                         = -1615042, // whisper, shared by two dragons

    //Sartharion Spells
    SPELL_BERSERK                               = 61632,    // Increases the caster's attack speed by 150% and all damage it deals by 500% for 5 min.
    SPELL_CLEAVE                                = 56909,    // Inflicts 35% weapon damage to an enemy and its nearest allies, affecting up to 10 targets.
    SPELL_FLAME_BREATH                          = 56908,    // Inflicts 8750 to 11250 Fire damage to enemies in a cone in front of the caster.
    SPELL_FLAME_BREATH_H                        = 58956,    // Inflicts 10938 to 14062 Fire damage to enemies in a cone in front of the caster.
    SPELL_TAIL_LASH                             = 56910,    // A sweeping tail strike hits all enemies behind the caster, inflicting 3063 to 3937 damage and stunning them for 2 sec.
    SPELL_TAIL_LASH_H                           = 58957,    // A sweeping tail strike hits all enemies behind the caster, inflicting 4375 to 5625 damage and stunning them for 2 sec.
    SPELL_WILL_OF_SARTHARION                    = 61254,    // Sartharion's presence bolsters the resolve of the Twilight Drakes, increasing their total health by 25%. This effect also increases Sartharion's health by 25%.
    SPELL_LAVA_STRIKE                           = 57571,    // (Real spell casted should be 57578) 57571 then trigger visual missile, then summon Lava Blaze on impact(spell 57572)
    SPELL_TWILIGHT_REVENGE                      = 60639,

    SPELL_PYROBUFFET                            = 56916,    // currently used for hard enrage after 15 minutes
    SPELL_PYROBUFFET_RANGE                      = 58907,    // possibly used when player get too far away from dummy creatures (2x creature entry 30494)

    SPELL_TWILIGHT_SHIFT_ENTER                  = 57620,    // enter phase. Player get this when click GO
    SPELL_TWILIGHT_SHIFT_REMOVAL                = 61187,    // leave phase
    SPELL_TWILIGHT_SHIFT_REMOVAL_ALL            = 61190,    // leave phase (probably version to make all leave)

    //Mini bosses common spells
    SPELL_TWILIGHT_RESIDUE                      = 61885,    // makes immune to shadow damage, applied when leave phase

    //Miniboses (Vesperon, Shadron, Tenebron)
    SPELL_SHADOW_BREATH_H                       = 59126,    // Inflicts 8788 to 10212 Fire damage to enemies in a cone in front of the caster.
    SPELL_SHADOW_BREATH                         = 57570,    // Inflicts 6938 to 8062 Fire damage to enemies in a cone in front of the caster.

    SPELL_SHADOW_FISSURE_H                      = 59127,    // Deals 9488 to 13512 Shadow damage to any enemy within the Shadow fissure after 5 sec.
    SPELL_SHADOW_FISSURE                        = 57579,    // Deals 6188 to 8812 Shadow damage to any enemy within the Shadow fissure after 5 sec.

    //Vesperon
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    NPC_ACOLYTE_OF_VESPERON                     = 31219,    // Acolyte of Vesperon
    SPELL_POWER_OF_VESPERON                     = 61251,    // Vesperon's presence decreases the maximum health of all enemies by 25%.
    SPELL_TWILIGHT_TORMENT_VESP                 = 57948,    // (Shadow only) trigger 57935 then 57988
    SPELL_TWILIGHT_TORMENT_VESP_ACO             = 58853,    // (Fire and Shadow) trigger 58835 then 57988

    //Shadron
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    NPC_ACOLYTE_OF_SHADRON                      = 31218,    // Acolyte of Shadron
    SPELL_POWER_OF_SHADRON                      = 58105,    // Shadron's presence increases Fire damage taken by all enemies by 100%.
    SPELL_GIFT_OF_TWILIGTH_SHA                  = 57835,    // TARGET_SCRIPT shadron
    SPELL_GIFT_OF_TWILIGTH_SAR                  = 58766,    // TARGET_SCRIPT sartharion

    //Tenebron
    //in the portal spawns 6 eggs, if not killed in time (approx. 20s)  they will hatch,  whelps can cast 60708
    SPELL_POWER_OF_TENEBRON                     = 61248,    // Tenebron's presence increases Shadow damage taken by all enemies by 100%.
    //Tenebron, dummy spell
    SPELL_SUMMON_TWILIGHT_WHELP                 = 58035,    // doesn't work, will spawn NPC_TWILIGHT_WHELP
    SPELL_SUMMON_SARTHARION_TWILIGHT_WHELP      = 58826,    // doesn't work, will spawn NPC_SHARTHARION_TWILIGHT_WHELP

    SPELL_HATCH_EGGS_H                          = 59189,
    SPELL_HATCH_EGGS                            = 58542,
    SPELL_HATCH_EGGS_EFFECT_H                   = 59190,
    SPELL_HATCH_EGGS_EFFECT                     = 58685,

    //Whelps
    NPC_TWILIGHT_WHELP                          = 30890,
    NPC_SARTHARION_TWILIGHT_WHELP              = 31214,
    SPELL_FADE_ARMOR                            = 60708,    // Reduces the armor of an enemy by 1500 for 15s

    //flame tsunami
    SPELL_FLAME_TSUNAMI                         = 57494,    // the visual dummy le skin flamme
    SPELL_FLAME_TSUNAMI_LEAP                    = 60241,    // SPELL_EFFECT_138 some leap effect, causing caster to move in direction
    SPELL_FLAME_TSUNAMI_DMG_AURA                = 57492,    // periodic damage, npc has this aura

    NPC_FLAME_TSUNAMI                           = 30616,    // for the flame waves
    NPC_LAVA_BLAZE                              = 30643,    // adds spawning from flame strike

    //using these custom points for dragons start and end
    POINT_ID_INIT                               = 100,
    POINT_ID_LAND                               = 200
};

struct Waypoint
{
    float m_fX, m_fY, m_fZ;
};

//each dragons special points. First where fly to before connect to connon, second where land point is.
Waypoint m_aTene[]=
{
    {3212.854, 575.597, 109.856},                           //init
    {3246.425, 565.367, 61.249}                             //end
};

Waypoint m_aShad[]=
{
    {3293.238, 472.223, 106.968},
    {3271.669, 526.907, 61.931}
};

Waypoint m_aVesp[]=
{
    {3193.310, 472.861, 102.697},
    {3227.268, 533.238, 59.995}
};

//points around raid "isle", counter clockwise. should probably be adjusted to be more alike
Waypoint m_aDragonCommon[]=
{
    {3214.012, 468.932, 98.652},
    {3244.950, 468.427, 98.652},
    {3283.520, 496.869, 98.652},
    {3287.316, 555.875, 98.652},
    {3250.479, 585.827, 98.652},
    {3209.969, 566.523, 98.652}
};

static float Vague_Coords[3][2][3]=
{
	{ {3209.845f,551.030f,55.3f}, {3279.845f,547.119f,55.3f} },
	{ {3210.700f,536.724f,55.3f}, {3284.020f,524.381f,55.3f} },
	{ {3210.772f,518.768f,55.3f}, {3279.720f,506.639f,55.3f} },
};

/*######
## Boss Sartharion
######*/

struct MANGOS_DLL_DECL boss_sartharionAI : public ScriptedAI
{
    boss_sartharionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsHeroic = !pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    bool m_bIsBerserk;
    bool m_bIsSoftEnraged;

    uint32 m_uiEnrageTimer;
    bool m_bIsHardEnraged;

    uint32 m_uiTenebronTimer;
    uint32 m_uiShadronTimer;
    uint32 m_uiVesperonTimer;

    uint32 m_uiFlameTsunamiTimer;
    uint32 m_uiLavaStrikeTimer;

    bool m_bHasCalledTenebron;
    bool m_bHasCalledShadron;
    bool m_bHasCalledVesperon;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_FLAME_BREATH_H,20000,25000,10000,TARGET_MAIN,0,SAY_SARTHARION_BREATH);
			Tasks.AddEvent(SPELL_TAIL_LASH_H,19000,15000,5000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_FLAME_BREATH,20000,25000,10000,TARGET_MAIN,0,SAY_SARTHARION_BREATH);
			Tasks.AddEvent(SPELL_TAIL_LASH,19000,15000,5000,TARGET_MAIN);
		}
		Tasks.AddEvent(SPELL_CLEAVE,7000,7000,3000,TARGET_MAIN);
        m_bIsBerserk = false;
        m_bIsSoftEnraged = false;

        m_uiEnrageTimer = MINUTE*15*IN_MILISECONDS;
        m_bIsHardEnraged = false;

        m_uiTenebronTimer = 30000;
        m_uiShadronTimer = 75000;
        m_uiVesperonTimer = 120000;

        m_uiFlameTsunamiTimer = 30000;
        m_uiLavaStrikeTimer = 5000;

        m_bHasCalledTenebron = false;
        m_bHasCalledShadron = false;
        m_bHasCalledVesperon = false;

        if (me->HasAura(SPELL_TWILIGHT_REVENGE))
            me->RemoveAurasDueToSpell(SPELL_TWILIGHT_REVENGE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_SARTHARION_AGGRO,me);

        me->SetInCombatWithZone();

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, IN_PROGRESS);
            FetchDragons();
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_SARTHARION_DEATH,me);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, DONE);

		Tasks.GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME, 2);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SARTHARION_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SARTHARION_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SARTHARION_SLAY_3, me); break;
        }
    }

    void FetchDragons()
    {
        Unit* pTene = Unit::GetUnit(*me,m_pInstance->GetData64(DATA_TENEBRON));
        Unit* pShad = Unit::GetUnit(*me,m_pInstance->GetData64(DATA_SHADRON));
        Unit* pVesp = Unit::GetUnit(*me,m_pInstance->GetData64(DATA_VESPERON));

        //if at least one of the dragons are alive and are being called
        bool bCanUseWill = false;

        if (pTene && pTene->isAlive() && !pTene->getVictim())
        {
            bCanUseWill = true;
            pTene->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aTene[0].m_fX, m_aTene[0].m_fY, m_aTene[0].m_fZ);

            if (!pTene->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pTene->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if (pShad && pShad->isAlive() && !pShad->getVictim())
        {
            bCanUseWill = true;
            pShad->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aShad[0].m_fX, m_aShad[0].m_fY, m_aShad[0].m_fZ);

            if (!pShad->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pShad->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if (pVesp && pVesp->isAlive() && !pVesp->getVictim())
        {
            bCanUseWill = true;
            pVesp->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aVesp[0].m_fX, m_aVesp[0].m_fY, m_aVesp[0].m_fZ);

            if (!pVesp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pVesp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if (bCanUseWill && !me->HasAura(SPELL_WILL_OF_SARTHARION,0))
		{
			Tasks.SetAuraStack(SPELL_WILL_OF_SARTHARION,1,me,me,1);
			me->SetHealth(me->GetMaxHealth());
			
		}
    }

    void CallDragon(uint32 uiDataId)
    {
        if (m_pInstance)
        {
            Creature* pTemp = (Creature*)Unit::GetUnit((*me),m_pInstance->GetData64(uiDataId));

            if (pTemp && pTemp->isAlive() && !pTemp->getVictim())
            {
                if (pTemp->HasMonsterMoveFlag(MONSTER_MOVE_WALK))
                    pTemp->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);

                if (pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                int32 iTextId = 0;

                switch(pTemp->GetEntry())
                {
                    case NPC_TENEBRON:
                        iTextId = SAY_SARTHARION_CALL_TENEBRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aTene[1].m_fX, m_aTene[1].m_fY, m_aTene[1].m_fZ);
                        break;
                    case NPC_SHADRON:
                        iTextId = SAY_SARTHARION_CALL_SHADRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aShad[1].m_fX, m_aShad[1].m_fY, m_aShad[1].m_fZ);
                        break;
                    case NPC_VESPERON:
                        iTextId = SAY_SARTHARION_CALL_VESPERON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aVesp[1].m_fX, m_aVesp[1].m_fY, m_aVesp[1].m_fZ);
                        break;
                }

                DoScriptText(iTextId, me);
            }
        }
    }

    void SendFlameTsunami()
    {
        Map* pMap = me->GetMap();

        if (pMap && pMap->IsDungeon())
        {
			Tasks.Speak(CHAT_TYPE_BOSS_EMOTE,0,"La lave autour de Sartharion bouillonne !");
			switch(urand(0,3))
			{
				case 0:
					if(Unit* tmp = me->SummonCreature(NPC_FLAME_TSUNAMI,Vague_Coords[0][0][0],Vague_Coords[0][0][1],Vague_Coords[0][0][2],0,TEMPSUMMON_TIMED_DESPAWN,30000))
						tmp->GetMotionMaster()->MovePoint(0,Vague_Coords[0][1][0],Vague_Coords[0][1][1],Vague_Coords[0][1][2]);
					if(Unit* tmp = me->SummonCreature(NPC_FLAME_TSUNAMI,Vague_Coords[2][0][0],Vague_Coords[2][0][1],Vague_Coords[2][0][2],0,TEMPSUMMON_TIMED_DESPAWN,30000))
						tmp->GetMotionMaster()->MovePoint(0,Vague_Coords[2][1][0],Vague_Coords[2][1][1],Vague_Coords[2][1][2]);
					break;
				case 1:
					if(Unit* tmp = me->SummonCreature(NPC_FLAME_TSUNAMI,Vague_Coords[1][0][0],Vague_Coords[1][0][1],Vague_Coords[1][0][2],0,TEMPSUMMON_TIMED_DESPAWN,30000))
						tmp->GetMotionMaster()->MovePoint(0,Vague_Coords[1][1][0],Vague_Coords[1][1][1],Vague_Coords[1][1][2]);
					break;
				case 2:
					if(Unit* tmp = me->SummonCreature(NPC_FLAME_TSUNAMI,Vague_Coords[0][1][0],Vague_Coords[0][1][1],Vague_Coords[0][1][2],0,TEMPSUMMON_TIMED_DESPAWN,30000))
						tmp->GetMotionMaster()->MovePoint(0,Vague_Coords[0][0][0],Vague_Coords[0][0][1],Vague_Coords[0][0][2]);
					if(Unit* tmp = me->SummonCreature(NPC_FLAME_TSUNAMI,Vague_Coords[2][1][0],Vague_Coords[2][1][1],Vague_Coords[2][1][2],0,TEMPSUMMON_TIMED_DESPAWN,30000))
						tmp->GetMotionMaster()->MovePoint(0,Vague_Coords[2][0][0],Vague_Coords[2][0][1],Vague_Coords[2][0][2]);
					break;
				case 3:
					if(Unit* tmp = me->SummonCreature(NPC_FLAME_TSUNAMI,Vague_Coords[1][1][0],Vague_Coords[1][1][1],Vague_Coords[1][1][2],0,TEMPSUMMON_TIMED_DESPAWN,30000))
						tmp->GetMotionMaster()->MovePoint(0,Vague_Coords[1][0][0],Vague_Coords[1][0][1],Vague_Coords[1][0][2]);
					break;
			}
			
			
			
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        //spell will target dragons, if they are still alive at 35%
        if (!m_bIsBerserk && (me->GetHealth()*100 / me->GetMaxHealth()) <= 35)
        {
			Creature* pTemp = (Creature*)Unit::GetUnit((*me),m_pInstance->GetData64(DATA_TENEBRON));
			Creature* pTemp2 = (Creature*)Unit::GetUnit((*me),m_pInstance->GetData64(DATA_SHADRON));
			Creature* pTemp3 = (Creature*)Unit::GetUnit((*me),m_pInstance->GetData64(DATA_VESPERON));
			if((pTemp && pTemp->isAlive()) || (pTemp2 && pTemp2->isAlive()) || (pTemp3 && pTemp3->isAlive()))
			{
				DoScriptText(SAY_SARTHARION_BERSERK,me);
				Tasks.SetAuraStack(SPELL_BERSERK,1,me,me);
				m_bIsBerserk = true;
			}
        }

        //soft enrage
        if (!m_bIsSoftEnraged && (me->GetHealth()*100 / me->GetMaxHealth()) <= 10)
        {
            // TODO
            m_bIsSoftEnraged = true;
        }

        // hard enrage
        if (!m_bIsHardEnraged)
        {
            if (m_uiEnrageTimer < uiDiff)
            {
                DoCastMe(SPELL_PYROBUFFET, true);
                m_bIsHardEnraged = true;
            }
            else
                m_uiEnrageTimer -= uiDiff;
        }

        // flame tsunami
        if (m_uiFlameTsunamiTimer < uiDiff)
        {
            SendFlameTsunami();
            m_uiFlameTsunamiTimer = 30000;
        }
        else
            m_uiFlameTsunamiTimer -= uiDiff;

         // Lavas Strike
        if (m_uiLavaStrikeTimer < uiDiff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                DoCast(pTarget, SPELL_LAVA_STRIKE);

                switch(urand(0, 10))
                {
                    case 0: DoScriptText(SAY_SARTHARION_SPECIAL_1, me); break;
                    case 1: DoScriptText(SAY_SARTHARION_SPECIAL_2, me); break;
                    case 2: DoScriptText(SAY_SARTHARION_SPECIAL_3, me); break;
                }
            }
            m_uiLavaStrikeTimer = urand(5000, 20000);
        }
        else
            m_uiLavaStrikeTimer -= uiDiff;

        // call tenebron
        if (!m_bHasCalledTenebron && m_uiTenebronTimer < uiDiff)
        {
            CallDragon(DATA_TENEBRON);
            m_bHasCalledTenebron = true;
        }
        else
            m_uiTenebronTimer -= uiDiff;

        // call shadron
        if (!m_bHasCalledShadron && m_uiShadronTimer < uiDiff)
        {
            CallDragon(DATA_SHADRON);
            m_bHasCalledShadron = true;
        }
        else
            m_uiShadronTimer -= uiDiff;

        // call vesperon
        if (!m_bHasCalledVesperon && m_uiVesperonTimer < uiDiff)
        {
            CallDragon(DATA_VESPERON);
            m_bHasCalledVesperon = true;
        }
        else
            m_uiVesperonTimer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

CreatureAI* GetAI_boss_sartharion(Creature* pCreature)
{
    return new boss_sartharionAI(pCreature);
}

enum TeneText
{
    SAY_TENEBRON_AGGRO                      = -1615009,
    SAY_TENEBRON_SLAY_1                     = -1615010,
    SAY_TENEBRON_SLAY_2                     = -1615011,
    SAY_TENEBRON_DEATH                      = -1615012,
    SAY_TENEBRON_BREATH                     = -1615013,
    SAY_TENEBRON_RESPOND                    = -1615014,
    SAY_TENEBRON_SPECIAL_1                  = -1615015,
    SAY_TENEBRON_SPECIAL_2                  = -1615016
};

enum ShadText
{
    SAY_SHADRON_AGGRO                       = -1615000,
    SAY_SHADRON_SLAY_1                      = -1615001,
    SAY_SHADRON_SLAY_2                      = -1615002,
    SAY_SHADRON_DEATH                       = -1615003,
    SAY_SHADRON_BREATH                      = -1615004,
    SAY_SHADRON_RESPOND                     = -1615005,
    SAY_SHADRON_SPECIAL_1                   = -1615006,
    SAY_SHADRON_SPECIAL_2                   = -1615007
};

enum VespText
{
    SAY_VESPERON_AGGRO                      = -1615033,
    SAY_VESPERON_SLAY_1                     = -1615034,
    SAY_VESPERON_SLAY_2                     = -1615035,
    SAY_VESPERON_DEATH                      = -1615036,
    SAY_VESPERON_BREATH                     = -1615037,
    SAY_VESPERON_RESPOND                    = -1615038,
    SAY_VESPERON_SPECIAL_1                  = -1615039,
    SAY_VESPERON_SPECIAL_2                  = -1615040
};

//to control each dragons common abilities
struct MANGOS_DLL_DECL dummy_dragonAI : public ScriptedAI
{
    dummy_dragonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
		m_bIsHeroic = !pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    uint32 m_uiWaypointId;
    uint32 m_uiMoveNextTimer;
    int32 m_iPortalRespawnTime;
    bool m_bCanMoveFree;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        m_uiWaypointId = 0;
        m_uiMoveNextTimer = 500;
        m_iPortalRespawnTime = 30000;
        m_bCanMoveFree = false;
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (!m_pInstance || uiType != POINT_MOTION_TYPE)
            return;

        //if healers messed up the raid and we was already initialized
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            EnterEvadeMode();
            return;
        }

        //this is the end (!)
        if (uiPointId == POINT_ID_LAND)
        {
            me->GetMotionMaster()->Clear();
            m_bCanMoveFree = false;
            me->SetInCombatWithZone();
            return;
        }

        //get amount of common points
        uint32 uiCommonWPCount = sizeof(m_aDragonCommon)/sizeof(Waypoint);

        //increase
        m_uiWaypointId = uiPointId+1;

        //if we have reached a point bigger or equal to count, it mean we must reset to point 0
        if (m_uiWaypointId >= uiCommonWPCount)
        {
            if (!m_bCanMoveFree)
                m_bCanMoveFree = true;

            m_uiWaypointId = 0;
        }

        m_uiMoveNextTimer = 500;
    }

    //used when open portal and spawn mobs in phase
    void DoRaidWhisper(int32 iTextId)
    {
        Map* pMap = me->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    DoScriptText(iTextId, me, i->getSource());
            }
        }
    }

    //"opens" the portal and does the "opening" whisper
    void OpenPortal()
    {
        int32 iTextId = 0;

        //there are 4 portal spawn locations, each are expected to be spawned with negative spawntimesecs in database

        //using a grid search here seem to be more efficient than caching all four guids
        //in instance script and calculate range to each.
        GameObject* pPortal = GetClosestGameObjectWithEntry(me,GO_TWILIGHT_PORTAL,50.0f);

        switch(me->GetEntry())
        {
            case NPC_TENEBRON:
                iTextId = WHISPER_HATCH_EGGS;
                break;
            case NPC_SHADRON:
            case NPC_VESPERON:
                iTextId = WHISPER_OPEN_PORTAL;
                break;
        }

        DoRaidWhisper(iTextId);

        //By using SetRespawnTime() we will actually "spawn" the object with our defined time.
        //Once time is up, portal will disappear again.
        if (pPortal && !pPortal->isSpawned())
            pPortal->SetRespawnTime(m_iPortalRespawnTime);

        //Unclear what are expected to happen if one drake has a portal open already
        //Refresh respawnTime so time again are set to 30secs?
    }

    //Removes each drakes unique debuff from players
    void RemoveDebuff(uint32 uiSpellId)
    {
        Map* pMap = me->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive() && i->getSource()->HasAura(uiSpellId))
                    i->getSource()->RemoveAurasDueToSpell(uiSpellId);
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        int32 iTextId = 0;
        uint32 uiSpellId = 0;

        switch(me->GetEntry())
        {
            case NPC_TENEBRON:
                iTextId = SAY_TENEBRON_DEATH;
                uiSpellId = SPELL_POWER_OF_TENEBRON;
                break;
            case NPC_SHADRON:
                iTextId = SAY_SHADRON_DEATH;
                uiSpellId = SPELL_POWER_OF_SHADRON;
                break;
            case NPC_VESPERON:
                iTextId = SAY_VESPERON_DEATH;
                uiSpellId = SPELL_POWER_OF_VESPERON;
                break;
        }

        DoScriptText(iTextId, me);

        RemoveDebuff(uiSpellId);

        if (m_pInstance)
        {
            // not if solo mini-boss fight
            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
                return;

            // Twilight Revenge to main boss
            if (Unit* pSartharion = Unit::GetUnit((*me), m_pInstance->GetData64(DATA_SARTHARION)))
            {
                if (pSartharion->isAlive())
                    me->CastSpell(pSartharion,SPELL_TWILIGHT_REVENGE,true);
            }
        }
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(57874))
					{
						pPlayer->RemoveAurasDueToSpell(57874);
						pPlayer->SetPhaseMask(0x00000001,true);
						Tasks.SetAuraStack(61885,1,pPlayer,pPlayer,1);
					}
		Tasks.GiveEmblemsToGroup((m_bIsHeroic) ? VAILLANCE : HEROISME);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bCanMoveFree && m_uiMoveNextTimer)
        {
            if (m_uiMoveNextTimer <= uiDiff)
            {
                me->GetMotionMaster()->MovePoint(m_uiWaypointId,
                    m_aDragonCommon[m_uiWaypointId].m_fX, m_aDragonCommon[m_uiWaypointId].m_fY, m_aDragonCommon[m_uiWaypointId].m_fZ);

                m_uiMoveNextTimer = 0;
            }
            else
                m_uiMoveNextTimer -= uiDiff;
        }
    }
};

/*######
## Mob Tenebron
######*/

struct MANGOS_DLL_DECL mob_tenebronAI : public dummy_dragonAI
{
    mob_tenebronAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    uint32 EggTimer;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SHADOW_FISSURE_H : SPELL_SHADOW_FISSURE,5000,15000,5000);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SHADOW_BREATH_H : SPELL_SHADOW_BREATH,20000,20000,5000,TARGET_MAIN,
			0,SAY_TENEBRON_BREATH);
        EggTimer = 30000;
		if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_TENEBRON_AGGRO, me);
        DoCastMe( SPELL_POWER_OF_TENEBRON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_TENEBRON_SLAY_1 : SAY_TENEBRON_SLAY_2, me);
    }

	void JustDied(Unit* pwho)
	{
		dummy_dragonAI::JustDied(pwho);
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(57874))
					{
						pPlayer->RemoveAurasDueToSpell(57874);
						pPlayer->SetPhaseMask(0x00000001,true);
						Tasks.SetAuraStack(61885,1,pPlayer,pPlayer,1);
					}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!me->SelectHostileTarget() || !me->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

		if(EggTimer <= uiDiff)
		{
			for(uint8 i=0;i<6;i++)
				if(Creature* cr = Tasks.CallCreature(30882,THREE_MINS,NEAR_7M))
					cr->SetPhaseMask(0x16,true);
			EggTimer = 30000;
		}
		else
			EggTimer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_tenebron(Creature* pCreature)
{
    return new mob_tenebronAI(pCreature);
}


struct MANGOS_DLL_DECL mob_shadronAI : public dummy_dragonAI
{
    mob_shadronAI(Creature* pCreature) : dummy_dragonAI(pCreature) { 
		pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
		Reset(); }

    uint32 m_uiAcolyteShadronTimer;
	MobEventTasks Tasks;
	ScriptedInstance* pInstance;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SHADOW_FISSURE_H : SPELL_SHADOW_FISSURE,5000,15000,5000);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SHADOW_BREATH_H : SPELL_SHADOW_BREATH,20000,20000,5000,TARGET_MAIN,
			0,SAY_SHADRON_BREATH);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SHADOW_FISSURE_H: SPELL_SHADOW_FISSURE,30000,30000);
        m_uiAcolyteShadronTimer = 60000;

        if (me->HasAura(SPELL_TWILIGHT_TORMENT_VESP))
            me->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP);

        if (me->HasAura(SPELL_GIFT_OF_TWILIGTH_SHA))
            me->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);

		if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_SHADRON_AGGRO,me);
        DoCastMe( SPELL_POWER_OF_SHADRON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SHADRON_SLAY_1 : SAY_SHADRON_SLAY_2, me);
    }

	void DoShadronEvent()
	{
		if(Creature* tmp = Tasks.CallCreature(NPC_ACOLYTE_OF_SHADRON,60000))
		{
			tmp->SetPhaseMask(0x16,true);
		}
		me->CastStop();
		
		if(Unit* Sartha = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_SARTHARION) : 0))
		{
			if(Sartha->isAlive())
				if(m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
					Tasks.SetAuraStack(SPELL_GIFT_OF_TWILIGTH_SAR,1,Sartha,me,1);
				else
					Tasks.SetAuraStack(SPELL_GIFT_OF_TWILIGTH_SHA,1,me,me,1);
		}
	}

	void JustDied(Unit* pwho)
	{
		dummy_dragonAI::JustDied(pwho);
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(57874))
					{
						pPlayer->RemoveAurasDueToSpell(57874);
						pPlayer->SetPhaseMask(0x00000001,true);
						Tasks.SetAuraStack(61885,1,pPlayer,pPlayer,1);
					}
	}
    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!me->SelectHostileTarget() || !me->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

		if(m_uiAcolyteShadronTimer <= uiDiff)
		{
			DoShadronEvent();
			m_uiAcolyteShadronTimer = 60000;
		}
		else
			m_uiAcolyteShadronTimer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadron(Creature* pCreature)
{
    return new mob_shadronAI(pCreature);
}

struct MANGOS_DLL_DECL mob_vesperonAI : public dummy_dragonAI
{
    mob_vesperonAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    uint32 m_uiShadowBreathTimer;
    uint32 m_uiShadowFissureTimer;
    uint32 m_uiAcolyteVesperonTimer;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SHADOW_FISSURE_H : SPELL_SHADOW_FISSURE,5000,15000,5000);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_SHADOW_BREATH_H : SPELL_SHADOW_BREATH,20000,20000,5000,TARGET_MAIN,
			0,SAY_VESPERON_BREATH);
        m_uiShadowBreathTimer = 20000;
        m_uiShadowFissureTimer = 5000;
        m_uiAcolyteVesperonTimer = 60000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_VESPERON_AGGRO,me);
        DoCastMe( SPELL_POWER_OF_VESPERON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_VESPERON_SLAY_1 : SAY_VESPERON_SLAY_2, me);
    }

	void JustDied(Unit* pwho)
	{
		dummy_dragonAI::JustDied(pwho);
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(57874))
					{
						pPlayer->RemoveAurasDueToSpell(57874);
						pPlayer->SetPhaseMask(0x00000001,true);
						Tasks.SetAuraStack(61885,1,pPlayer,pPlayer,1);
					}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!me->SelectHostileTarget() || !me->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_vesperon(Creature* pCreature)
{
    return new mob_vesperonAI(pCreature);
}

struct MANGOS_DLL_DECL mob_acolyte_of_shadronAI : public ScriptedAI
{
    mob_acolyte_of_shadronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

	MobEventTasks Tasks;
	bool m_bIsHeroic;
    ScriptedInstance* m_pInstance;

    void Reset()
    {
		Tasks.SetObjects(this,me);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            Creature* pDebuffTarget = NULL;

            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS)
            {
                pDebuffTarget = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SARTHARION));

                if (pDebuffTarget && pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_GIFT_OF_TWILIGTH_SAR))
                    pDebuffTarget->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SAR);
            }
            else
            {
                pDebuffTarget = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_SHADRON));

                if (pDebuffTarget && pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_GIFT_OF_TWILIGTH_SHA))
                    pDebuffTarget->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGTH_SHA);
            }
        }
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(57874))
					{
						pPlayer->RemoveAurasDueToSpell(57874);
						pPlayer->SetPhaseMask(0x00000001,true);
						Tasks.SetAuraStack(61885,1,pPlayer,pPlayer,1);
					}
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_acolyte_of_shadron(Creature* pCreature)
{
    return new mob_acolyte_of_shadronAI(pCreature);
}

struct MANGOS_DLL_DECL mob_acolyte_of_vesperonAI : public ScriptedAI
{
    mob_acolyte_of_vesperonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
        DoCastMe(SPELL_TWILIGHT_TORMENT_VESP_ACO);
    }

    void JustDied(Unit* pKiller)
    {
        // remove twilight torment on Vesperon
        if (m_pInstance)
        {
            Creature* pVesperon = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_VESPERON));

            if (pVesperon && pVesperon->isAlive() && pVesperon->HasAura(SPELL_TWILIGHT_TORMENT_VESP))
                pVesperon->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP);
        }
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		if (!lPlayers.isEmpty())
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && pPlayer->HasAura(57874))
					{
						pPlayer->RemoveAurasDueToSpell(57874);
						pPlayer->SetPhaseMask(0x00000001,true);
						Tasks.SetAuraStack(61885,1,pPlayer,pPlayer,1);
					}
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_acolyte_of_vesperon(Creature* pCreature)
{
    return new mob_acolyte_of_vesperonAI(pCreature);
}

struct MANGOS_DLL_DECL mob_twilight_eggsAI : public ScriptedAI
{
    mob_twilight_eggsAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

	uint32 hatch_Timer;
	MobEventTasks Tasks;
    void Reset()
    {
		Tasks.SetObjects(this,me);
		hatch_Timer = 20000;
		SetCombatMovement(false);
    }

    void UpdateAI(const uint32 diff)
	{
		if(hatch_Timer <= diff)
		{
			if(me->isAlive())
			{
				if(Creature* cr = Tasks.CallCreature(NPC_TWILIGHT_WHELP,THREE_MINS))
					cr->SetPhaseMask(0x01,true);
				Tasks.Kill(me);
			}
			hatch_Timer = DAY;
		}
		else
			hatch_Timer -= diff;
	}
};

CreatureAI* GetAI_mob_twilight_eggs(Creature* pCreature)
{
    return new mob_twilight_eggsAI(pCreature);
}

struct MANGOS_DLL_DECL mob_twilight_fissureAI : public Scripted_NoMovementAI
{
    mob_twilight_fissureAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) 
	{
		Reset(); 
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
	}

	bool m_bIsHeroic;
	MobEventTasks Tasks;
    void Reset()
    {
		Tasks.SetObjects(this,me);
		SetCombatMovement(false);
		me->SetLevel(83);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		Tasks.AddEvent(50657,500,750,0,TARGET_ME);
		Tasks.AddEvent(m_bIsHeroic ? 59128:57581,5000,5000,0,TARGET_NEAR);
    }

	void UpdateAI(const uint32 diff)
	{
		Tasks.UpdateEvent(diff);
	}

};

CreatureAI* GetAI_mob_twilight_fissure(Creature* pCreature)
{
    return new mob_twilight_fissureAI(pCreature);
}

/*######
## Mob Twilight Whelps
######*/

struct MANGOS_DLL_DECL mob_twilight_whelpAI : public ScriptedAI
{
    mob_twilight_whelpAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiFadeArmorTimer;

    void Reset()
    {
        m_uiFadeArmorTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        // twilight torment
        if (m_uiFadeArmorTimer < uiDiff)
        {
            DoCastVictim( SPELL_FADE_ARMOR);
            m_uiFadeArmorTimer = urand(5000, 10000);
        }
        else
            m_uiFadeArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_twilight_whelp(Creature* pCreature)
{
    return new mob_twilight_whelpAI(pCreature);
}

struct MANGOS_DLL_DECL sartha_tsunamiAI : public ScriptedAI
{
    sartha_tsunamiAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiBumpTimer;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.SetAuraStack(SPELL_FLAME_TSUNAMI,1,me,me,1);
		SetCombatMovement(false);
        m_uiBumpTimer = 1000;
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);

    }

	void TsunamiBump()
	{
		Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
        if (!PlayerList.isEmpty())
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (Player* plr = i->getSource())
					if(plr->isAlive() && plr->GetDistance2d(me) < 8.0f)
					{
						plr->CastStop();
						Tasks.SetAuraStack(57491,1,plr,plr,1);
						plr->CastSpell(plr,SPELL_FLAME_TSUNAMI_LEAP,true);
					}

	}

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!me->SelectHostileTarget() || !me->getVictim())
            return;

        // twilight torment
        if (m_uiBumpTimer < uiDiff)
        {
            TsunamiBump();
            m_uiBumpTimer = 1000;
        }
        else
            m_uiBumpTimer -= uiDiff;
    }
};

CreatureAI* GetAI_sartha_tsunami(Creature* pCreature)
{
    return new sartha_tsunamiAI(pCreature);
}

void AddSC_boss_sartharion()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sartharion";
    newscript->GetAI = &GetAI_boss_sartharion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_vesperon";
    newscript->GetAI = &GetAI_mob_vesperon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadron";
    newscript->GetAI = &GetAI_mob_shadron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_tenebron";
    newscript->GetAI = &GetAI_mob_tenebron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_acolyte_of_shadron";
    newscript->GetAI = &GetAI_mob_acolyte_of_shadron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_acolyte_of_vesperon";
    newscript->GetAI = &GetAI_mob_acolyte_of_vesperon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_eggs";
    newscript->GetAI = &GetAI_mob_twilight_eggs;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_twilight_whelp";
    newscript->GetAI = &GetAI_mob_twilight_whelp;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_twilight_fissure";
    newscript->GetAI = &GetAI_mob_twilight_fissure;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "sartha_tsunami";
    newscript->GetAI = &GetAI_sartha_tsunami;
    newscript->RegisterSelf();
}
