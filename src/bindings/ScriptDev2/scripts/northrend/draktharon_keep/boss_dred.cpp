/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "drak_tharon_keep.h"

enum eSpells
{
    SPELL_BELLOWING_ROAR                                   = 22686, // fears the group, can be resisted/dispelled
    SPELL_GRIEVOUS_BITE                                    = 48920,
    SPELL_MANGLING_SLASH                                   = 48873, //casted on the current tank, adds debuf
    SPELL_FEARSOME_ROAR                                    = 48849,
    H_SPELL_FEARSOME_ROAR                                  = 59422, //Not stacking, debuff
    SPELL_PIERCING_SLASH                                   = 48878, //debuff -->Armor reduced by 75%
    SPELL_RAPTOR_CALL                                      = 59416, //dummy
    SPELL_GUT_RIP                                          = 49710,
    SPELL_REND                                             = 13738
};

enum eArchivements
{
    ACHIEV_BETTER_OFF_DRED                                 = 2039
};

enum Creatures
{
    NPC_RAPTOR_1                                           = 26641,
    NPC_RAPTOR_2                                           = 26628
};

struct MANGOS_DLL_DECL boss_dredAI : public ScriptedAI
{
    boss_dredAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
		Reset();
    }

    uint32 RaptorCallTimer;
	bool m_bIsHeroic;

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_BELLOWING_ROAR,33000,40000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_GRIEVOUS_BITE,20000,20000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_MANGLING_SLASH,18500,20000,0,TARGET_MAIN);
		Tasks.AddEvent(SPELL_PIERCING_SLASH,17000,20000,0,TARGET_MAIN);
		if(m_bIsHeroic)
			Tasks.AddEvent(H_SPELL_FEARSOME_ROAR,13000,16000,2000,TARGET_MAIN);
		else
			Tasks.AddEvent(SPELL_FEARSOME_ROAR,13000,16000,2000,TARGET_MAIN);

        if (pInstance)
        {
            pInstance->SetData(DATA_DRED_EVENT,NOT_STARTED);
            pInstance->SetData(DATA_KING_DRED_ACHIEV, 0);
        }

        RaptorCallTimer    = urand(20000,25000);
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
            pInstance->SetData(DATA_DRED_EVENT,IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
		if (!CanDoSomething())
            return;

        if (RaptorCallTimer < diff)
        {
            DoCastMe(SPELL_RAPTOR_CALL);

            float x,y,z;

            me->GetClosePoint(x,y,z,me->GetObjectSize()/3,10.0f);
			Tasks.CallCreature(urand(0,1) ? NPC_RAPTOR_1 : NPC_RAPTOR_2,TEN_MINS,NEAR_7M,AGGRESSIVE_RANDOM,x,y,z);
            RaptorCallTimer = urand(20000,25000);
        } 
		else 
			RaptorCallTimer -= diff;

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        if (pInstance)
        {
            pInstance->SetData(DATA_DRED_EVENT,DONE);

            /* todo : HF
			if (m_bIsHeroic && pInstance->GetData(DATA_KING_DRED_ACHIEV) == 6)
                pInstance->DoCompleteAchievement(ACHIEV_BETTER_OFF_DRED);
				*/
        }
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
};

CreatureAI* GetAI_boss_dred(Creature* pCreature)
{
    return new boss_dredAI (pCreature);
}

struct MANGOS_DLL_DECL npc_drakkari_gutripperAI : public ScriptedAI
{
    npc_drakkari_gutripperAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		Reset();
    }

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_GUT_RIP,urand(10000,15000),10000,5000);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;
		
		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        if (pInstance)
        {
            if (me->GetMap()->GetDifficulty() && pInstance->GetData(DATA_DRED_EVENT) == IN_PROGRESS && pInstance->GetData(DATA_KING_DRED_ACHIEV) < 6)
            {
                pInstance->SetData(DATA_KING_DRED_ACHIEV, pInstance->GetData(DATA_KING_DRED_ACHIEV) + 1);
            }
        }
    }
};

CreatureAI* GetAI_npc_drakkari_gutripper(Creature* pCreature)
{
    return new npc_drakkari_gutripperAI (pCreature);
}

struct MANGOS_DLL_DECL npc_drakkari_scytheclawAI : public ScriptedAI
{
    npc_drakkari_scytheclawAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		Reset();
    }

    ScriptedInstance* pInstance;
	MobEventTasks Tasks;

    void Reset()
    {
        Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_REND,5000,10000,5000);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!CanDoSomething())
            return;

        Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        if (pInstance)
        {
            if (me->GetMap()->GetDifficulty() && pInstance->GetData(DATA_DRED_EVENT) == IN_PROGRESS && pInstance->GetData(DATA_KING_DRED_ACHIEV) < 6)
            {
                pInstance->SetData(DATA_KING_DRED_ACHIEV, pInstance->GetData(DATA_KING_DRED_ACHIEV) + 1);
            }
        }
    }
};

CreatureAI* GetAI_npc_drakkari_scytheclaw(Creature* pCreature)
{
    return new npc_drakkari_scytheclawAI (pCreature);
}

void AddSC_boss_dred()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_drakkari_gutripper";
    newscript->GetAI = &GetAI_npc_drakkari_gutripper;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_drakkari_scytheclaw";
    newscript->GetAI = &GetAI_npc_drakkari_scytheclaw;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_dred";
    newscript->GetAI = &GetAI_boss_dred;
    newscript->RegisterSelf();
}
