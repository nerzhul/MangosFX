/* LibDevFS by Frost Sapphire Studios
*/

#include "precompiled.h"
#include "drak_tharon_keep.h"

enum Spells
{
    //skeletal spells (phase 1)
    SPELL_CURSE_OF_LIFE                                    = 49527,
    H_SPELL_CURSE_OF_LIFE                                  = 59972,
    SPELL_RAIN_OF_FIRE                                     = 49518,
    H_SPELL_RAIN_OF_FIRE                                   = 59971,
    SPELL_SHADOW_VOLLEY                                    = 49528,
    H_SPELL_SHADOW_VOLLEY                                  = 59973,
    SPELL_DECAY_FLESH                                      = 49356, //casted at end of phase 1, starts phase 2
    //flesh spells (phase 2)
    SPELL_GIFT_OF_THARON_JA                                = 52509,
    SPELL_EYE_BEAM                                         = 49544,
    H_SPELL_EYE_BEAM                                       = 59965,
    SPELL_LIGHTNING_BREATH                                 = 49537,
    H_SPELL_LIGHTNING_BREATH                               = 59963,
    SPELL_POISON_CLOUD                                     = 49548,
    H_SPELL_POISON_CLOUD                                   = 59969,
    SPELL_RETURN_FLESH                                     = 53463 //Channeled spell ending phase two and returning to phase 1. This ability will stun the party for 6 seconds.
};

#include "precompiled.h"

enum
{
    SAY_AGGRO                       = -1600012,
    SAY_KILL_1                      = -1600013,
    SAY_KILL_2                      = -1600014,
    SAY_FLESH_1                     = -1600015,
    SAY_FLESH_2                     = -1600016,
    SAY_SKELETON_1                  = -1600017,
    SAY_SKELETON_2                  = -1600018,
    SAY_DEATH                       = -1600019
};
enum Models
{
    MODEL_FLESH                                            = 27073,
    MODEL_SKELETON                                         = 27511
};
enum CombatPhase
{
    SKELETAL,
    GOING_FLESH,
    FLESH,
    GOING_SKELETAL
};

struct MANGOS_DLL_DECL boss_tharon_jaAI : public ScriptedAI
{
    boss_tharon_jaAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
		Reset();
    }

    uint32 uiPhaseTimer;

	bool m_bIsHeroic;
    
    CombatPhase Phase;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(H_SPELL_CURSE_OF_LIFE,1000,10000,5000);
			Tasks.AddEvent(H_SPELL_SHADOW_VOLLEY,9000,8000,2000,TARGET_MAIN);
			Tasks.AddEvent(H_SPELL_RAIN_OF_FIRE,urand(14000,18000),14000,4000,TARGET_MAIN);
			Tasks.AddEvent(H_SPELL_LIGHTNING_BREATH,3500,6000,1000,TARGET_RANDOM,2);
			Tasks.AddEvent(H_SPELL_EYE_BEAM,urand(4000,8000),4000,2000,TARGET_RANDOM,2);
			Tasks.AddEvent(H_SPELL_POISON_CLOUD,6000,10000,2000,TARGET_MAIN,2);
		}
		else
		{
			Tasks.AddEvent(SPELL_CURSE_OF_LIFE,1000,10000,5000);
			Tasks.AddEvent(SPELL_SHADOW_VOLLEY,9000,8000,2000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_RAIN_OF_FIRE,urand(14000,18000),14000,4000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_LIGHTNING_BREATH,3500,6000,1000,TARGET_RANDOM,2);
			Tasks.AddEvent(SPELL_EYE_BEAM,urand(4000,8000),4000,2000,TARGET_RANDOM,2);
			Tasks.AddEvent(SPELL_POISON_CLOUD,6000,10000,2000,TARGET_MAIN,2);
		}
        uiPhaseTimer = 20000;
        Phase = SKELETAL;
        me->SetDisplayId(me->GetNativeDisplayId());
        if (pInstance)
            pInstance->SetData(DATA_THARON_JA_EVENT, NOT_STARTED);
    }
    
    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);

        if (pInstance)
            pInstance->SetData(DATA_THARON_JA_EVENT, IN_PROGRESS);
    }
    
    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        switch (Phase)
        {
            case SKELETAL:
				Tasks.UpdateEvent(diff,Phase);
                
                if (uiPhaseTimer < diff)
                {
                    DoCastVictim(SPELL_DECAY_FLESH);
                    Phase = GOING_FLESH;
                    uiPhaseTimer = 6000;
                } 
				else 
					uiPhaseTimer -= diff;

                DoMeleeAttackIfReady();
                break;
            case GOING_FLESH:
                if (uiPhaseTimer < diff)
                {
					if(urand(0,1) == 1)
						DoScriptText(SAY_FLESH_1,me);
					else
						DoScriptText(SAY_FLESH_2,me);
                    me->SetDisplayId(MODEL_FLESH);
                   ThreatList const& threatlist = me->getThreatManager().getThreatList();
                    for (ThreatList::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                    {
                        Unit *pTemp = Unit::GetUnit((*me),(*itr)->getUnitGuid());
                        if (pTemp && pTemp->GetTypeId() == TYPEID_PLAYER)
							ModifyAuraStack(SPELL_GIFT_OF_THARON_JA,1,pTemp);
                    }
                    uiPhaseTimer = 20000;
                    Phase = FLESH;
                } else uiPhaseTimer -= diff;
                break;
            case FLESH:
				Tasks.UpdateEvent(diff,Phase);
                
                if (uiPhaseTimer < diff)
                {
                    DoCastMe(SPELL_RETURN_FLESH);
                    Phase = GOING_SKELETAL;
                    uiPhaseTimer = 6000;
                } 
				else 
					uiPhaseTimer -= diff;
                DoMeleeAttackIfReady();
                break;
            case GOING_SKELETAL:
                if (uiPhaseTimer < diff)
                {
					DoScriptText(urand(0,1) ? SAY_SKELETON_2 : SAY_SKELETON_1, me);
                    me->DeMorph();
                    Phase = SKELETAL;
                    uiPhaseTimer = 20000;
                   ThreatList const& threatlist = me->getThreatManager().getThreatList();
                    for (ThreatList::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                    {
                        Unit *pTemp = Unit::GetUnit((*me),(*itr)->getUnitGuid());
                        if (pTemp && pTemp->GetTypeId() == TYPEID_PLAYER)
                        {
                            if (pTemp->HasAura(SPELL_GIFT_OF_THARON_JA))
								pTemp->RemoveAurasDueToSpell(SPELL_GIFT_OF_THARON_JA);
                            pTemp->DeMorph();
                        }
                    }
                } else uiPhaseTimer -= diff;
                break;
        }
    }

    void KilledUnit(Unit *victim)
    {
		DoScriptText(urand(0,1) ? SAY_KILL_1 : SAY_KILL_2,me);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH,me);

        if (pInstance)
            pInstance->SetData(DATA_THARON_JA_EVENT, DONE);
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
		GiveRandomReward();
    }
};

CreatureAI* GetAI_boss_tharon_ja(Creature* pCreature)
{
    return new boss_tharon_jaAI (pCreature);
}

void AddSC_boss_tharonja()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_tharonja";
    newscript->GetAI = &GetAI_boss_tharon_ja;
    newscript->RegisterSelf();
}
