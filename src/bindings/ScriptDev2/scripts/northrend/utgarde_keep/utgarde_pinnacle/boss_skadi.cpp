/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO                   = -1575019,
    SAY_DRAKEBREATH_1           = -1575020,
    SAY_DRAKEBREATH_2           = -1575021,
    SAY_DRAKEBREATH_3           = -1575022,
    SAY_DRAKE_HARPOON_1         = -1575023,
    SAY_DRAKE_HARPOON_2         = -1575024,
    SAY_KILL_1                  = -1575025,
    SAY_KILL_2                  = -1575026,
    SAY_KILL_3                  = -1575027,
    SAY_DEATH                   = -1575028,
    SAY_DRAKE_DEATH             = -1575029,
    EMOTE_HARPOON_RANGE         = -1575030,

	SPELL_ECRASER_N				= 50234,
	SPELL_ECRASER_H				= 59330,
	SPELL_LANCE_N				= 50255,
	SPELL_LANCE_H				= 59331,
	SPELL_TOURBILLON_N			= 50288,
	SPELL_TOURBILLON_H			= 59332,

	NPC_YMIRJAR_WARRIOR         = 26690,
    NPC_YMIRJAR_WITCH_DOCTOR    = 26691,
    NPC_YMIRJAR_HARPOONER       = 26692
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations SpawnLoc[]=
{
    {340.556, -511.493, 104.352},
    {367.741, -512.865, 104.828},
    {399.546, -512.755, 104.834},
    {430.551, -514.320, 105.055},
    {468.931, -513.555, 104.723}
};

/*######
## boss_skadi
######*/

struct MANGOS_DLL_DECL boss_skadiAI : public ScriptedAI
{
	uint8 m_uiphase;
	uint32 m_uiWaypointId;
	uint8 m_uiSpawn_counter;
	uint32 m_uiMoveNext_Timer;
	uint32 m_uiPlayerCheck_Timer;

    boss_skadiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_TOURBILLON_H,11000,10000,3000,TARGET_MAIN,2);
			Tasks.AddEvent(SPELL_ECRASER_H,7000,7000,5000,TARGET_MAIN,2);
			Tasks.AddEvent(SPELL_LANCE_H,4000,4000,5000,TARGET_RANDOM,2);
		}
		else
		{
			Tasks.AddEvent(SPELL_TOURBILLON_N,11000,10000,3000,TARGET_MAIN,2);
			Tasks.AddEvent(SPELL_ECRASER_N,7000,7000,5000,TARGET_MAIN,2);
			Tasks.AddEvent(SPELL_LANCE_N,4000,4000,5000,TARGET_RANDOM,2);
		}
		m_uiphase = m_uiWaypointId = m_uiSpawn_counter = 0;
		me->Unmount();
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		m_uiMoveNext_Timer = 25000;
		m_uiPlayerCheck_Timer = 1000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, me); break;
            case 1: DoScriptText(SAY_KILL_2, me); break;
            case 2: DoScriptText(SAY_KILL_3, me); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		Tasks.GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

	void AttackStart(Unit* pWho)
    {
        if (m_uiphase < 2)
            return;

        if (!pWho || pWho == me)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
    	if (!pWho)
    	    return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(me) && me->IsHostileTo(pWho) &&
        	!m_uiphase && pWho->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(pWho, 20))
        {
            if(m_pInstance)
                m_pInstance->SetData(TYPE_SKADI, IN_PROGRESS);

            DoScriptText(SAY_AGGRO, me);

            me->SetInCombatWithZone();
            //me->Mount(27043);
            //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            /*if (me->HasMonsterMoveFlag(MONSTER_MOVE_WALK))
                me->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);*/
            //me->GetMotionMaster()->MovePoint(m_uiWaypointId, 340.259, -510.541, 120.869);
        	//m_uiphase = 1;
			m_uiphase = 2;
        }
	}

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
                return;

        if ((m_uiSpawn_counter >= 4 && !m_bIsHeroic) || (m_uiSpawn_counter >= 5 && m_bIsHeroic))
        {
            m_uiWaypointId = 200;
            m_uiMoveNext_Timer = 3000;
        }
        else
        {
            switch(uiPointId)
            {
                case 0:
                    SpawnMobs(m_uiSpawn_counter);
                    m_uiWaypointId = 1;
                    ++m_uiSpawn_counter;
                    m_uiMoveNext_Timer = 3000;
                    break;
                case 1:
                    SpawnMobs(m_uiSpawn_counter);
                    m_uiWaypointId = 0;
                    ++m_uiSpawn_counter;
                    m_uiMoveNext_Timer = 3000;
                    break;
           }
        }
    }

    void SpawnMobs(uint32 uiSpot)
    {
        uint8 maxSpawn = (m_bIsHeroic ? 6 : 4);
        for(uint8 i = 0; i < maxSpawn; ++i)
        {
            switch (rand()%3)
            {
                case 0: 
					Tasks.CallCreature(NPC_YMIRJAR_WARRIOR,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[uiSpot].x+rand()%5,SpawnLoc[uiSpot].y+rand()%5, SpawnLoc[uiSpot].z);
					break;
                case 1:
					Tasks.CallCreature(NPC_YMIRJAR_WITCH_DOCTOR,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[uiSpot].x+rand()%5,SpawnLoc[uiSpot].y+rand()%5, SpawnLoc[uiSpot].z);
					break;
                case 2: 
					Tasks.CallCreature(NPC_YMIRJAR_HARPOONER,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,SpawnLoc[uiSpot].x+rand()%5,SpawnLoc[uiSpot].y+rand()%5, SpawnLoc[uiSpot].z);
					break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiphase == 0)
            return;
        // Flying & adds
        /*else if (m_uiphase == 1)
        {
            if (m_uiPlayerCheck_Timer < uiDiff)
            {
                Map *map = me->GetMap();
                if (map->IsDungeon() && m_pInstance->GetData(TYPE_SKADI) == IN_PROGRESS)
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();
 
                    if (PlayerList.isEmpty())
                        return;

                    bool bIsAlive = false;
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
                        	 bIsAlive = true;

                    if (!bIsAlive)
                    {
                        me->GetMotionMaster()->Clear(false);
                        me->StopMoving();
                        EnterEvadeMode();
                        return;
                    }
                }
                m_uiPlayerCheck_Timer = 1000;
            } else m_uiPlayerCheck_Timer -= uiDiff;

            if (m_uiMoveNext_Timer < uiDiff)
            {
                switch(m_uiWaypointId)
                {
                    case 0: me->GetMotionMaster()->MovePoint(m_uiWaypointId, 340.259, -510.541, 120.869); break;
                    case 1: me->GetMotionMaster()->MovePoint(m_uiWaypointId, 472.977, -513.636, 120.869); break;
                    case 200:
                        me->GetMotionMaster()->Clear();
                        me->Unmount();
                        m_uiphase = 2;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                            AttackStart(pTarget);
                    break;
                }
                m_uiMoveNext_Timer = 25000; // prevent stuck
            } else m_uiMoveNext_Timer -= uiDiff;
        }*/
        // Land & attack
		else if (m_uiphase == 2)
		{
			Tasks.UpdateEvent(uiDiff,2);

			DoMeleeAttackIfReady();
		}
    }
};

CreatureAI* GetAI_boss_skadi(Creature* pCreature)
{
    return new boss_skadiAI(pCreature);
}

void AddSC_boss_skadi()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_skadi";
    newscript->GetAI = &GetAI_boss_skadi;
    newscript->RegisterSelf();
}
