#include "precompiled.h"

enum {
	WSTATE_PRINCIPAL	=	3816,
	WSTATE_SCEAU		=	3815,
	WSTATE_PORTAL		=	3810,
	MAX_ENCOUNTER		=	2,
	NPC_PORTAL			=	31011,
	NPC_CYANIGOSA		=	31134,
	NPC_GUARDIAN		=	30660,
	EMOTE_GUARDIAN		=	-2000003,
	EMOTE_ESCADRILLE	=	-2000004,
	NPC_ICHORON			=	29313,
	NPC_XEVOZZ			=	29266,
	NPC_ZURAMAT			=	29314,
	NPC_MORAGG			=	29316,
	NPC_LAVANTHOR		=	29312,
	NPC_EREKEM			=	29315,
	ADD_EREKEM			=	29395,
	NPC_CASTER			=	30667,
	NPC_CAPTAIN			=	30666,
	NPC_ECUMEUR			=	30668,
	NPC_TRAQUEUR		=	32191,
	NPC_SCEAU			=	30896,
};

struct Wave
{
    uint32 PortalBoss;                                      //protector of current portal
    uint32 NextPortalTime;                                  //time to next portal, or 0 if portal boss need to be killed
};

struct portal_struct
{
	float x,y,z,o;
};

static portal_struct portal_coords[] =
{
	{1936.07,803.198,53.3749,3.12414},
	{1877.51,850.104,44.6599,4.7822},
	{1890.64,753.471,48.7224,1.71042},
	{1930.045,838.802,47.141,3.089},
	{1923.164,759.784,50.867,2.262},
	{1855.862,765.493,38.652,0.928},
	{1908.332,830.241,38.715,3.560},
	//Invoc boss portal
	{1893.640f,804.737f,38.450f,3.112f},
};

enum boss_doors{
	LAVANTHOR_DOOR		= 191566,
	MORAGG_DOOR			= 191606,
	ZURAMAT_DOOR		= 191565,
	XEVOZZ_DOOR			= 191556,
	ICHORON_DOOR		= 191722,
	EREKEM_DOOR_1		= 191562,
	EREKEM_DOOR_2		= 191564,
	EREKEM_DOOR_3		= 191563,
};



struct MANGOS_DLL_DECL instance_violethold : public ScriptedInstance
{
	instance_violethold(Map* pMap) : ScriptedInstance(pMap) {Initialize();};
	uint8 m_uiRiftWaveId;
	uint32 m_auiEncounter[MAX_ENCOUNTER];
	uint16 m_BossCount;

	uint32 SpawnPortal_Timer;
    uint32 m_uiNextPortal_Timer;
	uint32 CheckClose_Timer;

	uint32 m_uiRiftPortalCount;
	uint32 lastportal;

	uint64 pCyanigosa;
	uint32 ShieldScore;
	uint64 portals[8];
	uint64 crPortals[8][4];
	uint64 sceau;
	bool portal_opened[8];
	bool boss_down[6];
	bool stopEvent;
	bool late;
	GameObject* jail[8];
	uint64 boss[8];
	uint64 pLieutenant;
	uint8 add;
	std::vector<uint64> XevozzAdds;
	std::vector<uint64> IchoronAdds;

	void Initialize()
    {
		Clear();
	}

	void Clear()
	{
		memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
		add = 0;
		m_uiRiftWaveId = 0;
		m_uiNextPortal_Timer = 20000;
		m_uiRiftPortalCount = 0;
		ShieldScore = 100;
		m_BossCount = 1;
		for(int i = 0; i < 8; i++)
		{
			portal_opened[i] = false;
			portals[i] = 0;
		}

		for(int i = 0; i < 6; i++)
			boss_down[i] = false;


		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 4; j++)
				crPortals[i][j] = 0;

		SpawnPortal_Timer = 32000;
		CheckClose_Timer = 34000;

		stopEvent = true;
	}

	void InitWorldState()
    {
		DoUpdateWorldState(WSTATE_SCEAU,100);
		DoUpdateWorldState(WSTATE_PORTAL,0);
    }

	bool CanProgressEvent()
    {
        if (instance->GetPlayers().isEmpty())
            return false;

        return true;
    }

	void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
		{
			case NPC_CYANIGOSA:
				pCyanigosa = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				pCreature->SetVisibility(VISIBILITY_OFF);
				break;
			case NPC_ICHORON:
				boss[4] = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case NPC_XEVOZZ:
				boss[3] = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case NPC_ZURAMAT:
				boss[2] = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case NPC_MORAGG:
				boss[1]	= pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case NPC_LAVANTHOR:
				boss[0] = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case NPC_EREKEM:
				boss[5] = pCreature->GetGUID();
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				break;
			case ADD_EREKEM:
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pCreature->CastSpell(pCreature,66830,false);
				boss[6+add] = pCreature->GetGUID();
				add++;
				break;
			case 30658:
				pLieutenant = pCreature->GetGUID();
				break;
			case 29271:
			case 32582:
				XevozzAdds.push_back(pCreature->GetGUID());
				break;
			case 29321:
				IchoronAdds.push_back(pCreature->GetGUID());
				break;
			case NPC_SCEAU:
				sceau = pCreature->GetGUID();
				pCreature->SetVisibility(VISIBILITY_OFF);
				break;
			default:
				break;

		}
    }

	void OnCreatureRespawn(Creature* pCreature)
	{
		pCreature->ForcedDespawn();
	}

    void OnPlayerEnter(Player* pPlayer)
    {
		if(!stopEvent)
		{
			pPlayer->SendUpdateWorldState(WSTATE_PRINCIPAL,1);
			pPlayer->SendUpdateWorldState(WSTATE_SCEAU,ShieldScore);
			pPlayer->SendUpdateWorldState(WSTATE_PORTAL,m_uiRiftWaveId);
		}
		else
			pPlayer->SendUpdateWorldState(WSTATE_PRINCIPAL,0);
	}

	void SetData(uint32 index,uint32 value)
	{
		if(index == 1 && value == 1)
			StartEvent();
		else if(index == 2 && value == 1)
		{
			for(std::vector<uint64>::iterator itr = XevozzAdds.begin(); itr!= XevozzAdds.end(); ++itr)
			{
				Unit *pAdd = GetUnitInMap(*itr);
				if (pAdd && pAdd->isAlive())
				{
					pAdd->SetVisibility(VISIBILITY_OFF);
					pAdd->setDeathState(JUST_DIED);
				}
			}
			XevozzAdds.clear();
		}
		else if(index == 3 && value == 1)
		{
			for(std::vector<uint64>::iterator itr = IchoronAdds.begin(); itr!= IchoronAdds.end(); ++itr)
			{
				Unit *pAdd = GetUnitInMap(*itr);
				if (pAdd && pAdd->isAlive())
				{
					pAdd->SetVisibility(VISIBILITY_OFF);
					pAdd->setDeathState(JUST_DIED);
				}
			}
			IchoronAdds.clear();
		}
		else if(index == 4)
		{
			if(ShieldScore > 4)
				ShieldScore -= 4;
			else
				ShieldScore = 0;
			DoUpdateWorldState(WSTATE_SCEAU,ShieldScore);

		}
	}
	void StartEvent()
	{
		DoUpdateWorldState(WSTATE_SCEAU,100);
		DoUpdateWorldState(WSTATE_PRINCIPAL,1);
		if(Creature* pLieutenantCr = (Creature*)GetUnitInMap(pLieutenant))
			pLieutenantCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		stopEvent = false;
	}

	void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
			case LAVANTHOR_DOOR:
				jail[0] = pGo;
				break;
			case MORAGG_DOOR:
				jail[1] = pGo;
				break;
			case ZURAMAT_DOOR:
				jail[2] = pGo;
				break;
			case XEVOZZ_DOOR:
				jail[3] = pGo;
				break;
			case ICHORON_DOOR:
				jail[4] = pGo;
				break;
			case EREKEM_DOOR_1:
				jail[5] = pGo;
				break;
			case EREKEM_DOOR_2:
				jail[6] = pGo;
				break;
			case EREKEM_DOOR_3:
				jail[7] = pGo;
				break;
			default:
				break;
		}
		pGo->SetFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND);
	}

	uint64 GetData64(uint32 dataN)
	{
		if(dataN == 1)
			return boss[3];
		else if(dataN == 2 && sceau)
			return sceau;
		return 0;
	}

	void SpawnPortal(bool IsBoss)
	{
		if(IsBoss == false)
		{
			int random = urand(0,6);
			while(portal_opened[random] == true && !(portal_opened[0] == true && portal_opened[1] == true && portal_opened[2] == true && portal_opened[3] == true && portal_opened[4] == true && portal_opened[5] == true && portal_opened[6] == true))
				random = urand(0,6);
			if(!(portal_opened[0] == true && portal_opened[1] == true && portal_opened[2] == true && portal_opened[3] == true && portal_opened[4] == true && portal_opened[5] == true && portal_opened[6] == true))				
			{
				lastportal = random;
				if(Creature* cyanigosa = (Creature*)GetUnitInMap(pCyanigosa))
					if(Creature* cr = cyanigosa->SummonCreature(NPC_PORTAL,portal_coords[random].x,portal_coords[random].y,portal_coords[random].z,portal_coords[random].o,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,600000))
					{
						portals[random] = cr->GetGUID();
						cr->SetRespawnDelay(RESPAWN_ONE_DAY);
					}
				portal_opened[random] = true;
			}
		}
		else
		{
			int random = 7;
			if(Creature* cyanigosa = (Creature*)GetUnitInMap(pCyanigosa))
				if(Creature* cr = cyanigosa->SummonCreature(NPC_PORTAL,portal_coords[7].x,portal_coords[7].y,portal_coords[7].z,portal_coords[7].o,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,600000))
					portals[7] = cr->GetGUID();
			portal_opened[7] = true;
			int rand_boss = urand(0,5);
			while(boss_down[rand_boss] == true)
				rand_boss = urand(0,5);
			boss_down[rand_boss] = true;
			// Ouverture des cellules pour le boss
			if(rand_boss != 5)
			{
				if(Creature* cyanigosa = (Creature*)GetUnitInMap(pCyanigosa))
					jail[rand_boss]->Use(cyanigosa);
			}
			// Si c'est Erekem on ouvre les 3
			else
			{
				if(Creature* cyanigosa = (Creature*)GetUnitInMap(pCyanigosa))
				{
					jail[6]->Use(cyanigosa);
					jail[7]->Use(cyanigosa);
					jail[5]->Use(cyanigosa);
				}
				if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[7]))
				{
					pBossCr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					pBossCr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					pBossCr->RemoveAllAuras();
					pBossCr->RemoveAurasDueToSpell(66830);
				}
				if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[6]))
				{
					pBossCr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					pBossCr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					pBossCr->RemoveAllAuras();
					pBossCr->RemoveAurasDueToSpell(66830);
				}
			}

			// On rend le boss vulnérable et opérationnel
			if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss]))
			{
				pBossCr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				pBossCr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				pBossCr->RemoveAllAuras();
				pBossCr->RemoveAurasDueToSpell(66830);
			}
			ClosePortal(7);
			            
			switch(rand_boss)
			{
				case 4:
					if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss]))
					{
						DoScriptText(-2000005,pBossCr);
						pBossCr->AddSplineFlag(SPLINEFLAG_WALKMODE);
						pBossCr->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
					}
					break;
				case 2:
					if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss]))
					{
						DoScriptText(-2000025,pBossCr);
						pBossCr->AddSplineFlag(SPLINEFLAG_WALKMODE);
						pBossCr->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
					}
					break;
				case 5:
					if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss]))
					{
						DoScriptText(-2000032,pBossCr);
						pBossCr->AddSplineFlag(SPLINEFLAG_WALKMODE);
						pBossCr->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
					}
					if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss+1]))
					{
						pBossCr->AddSplineFlag(SPLINEFLAG_WALKMODE);
						pBossCr->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
					}
					if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss+2]))
					{
						pBossCr->AddSplineFlag(SPLINEFLAG_WALKMODE);
						pBossCr->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
					}
					break;
				case 3:
					if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss]))
					{
						DoScriptText(-2000039,pBossCr);
						pBossCr->AddSplineFlag(SPLINEFLAG_WALKMODE);
						pBossCr->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
					}
					break;
            	case 0:
				case 1:
					if(Creature* pBossCr = (Creature*)GetUnitInMap(boss[rand_boss]))
					{
						pBossCr->AddSplineFlag(SPLINEFLAG_WALKMODE);
						pBossCr->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
					}
            		break;
			}
		}
	}

	void SpawnMobs(uint32 lstportal, bool IsGuardian)
	{
		if(IsGuardian == true)
		{
			if(Creature* cyanigosa = (Creature*)GetUnitInMap(pCyanigosa))
				if(Creature* cr = cyanigosa->SummonCreature(NPC_GUARDIAN,portal_coords[lastportal].x,portal_coords[lastportal].y,portal_coords[lastportal].z,portal_coords[lastportal].o,TEMPSUMMON_TIMED_DESPAWN,600000))
				{
					crPortals[lstportal][0] = cr->GetGUID();
					cr->SetRespawnDelay(RESPAWN_ONE_DAY);
				}
			if(Creature* pLieutenantCr = (Creature*)GetUnitInMap(pLieutenant))
				DoScriptText(EMOTE_GUARDIAN,pLieutenantCr);
		}
		else
		{
			if(Creature* cyanigosa = (Creature*)GetUnitInMap(pCyanigosa))
			{
				if(Creature* cr = cyanigosa->SummonCreature(NPC_CASTER,portal_coords[lastportal].x+2,portal_coords[lastportal].y+2,portal_coords[lastportal].z,portal_coords[lastportal].o,TEMPSUMMON_TIMED_DESPAWN,600000))
				{
					crPortals[lstportal][0] = cr->GetGUID();
					cr->SetRespawnDelay(RESPAWN_ONE_DAY);
				}
				if(Creature* cr = cyanigosa->SummonCreature(NPC_ECUMEUR,portal_coords[lastportal].x+2,portal_coords[lastportal].y,portal_coords[lastportal].z,portal_coords[lastportal].o,TEMPSUMMON_TIMED_DESPAWN,600000))
				{
					crPortals[lstportal][1] = cr->GetGUID();
					cr->SetRespawnDelay(RESPAWN_ONE_DAY);
				}
				if(Creature* cr = cyanigosa->SummonCreature(NPC_CAPTAIN,portal_coords[lastportal].x,portal_coords[lastportal].y+2,portal_coords[lastportal].z,portal_coords[lastportal].o,TEMPSUMMON_TIMED_DESPAWN,600000))
				{
					crPortals[lstportal][2] = cr->GetGUID();
					cr->SetRespawnDelay(RESPAWN_ONE_DAY);
				}
				if(Creature* cr = cyanigosa->SummonCreature(NPC_TRAQUEUR,portal_coords[lastportal].x,portal_coords[lastportal].y,portal_coords[lastportal].z,portal_coords[lastportal].o,TEMPSUMMON_TIMED_DESPAWN,600000))
				{
					crPortals[lstportal][3] = cr->GetGUID();
					cr->SetRespawnDelay(RESPAWN_ONE_DAY);
				}
			}
			if(Creature* pLieutenantCr = (Creature*)GetUnitInMap(pLieutenant))
				DoScriptText(EMOTE_ESCADRILLE,pLieutenantCr);
		}
		
	}

	void ClosePortal(uint32 id_portal)
	{
		if(Creature* cr = ((Creature*)GetUnitInMap(portals[id_portal])))
			cr->RemoveFromWorld();
		portals[id_portal] = 0;
		portal_opened[id_portal] = false;
	}

	void EventCyanigosa()
	{
		m_uiNextPortal_Timer = RESPAWN_ONE_DAY*1000;
		stopEvent = true;
		if(Creature* cyanigosa = (Creature*)GetUnitInMap(pCyanigosa))
		{
			cyanigosa->RemoveAllAuras();
			cyanigosa->RemoveAurasDueToSpell(66830);
			cyanigosa->SetVisibility(VISIBILITY_ON);
			cyanigosa->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			cyanigosa->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			cyanigosa->GetMotionMaster()->MovePoint(0, portal_coords[7].x, portal_coords[7].y, portal_coords[7].z);
		}
	}

	void Update(uint32 diff)
    {
		if (!CanProgressEvent())
        {
            Clear();
            return;
        }
		
		if(stopEvent)
			return;

		if(ShieldScore <= 0)
		{
			stopEvent = true;
			return;
		}

        if (m_uiNextPortal_Timer <= diff)
		{
			++m_uiRiftPortalCount;
			DoUpdateWorldState(WSTATE_PORTAL,m_uiRiftPortalCount);
			if(m_uiRiftPortalCount % 6 == 0)
			{
				if(m_BossCount < 3)
				{
					++m_BossCount;
					SpawnPortal(true);
					m_uiNextPortal_Timer = 120000;
				}
				else
					EventCyanigosa();
			}
			else
			{
				SpawnPortal(false);
				SpawnPortal_Timer = 10000;
				m_uiNextPortal_Timer = 70000;
			}
		}
		else
			m_uiNextPortal_Timer -= diff;

		if(SpawnPortal_Timer <= diff)
		{
			switch(lastportal)
			{
				case 0:
				case 1:
				case 5:
				case 6:
					SpawnMobs(lastportal,true);
					break;
				case 2:
				case 3:
				case 4:
					SpawnMobs(lastportal,false);
					break;
				case 7:
					SpawnPortal_Timer = 12000000;
					break;
			}
			// On force le portail a ne plus pop pour le moment
			SpawnPortal_Timer = 120000000;
		}
		else
			SpawnPortal_Timer -= diff;

	if(CheckClose_Timer <= diff)
	{
		for(uint32 i = 0; i < 7;i++)
		{
			if(portals[i])
			{
				switch(i)
				{
					case 0:
					case 1:
					case 5:
					case 6:
						if(Creature* cr = ((Creature*)GetUnitInMap(crPortals[i][0])))
							if(cr->isAlive() == false)
							{
								ClosePortal(i);
								crPortals[i][0] = 0;
							}
						break;
					case 2:
					case 3:
					case 4:
					{
						Creature* cr = ((Creature*)GetUnitInMap(crPortals[i][0]));
						Creature* cr2 = ((Creature*)GetUnitInMap(crPortals[i][1]));
						Creature* cr3 = ((Creature*)GetUnitInMap(crPortals[i][2]));
						Creature* cr4 = ((Creature*)GetUnitInMap(crPortals[i][3]));
						if(cr && cr2 && cr3 && cr4)
							if(!cr->isAlive() && !cr2->isAlive() && !cr3->isAlive() && !cr4->isAlive())
							{
								ClosePortal(i);
								for(int j = 0; j < 4; j++)
									crPortals[i][j] = 0;
							}
						break;
					}
				}
			}
		}
		CheckClose_Timer = 2000;
	}
	else
		CheckClose_Timer -= diff;
	}
};

InstanceData* GetInstanceData_instance_violethold(Map* pMap)
{
    return new instance_violethold(pMap);
}

#define ACTIVE_CRYSTAL "Activer les cristaux si on a des ennuis, c'est noté"
#define SECURE "Mettez vos hommes en sécurité, nous allons repousser les troupes du vol bleu."

bool GossipHello_npc_Sinclari(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ACTIVE_CRYSTAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(110000, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_Sinclari(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	ScriptedInstance* pInstance;
	pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, SECURE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(110001, pCreature->GetGUID());
            break;
		case GOSSIP_ACTION_INFO_DEF+2:
			pPlayer->CLOSE_GOSSIP_MENU();
			pInstance->SetData(1,1);
			pCreature->AddSplineFlag(SPLINEFLAG_WALKMODE);
			pCreature->GetMotionMaster()->MovePoint(0, 1817.122f, 804.02f, 45.01f);
			break;
    }

    return true;
}

struct MANGOS_DLL_DECL portal_AddAI : public LibDevFSAI
{	
	uint32 spawn_Timer;

	portal_AddAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
		InitInstance();
	}

    void Reset()
    {
		spawn_Timer = 30000;
    }

    void UpdateAI(const uint32 diff)
    {
		if(spawn_Timer <= diff)
		{
			int rand = urand(0,3);
			Creature* tmp,*tmp2;
			switch(rand)
			{
				case 0:
					tmp = CallCreature(30662,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					tmp2 = CallCreature(30662,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					break;
				case 1:
					tmp = CallCreature(30665,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					tmp2 = CallCreature(30665,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					break;
				case 2:
					tmp = CallCreature(30661,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					tmp2 = CallCreature(30661,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					break;
				case 3:
					tmp = CallCreature(30664,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					tmp2 = CallCreature(30664,THREE_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					break;
			}
			
			if(tmp && tmp->isAlive())
			{
				tmp->GetMotionMaster()->MovePoint(0, 1817.122f, 804.02f, 45.01f);
				if (Creature* sceau = GetInstanceCreature(2))
					if(sceau->isAlive())
						tmp->AddThreat(sceau);
			}
			if(tmp2 && tmp->isAlive())
			{
				tmp2->GetMotionMaster()->MovePoint(0, 1817.122f, 804.02f, 45.01f);
				if (Creature* sceau = GetInstanceCreature(2))
					if(sceau->isAlive())
						tmp->AddThreat(sceau);
			}

			spawn_Timer = 30000;
		}
		else spawn_Timer -= diff;
	}

};

CreatureAI* GetAI_portal_add(Creature* pCreature)
{
    return new portal_AddAI(pCreature);
} 

struct MANGOS_DLL_DECL seal_AddAI : public ScriptedAI
{	
    ScriptedInstance* pInstance;

	seal_AddAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		Reset();
	}

    void Reset() {}

	void DamageTaken(Unit* pwho, uint32 &dmg)
	{
		dmg = 0;
		if(pInstance)
			pInstance->SetData(4,0);
	}
};

CreatureAI* GetAI_violet_seal(Creature* pCreature)
{
    return new seal_AddAI(pCreature);
} 


void AddSC_instance_violethold()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_violethold";
    newscript->GetInstanceData = &GetInstanceData_instance_violethold;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_Sinclari";
    newscript->pGossipHello = &GossipHello_npc_Sinclari;
    newscript->pGossipSelect = &GossipSelect_npc_Sinclari;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "add_portal_fp";
    newscript->GetAI = &GetAI_portal_add;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_violethold_seal";
    newscript->GetAI = &GetAI_violet_seal;
    newscript->RegisterSelf();

}
