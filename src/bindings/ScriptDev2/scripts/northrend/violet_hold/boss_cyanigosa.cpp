/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"


enum
{
    SAY_AGGRO               = -2000015,
	SAY_SPAWN				= -2000014,
    SAY_DEATH               = -2000016,
    SPELL_BLIZZARD_NORMAL   = 58693,
    SPELL_BLIZZARD_HERO     = 59369,
    SPELL_QUEUE_NORMAL      = 58690,
    SPELL_QUEUE_HERO        = 59283,
    SPELL_MANA              = 59374,
    SPELL_SOUFFLE_NORMAL    = 58688,
    SPELL_SOUFFLE_HERO      = 59281,
    SPELL_TELE_BOSS         = 58694,
	SPELL_ENERGY_NORMAL		= 58688,
	SPELL_ENERGY_HERO		= 59281,
};
                                   
                                 
   
/*######
## boss_Cyanigosa
######*/

struct MANGOS_DLL_DECL boss_cyanigosaAI : public ScriptedAI
{	
    uint32 spell_blizzard_timer;                               
    uint32 spell_teleport_timer;  

	MobEventTasks Tasks;
                              
    void Reset()
	{
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(SPELL_SOUFFLE_HERO,8000,22000,0,TARGET_MAIN,0,-2000021);
			Tasks.AddEvent(SPELL_QUEUE_HERO,5000,7000,0,TARGET_ME);
		}
		else
		{
			Tasks.AddEvent(SPELL_SOUFFLE_NORMAL,8000,22000,0,TARGET_MAIN,0,-2000021);
			Tasks.AddEvent(SPELL_QUEUE_NORMAL,5000,7000,0,TARGET_ME);
		}
		spell_blizzard_timer = 12000;     // Blizzard toutes les 15 secondes                     
		spell_teleport_timer = 44000;     //Teleport à 44 sec
	}
    
	 
	 
	boss_cyanigosaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsHeroic;
	    
    void UpdateAI(const uint32 diff)
	{	

        if (!CanDoSomething() || me->HasAura(66830,0))
            return;

		if (spell_teleport_timer < diff)  
		{  
				DoScriptText(-2000020,me);
				if(m_bIsHeroic)
				{
					DoCastVictim(SPELL_TELE_BOSS);
					DoCastVictim(SPELL_ENERGY_HERO);
				}
				else
					DoCastVictim(SPELL_ENERGY_NORMAL);
				spell_teleport_timer = 30000;
		}
		else
				spell_teleport_timer -= diff;

		if (spell_blizzard_timer < diff)
		{   
				DoScriptText(urand(0,1) ? -2000022 : -2000023,me);
				DoCastRandom(m_bIsHeroic ? SPELL_BLIZZARD_HERO : SPELL_BLIZZARD_NORMAL);
				spell_blizzard_timer = 15000; //Boucle du Blizzard toutes les 15 sec
		}
		else
			spell_blizzard_timer -= diff;

		Tasks.UpdateEvent(diff);

		DoMeleeAttackIfReady();
	}             

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
		DoCastMe(58668);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
		GiveRandomReward();
    }

    void KilledUnit(Unit* pVictim)
    {
        uint8 randd = urand(0,2);
		switch(randd)
		{
			case 0:
				DoScriptText(-2000017,me);
				break;
			case 1:
				DoScriptText(-2000018,me);
				break;
			case 2:
				DoScriptText(-2000019,me);
				break;
		}
    }

};

CreatureAI* GetAI_boss_cyanigosa(Creature* pCreature)
{
    return new boss_cyanigosaAI(pCreature);
} 

void AddSC_boss_cyanigosa()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_cyanigosa";
    newscript->GetAI = &GetAI_boss_cyanigosa;
    newscript->RegisterSelf(); 
}
