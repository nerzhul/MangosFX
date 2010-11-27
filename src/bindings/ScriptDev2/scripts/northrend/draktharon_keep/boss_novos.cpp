/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "drak_tharon_keep.h"

enum Spells
{
    SPELL_ARCANE_BLAST                        = 49198,
    H_SPELL_ARCANE_BLAST                      = 59909,
    SPELL_ARCANE_FIELD                        = 47346,
    SPELL_BLIZZARD                            = 49034,
    H_SPELL_BLIZZARD                          = 59854,
    SPELL_FROSTBOLT                           = 49037,
    H_SPELL_FROSTBOLT                         = 59855,
    SPELL_WRATH_OF_MISERY                     = 50089,
    H_SPELL_WRATH_OF_MISERY                   = 59856,
    SPELL_SUMMON_MINIONS                      = 59910 //Summons an army of Fetid Troll Corpses to assist the caster.
};
//not in db
enum Yells
{
    SAY_AGGRO                              = -1600000,
    SAY_KILL                               = -1600001,
    SAY_DEATH                              = -1600002,
    SAY_NECRO_ADD                          = -1600003,
    SAY_REUBBLE_1                          = -1600004,
    SAY_REUBBLE_2                          = -1600005,

	EMOTE_ASSISTANCE			           = -1600011
};
enum Creatures
{
    CREATURE_RISEN_SHADOWCASTER            = 27600,
    CREATURE_FETID_TROLL_CORPSE            = 27598,
    CREATURE_HULKING_CORPSE                = 27597,
    CREATURE_CRYSTAL_HANDLER               = 26627
};
enum CombatPhase
{
    IDLE,
    PHASE_1,
    PHASE_2
};
enum Achievements
{
    ACHIEV_OH_NOVOS                   = 2057
};

struct Location
{
    float x,y,z;
};

static Location AddSpawnPoint = { -379.20, -816.76, 59.70};
static Location CrystalHandlerSpawnPoint = { -326.626343, -709.956604, 27.813314 };
static Location AddDestinyPoint = { -379.314545, -772.577637, 28.58837 };

struct MANGOS_DLL_DECL boss_novosAI : public Scripted_NoMovementAI
{
    boss_novosAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
        Reset();
    }

    uint32 uiTimer;
    uint32 uiCrystalHandlerTimer;
	uint8 crystals;
	Creature *pCrystalHandler;

    bool bAchiev;
	bool m_bIsHeroic;
	Creature *pSummon;

    //std::list<uint64> luiCrystals;
	std::vector<Creature*> NovosAdds;

    CombatPhase Phase;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();

		NovosAdds.clear();
		crystals = 4;
        Phase = IDLE;
        //luiCrystals.clear();
        bAchiev = true;
        if (pInstance)
        {
            pInstance->SetData(DATA_NOVOS_EVENT, NOT_STARTED);
			/*if(pInstance->GetData64(DATA_NOVOS_CRYSTAL_1))
				luiCrystals.push_back(pInstance->GetData64(DATA_NOVOS_CRYSTAL_1));
			if(pInstance->GetData64(DATA_NOVOS_CRYSTAL_2))
			 luiCrystals.push_back(pInstance->GetData64(DATA_NOVOS_CRYSTAL_2));
			if(pInstance->GetData64(DATA_NOVOS_CRYSTAL_3))
				luiCrystals.push_back(pInstance->GetData64(DATA_NOVOS_CRYSTAL_3));
			if(pInstance->GetData64(DATA_NOVOS_CRYSTAL_4))
				luiCrystals.push_back(pInstance->GetData64(DATA_NOVOS_CRYSTAL_4));
            for (std::list<uint64>::iterator itr = luiCrystals.begin(); itr != luiCrystals.end(); ++itr)
            {
                if (GameObject* pTemp = pInstance->instance->GetGameObject(*itr))
                    pTemp->SetGoState(GO_STATE_READY);
            }*/
        }
		FreezeMob(false,me,true);

		Phase = PHASE_1;
		uiTimer = 1000;
		pCrystalHandler = NULL;
		uiCrystalHandlerTimer = 30000;
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);
        
        DoCastMe(SPELL_ARCANE_FIELD);
        if (pInstance)
        {
            /*for (std::list<uint64>::iterator itr = luiCrystals.begin(); itr != luiCrystals.end(); ++itr)
            {
                if (GameObject *pTemp = pInstance->instance->GetGameObject(*itr))
                    pTemp->SetGoState(GO_STATE_ACTIVE);
            }*/
            pInstance->SetData(DATA_NOVOS_EVENT, IN_PROGRESS);
        }
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
		//Return since we have no target
		if (!CanDoSomething())
            return;

        if(Phase == PHASE_1)
		{
            if (uiTimer <= diff)
            {
				if(pCrystalHandler && !pCrystalHandler->isAlive())
				{
					crystals--;
					if(crystals < 1)
					{
						me->CastStop();
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
						Phase = PHASE_2;
						uiTimer = 1000;
					}
					pCrystalHandler = NULL;
				}
				
				switch(urand(0,2))
				{
					case 0:
						Tasks.CallCreature(CREATURE_HULKING_CORPSE,TEN_MINS,PREC_COORDS,GO_TO_CREATOR,AddSpawnPoint.x,AddSpawnPoint.y,AddSpawnPoint.z);
						break;
					case 1:
						Tasks.CallCreature(CREATURE_FETID_TROLL_CORPSE,TEN_MINS,PREC_COORDS,GO_TO_CREATOR,AddSpawnPoint.x,AddSpawnPoint.y,AddSpawnPoint.z);
						break;
					case 2:
						Tasks.CallCreature(CREATURE_RISEN_SHADOWCASTER,TEN_MINS,PREC_COORDS,GO_TO_CREATOR,AddSpawnPoint.x,AddSpawnPoint.y,AddSpawnPoint.z);
						break;
				}
                //If spell is casted stops casting arcane field so no spell casting
                uiTimer = 4500;
            } 
			else
				uiTimer -= diff;

            if (uiCrystalHandlerTimer <= diff)
            {

                if(pCrystalHandler = me->SummonCreature(CREATURE_CRYSTAL_HANDLER, CrystalHandlerSpawnPoint.x, CrystalHandlerSpawnPoint.y , CrystalHandlerSpawnPoint.z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN,20000))
				{
					pCrystalHandler->GetMotionMaster()->MovePoint(0, AddDestinyPoint.x, AddDestinyPoint.y, AddDestinyPoint.z);
					//NovosAdds.push_back(pCrystalHandler);
				}
                uiCrystalHandlerTimer = urand(20000,30000);
            } 
			else 
				uiCrystalHandlerTimer -= diff;
		}
		else
		{
            if (uiTimer <= diff)
            {
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
				{
					switch(urand(0,2))
					{
						case 0:
							DoCastRandom(m_bIsHeroic ? H_SPELL_ARCANE_BLAST : SPELL_ARCANE_BLAST);
							break;

						case 1:
							DoCastRandom(m_bIsHeroic ? H_SPELL_BLIZZARD : SPELL_BLIZZARD);
							break;

						case 2:
							DoCastRandom(m_bIsHeroic ? H_SPELL_WRATH_OF_MISERY : SPELL_WRATH_OF_MISERY);
							break;
					}
				}
                uiTimer = urand(1000,3000);
            } 
			else 
				uiTimer -= diff;

		}
    }
    void JustDied(Unit* killer)
    {
        if (pInstance)
        {
            pInstance->SetData(DATA_NOVOS_EVENT, DONE);

            /*if (m_bIsHeroic && bAchiev)
                pInstance->DoCompleteAchievement(ACHIEV_OH_NOVOS);*/
        }

		Tasks.CleanMyAdds();
		NovosAdds.clear();
		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void KilledUnit(Unit *victim)
    {
        if (victim == me)
            return;
        DoScriptText(SAY_KILL, me);
    }
};

struct MANGOS_DLL_DECL mob_crystal_handlerAI : public ScriptedAI
{
    mob_crystal_handlerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
		Reset();
    }

	MobEventTasks Tasks;

	void Reset(){
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(49668,3000,5000,1500,TARGET_MAIN);
	};

    void JustDied(Unit* killer)
    {
		/*if (Creature* pNovos = ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_NOVOS) : 0)))
            ((boss_novosAI*)pNovos->AI())->RemoveCrystal();*/
    }

	void UpdateAI(const uint32 diff)
	{
		if (!CanDoSomething())
            return;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();

	}
};

struct MANGOS_DLL_DECL mob_novos_minionAI : public ScriptedAI
{
    mob_novos_minionAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
		Reset();
    }

	void Reset() {}
    void MovementInform(uint32 type, uint32 id)
    {
        /*if(type != POINT_MOTION_TYPE)
            return;*/
        if (Creature* pNovos = ((Creature*)Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_NOVOS) : 0)))
            ((boss_novosAI*)pNovos)->bAchiev = false;
    }
};

CreatureAI* GetAI_boss_novos(Creature* pCreature)
{
    return new boss_novosAI(pCreature);
}

CreatureAI* GetAI_mob_crystal_handler(Creature* pCreature)
{
    return new mob_crystal_handlerAI(pCreature);
}

CreatureAI* GetAI_mob_novos_minion(Creature* pCreature)
{
    return new mob_novos_minionAI(pCreature);
}

void AddSC_boss_novos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_novos";
    newscript->GetAI = &GetAI_boss_novos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_crystal_handler";
    newscript->GetAI = &GetAI_mob_crystal_handler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_novos_minion";
    newscript->GetAI = &GetAI_mob_novos_minion;
    newscript->RegisterSelf();
}
