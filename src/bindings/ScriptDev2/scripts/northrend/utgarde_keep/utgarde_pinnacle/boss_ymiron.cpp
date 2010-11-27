/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO                   = -1575031,
    SAY_SUMMON_BJORN            = -1575032,
    SAY_SUMMON_HALDOR           = -1575033,
    SAY_SUMMON_RANULF           = -1575034,
    SAY_SUMMON_TORGYN           = -1575035,
    SAY_SLAY_1                  = -1575036,
    SAY_SLAY_2                  = -1575037,
    SAY_SLAY_3                  = -1575038,
    SAY_SLAY_4                  = -1575039,
    SAY_DEATH                   = -1575040,

	SPELL_BANE_N                            = 48294,
	SPELL_BANE_H                            = 59301,
    SPELL_DARK_SLASH                        = 48292,
    SPELL_FETID_ROT_N                       = 48291,
    SPELL_FETID_ROT_H                       = 59300,
    SPELL_SCREAMS_OF_THE_DEAD               = 51750,
    SPELL_SPIRIT_BURST_N                    = 48529, // when Ranulf
    SPELL_SPIRIT_BURST_H                    = 59305, // when Ranulf
    SPELL_SPIRIT_STRIKE_N                   = 48423, // when Haldor
    SPELL_SPIRIT_STRIKE_H                   = 59304, // when Haldor
    SPELL_ANCESTORS_VENGEANCE               = 16939, // 2 normal, 4 heroic

    SPELL_SUMMON_AVENGING_SPIRIT            = 48592,
    SPELL_SUMMON_SPIRIT_FOUNT               = 48386,

    SPELL_CHANNEL_SPIRIT_TO_YMIRON          = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT          = 48307,

    SPELL_SPIRIT_FOUNT_N                    = 48380,
    SPELL_SPIRIT_FOUNT_H                    = 59320,

    NPC_BJORN                               = 27303, // Near Right Boat, summon Spirit Fount
    NPC_BJORN_VISUAL                        = 27304,
    NPC_HALDOR                              = 27307, // Near Left Boat, debuff Spirit Strike on player
    NPC_HALDOR_VISUAL                       = 27310,
    NPC_RANULF                              = 27308, // Far Left Boat, ability to cast spirit burst
    NPC_RANULF_VISUAL                       = 27311,
    NPC_TORGYN                              = 27309, // Far Right Boat, summon 4 Avenging Spirit
    NPC_TORGYN_VISUAL                       = 27312,

    NPC_SPIRIT_FOUNT                        = 27339,
    NPC_AVENGING_SPIRIT                     = 27386
};

struct ActiveBoatStruct
{
    uint32 npc;
    uint32 say;
    float MoveX,MoveY,MoveZ,SpawnX,SpawnY,SpawnZ,SpawnO;
};

static ActiveBoatStruct ActiveBot[4] =
{
    {NPC_BJORN_VISUAL,  SAY_SUMMON_BJORN,  404.379, -335.335, 104.756, 413.594, -335.408, 107.995, 3.157},
    {NPC_HALDOR_VISUAL, SAY_SUMMON_HALDOR, 380.813, -335.069, 104.756, 369.994, -334.771, 107.995, 6.232},
    {NPC_RANULF_VISUAL, SAY_SUMMON_RANULF, 381.546, -314.362, 104.756, 370.841, -314.426, 107.995, 6.232},
    {NPC_TORGYN_VISUAL, SAY_SUMMON_TORGYN, 404.310, -314.761, 104.756, 413.992, -314.703, 107.995, 3.157}
};
/*######
## boss_ymiron
######*/

struct MANGOS_DLL_DECL boss_ymironAI : public LibDevFSAI
{
	uint8 m_uiActiveOrder[4];
	bool m_bIsWalking;
    bool m_bIsPause;
    bool m_bIsActiveWithBJORN;
    bool m_bIsActiveWithHALDOR;
    bool m_bIsActiveWithRANULF;
    bool m_bIsActiveWithTORGYN;
 
    uint8 m_uiActivedNumber;

    uint32 m_uiFetidRot_Timer;
    uint32 m_uiBane_Timer;
    uint32 m_uiDarkSlash_Timer;
    uint32 m_uiAncestors_Vengeance_Timer;

    uint32 m_uiAbility_BJORN_Timer;
    uint32 m_uiAbility_HALDOR_Timer;
    uint32 m_uiAbility_RANULF_Timer;
    uint32 m_uiAbility_TORGYN_Timer;

    uint32 m_uiPause_Timer;
    uint32 m_uiHealthAmountModifier;
    uint32 m_uiHealthAmountMultipler;

    uint64 m_uiActivedCreatureGUID;
    uint64 m_uiOrbGUID;

    boss_ymironAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
        m_difficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    bool m_difficulty;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_ANCESTORS_VENGEANCE,40000,45000,5000,TARGET_ME);
		if(m_difficulty)
		{
			Tasks.AddEvent(SPELL_BANE_H,5000,20000,5000,TARGET_ME);
			Tasks.AddEvent(SPELL_FETID_ROT_H,10000,10000,5000,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(SPELL_BANE_N,5000,20000,5000,TARGET_ME);
			Tasks.AddEvent(SPELL_FETID_ROT_N,10000,10000,5000,TARGET_MAIN);
		}
		m_bIsPause = false;
		srand(time(NULL));
        for (int i = 0; i < 4; ++i)
            m_uiActiveOrder[i] = i;
        for (int i = 0; i < 3; ++i)
        {
            int r = i + (rand()%(4-i));
            int temp = m_uiActiveOrder[i];
            m_uiActiveOrder[i] = m_uiActiveOrder[r];
            m_uiActiveOrder[r] = temp;
        }

        SpellEntry* TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_CHANNEL_SPIRIT_TO_YMIRON);
        if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 16)
        {
               TempSpell->EffectImplicitTargetA[0] = 53;
               TempSpell->EffectImplicitTargetA[1] = 0;
               TempSpell->EffectImplicitTargetB[0] = 16;
               TempSpell->EffectImplicitTargetB[1] = 0;
        }

		m_bIsActiveWithBJORN = false;
        m_bIsActiveWithHALDOR = false;
        m_bIsActiveWithRANULF = false;
        m_bIsActiveWithTORGYN = false;

        m_uiFetidRot_Timer            = 8000+rand()%5000;
        m_uiDarkSlash_Timer           = 28000+rand()%5000;
        m_uiAncestors_Vengeance_Timer = (m_difficulty ? 60000 : 45000);
        m_uiPause_Timer               = 0;

        m_uiAbility_BJORN_Timer  = 0;
        m_uiAbility_HALDOR_Timer = 0;
        m_uiAbility_RANULF_Timer = 0;
        m_uiAbility_TORGYN_Timer = 0;

        m_uiActivedNumber        = 0;
        m_uiHealthAmountModifier = 1;
        m_uiHealthAmountMultipler = (m_difficulty ? 20 : 25);

        DespawnBoatGhosts(m_uiActivedCreatureGUID);
        DespawnBoatGhosts(m_uiOrbGUID);

        if(pInstance)
            pInstance->SetData(TYPE_YMIRON, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
		if(pInstance)
            pInstance->SetData(TYPE_YMIRON, IN_PROGRESS);
    }

	void AttackStart(Unit* pWho)
    {
        if (m_bIsPause)
            return;

        if (!pWho || pWho == me)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void DespawnBoatGhosts(uint64& m_uiCreatureGUID)
    {
        if (m_uiCreatureGUID)
            if (Creature* pTemp = (Creature*)Unit::GetUnit(*me, m_uiCreatureGUID))
                //pTemp->ForcedDespawn();
                pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        m_uiCreatureGUID = 0;
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
            case 2: DoScriptText(SAY_SLAY_3, me); break;
            case 3: DoScriptText(SAY_SLAY_4, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);

		DespawnBoatGhosts(m_uiActivedCreatureGUID);
        DespawnBoatGhosts(m_uiOrbGUID);

        if(pInstance)
            pInstance->SetData(TYPE_YMIRON, DONE);

		GiveEmblemsToGroup(m_difficulty ? HEROISME : 0,1,true);
		GiveRandomReward();
    }

    void UpdateAI(const uint32 diff)
    {
		if (m_bIsWalking)
        {
            if (m_uiPause_Timer < diff)
            {
                DoScriptText(ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].say, me);
                DoCastMe( SPELL_CHANNEL_YMIRON_TO_SPIRIT); // should be on spirit
                if (Creature* pTemp = me->SummonCreature(ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].npc, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnX, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnY, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnZ, ActiveBot[m_uiActiveOrder[m_uiActivedNumber]].SpawnO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    m_uiActivedCreatureGUID = pTemp->GetGUID();
                    pTemp->CastSpell(me, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pTemp->SetSplineFlags(SPLINEFLAG_NO_SPLINE);
                    switch(m_uiActiveOrder[m_uiActivedNumber])
                    {
                        case 0: m_bIsActiveWithBJORN  = true; break;
                        case 1: m_bIsActiveWithHALDOR = true; break;
                        case 2: m_bIsActiveWithRANULF = true; break;
                        case 3: m_bIsActiveWithTORGYN = true; break;
                    }
                }

                m_bIsPause = true;
                m_bIsWalking = false;
                m_uiPause_Timer = 3000;
            } else m_uiPause_Timer -= diff;
            return;
        }
        else if (m_bIsPause)
        {
            if (m_uiPause_Timer < diff)
            {
                m_uiAbility_BJORN_Timer = 5000;
                m_uiAbility_HALDOR_Timer = 5000;
                m_uiAbility_RANULF_Timer = 5000;
                m_uiAbility_TORGYN_Timer = 5000;

                m_bIsPause = false;
                m_uiPause_Timer = 0;
            } 
			else
				m_uiPause_Timer -= diff;
            return;
        }

        //Return since we have no target
		if (!CanDoSomething())
            return;

        if (!m_bIsPause)
        {
			Tasks.UpdateEvent(diff);

             if (m_uiDarkSlash_Timer < diff)
            {
                //DoCastVictim( SPELL_DARK_SLASH); // not working
                int32 dmg = me->getVictim()->GetHealth() / 50; // workaround for damage
                me->CastCustomSpell(me->getVictim(), SPELL_DARK_SLASH, &dmg, 0, 0, false);
                m_uiDarkSlash_Timer = urand(30000,35000);
            } 
			else
				m_uiDarkSlash_Timer -= diff;

            // Abilities ------------------------------------------------------------------------------
            if (m_bIsActiveWithBJORN && m_uiAbility_BJORN_Timer < diff)
            {
                //DoCastMe( SPELL_SUMMON_SPIRIT_FOUNT); // works fine, but using summon has better control
                if (Creature* pTemp = me->SummonCreature(NPC_SPIRIT_FOUNT, 385+rand()%10, -330+rand()%10, 104.756, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000))
                {
                    pTemp->SetSpeedRate(MOVE_RUN, 0.4f);
                    pTemp->CastSpell(pTemp, m_difficulty ? SPELL_SPIRIT_FOUNT_H : SPELL_SPIRIT_FOUNT_N, true);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pTemp->SetDisplayId(11686);
                    //pTemp->GetMotionMaster()->MoveChase(me->getVictim());
                    m_uiOrbGUID = pTemp->GetGUID();
                }
                m_bIsActiveWithBJORN = false; // only one orb
            } 
			else 
				m_uiAbility_BJORN_Timer -= diff;

            if (m_bIsActiveWithHALDOR && m_uiAbility_HALDOR_Timer < diff)
            {
                DoCastVictim( m_difficulty ? SPELL_SPIRIT_STRIKE_H : SPELL_SPIRIT_STRIKE_N);
                m_uiAbility_HALDOR_Timer = 5000; // overtime
            } 
			else 
				m_uiAbility_HALDOR_Timer -= diff;

            if (m_bIsActiveWithRANULF && m_uiAbility_RANULF_Timer < diff)
            {
                DoCastMe( m_difficulty ? SPELL_SPIRIT_BURST_H : SPELL_SPIRIT_BURST_N);
                m_uiAbility_RANULF_Timer = 10000; // overtime
            } 
			else 
				m_uiAbility_RANULF_Timer -= diff;

            if (m_bIsActiveWithTORGYN && m_uiAbility_TORGYN_Timer < diff)
            {
                float x,y,z;
                x = me->GetPositionX()-5;
                y = me->GetPositionY()-5;
                z = me->GetPositionZ();
                for(uint8 i = 0; i < 4; ++i)
                {
                    //DoCastMe( SPELL_SUMMON_AVENGING_SPIRIT); // works fine, but using summon has better control
                    if (Creature* pTemp = me->SummonCreature(NPC_AVENGING_SPIRIT, x+rand()%10, y+rand()%10, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                    {
                        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                       {
                            pTemp->AddThreat(pTarget, 0.0f);
                            pTemp->AI()->AttackStart(pTarget);
                        }
                    }
                }
                m_uiAbility_TORGYN_Timer = 15000; // overtime
            } 
			else
				m_uiAbility_TORGYN_Timer -= diff;

            // Health check -----------------------------------------------------------------------------
           if ((me->GetHealth()*100 / me->GetMaxHealth()) < (100-(m_uiHealthAmountMultipler * m_uiHealthAmountModifier)))
           {
            	uint8 m_uiOrder = m_uiHealthAmountModifier - 1;
            	++m_uiHealthAmountModifier;

                me->InterruptNonMeleeSpells(true);
                DoCastMe( SPELL_SCREAMS_OF_THE_DEAD);
                me->GetMotionMaster()->Clear();
                me->StopMoving();
                me->AttackStop();
                //me->GetMotionMaster()->MovePoint(0, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveX, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveY, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveZ);
                me->GetMap()->CreatureRelocation(me, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveX, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveY, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveZ, me->GetOrientation());
				me->SendMonsterMove(ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveX, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveY, ActiveBot[m_uiActiveOrder[m_uiOrder]].MoveZ, SPLINETYPE_NORMAL, me->GetSplineFlags(), 1);

                DespawnBoatGhosts(m_uiActivedCreatureGUID);
                DespawnBoatGhosts(m_uiOrbGUID);

                m_bIsActiveWithBJORN  = false;
                m_bIsActiveWithHALDOR = false;
                m_bIsActiveWithRANULF = false;
                m_bIsActiveWithTORGYN = false;

                m_uiBane_Timer                += 8000;
                m_uiFetidRot_Timer            += 8000;
                m_uiDarkSlash_Timer           += 8000;
                m_uiAncestors_Vengeance_Timer += 8000;

                m_uiActivedNumber = m_uiOrder;
                m_bIsWalking = true;
                //m_bIsPause = true;
                m_uiPause_Timer = 2000;
                return;
            }

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_ymiron(Creature* pCreature)
{
    return new boss_ymironAI(pCreature);
}

void AddSC_boss_ymiron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ymiron";
    newscript->GetAI = &GetAI_boss_ymiron;
    newscript->RegisterSelf();
}
