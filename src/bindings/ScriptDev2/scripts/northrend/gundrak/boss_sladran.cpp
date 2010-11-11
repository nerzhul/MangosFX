/* ScriptData
SDName: Boss_Sladran
SD%Complete: 20%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"


enum
{
    SAY_AGGRO                       = -1604000,
    SAY_SUMMON_SNAKE                = -1604001,
    SAY_SUMMON_CONSTRICT            = -1604002,
    SAY_SLAY_1                      = -1604003,
    SAY_SLAY_2                      = -1604004,
    SAY_SLAY_3                      = -1604005,
    SAY_DEATH                       = -1604006,
    EMOTE_NOVA                      = -1604007,
    spell_nova_normal               = 55081,
    spell_morsure_normal            = 48287,
    spell_eclair_hero               = 59839,
    spell_eclair_normal             = 54970,
    spell_morsure_hero              = 59840,
    spell_nova_hero                 = 59842,
    id_serpent_hero                 = 30940,
	spell_add_normal				= 54987,
	spell_add_hero					= 58996,
	id_constrictor_normal			= 29713,
	id_serpent_normal				= 29680,
	id_constrictor_hero				= 30943,
	spell_poigne					= 55093,
	spell_morsure_normal_add		= 54987,
	spell_morsure_hero_add			= 58996,    
	
};  
bool aggro = false;
/*######
## boss_sladran
######*/

struct MANGOS_DLL_DECL boss_sladranAI : public ScriptedAI
{
    uint32 invoc_serpent_timer;
   
    boss_sladranAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool m_bIsHeroic;
	MobEventTasks Tasks;
	
    void Reset()
    {
		Tasks.SetObjects(this,me);
		if(m_bIsHeroic)
		{
			Tasks.AddEvent(spell_eclair_hero,10000,10000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_morsure_hero,5000,10000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_nova_hero,13500,20000,0,TARGET_MAIN);
		}
		else
		{
			Tasks.AddEvent(spell_eclair_normal,10000,10000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_morsure_normal,5000,10000,0,TARGET_MAIN);
			Tasks.AddEvent(spell_nova_normal,13500,20000,0,TARGET_MAIN);
		}

         invoc_serpent_timer	= 25000; 
    }

    void Aggro(Unit* pWho)
    {			
		DoScriptText(SAY_AGGRO, me);
	}

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0,2))
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

		     
	void invoc_add()
	{
		DoScriptText(SAY_SUMMON_SNAKE, me);
		if (m_bIsHeroic)
		{
			Tasks.CallCreature(id_serpent_hero,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM, 1775.325f , 629.444f, 128.0f);
			Tasks.CallCreature(id_constrictor_hero,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM, 1775.325f , 629.444f, 128.0f);
		}
		else
		{
			Tasks.CallCreature(id_serpent_normal,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM, 1775.325f , 629.444f, 128.0f);
			Tasks.CallCreature(id_constrictor_normal,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM, 1775.325f , 629.444f, 128.0f);
		}
	}

    		
	void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething())
            return;
                               
        if (invoc_serpent_timer < diff)
        {
			invoc_add();
            invoc_serpent_timer = 13000;
		}
		else
			invoc_serpent_timer -= diff;

		Tasks.UpdateEvent(diff);
        
        DoMeleeAttackIfReady();
	}
};

	

	

 
struct MANGOS_DLL_DECL mob_sladranAI : public ScriptedAI //script Serpent
{
	uint32 spell_morsure_timer;

	mob_sladranAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool m_bIsHeroic;

     
	void Reset()
	{     
		spell_morsure_timer = 7000;               
	}	

    
   
    void UpdateAI(const uint32 diff)
    {
       	if (spell_morsure_timer < diff)
        {					
			Unit* target;
			target = SelectUnit(SELECT_TARGET_TOPAGGRO,0);
            if ( m_bIsHeroic ==true )
            {	
				DoCast(target,spell_morsure_hero_add);
				spell_morsure_timer = 7000;
            }
            else
            {
               DoCast(target,spell_morsure_normal_add);
               spell_morsure_timer = 7000;
            }
		}
        else
            spell_morsure_timer -= diff;

		DoMeleeAttackIfReady();
		       
    }
};

struct MANGOS_DLL_DECL mob_sladran2AI : public ScriptedAI //script Constrictor
{
	uint32 spell_poigne_timer;

	mob_sladran2AI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool m_bIsHeroic;

     
	void Reset()
	{    
		spell_poigne_timer = 9000;               
	}	

    
   
    void UpdateAI(const uint32 diff)
    {
       	if (spell_poigne_timer < diff)
        {						
			Unit* target;
			target = SelectUnit(SELECT_TARGET_TOPAGGRO,0);
		    DoCast(target,spell_poigne);
            spell_poigne_timer =9000;
			DoCast(target,59842);
			spell_poigne_timer = 9000;
		}
        else
           spell_poigne_timer -= diff;

		DoMeleeAttackIfReady();
		       
    }
};

CreatureAI* GetAI_mob_sladran2(Creature* pCreature)
{
    return new mob_sladran2AI(pCreature);
}
	
CreatureAI* GetAI_mob_sladran(Creature* pCreature)
{
    return new mob_sladranAI(pCreature);
}
	
CreatureAI* GetAI_boss_sladran(Creature* pCreature)
{
    return new boss_sladranAI(pCreature);
}

void AddSC_boss_sladran()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_sladran";
    newscript->GetAI = &GetAI_boss_sladran;
    newscript->RegisterSelf();
}
void AddSC_mob_sladran()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "mob_sladran";
    newscript->GetAI = &GetAI_mob_sladran;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sladran2";
    newscript->GetAI = &GetAI_mob_sladran2;
    newscript->RegisterSelf();
}