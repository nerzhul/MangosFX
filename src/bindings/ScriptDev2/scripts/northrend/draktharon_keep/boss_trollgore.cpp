/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "drak_tharon_keep.h"

enum Spells
{
    SPELL_INFECTED_WOUND                                   = 49637,
    SPELL_CRUSH                                            = 49639,
    SPELL_CORPSE_EXPLODE                                   = 49555,
    SPELL_CONSUME                                          = 49380,
    //Heroic spells
    H_SPELL_CORPSE_EXPLODE                                 = 59807,
    H_SPELL_CONSUME                                        = 59803
};
//not in db

enum
{
    SAY_AGGRO                       = -1600000,
    SAY_CONSUME                     = -1600001,
    SAY_DEATH                       = -1600002,
    SAY_EXPLODE                     = -1600003,
    SAY_KILL                        = -1600004
};
enum Achievements
{
    ACHIEV_CONSUMPTION_JUNCTION                            = 2151
};
enum Creatures
{
    NPC_DRAKKARI_INVADER_1                                 = 27753,
    NPC_DRAKKARI_INVADER_2                                 = 27709
};

struct Locations
{
	float x,y,z,o;
};

Locations AddSpawnPoint = { -260.493011, -622.968018, 26.605301, 3.036870 };

struct MANGOS_DLL_DECL boss_trollgoreAI : public ScriptedAI
{
    boss_trollgoreAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
		Reset();
    }
    uint32 uiAuraCountTimer;
    uint32 uiSpawnTimer;

    bool bAchiev,m_bIsHeroic;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_CRUSH,2500,10000,5000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_INFECTED_WOUND,8000,25000,5000,TARGET_MAIN);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(H_SPELL_CONSUME,15000,15000,0,TARGET_MAIN);
			Tasks.AddEvent(H_SPELL_CORPSE_EXPLODE,3000,15000,4000,TARGET_ME,0,SAY_EXPLODE);
		}
		else
		{
			Tasks.AddEvent(SPELL_CONSUME,15000,15000,0,TARGET_MAIN);
			Tasks.AddEvent(SPELL_CORPSE_EXPLODE,3000,15000,4000,TARGET_ME,0,SAY_EXPLODE);
		}
        uiAuraCountTimer = 15500;
        uiSpawnTimer = urand(30000,40000);

        bAchiev = m_bIsHeroic;

		if(m_bIsHeroic == true)
		{	
			if (me->HasAura(H_SPELL_CONSUME))
				me->RemoveAurasDueToSpell(H_SPELL_CONSUME);
		}
		else
			if (me->HasAura(SPELL_CONSUME))
				me->RemoveAurasDueToSpell(SPELL_CONSUME);
        
        if (pInstance)
            pInstance->SetData(DATA_TROLLGORE_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);

        if (pInstance)
            pInstance->SetData(DATA_TROLLGORE_EVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
		if (!CanDoSomething())
            return;

        if (uiSpawnTimer < diff)
        {
			uint8 _rand;
			if(m_bIsHeroic == true)
				_rand = urand(2,5);
			else
				_rand = urand(2,3);

            for (uint8 i = 0; i < _rand; ++i)
				Tasks.CallCreature(urand(0,1) ? NPC_DRAKKARI_INVADER_1 : NPC_DRAKKARI_INVADER_2,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,AddSpawnPoint.x, AddSpawnPoint.y, AddSpawnPoint.z);
            uiSpawnTimer = urand(30000,40000);
        } 
		else 
			uiSpawnTimer -= diff;

        if (bAchiev)
        {
            if (uiAuraCountTimer < diff)
            {
                if (me->HasAura(SPELL_CONSUME) || me->HasAura(H_SPELL_CONSUME))
                {
                    Aura *pConsumeAura;
					if(m_bIsHeroic)
						pConsumeAura = me->GetAura(H_SPELL_CONSUME,0);
					else
						pConsumeAura = me->GetAura(SPELL_CONSUME,0);

                    if (pConsumeAura && pConsumeAura->GetStackAmount() > 9)
                        bAchiev = false;
                }
                uiAuraCountTimer = 16000;
            } 
			else 
				uiAuraCountTimer -= diff;
        }

		Tasks.UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
    
    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, me);

		Tasks.CleanMyAdds();

        if (pInstance)
        {
            /*if (bAchiev)
                pInstance->DoCompleteAchievement(ACHIEV_CONSUMPTION_JUNCTION);*/
            pInstance->SetData(DATA_TROLLGORE_EVENT, DONE);
        }
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }
    
    void KilledUnit(Unit *victim)
    {
        if (victim == me)
            return;
        DoScriptText(SAY_KILL, me);
    }
};

CreatureAI* GetAI_boss_trollgore(Creature* pCreature)
{
    return new boss_trollgoreAI (pCreature);
}

void AddSC_boss_trollgore()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_trollgore";
    newscript->GetAI = &GetAI_boss_trollgore;
    newscript->RegisterSelf();
}
