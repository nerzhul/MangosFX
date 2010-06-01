#include "precompiled.h"
#include "halls_of_reflection.h"

enum Spells
{
		SPELL_SHIELD			=	72194,
		SPELL_STRIKE			=	72198,
		SPELL_RAGE				=	72203,

		SPELL_DESTRUCT_CIRCLE	=	72320,
		SPELL_DARK_HEAL			=	72322,
		SPELL_WORD_PAIN			=	72318,
		SPELL_FEAR				=	72321,

		SPELL_BAD_STING			=	72222,
		SPELL_FROST_TRAP		=	72215,
		SPELL_SHOT				=	72208,
		SPELL_FROST_SHOT		=	72268,

		SPELL_SHADOW_STEP		=	72326,
		SPELL_POISON			=	72329,
		SPELL_DAGGER			=	72333,
		SPELL_STUN				=	72335,

		SPELL_FIREBALL			=	72163,
		SPELL_FLAME_SHOCK		=	72169,
		SPELL_FROSTBOLT			=	72166,
		SPELL_HALLUCINATION		=	72344,
		SPELL_ICECHAINS			=	72171,
};


struct MANGOS_DLL_DECL HoR_WarriorAI : public LibDevFSAI
{
    HoR_WarriorAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_SHIELD,5000,12000);
		AddEventOnTank(SPELL_STRIKE,3000,3000,1500);
		if(m_difficulty)
			AddEventOnMe(SPELL_RAGE,20000,25000);
    }

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
		{
			if(!me->HasAura(66830))
				DoCastMe(66830);
			EnterEvadeMode();
            return;
		}
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Warrior(Creature* pCreature)
{
    return new HoR_WarriorAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_RogueAI : public LibDevFSAI
{
    HoR_RogueAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_SHADOW_STEP,200,25000,5000);
		AddEventOnTank(SPELL_POISON,3000,3000,1000);
		AddEvent(SPELL_DAGGER,4000,10000,2000);
		AddEventOnTank(SPELL_STUN,6000,12000,1000);
    }

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
		{
			if(!me->HasAura(66830))
				DoCastMe(66830);
			EnterEvadeMode();
            return;
		}
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Rogue(Creature* pCreature)
{
    return new HoR_RogueAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_MageAI : public LibDevFSAI
{
    HoR_MageAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_FLAME_SHOCK,200,15000,2000);
		AddEventOnTank(SPELL_FIREBALL,3000,4000,1000);
		AddEventMaxPrioOnTank(SPELL_FROSTBOLT,6000,6000,1000);
		AddEvent(SPELL_ICECHAINS,10000,15000,5000);
    }

    void Reset()
    {
		ResetTimers();
    }

	void DamageTaken(Unit* who,uint32 &dmg)
	{
		if(dmg >= me->GetHealth() && who != me)
		{
			dmg = 0;
			DoCastMe(SPELL_HALLUCINATION);
			Kill(me);
		}
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
		{
			if(!me->HasAura(66830))
				DoCastMe(66830);
			EnterEvadeMode();
            return;
		}
	
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_HoR_Mage(Creature* pCreature)
{
    return new HoR_MageAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_HuntAI : public LibDevFSAI
{
    HoR_HuntAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnMe(SPELL_FROST_TRAP,5000,60000);
		AddEventOnTank(SPELL_BAD_STING,3000,20000);
		AddEventOnTank(SPELL_SHOT,500,2000,1000);
		AddEventMaxPrioOnTank(SPELL_FROST_SHOT,4000,12000,2000);
    }

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
		{
			if(!me->HasAura(66830))
				DoCastMe(66830);
			EnterEvadeMode();
            return;
		}
	
		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_HoR_Hunt(Creature* pCreature)
{
    return new HoR_HuntAI (pCreature);
}

struct MANGOS_DLL_DECL HoR_PriestAI : public LibDevFSAI
{
    HoR_PriestAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_DESTRUCT_CIRCLE,5000,9000,3000);
		AddEvent(SPELL_DARK_HEAL,10000,30000,0,HEAL_MY_FRIEND);
		AddEvent(SPELL_WORD_PAIN,3000,3000,1500);
		AddEvent(SPELL_FEAR,5000,15000,1000);
    }

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething() || me->HasAura(66830))
		{
			if(!me->HasAura(66830))
				DoCastMe(66830);
			EnterEvadeMode();
            return;
		}
	
		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_HoR_Priest(Creature* pCreature)
{
    return new HoR_PriestAI (pCreature);
}

bool GossipHello_hor_frostmourne_event(Player *player, Creature *mCreature)
{
     if (mCreature->isQuestGiver())
        player->PrepareQuestMenu( mCreature->GetGUID());

	player->ADD_GOSSIP_ITEM(0, "Allons chercher Deuillegivre !", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, mCreature->GetGUID());
    return true;
}

bool GossipSelect_hor_frostmourne_event(Player *player, Creature *mCreature, uint32 sender, uint32 action )
{
    if(action == GOSSIP_ACTION_INFO_DEF)
    {
		mCreature->GetInstanceData()->SetData(TYPE_EVENT_FROSTMOURNE,IN_PROGRESS);
        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

bool GossipHello_hor_lichking_event(Player *player, Creature *mCreature)
{
     if (mCreature->isQuestGiver())
        player->PrepareQuestMenu( mCreature->GetGUID());

	player->ADD_GOSSIP_ITEM(0, "Fuyons, tant qu'il est encore temps", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, mCreature->GetGUID());
    return true;
}

bool GossipSelect_hor_lichking_event(Player *player, Creature *mCreature, uint32 sender, uint32 action )
{
    if(action == GOSSIP_ACTION_INFO_DEF)
    {
		mCreature->GetInstanceData()->SetData(TYPE_EVENT_ESCAPE,IN_PROGRESS);
        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

struct MANGOS_DLL_DECL HoR_escape_fLeadAI : public LibDevFSAI
{
    HoR_escape_fLeadAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		if(me->GetEntry() == 36955)
			DoCastMe(SPELL_ICE_BARRER);
    }

    void Reset()
    {
		ResetTimers();
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

CreatureAI* GetAI_HoR_escape_fLead(Creature* pCreature)
{
    return new HoR_escape_fLeadAI (pCreature);
}

CreatureAI* GetAI_HoR_LichKing_Escape(Creature* pCreature)
{
    return new HoR_LichKing_EscapeAI (pCreature);
}

void AddSC_halls_of_reflection()
{
	Script *newscript;

	newscript = new Script;
    newscript->Name = "HoR_Warrior";
    newscript->GetAI = &GetAI_HoR_Warrior;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Priest";
    newscript->GetAI = &GetAI_HoR_Priest;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Mage";
    newscript->GetAI = &GetAI_HoR_Mage;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Rogue";
    newscript->GetAI = &GetAI_HoR_Rogue;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_Hunt";
    newscript->GetAI = &GetAI_HoR_Hunt;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "hor_frostmourne_event";
    newscript->pGossipHello = &GossipHello_hor_frostmourne_event;
    newscript->pGossipSelect = &GossipSelect_hor_frostmourne_event;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "hor_lichking_event";
	newscript->GetAI = &GetAI_HoR_escape_fLead;
    newscript->pGossipHello = &GossipHello_hor_lichking_event;
    newscript->pGossipSelect = &GossipSelect_hor_lichking_event;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "HoR_escape_lichking";
    newscript->GetAI = &GetAI_HoR_LichKing_Escape;
    newscript->RegisterSelf();
}