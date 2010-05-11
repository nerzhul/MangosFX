#include "precompiled.h"
#include "vault_of_archavon.h"

enum Spells
{
	SPELL_BURNING_FURY_AURA		 = 66895,
	SPELL_CINDER				 = 66684,
	SPELL_METEOR_FISTS			 = 66725,

	SPELL_BURNING_BREATH		 = 66665,
	SPELL_BB_EFFECT				 = 66670,
	SPELL_METEOR_FISTS_EFF		 = 66765,
	
};

struct MANGOS_DLL_DECL boss_koralonAI : public LibDevFSAI
{
    boss_koralonAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
		AddEventMaxPrioOnMe(SPELL_METEOR_FISTS,25000,45000);
		AddEvent(SPELL_CINDER,5000,10000,2000);
		if(m_difficulty)
			AddEvent(SPELL_CINDER,7000,10000,2000);
    }

    uint32 BurningBreathTimer;
    uint32 BBTickTimer;
    uint32 BBTicks;
    bool BB;

    void Reset()
    {
		ResetTimers();
		me->RemoveAurasDueToSpell(SPELL_BURNING_FURY_AURA);
        BurningBreathTimer = 10000;

        BB = false;

        if(pInstance)
			pInstance->SetData(TYPE_KORALON, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoCast(me, SPELL_BURNING_FURY_AURA);

        if(pInstance) 
			pInstance->SetData(TYPE_KORALON, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_KORALON, DONE);
		GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : CONQUETE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

        if(BurningBreathTimer < diff)
        {
            DoCastMe(SPELL_BURNING_BREATH);
            BurningBreathTimer = 45000;

            BB = true;
            BBTickTimer = 1000;
            BBTicks = 0;
        }
        else BurningBreathTimer -= diff;

        if(BB)
        {
            if(BBTickTimer < diff)
            {
                DoCastVictim(SPELL_BB_EFFECT, true);
                BBTickTimer = 1000;
                ++BBTicks;
                if(BBTicks > 2) BB = false;
            }
            else BBTickTimer -= diff;
        }

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_koralonAI(Creature* pCreature)
{
    return new boss_koralonAI(pCreature);
}

void AddSC_boss_koralon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_koralon";
    newscript->GetAI = &GetAI_boss_koralonAI;
    newscript->RegisterSelf();
}
