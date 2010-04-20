/* LibDevFS by FrostSapphire Studios
 */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_INTRO_1                 = -1575000,
    SAY_INTRO_2_ARTHAS          = -1575001,
    SAY_INTRO_3                 = -1575002,
    SAY_INTRO_4_ARTHAS          = -1575003,
    SAY_INTRO_5                 = -1575004,

    SAY_AGGRO                   = -1575005,
    SAY_SLAY_1                  = -1575006,
    SAY_SLAY_2                  = -1575007,
    SAY_SLAY_3                  = -1575008,
    SAY_SACRIFICE_1             = -1575009,
    SAY_SACRIFICE_2             = -1575010,
    SAY_SACRIFICE_3             = -1575011,
    SAY_SACRIFICE_4             = -1575012,
    SAY_SACRIFICE_5             = -1575013,
    SAY_DEATH                   = -1575014,

    NPC_SVALA_SORROW            = 26668,
    NPC_ARTHAS_IMAGE            = 29280,
	NPC_SVALA_ADD				= 27281,

    SPELL_ARTHAS_VISUAL         = 54134,

    // don't know how these should work in relation to each other
    SPELL_TRANSFORMING          = 54205,
    SPELL_TRANSFORMING_FLOATING = 54140,
    SPELL_TRANSFORMING_CHANNEL  = 54142,

    SPELL_RITUAL_OF_SWORD       = 48276,
    SPELL_CALL_FLAMES           = 48258,
	SPELL_FLAMES				= 44190,
    SPELL_SINISTER_STRIKE       = 15667,
    SPELL_SINISTER_STRIKE_H     = 59409,

	SPELL_ADD_1					= 48271,
	SPELL_ADD_2					= 48274,
	SPELL_ADD_3					= 48275,
	SPELL_PARALYSE				= 48278,
	SPELL_RITUAL_N				= 48277,
	SPELL_RITUAL_H				= 59930,
	SPELL_PREPARE_RITUAL		= 48267,

	SPELL_SHADOWS_IN_THE_DARK   = 59407,

    EQUIP_ARMAGEDDON            = 40343,
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations RitualChannelerLoc[]=
{
    {296.42, -355.01, 90.94},
    {302.36, -352.01, 90.54},
    {291.39, -350.89, 90.54}
};
/*######
## boss_svala
######*/

struct MANGOS_DLL_DECL boss_svalaAI : public ScriptedAI
{
	uint32 Strike_Timer;
	uint32 Ritual_Timer;
	Unit* target,*m_uiSacrificer;
	bool InRitual;
	bool RitualChannelerAlive;
	Creature* pRitualChanneler[3];
	uint32 m_uiSacrifice_Timer;
	uint32 m_uiRitualChanneler_Timer;

    boss_svalaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        m_bIsIntroDone = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    Creature* pArthas;

    bool m_bIsIntroDone;
    uint32 m_uiIntroTimer;
    uint32 m_uiIntroCount;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_CALL_FLAMES,20000,18000);
		if(m_bIsHeroic)
			Tasks.AddEvent(SPELL_SINISTER_STRIKE_H,2500,2000,3000,TARGET_MAIN);
		else
			Tasks.AddEvent(SPELL_SINISTER_STRIKE,2500,2000,3000,TARGET_MAIN);
        pArthas = NULL;
		m_uiSacrificer = NULL;
		target = NULL;

        m_uiIntroTimer = 2500;
        m_uiIntroCount = 0;
		for(uint8 i=0;i<3;i++)
			pRitualChanneler[i] = NULL;

        if (me->isAlive() && m_pInstance && m_pInstance->GetData(TYPE_SVALA) > IN_PROGRESS)
        {
            if (me->GetEntry() != NPC_SVALA_SORROW)
                me->UpdateEntry(NPC_SVALA_SORROW);

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            m_bIsIntroDone = true;
        }
		Strike_Timer = 2500;
		InRitual = false;
		RitualChannelerAlive = false;

        Ritual_Timer = 20000;
        m_uiRitualChanneler_Timer = 1000;
        m_uiSacrifice_Timer = 50000;
		
		SpellEntry* TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_PARALYSE);
		if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 0)
		{
			   TempSpell->EffectImplicitTargetA[0] = 77;
			   TempSpell->EffectImplicitTargetA[1] = 0;
			   TempSpell->EffectImplicitTargetB[0] = 0;
			   TempSpell->EffectImplicitTargetB[1] = 0;
		}
    }

    void JustReachedHome()
    {
        DoMoveToPosition();
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bIsIntroDone)
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_SVALA) == IN_PROGRESS)
            {
                m_pInstance->SetData(TYPE_SVALA, SPECIAL);

                float fX, fY, fZ;
                me->GetClosePoint(fX, fY, fZ, me->GetObjectSize(), 16.0f, 0.0f);

                // we assume me is spawned in proper location
                me->SummonCreature(NPC_ARTHAS_IMAGE, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
            }

            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        if (me->HasMonsterMoveFlag(MONSTER_MOVE_SPLINE))
            me->RemoveMonsterMoveFlag(MONSTER_MOVE_SPLINE);

        DoScriptText(SAY_AGGRO, me);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ARTHAS_IMAGE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_ARTHAS_VISUAL, true);
            pArthas = pSummoned;
            pSummoned->SetFacingToObject(me);
			pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        }
		else if (pSummoned->GetEntry() == NPC_SVALA_ADD)
		{
			pSummoned->CastSpell(target,SPELL_PARALYSE,false);
		}
    }

    void SummonedCreatureDespawn(Creature* pDespawned)
    {
        if (pDespawned->GetEntry() == NPC_ARTHAS_IMAGE)
            pArthas = NULL;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_TRANSFORMING)
        {
            if (pArthas)
                pArthas->InterruptNonMeleeSpells(true);

            me->UpdateEntry(NPC_SVALA_SORROW);
        }
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

    void DoMoveToPosition()
    {
        float fX, fZ, fY;
        me->GetRespawnCoord(fX, fY, fZ);

        me->AddMonsterMoveFlag(MONSTER_MOVE_SPLINE);

        me->SendMonsterMoveWithSpeed(fX, fY, fZ + 5.0f, m_uiIntroTimer);
        me->GetMap()->CreatureRelocation(me, fX, fY, fZ + 5.0f, me->GetOrientation());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
        {
			if(m_pInstance->GetData(TYPE_SVALA) == IN_PROGRESS)
				m_bIsIntroDone = false;

            if (m_bIsIntroDone)
                return;

            if (pArthas && pArthas->isAlive())
            {
                if (m_uiIntroTimer < uiDiff)
                {
                    m_uiIntroTimer = 10000;

                    switch(m_uiIntroCount)
                    {
                        case 0:
                            DoScriptText(SAY_INTRO_1, me);
                            break;
                        case 1:
                            DoScriptText(SAY_INTRO_2_ARTHAS, pArthas);
                            break;
                        case 2:
                            pArthas->CastSpell(me, SPELL_TRANSFORMING_CHANNEL, false);
                            me->CastSpell(me, SPELL_TRANSFORMING_FLOATING, false);
                            DoMoveToPosition();
                            break;
                        case 3:
                            me->CastSpell(me, SPELL_TRANSFORMING, false);
                            DoScriptText(SAY_INTRO_3, me);
                            break;
                        case 4:
                            DoScriptText(SAY_INTRO_4_ARTHAS, pArthas);
                            break;
                        case 5:
                            DoScriptText(SAY_INTRO_5, me);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
							m_pInstance->SetData(TYPE_SVALA, DONE);
                            m_bIsIntroDone = true;
                            break;
                    }

                    ++m_uiIntroCount;
                }
                else
                    m_uiIntroTimer -= uiDiff;
            }

            return;
        }

		if(!InRitual)
		{
			if(Ritual_Timer <= uiDiff)
			{
				InRitual = true;
				target = SelectUnit(SELECT_TARGET_RANDOM, 0);
	            	
				me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(0));
				me->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
				DoCast(target, SPELL_RITUAL_OF_SWORD);
				
				switch(urand(0,4))
				{
					case 0: DoScriptText(SAY_SACRIFICE_1, me);break;
					case 1: DoScriptText(SAY_SACRIFICE_2, me);break;
					case 2: DoScriptText(SAY_SACRIFICE_3, me);break;
					case 3: DoScriptText(SAY_SACRIFICE_4, me);break;
					case 4: DoScriptText(SAY_SACRIFICE_5, me);break;
				}

				// worksaround below
				DoTeleportPlayer(target, 296.632, -346.075, 90.63, 4.6);
				m_uiSacrificer = target;
				m_uiSacrifice_Timer = 25000;
				InRitual = true;

				for(uint8 i = 0; i<3; ++i)
				{
					pRitualChanneler[i] = me->SummonCreature(NPC_SVALA_ADD, RitualChannelerLoc[i].x, RitualChannelerLoc[i].y, RitualChannelerLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 360000);
					pRitualChanneler[i]->SetRespawnDelay(RESPAWN_ONE_DAY);
					pRitualChanneler[i]->CastSpell(target,SPELL_PARALYSE,false);
				}
				me->GetMap()->CreatureRelocation(me, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() +5.0f, me->GetOrientation());
				Ritual_Timer = urand(18000,22000);
			}
			else
				Ritual_Timer -= uiDiff;
		}
		else
		{
            // check if Ritual Channeler dies
            if(m_uiRitualChanneler_Timer <= uiDiff && InRitual)
            {
            	RitualChannelerAlive = false;
                for(uint8 i=0; i<3; ++i)
                {
                    if (pRitualChanneler[i])
                    	if (pRitualChanneler[i]->isAlive())
						{
                    	    RitualChannelerAlive = true;
							if (m_uiSacrificer && !m_uiSacrificer->HasAura(SPELL_PARALYSE))
								Tasks.SetAuraStack(SPELL_PARALYSE,1,target,target,1);
						}
                }

                if (!RitualChannelerAlive)
                {
					m_uiSacrificer->RemoveAurasDueToSpell(SPELL_PARALYSE);
                	me->InterruptNonMeleeSpells(false);
					me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_ARMAGEDDON));
                	InRitual = false;
                	m_uiSacrifice_Timer = 25000;
                	m_uiSacrificer = NULL;
                }

                m_uiRitualChanneler_Timer = 500;
            }
			else 
				m_uiRitualChanneler_Timer -= uiDiff;

            if(m_uiSacrifice_Timer <= uiDiff && InRitual)
            {
               	me->InterruptNonMeleeSpells(false);
               	me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_ARMAGEDDON));
                for(uint8 i=0; i<3; ++i)
                {
                    if (pRitualChanneler[i])
                    	if (pRitualChanneler[i]->isAlive())
                    	    pRitualChanneler[i]->ForcedDespawn();
                }
                if (m_uiSacrificer)
                    if (m_uiSacrificer->isAlive())
                        m_uiSacrificer->DealDamage(m_uiSacrificer, m_uiSacrificer->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                InRitual = false;
                m_uiSacrificer = NULL;
                m_uiSacrifice_Timer = 25000;
            }
			else
				m_uiSacrifice_Timer -= uiDiff;
		}

		if(!InRitual)
		{
			Tasks.UpdateEvent(uiDiff);

			DoMeleeAttackIfReady();
		}
    }
};

CreatureAI* GetAI_boss_svala(Creature* pCreature)
{
    return new boss_svalaAI(pCreature);
}

bool AreaTrigger_at_svala_intro(Player* pPlayer, AreaTriggerEntry* pAt)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SVALA) == NOT_STARTED)
            pInstance->SetData(TYPE_SVALA, IN_PROGRESS);
    }

    return false;
}

void AddSC_boss_svala()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_svala";
    newscript->GetAI = &GetAI_boss_svala;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_svala_intro";
    newscript->pAreaTrigger = &AreaTrigger_at_svala_intro;
    newscript->RegisterSelf();
}
