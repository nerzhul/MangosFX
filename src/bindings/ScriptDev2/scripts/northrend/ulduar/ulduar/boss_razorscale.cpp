#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    SPELL_FLAMEBUFFET      = 64016,
    SPELL_FIREBALL         = 62796,
	SPELL_FIREBALL_H	   = 63815,

    SPELL_WINGBUFFET       = 62666,
    SPELL_FLAMEBREATH      = 63317,
	SPELL_FLAMEBREATH_H	   = 64021,
    SPELL_FUSEARMOR        = 64771,
    SPELL_DEVOURINGFLAME   = 63014,
	SPELL_ENRAGE		   = 26662,
};

enum Mobs
{
    NPC_DARK_RUNE_SENTINEL = 33846,
	NPC_DARK_RUNE_VEILLEUR = 33453,
	NPC_DARK_RUNE_ACOLYTE  = 33110
};

const static float FlyCoords[9][3] = {
	{639.327f,	-167.458f,	413.311f},
	{655.069f,	-222.422f,	414.595f},
	{652.142f,	-228.812f,	411.522f},
	{607.290f,	-239.414f,	415.905f},
	{639.327f,	-167.457f,	413.311f},
	{539.570f,	-224.814f,	416.250f},
	{502.194f,	-170.618f,	418.254f},
	{529.804f,	-129.450f,	415.120f},
	{580.558f,	-145.464f,	416.125f},
};
const static float InitCoords[3] = {585.255f,	-171.406f,	450.177f};

const static float SpawnLocs[6][2] = 
{
	{572.849f,	-185.682f},
	{588.538f,	-191.946f},
	{603.723f,	-182.480f},
	{610.101f,	-219.821f},
	{583.497f,	-219.978f},
	{557.380f,	-205.872f}
};

enum Phases
{
	PHASE_SLEEP		=	4,
	PHASE_FLY_I		=	0,
	PHASE_GROUND	=	1,
	PHASE_FLY		=	2,
	PHASE_GROUND_F	=	3,
};

struct MANGOS_DLL_DECL boss_razorscaleAI : public LibDevFSAI
{
    boss_razorscaleAI(Creature *pCreature) : LibDevFSAI(pCreature) 
	{
		InitInstance();
		if(m_difficulty)
			AddEvent(SPELL_FIREBALL_H,14000,14000);
		else
			AddEvent(SPELL_FIREBALL,14000,14000);
		AddEventOnTank(SPELL_FUSEARMOR,15000,10000,0,2);
		AddEventOnTank(SPELL_FUSEARMOR,15000,10000,0,3);
		AddEventOnTank(SPELL_WINGBUFFET,17000,7000,5000,2);
		AddEventOnTank(SPELL_WINGBUFFET,17000,7000,5000,3);
		AddEvent(SPELL_DEVOURINGFLAME,2000,9000,0,TARGET_RANDOM,1);
	}

    uint32 FlameBreathTimer;
    uint32 FlameBuffetTimer;
    uint32 SummonAddsTimer;
    //uint32 StunTimer;
    //uint32 CastSpellsTimer;
	uint32 Enrage_Timer;
	ScriptedInstance* m_pInstance;
	int8 Phase;
    bool IsFlying;
	bool m_difficulty;
	
	uint8 FlyPoint;
	uint32 MoveTimer;
	uint32 Changephase_Timer;
	uint32 CheckDist_Timer;

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
		
        Phase = PHASE_SLEEP;
        FlameBreathTimer = 20000;
        FlameBuffetTimer = 3000;
        SummonAddsTimer = 15000;
        //StunTimer = 30000;
        //CastSpellsTimer = 0;
		Enrage_Timer = 360000;

		MoveTimer = 1500;
		Changephase_Timer = 90000;
		CheckDist_Timer = 500;

        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        me->ApplySpellImmune(1, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);

		if (m_pInstance)
           m_pInstance->SetData(TYPE_RAZORSCALE, NOT_STARTED);
    }

    void JustDied(Unit* Killer)
    {
		GiveEmblemsToGroup((m_difficulty) ? CONQUETE : VAILLANCE);
		if (m_pInstance)
           m_pInstance->SetData(TYPE_RAZORSCALE, DONE);
    }
	
	void CallRazorscale()
	{
		Phase = PHASE_FLY_I;
		FlyPhase();
		if (m_pInstance)
           m_pInstance->SetData(TYPE_RAZORSCALE, IN_PROGRESS);
		AggroAllPlayers(350.0f);
	}

	void EnterCombat(Unit* who)
	{
		Phase = PHASE_FLY_I;
		FlyPhase();
	}

	void CheckPlayerDist()
	{
		if(me->getVictim() && me->getVictim()->GetDistance2d(me) > 5.0f)
			DoCastVictim((m_difficulty ? SPELL_FIREBALL_H : SPELL_FIREBALL));
	}

	void UpdateAI(const uint32 diff)
    {
		if (!CanDoSomething() && Phase == PHASE_SLEEP)
            return;

        if (me->getVictim() && !me->getVictim()->GetCharmerOrOwnerPlayerOrPlayerItself())
            Kill(me->getVictim());

		if(Phase == PHASE_FLY_I || Phase == PHASE_FLY)
		{
			if(MoveTimer <= diff)
			{
				UpdateFlyPhase();
				MoveTimer = 1000;
			}
			else
				MoveTimer -= diff;
			
			if(SummonAddsTimer <= diff)
			{
                SummonAdds();
				SummonAddsTimer = 37000;
			}
            else
				SummonAddsTimer -= diff;
		}

		if(Enrage_Timer <= diff)
		{
			if(Phase != PHASE_GROUND_F)
			{
				Enrage_Timer = 300000;
				DoCastMe(SPELL_ENRAGE);
			}
			else
				Enrage_Timer = DAY*1000;
		}
		else
			Enrage_Timer -= diff;

		if(Phase != PHASE_GROUND_F)
		{
			if(Changephase_Timer <= diff)
			{
				if(Phase == PHASE_FLY_I || Phase == PHASE_FLY)
				{
					Speak(CHAT_TYPE_BOSS_EMOTE,0,"Remuez vous ! Elle ne va pas rester au sol longtemps !");
					GroundPhase();
				}
				else if(Phase == PHASE_GROUND)
				{
					FlyPhase();
					Phase = PHASE_FLY;
				}
				Changephase_Timer = 90000;
			}
			else
				Changephase_Timer -= diff;
		}

        if (CheckPercentLife(50) && Phase != PHASE_GROUND_F)
        {
			GroundPhase();
			Phase = PHASE_GROUND_F;
			Speak(CHAT_TYPE_BOSS_EMOTE,0,"Tranchécaille reste au sol définitivement");
        }

        if (Phase >= PHASE_FLY && Phase < PHASE_SLEEP)
        {
            if (FlameBreathTimer <= diff)
            {
				Speak(CHAT_TYPE_BOSS_EMOTE,0,"Tranchécaille inspire profondemment");
				DoCastVictim((m_difficulty ? SPELL_FLAMEBREATH_H : SPELL_FLAMEBREATH));
                FlameBreathTimer = 15000;
            }
			else 
				FlameBreathTimer -= diff;

            if (Phase == PHASE_GROUND_F)
            {
                if (FlameBuffetTimer <= diff)
                {
                    std::list<Unit*> pTargets;
					//SelectTargetList(pTargets, (m_difficulty ? 9 : 3), SELECT_TARGET_RANDOM, 100, true);
                    uint8 i = 0;
                    /*for (std::list<Unit*>::iterator itr = pTargets.begin(); itr != pTargets.end();)
                    {
                        if (me->HasInArc(M_PI, *itr))
                        {
                            DoCast(*itr, SPELL_FLAMEBUFFET, true);
                            ++i;
                        }
                        if (++itr == pTargets.end() || i == (m_difficulty ? 9 : 3))
                        {
                            AttackStart(*--itr); // seems to attack targets randomly during perma-ground phase..
                            break;
                        }
                    }*/
                    FlameBuffetTimer = 25000;
                } 
				else
					FlameBuffetTimer -= diff;

            }
		}
		
		UpdateEvent(diff,Phase);
		UpdateEvent(diff);
		if(Phase == PHASE_GROUND || Phase == PHASE_GROUND_F)
		{
			if(CheckDist_Timer <= diff)
			{
				CheckPlayerDist();
				CheckDist_Timer = 750;
			}
			else
				CheckDist_Timer -= diff;
			DoMeleeAttackIfReady();
		}
    }

    void SummonAdds()
    {
		uint8 random = urand((m_difficulty ? 4 : 2),(m_difficulty ? 7 : 4));
		
        for (uint8 i = 0; i < random; ++i)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
				uint8 randpos = urand(0,5);
                float z = me->GetBaseMap()->GetHeight(SpawnLocs[randpos][0], SpawnLocs[randpos][1], MAX_HEIGHT);                         // Ground level
				uint32 idAdd = 0;
				switch(urand(0,2))
				{
					case 0:
						idAdd = NPC_DARK_RUNE_SENTINEL;
						break;
					case 1:
						idAdd = NPC_DARK_RUNE_VEILLEUR;
						break;
					case 2:
						idAdd = NPC_DARK_RUNE_ACOLYTE;
						break;
				}
                CallCreature(idAdd, TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM, SpawnLocs[randpos][0], SpawnLocs[randpos][1], z);
            }
        }
    }

	void UpdateFlyPhase()
	{
		
		FlyPoint++;
		if(FlyPoint >= 9)
			FlyPoint = 0;
		me->GetMotionMaster()->MovePoint(0,FlyCoords[FlyPoint][0],FlyCoords[FlyPoint][1],FlyCoords[FlyPoint][2]);
	}

    void FlyPhase()
    {
        SetFlying(true,me);
		Speak(CHAT_TYPE_BOSS_EMOTE,0,"Tranchécaille s'envole !");
		IsFlying = true;
		SetCombatMovement(false);
		FlyPoint = 0;
    }

	void GroundPhase()
	{
		SetFlying(false,me);
		Phase = PHASE_GROUND;
		SetCombatMovement(true);
        me->SetSpeedRate(MOVE_WALK, 1.5f, false);
		Relocate(585.315f,-172.646f,391.517f);
		IsFlying = false;
		CheckDist_Timer = 3000;
	}
};

bool GossipHello_npc_RazorScale(Player* pPlayer, Creature* pCreature)
{
	if(((ScriptedInstance*)pCreature->GetInstanceData())->GetData(TYPE_RAZORSCALE) != NOT_STARTED)
		return false;

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Appeler Tranchecaille !", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_RazorScale(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
			if (Creature *Rzsc =(Creature*) Unit::GetUnit(*pCreature, ((ScriptedInstance*)pCreature->GetInstanceData())->GetData64(TYPE_RAZORSCALE)))
			{
				((boss_razorscaleAI*)Rzsc->AI())->CallRazorscale();
			}
			pCreature->MonsterYell("Ramenez Tranchecaille ici !", 0, pCreature ? pCreature->GetGUID() : 0);
			pPlayer->CLOSE_GOSSIP_MENU();
            break;
    }
    return true;
}

CreatureAI* GetAI_boss_razorscale(Creature* pCreature)
{
    return new boss_razorscaleAI (pCreature);
}

void AddSC_boss_razorscale()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_razorscale";
    newscript->GetAI = &GetAI_boss_razorscale;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_event_razorscale";
    newscript->pGossipHello =  &GossipHello_npc_RazorScale;
    newscript->pGossipSelect = &GossipSelect_npc_RazorScale;
    newscript->RegisterSelf();
}
