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
		InitIA();
		AddEventMaxPrioOnMe(SPELL_METEOR_FISTS,25000,35000);
		AddEvent(SPELL_CINDER,15000,45000,0,TARGET_RANDOM,0,0,false,m_difficulty ? 5 : 3);
    }

    uint32 BurningBreathTimer;
    uint32 FlamesTimer;

    uint32 BBTickTimer;
    uint32 BBTicks;
    bool BB;

	void DamageDeal(Unit* pDoneTo, uint32& dmg)
	{
		uint32 fireDmg = dmg;
		dmg = 0;
		if(me->getVictim() && pDoneTo == me->getVictim() && dmg > 9000 && me->HasAura(SPELL_METEOR_FISTS))
		{
			if(Unit* offTank = SelectUnit(SELECT_TARGET_TOPAGGRO,1))
				if(offTank->isAlive())
					me->DealDamage(offTank,fireDmg, NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE, GetSpellStore()->LookupEntry(SPELL_METEOR_FISTS), false);	
			me->DealDamage(me->getVictim(),fireDmg, NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE, GetSpellStore()->LookupEntry(SPELL_METEOR_FISTS), false);
		}
	}

    void Reset()
    {
		me->RemoveAurasDueToSpell(SPELL_BURNING_FURY_AURA);
        BurningBreathTimer = 25000;
        FlamesTimer = 15000;

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
                DoCast(NULL, SPELL_BB_EFFECT, true);
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
