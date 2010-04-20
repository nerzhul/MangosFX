/* ScriptDev Frost Sapphire Studios */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO           = -1575015,
    SAY_SLAY_1          = -1575016,
    SAY_SLAY_2          = -1575017,
    SAY_DEATH           = -1575018,

	SPELL_ARCING_SMASH                       = 48260,
    SPELL_IMPALE_N                           = 48261,
    SPELL_IMPALE_H                           = 59268,
    SPELL_WITHERING_ROAR_N                   = 48256,
    SPELL_WITHERING_ROAR_H                   = 59267,

    SPELL_FREEZE_ANIM                        = 16245,

    // Massive Jormungar
    SPELL_ACID_SPIT                          = 48132,
    SPELL_ACID_SPLATTER_N                    = 48136,
    SPELL_ACID_SPLATTER_H                    = 59272,
    SPELL_POISON_BREATH_N                    = 48133,
    SPELL_POISON_BREATH_H                    = 59271,
    NPC_JORMUNGAR_WORM                       = 27228,

    // Ferocious Rhino
    SPELL_GORE_N                             = 48130,
    SPELL_GORE_H                             = 59264,
    SPELL_GRIEVOUS_WOUND_N                   = 48105,
    SPELL_GRIEVOUS_WOUND_H                   = 59263,
    SPELL_STOMP                              = 48131,

    // Ravenous Furbolg
    SPELL_CHAIN_LIGHTING_N                   = 48140,
    SPELL_CHAIN_LIGHTING_H                   = 59273,
    SPELL_CRAZED                             = 48139,
    SPELL_TERRIFYING_ROAD                    = 48144,

    // Frenzied Worgen
    SPELL_MORTAL_WOUND_N                     = 48137,
    SPELL_MORTAL_WOUND_H                     = 59265,
    SPELL_ENRAGE_1                           = 48138,
    SPELL_ENRAGE_2                           = 48142
};

/*######
## boss_gortok
######*/

struct MANGOS_DLL_DECL boss_gortokAI : public ScriptedAI
{
    uint32 m_uiAnimalCounter;
	uint32 m_uiAnimalCheck_Timer;
    boss_gortokAI(Creature* pCreature) : ScriptedAI(pCreature)
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
		Tasks.AddEvent(SPELL_ARCING_SMASH,15000,13000,4000);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_WITHERING_ROAR_H,10000,8000,4000,TARGET_ME);
			Tasks.AddEvent(SPELL_IMPALE_H,12000,8000,4000);
		}
		else
		{
			Tasks.AddEvent(SPELL_WITHERING_ROAR_N,10000,8000,4000,TARGET_ME);
			Tasks.AddEvent(SPELL_IMPALE_N,12000,8000,4000);
		}
        m_uiAnimalCounter = 0;
		m_uiAnimalCheck_Timer = 500;
        DoCastMe(SPELL_FREEZE_ANIM);

        if (m_pInstance)
        {
            if(me->isAlive())
            {
        	    m_pInstance->SetData(TYPE_GORTOK, NOT_STARTED);
        	    Tasks.FreezeMob(true,me);
        	}

            Unit* pTemp = NULL;
            if (pTemp = Unit::GetUnit((*me),m_pInstance->GetData64(DATA_MOB_FRENZIED_WORGEN)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    Tasks.FreezeMob(true,(Creature*)pTemp);
            }

            if (pTemp = Unit::GetUnit((*me),m_pInstance->GetData64(DATA_MOB_RAVENOUS_FURBOLG)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    Tasks.FreezeMob(true,(Creature*)pTemp);
            }

            if (pTemp = Unit::GetUnit((*me),m_pInstance->GetData64(DATA_MOB_MASSIVE_JORMUNGAR)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    Tasks.FreezeMob(true,(Creature*)pTemp);
            }

            if (pTemp = Unit::GetUnit((*me),m_pInstance->GetData64(DATA_MOB_FEROCIOUS_RHINO)))
            {
                if (pTemp->isDead())
                    ((Creature*)pTemp)->Respawn();
        	    Tasks.FreezeMob(true,(Creature*)pTemp);
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

	void AttackStart(Unit* pWho)
    {
        if (m_uiAnimalCounter < 5)
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

    void MoveInLineOfSight(Unit* pWho)
    {
    	if (!pWho)
    	    return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(me) && me->IsHostileTo(pWho) &&
        	!m_uiAnimalCounter && pWho->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(pWho, 25))
        {
            if(m_pInstance)
                m_pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);

        	++m_uiAnimalCounter;
        }
	}

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, me);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		if(m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, DONE);

		GiveEmblemsToGroup(m_bIsHeroic ? HEROISME : 0,1,true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiAnimalCounter)
        {
            if(m_uiAnimalCheck_Timer < uiDiff)
            {
            	Creature* pTemp = NULL;
				
				switch(m_uiAnimalCounter)
				{
					case 1:
                		pTemp = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_MOB_FRENZIED_WORGEN));
						break;
					case 2:
                       	pTemp = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_MOB_RAVENOUS_FURBOLG));
						break;
					case 3:
                		pTemp = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_MOB_MASSIVE_JORMUNGAR));
						break;
					case 4:
						pTemp = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_MOB_FEROCIOUS_RHINO));
						break;
				}

                if (pTemp)
                {
                    if (!pTemp->isAlive())
                        ++m_uiAnimalCounter;
					else
					{
						if(!pTemp->getVictim())
						{
							Tasks.FreezeMob(false,pTemp);
							((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
							pTemp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM,0));
						}
					}
                }

                if (m_uiAnimalCounter == 5)
                {
					Tasks.FreezeMob(false,me);
                    ((Unit*)me)->SetStandState(UNIT_STAND_STATE_STAND);
                    m_uiAnimalCheck_Timer = 10000000;
                    AttackStart(SelectUnit(SELECT_TARGET_RANDOM,0));
                }
                else
                {
                    m_uiAnimalCheck_Timer = 1000;
                    return;
                }
            }
			else
				m_uiAnimalCheck_Timer -= uiDiff;
        }

		if (!Tasks.CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Massive Jormungar
######*/
struct MANGOS_DLL_DECL mob_massive_jormungarAI : public ScriptedAI
{
    mob_massive_jormungarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;

    uint32 AcidSplatter_timer;
	MobEventTasks Tasks;

    void Reset() {
		Tasks.SetObjects(this,me);
		Tasks.CleanMyAdds();
		Tasks.AddEvent(SPELL_ACID_SPIT,3000,2000,2000);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_POISON_BREATH_H : SPELL_POISON_BREATH_N,10000,10000,4000);
    	AcidSplatter_timer = 12000;
        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_gortokAI*)pPalehoof->AI())->EnterEvadeMode();

		Tasks.FreezeMob(true,me);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == me)
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
		if (!Tasks.CanDoSomething())
            return;

        // Acid Splatter
        if(AcidSplatter_timer <= uiDiff)
        {
            DoCastMe( m_bIsHeroic ? SPELL_ACID_SPLATTER_H : SPELL_ACID_SPLATTER_N);

            for(uint8 i = 0; i<6; ++i)
				Tasks.CallCreature(NPC_JORMUNGAR_WORM);

            AcidSplatter_timer = urand(10000,14000);
        }
		else
			AcidSplatter_timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Ferocious Rhino
######*/
struct MANGOS_DLL_DECL mob_ferocious_rhinoAI : public ScriptedAI
{
    mob_ferocious_rhinoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset() 
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_STOMP,10000,8000,4000,TARGET_MAIN);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_GORE_H,15000,13000,4000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_GRIEVOUS_WOUND_H,20000,18000,4000);
		}
		else
		{
			Tasks.AddEvent(SPELL_GORE_N,15000,13000,4000,TARGET_MAIN);
			Tasks.AddEvent(SPELL_GRIEVOUS_WOUND_N,20000,18000,4000);
		}

        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_gortokAI*)pPalehoof->AI())->EnterEvadeMode();
		Tasks.FreezeMob(true,me);

    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == me)
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
		if (!Tasks.CanDoSomething())
            return;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Ravenous Furbolg
######*/
struct MANGOS_DLL_DECL mob_ravenous_furbolgAI : public ScriptedAI
{
    mob_ravenous_furbolgAI(Creature* pCreature) : ScriptedAI(pCreature) {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
    	Reset();
    }
	float percent;
    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset() 
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_CHAIN_LIGHTING_H : SPELL_CHAIN_LIGHTING_N,10000,9000,6000);
		Tasks.AddEvent(SPELL_TERRIFYING_ROAD,9000,10000,10000,TARGET_MAIN);

        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_gortokAI*)pPalehoof->AI())->EnterEvadeMode();
		Tasks.FreezeMob(true,me);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == me)
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!Tasks.CanDoSomething())
            return;

    	// Crazed
        if(percent < 20 && !me->HasAura(SPELL_CRAZED))
            DoCastMe( SPELL_CRAZED);
  
		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Frenzied Worgen
######*/
struct MANGOS_DLL_DECL mob_frenzied_worgenAI : public ScriptedAI
{
    mob_frenzied_worgenAI(Creature* pCreature) : ScriptedAI(pCreature) {
    	m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
    	Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;

    void Reset() 
	{
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(m_bIsHeroic ? SPELL_MORTAL_WOUND_H : SPELL_MORTAL_WOUND_N,5000,4000,5000,TARGET_MAIN);
		Tasks.AddEvent(SPELL_ENRAGE_2,10000,10000,0,TARGET_ME);

        if (m_pInstance)
            if (Creature* pPalehoof = (Creature*)Unit::GetUnit(*me, m_pInstance->GetData64(DATA_GORTOK_PALEHOOF)))
                ((boss_gortokAI*)pPalehoof->AI())->EnterEvadeMode();

		Tasks.FreezeMob(true,me);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == me)
            return;

        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        	return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!Tasks.CanDoSomething())
            return;

    	// Enrage1
		if(Tasks.CheckPercentLife(20) && !me->HasAura(SPELL_ENRAGE_1,0))
            DoCastMe( SPELL_ENRAGE_1);

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_boss_gortok(Creature* pCreature)
{
    return new boss_gortokAI(pCreature);
}

CreatureAI* GetAI_mob_massive_jormungar(Creature* pCreature)
{
    return new mob_massive_jormungarAI (pCreature);
}

CreatureAI* GetAI_mob_ferocious_rhino(Creature* pCreature)
{
    return new mob_ferocious_rhinoAI (pCreature);
}

CreatureAI* GetAI_mob_ravenous_furbolg(Creature* pCreature)
{
    return new mob_ravenous_furbolgAI (pCreature);
}

CreatureAI* GetAI_mob_frenzied_worgen(Creature* pCreature)
{
    return new mob_frenzied_worgenAI (pCreature);
}

void AddSC_boss_gortok()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_gortok";
    newscript->GetAI = &GetAI_boss_gortok;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_massive_jormungar";
    newscript->GetAI = &GetAI_mob_massive_jormungar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ferocious_rhino";
    newscript->GetAI = &GetAI_mob_ferocious_rhino;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ravenous_furbolg";
    newscript->GetAI = &GetAI_mob_ravenous_furbolg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_frenzied_worgen";
    newscript->GetAI = &GetAI_mob_frenzied_worgen;
    newscript->RegisterSelf();
}
