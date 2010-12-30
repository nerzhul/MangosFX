#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Summons
{
    NPC_FROST_SPHERE     = 34606,
    NPC_BURROW           = 34862,
    NPC_BURROWER         = 34607,
    NPC_SCARAB           = 34605,
    NPC_SPIKE            = 34660,
};

enum BossSpells
{
    SPELL_FREEZE_SLASH      = 66012,
    SPELL_PENETRATING_COLD  = 66013,
    SPELL_LEECHING_SWARM    = 66118,
    SPELL_LEECHING_HEAL     = 66125,
    SPELL_LEECHING_DAMAGE   = 66240,
    SPELL_MARK              = 67574,
    SPELL_SPIKE_CALL        = 66169,
    SPELL_SUBMERGE_ANUBARAK = 65981,
    SPELL_CLEAR_ALL_DEBUFFS = 34098,
    SPELL_EMERGE_ANUBARAK   = 65982,
    SPELL_SUMMON_BEATLES    = 66339,
    SPELL_SUMMON_BURROWER   = 66332,

    // Burrow
    SPELL_CHURNING_GROUND   = 66969,

    // Scarab
    SPELL_DETERMINATION     = 66092,
    SPELL_ACID_MANDIBLE     = 65774, //Passive - Triggered

    // Burrower
    SPELL_SPIDER_FRENZY     = 66128,
    SPELL_EXPOSE_WEAKNESS   = 67720, //Passive - Triggered
    SPELL_SHADOW_STRIKE     = 66134,
    SPELL_SUBMERGE_EFFECT   = 53421,
    SPELL_EMERGE_EFFECT     = 66947,

    SUMMON_SCARAB           = NPC_SCARAB,
    SUMMON_FROSTSPHERE      = NPC_FROST_SPHERE,
    SPELL_BERSERK           = 26662,

    //Frost Sphere
    SPELL_FROST_SPHERE      = 67539,
    SPELL_PERMAFROST        = 66193,
    SPELL_PERMAFROST_VISUAL = 65882,

    //Spike
    SPELL_SUMMON_SPIKE      = 66169,
    SPELL_SPIKE_SPEED1      = 65920,
    SPELL_SPIKE_TRAIL       = 65921,
    SPELL_SPIKE_SPEED2      = 65922,
    SPELL_SPIKE_SPEED3      = 65923,
    SPELL_SPIKE_FAIL        = 66181,
    SPELL_SPIKE_TELE        = 66170,
};

enum SummonActions
{
    ACTION_SHADOW_STRIKE,
    ACTION_SCARAB_SUBMERGE,
};

const float SphereSpawn[6][4] =
{
    { 786.6439f, 108.2498f, 155.6701f, 0 },
    { 806.8429f, 150.5902f, 155.6701f, 0 },
    { 759.1386f, 163.9654f, 155.6701f, 0 },
    { 744.3701f, 119.5211f, 155.6701f, 0 },
    { 710.0211f, 120.8152f, 155.6701f, 0 },
    { 706.6383f, 161.5266f, 155.6701f, 0 },
};

const float AnubarakLoc[][4]=
{
    {787.932556f, 133.289780f, 142.612152f, 0},  // 0 - Anub'arak start location
    {695.240051f, 137.834824f, 142.200000f, 0},  // 1 - Anub'arak move point location
    {694.886353f, 102.484665f, 142.119614f, 0},  // 3 - Nerub Spawn
    {694.500671f, 185.363968f, 142.117905f, 0},  // 5 - Nerub Spawn
    {731.987244f, 83.3824690f, 142.119614f, 0},  // 2 - Nerub Spawn
    {740.184509f, 193.443390f, 142.117584f, 0},  // 4 - Nerub Spawn
};


struct MANGOS_DLL_DECL boss_anubarakEdCAI : public LibDevFSAI
{
    boss_anubarakEdCAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_FREEZE_SLASH,15000,15000,0,1);
    }

    std::list<uint64> m_vBurrowGUID;
    uint64 m_aSphereGUID[6];

    uint32 m_uiPenetratingColdTimer;
    uint32 m_uiSummonNerubianTimer;
    uint32 m_uiNerubianShadowStrikeTimer;
    uint32 m_uiSubmergeTimer;
    uint32 m_uiPursuingSpikeTimer;
    uint32 m_uiSummonScarabTimer;
    uint32 m_uiSummonFrostSphereTimer;
    uint32 m_uiBerserkTimer;

    uint8  m_uiStage;
    bool   m_bIntro;
    bool   m_bReachedPhase3;
    uint64 m_uiTargetGUID;
    uint8  m_uiScarabSummoned;



    void Reset()
    {
		ResetTimers();
        m_uiPenetratingColdTimer = 20*IN_MILLISECONDS;
        m_uiNerubianShadowStrikeTimer = 30*IN_MILLISECONDS;
        m_uiSummonNerubianTimer = 10*IN_MILLISECONDS;
        m_uiSubmergeTimer = 80*IN_MILLISECONDS;

        m_uiPursuingSpikeTimer = 2*IN_MILLISECONDS;
        m_uiSummonScarabTimer = 2*IN_MILLISECONDS;

        m_uiSummonFrostSphereTimer = 20*IN_MILLISECONDS;

        m_uiBerserkTimer = 15*MINUTE*IN_MILLISECONDS;
        m_uiStage = 0;
        m_uiScarabSummoned = 0;
        m_bIntro = true;
        m_bReachedPhase3 = false;
        m_uiTargetGUID = 0;
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		CleanMyAdds();
        m_vBurrowGUID.clear();

    }

	void KilledUnit(Unit* pWho)
    {
        if(urand(0,1))
			Speak(CHAT_TYPE_SAY,16236,"Flat'chir");
		else
			Speak(CHAT_TYPE_SAY,16237,"Encore un �me pour repa�tre l'arm�e des morts");

		if(pWho->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
    }

    void MoveInLineOfSight(Unit* /*pWho*/)
    {
        if (!m_bIntro)
        {
            //DoScriptText(SAY_INTRO, me);
            m_bIntro = false;
        }
    }

    void JustReachedHome()
    {
       SetInstanceData(TYPE_ANUBARAK, FAIL);
        //Summon Scarab Swarms neutral at random places
        for (int i=0; i < 10; i++)
            if (Creature* pTemp = CallCreature(NPC_SCARAB, TEN_MINS, PREC_COORDS, AGGRESSIVE_RANDOM, AnubarakLoc[1][0]+urand(0, 50)-25, AnubarakLoc[1][1]+urand(0, 50)-25, AnubarakLoc[1][2]))
                pTemp->setFaction(31);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        SetInstanceData(TYPE_ANUBARAK, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(TRIOMPHE,4);
				break;
		}
		Yell(16238,"J'ai �chou�... ma�tre...");
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit *pTarget = GetRandomUnit(0);
        switch (pSummoned->GetEntry())
        {
            case NPC_BURROW:
                m_vBurrowGUID.push_back(pSummoned->GetGUID());
                pSummoned->SetReactState(REACT_PASSIVE);
                pSummoned->CastSpell(pSummoned, SPELL_CHURNING_GROUND, false);
                break;
            case NPC_SPIKE:
                pSummoned->CombatStart(pTarget);
                DoScriptText(EMOTE_SPIKE, me, pTarget);
                break;
        }
    }

    void SummonedCreatureDespawn(Creature* pSummoned)
    {
        switch (pSummoned->GetEntry())
        {
            case NPC_SPIKE:
                m_uiPursuingSpikeTimer = 2*IN_MILLISECONDS;
                break;
        }
    }

    void EnterCombat(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, me);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        me->SetInCombatWithZone();
        SetInstanceData(TYPE_ANUBARAK, IN_PROGRESS);
        //Spawn Burrow
        for (uint8 i=0; i < 4; i++)
            CallAggressiveCreature(NPC_BURROW, TEN_MINS, PREC_COORDS, AnubarakLoc[i+2][0],AnubarakLoc[i+2][1],AnubarakLoc[i+2][2]);
        //Spawn Frost Spheres
        for (int i=0; i < 6; i++)
			if (Unit *pSummoned = CallAggressiveCreature(NPC_FROST_SPHERE, TEN_MINS, PREC_COORDS, SphereSpawn[i][0],SphereSpawn[i][1],SphereSpawn[i][2]))
                m_aSphereGUID[i] = pSummoned->GetGUID();
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
		Speak(CHAT_TYPE_SAY,16234,"Ce terreau sera votre tombeau !");
        SetInstanceData(TYPE_ANUBARAK, IN_PROGRESS);
    }
    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		switch (m_uiStage)
        {
			case 0:
				UpdateEvent(diff,1);
				if (m_uiPenetratingColdTimer <= uiDiff)
				{
					me->CastCustomSpell(SPELL_PENETRATING_COLD, SPELLVALUE_MAX_TARGETS, RAID_MODE(2, 5));
					m_uiPenetratingColdTimer = 20*IN_MILLISECONDS;
				} else m_uiPenetratingColdTimer -= uiDiff;

				if (m_uiSummonNerubianTimer <= diff && (m_difficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_difficulty == RAID_DIFFICULTY_25MAN_HEROIC 
					|| !m_bReachedPhase3))
				{
					switch(m_difficulty)
					{
						case RAID_DIFFICULTY_10MAN_NORMAL:
							DoCastRandom(SPELL_SUMMON_BURROWER);
							break;
						case RAID_DIFFICULTY_25MAN_NORMAL:
							for(uint8 i=0;i<3;i++)
								DoCastRandom(SPELL_SUMMON_BURROWER);
							break;
						case RAID_DIFFICULTY_10MAN_HEROIC:
							for(uint8 i=0;i<2;i++)
								DoCastRandom(SPELL_SUMMON_BURROWER);
							break;
						case RAID_DIFFICULTY_25MAN_HEROIC:
							for(uint8 i=0;i<4;i++)
								DoCastRandom(SPELL_SUMMON_BURROWER);
							break;
					}
					
					m_uiSummonNerubianTimer = 45*IN_MILLISECONDS;
				} else m_uiSummonNerubianTimer -= uiDiff;

				if (IsHeroic() && m_uiNerubianShadowStrikeTimer <= uiDiff)
				{
					Summons.DoAction(NPC_BURROWER, ACTION_SHADOW_STRIKE);
					m_uiNerubianShadowStrikeTimer = 30*IN_MILLISECONDS;
				} else m_uiNerubianShadowStrikeTimer -= uiDiff;

				if (m_uiSubmergeTimer <= uiDiff && !m_bReachedPhase3 && !me->HasAura(SPELL_BERSERK))
				{
					m_uiStage = 1;
					m_uiSubmergeTimer = 60*IN_MILLISECONDS;
				} else m_uiSubmergeTimer -= uiDiff;
				break;
			case 1:
				DoCastMe(SPELL_SUBMERGE_ANUBARAK);
				DoCastMe(SPELL_CLEAR_ALL_DEBUFFS);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				Yell(16240,"Aoum Na'akish ! D�vorez mes serviteurs !");
				m_uiScarabSummoned = 0;
				m_uiSummonScarabTimer = 4*IN_MILLISECONDS;
				m_uiStage = 2;
				break;
			case 2:
				UpdateEvent(diff,3);

				if (m_uiSummonScarabTimer <= uiDiff)
				{
					/* WORKAROUND
					 * - The correct implementation is more likely the comment below but it needs spell knowledge
					 */
					std::list<uint64>::iterator i = m_vBurrowGUID.begin();
					uint32 at = urand(0, m_vBurrowGUID.size()-1);
					for (uint32 k = 0; k < at; k++)
						++i;
					if (Creature *pBurrow = Unit::GetCreature(*me, *i))
						pBurrow->CastSpell(pBurrow, 66340, false);
					m_uiScarabSummoned++;
					m_uiSummonScarabTimer = 4*IN_MILLISECONDS;
					if (m_uiScarabSummoned == 4) m_uiSummonScarabTimer = RAID_MODE(4, 20)*IN_MILLISECONDS;

					/*It seems that this spell have something more that needs to be taken into account
					//Need more sniff info
					DoCast(SPELL_SUMMON_BEATLES);
					// Just to make sure it won't happen again in this phase
					m_uiSummonScarabTimer = 90*IN_MILLISECONDS;*/
				} else m_uiSummonScarabTimer -= uiDiff;

				if (m_uiSubmergeTimer <= uiDiff)
				{
					m_uiStage = 3;
					m_uiSubmergeTimer = 80*IN_MILLISECONDS;
				} else m_uiSubmergeTimer -= uiDiff;
				break;
			case 3:
				m_uiStage = 0;
				DoCast(SPELL_SPIKE_TELE);
				CleanMyAdds(NPC_SPIKE);
				me->RemoveAurasDueToSpell(SPELL_SUBMERGE_ANUBARAK);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				DoCastMe(SPELL_EMERGE_ANUBARAK);
				me->GetMotionMaster()->MoveChase(me->getVictim());
				m_uiSummonNerubianTimer = 10*IN_MILLISECONDS;
				m_uiNerubianShadowStrikeTimer = 30*IN_MILLISECONDS;
				m_uiSummonScarabTimer = 2*IN_MILLISECONDS;
				break;
		}

		if (!IsHeroic())
		{
			if (m_uiSummonFrostSphereTimer <= uiDiff)
			{
				uint8 startAt = urand(0, 5);
				uint8 i = startAt;
				do
				{
					if (Unit *pSphere = Unit::GetCreature(*me, m_aSphereGUID[i]))
					{
						if (!pSphere->HasAura(SPELL_FROST_SPHERE))
						{
							if (Creature *pSummon = CallAggressiveCreature(NPC_FROST_SPHERE, TEN_MINS, PREC_COORDS, SphereSpawn[i][0],SphereSpawn[i][1],SphereSpawn[i][2]))
								m_aSphereGUID[i] = pSummon->GetGUID();
							break;
						}
					}
					i = (i+1)%6;
				} while (i != startAt);
				m_uiSummonFrostSphereTimer = urand(20, 30)*IN_MILLISECONDS;
			} else m_uiSummonFrostSphereTimer -= diff;
		}

		if (GetPercentLife() <= 30 && m_uiStage == 0 && !m_bReachedPhase3)
		{
			m_bReachedPhase3 = true;
			DoCastMe(SPELL_LEECHING_SWARM);
			Yell(16241,"L'essaim va vous submerger !");
		}

		if (m_uiBerserkTimer <= diff && !me->HasAura(SPELL_BERSERK))
		{
			DoCastMe(SPELL_BERSERK);
		} else m_uiBerserkTimer -= diff;
	
		UpdateEvent(diff);
		DoMeleeAttackIfReady();

	}

};

CreatureAI* GetAI_boss_anubarakEdC(Creature* pCreature)
{
    return new boss_anubarakEdCAI(pCreature);
}

struct MANGOS_DLL_DECL mob_swarm_scarabAI : public LibDevFSAI
{
    mob_swarm_scarabAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_ACID_MANDIBLE,5000,20000);
		AddEventOnTank(SPELL_DETERMINATION,5000,10000,20000);
    }

    void Reset()
    {
		ResetTimers();		
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}
};

CreatureAI* GetAI_mob_swarm_scarab(Creature* pCreature)
{
    return new mob_swarm_scarabAI(pCreature);
}

struct MANGOS_DLL_DECL mob_nerubian_borrowerAI : public LibDevFSAI
{
    mob_nerubian_borrowerAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_EXPOSE_WEAKNESS,5000,5000);
		AddEventOnMe(SPELL_SPIDER_FRENZY,9000,10000,1000);
    }

	uint32 Submerge_Timer;
    bool submerged;

    void Reset()
    {
		ResetTimers();
		Submerge_Timer = 2000;
        submerged = false;
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(Submerge_Timer <= diff)
		{
			if (CheckPercentLife(20) && !submerged)
			{
				DoCastMe(SPELL_SUBMERGE_1);
				submerged = true;
				Speak(CHAT_TYPE_TEXT_EMOTE,0,"Fouisseur n�rubien s'enfouit");
			}
			Submerge_Timer = urand(20000,30000);
		}
		else
			Submerge_Timer -= diff;

        if (GetPercentLife() > 50.0f && submerged)
        {
             me->RemoveAurasDueToSpell(SPELL_SUBMERGE_1);
             submerged = false;
             Speak(CHAT_TYPE_TEXT_EMOTE,0,"Fouisseur n�rubien sort du sol");
         };

		UpdateEvent(diff);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_nerubian_borrower(Creature* pCreature)
{
    return new mob_nerubian_borrowerAI(pCreature);
}

struct MANGOS_DLL_DECL anub_sphereAI : public ScriptedAI
{
    anub_sphereAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    
    uint32 explode_timer;
    MobEventTasks Tasks;
    void Reset()
    {
		Tasks.SetObjects(this,me);
		explode_timer = DAY*HOUR;
		SetCombatMovement(false);
    }

    void DamageTaken(Unit* u, uint32 &dmg)
    {
		if(dmg >= me->GetHealth())
		{
			 dmg = 0;
			 explode_timer = 1000;
		}
    }

    void UpdateAI(const uint32 diff)
    {
		if(explode_timer <= diff)
		{
			DoCastMe(SPELL_PERMAFROST);
			Kill(me);
		}
		else 
			explode_timer -= diff;
    }

};

CreatureAI* GetAI_anub_sphere(Creature* pCreature)
{
    return new anub_sphereAI(pCreature);
}

struct MANGOS_DLL_DECL anub_spikeAI : public LibDevFSAI
{
    anub_spikeAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_IMPALE,500,1500,500);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
    
    void Reset()
    {
		ResetTimers();
    }

	void KilledUnit(Unit* victim)
	{
		if(victim->GetTypeId() == TYPEID_PLAYER)
			SetInstanceData(TYPE_TRY,1);
	}

    void UpdateAI(const uint32 diff)
    {
		UpdateEvent(diff);
    }

};

CreatureAI* GetAI_anub_spike(Creature* pCreature)
{
    return new anub_spikeAI(pCreature);
}

void AddSC_boss_AA_toc10()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anubarakEdC";
    newscript->GetAI = &GetAI_boss_anubarakEdC;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "mob_swarm_scarab";
    newscript->GetAI = &GetAI_mob_swarm_scarab;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nerubian_borrower";
    newscript->GetAI = &GetAI_mob_nerubian_borrower;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "anub_sphere";
    newscript->GetAI = &GetAI_anub_sphere;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "anub_spike";
    newscript->GetAI = &GetAI_anub_spike;
    newscript->RegisterSelf();
};
