#include "precompiled.h"
#include "def_culling_of_stratholme.h"

#define GO_SHKAF_GATE       188686
#define GO_MALGANIS_GATE1   187711    
#define GO_MALGANIS_GATE2   187723 
#define GO_MALGANIS_CHEST   190663 

enum CullingMobs
{
	NPC_ABO				=	27736,
	NPC_ABO2			=	28201,
	NPC_DEMON_CRYPTES	=	27734,
	NPC_GOULE			=	28249,
	NPC_GOULE2			=	27729,
	NPC_NECRO			=	27732,
	NPC_NECRO2			=	28200,
	BOSS_SALRAMM		=	26530,
	BOSS_MEATHOOK		=	26529,
	BOSS_CORRUPTEUR		=	32273
};

struct Locations
{
    float x, y, z;
};

static Locations randspawn[]=
{
	{2227.0566f,	1331.131f,	127.2f}, // roi south
	{2128.769f,		1361.514f,	132.2f}, // roi nord
	{2194.861f,		1226.193f,	138.1f}, // festival ouest
	{2261.019f,		1173.843f,	139.1f}, // festival centre
	{2356.439f,		1184.324f,	131.11f}, // festival est
};

struct MANGOS_DLL_DECL instance_culling_of_stratholme : public ScriptedInstance
{
    instance_culling_of_stratholme(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[2];
	uint64 corrupteur_guid;
	uint16 nbvague;
	bool ActiveTimer;
	bool HeroicMap;
	uint8 lastspawn;
	uint32 Instance_Timer;
	uint32 StartVague_Timer;
	uint16 min_Timer;
    std::string strInstData;

	uint64 ArthasGUID;

    uint64 m_uiShkafGateGUID;
    uint64 m_uiMalGate1GUID;
    uint64 m_uiMalGate2GUID;
    uint64 m_uiMalChestGUID;

    void Initialize()
    {
		ArthasGUID = 0;
		lastspawn = 0;
        m_uiShkafGateGUID = 0;
        m_uiMalGate1GUID = 0;
        m_uiMalGate2GUID = 0;
        m_uiMalChestGUID = 0;
		corrupteur_guid = 0;
		m_auiEncounter[2] = m_auiEncounter[1] = NOT_STARTED;
		nbvague = 0;
		HeroicMap = ActiveTimer = false;
		DoUpdateWorldState(WS_VAGUE,0);
		Instance_Timer = 60000;
		StartVague_Timer = 10000;
		min_Timer = 30;
    }

    void OnObjectCreate(GameObject* pGo)
    {
        if (pGo->GetEntry() == GO_SHKAF_GATE)
            m_uiShkafGateGUID = pGo->GetGUID();

        if (pGo->GetEntry() == GO_MALGANIS_GATE1)
            m_uiMalGate1GUID = pGo->GetGUID();

        if (pGo->GetEntry() == GO_MALGANIS_GATE2)
            m_uiMalGate2GUID = pGo->GetGUID();

        if (pGo->GetEntry() == GO_MALGANIS_CHEST)
            m_uiMalChestGUID = pGo->GetGUID();
    }

	void OnPlayerEnter(Player* plr)
	{
		if(plr->GetDungeonDifficulty() == DUNGEON_DIFFICULTY_HEROIC)
			HeroicMap = true;
	}
	void OnCreatureCreate(Creature* pCr)
	{
		switch(pCr->GetEntry())
		{
			case 26499:
				ArthasGUID = pCr->GetGUID();
				break;
			default:
				break;
		}
	}

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_ARTHAS_EVENT:
                m_auiEncounter[0] = uiData;
                break;
			case TYPE_VAGUE_EVENT:
                m_auiEncounter[1] = uiData;
                break;

        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_ARTHAS_EVENT:
                return m_auiEncounter[0];
			case TYPE_VAGUE_EVENT:
				return m_auiEncounter[1];

        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_GO_SHKAF_GATE: return m_uiShkafGateGUID;
            case DATA_GO_MAL_GATE1: return m_uiMalGate1GUID;
            case DATA_GO_MAL_GATE2: return m_uiMalGate2GUID;
            case DATA_GO_MAL_CHEST: return m_uiMalChestGUID;
        }

        return 0;
    }

	void Update(uint32 diff)
	{

		if(GetData(TYPE_VAGUE_EVENT) == IN_PROGRESS)
		{
			if(!ActiveTimer && HeroicMap)
			{
				ActiveTimer = true;
				DoUpdateWorldState(WS_TIMER,min_Timer);
				if(Creature* Arthas = GetCreatureInMap(ArthasGUID))
					corrupteur_guid = Arthas->SummonCreature(BOSS_CORRUPTEUR,2328.122f,1273.243f,133.1f,2.09f,TEMPSUMMON_TIMED_DESPAWN,1800000)->GetGUID();
			}

			if(StartVague_Timer <= diff)
			{
				if(nbvague < 10)
				{
					nbvague++;
					uint8 tmpspawn = urand(0,4);
					while(tmpspawn == lastspawn)
						tmpspawn = urand(0,4);

					lastspawn = tmpspawn;
					DoUpdateWorldState(WS_VAGUE,nbvague);
					//event de pop des adds
					if(nbvague == 5)
					{
						if(Creature* Arthas = GetCreatureInMap(ArthasGUID))
						{
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO,randspawn[lastspawn].x+3.0f,randspawn[lastspawn].y+3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO2,randspawn[lastspawn].x+3.0f,randspawn[lastspawn].y-3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO,randspawn[lastspawn].x-3.0f,randspawn[lastspawn].y+3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO2,randspawn[lastspawn].x-3.0f,randspawn[lastspawn].y-3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(BOSS_SALRAMM,randspawn[lastspawn].x,randspawn[lastspawn].y,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
						}
					}
					else if(nbvague == 10)
					{
						if(Creature* Arthas = GetCreatureInMap(ArthasGUID))
						{
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO,randspawn[lastspawn].x+3.0f,randspawn[lastspawn].y+3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO2,randspawn[lastspawn].x+3.0f,randspawn[lastspawn].y-3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO,randspawn[lastspawn].x-3.0f,randspawn[lastspawn].y+3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_NECRO2,randspawn[lastspawn].x-3.0f,randspawn[lastspawn].y-3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(BOSS_MEATHOOK,randspawn[lastspawn].x,randspawn[lastspawn].y,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
						}
					}
					else
					{
						if(Creature* Arthas = GetCreatureInMap(ArthasGUID))
						{
							if(Creature* add = Arthas->SummonCreature(NPC_ABO,randspawn[lastspawn].x+3.0f,randspawn[lastspawn].y+3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_ABO2,randspawn[lastspawn].x-3.0f,randspawn[lastspawn].y+3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_GOULE,randspawn[lastspawn].x+3.0f,randspawn[lastspawn].y-3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(Creature* add = Arthas->SummonCreature(NPC_GOULE2,randspawn[lastspawn].x-3.0f,randspawn[lastspawn].y-3.0f,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
								add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(urand(0,1))
							{
								if(Creature* add = Arthas->SummonCreature(NPC_NECRO,randspawn[lastspawn].x+3.0f*2,randspawn[lastspawn].y+3.0f*2,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
									add->SetRespawnDelay(RESPAWN_ONE_DAY);
							}
							else
								if(Creature* add = Arthas->SummonCreature(NPC_DEMON_CRYPTES,randspawn[lastspawn].x+3.0f*2,randspawn[lastspawn].y+3.0f*2,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
									add->SetRespawnDelay(RESPAWN_ONE_DAY);
							if(urand(0,1))
							{
								if(Creature* add = Arthas->SummonCreature(NPC_NECRO2,randspawn[lastspawn].x+3.0f*2,randspawn[lastspawn].y-3.0f*2,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
									add->SetRespawnDelay(RESPAWN_ONE_DAY);
							}
							else
								if(Creature* add = Arthas->SummonCreature(NPC_GOULE,randspawn[lastspawn].x+3.0f*2,randspawn[lastspawn].y-3.0f*2,randspawn[lastspawn].z,0.0f,TEMPSUMMON_TIMED_DESPAWN,600000))
									add->SetRespawnDelay(RESPAWN_ONE_DAY);
						}
					}
				}
				else
				{
					m_auiEncounter[1] = DONE;
					SetData(TYPE_ARTHAS_EVENT, IN_PROGRESS);
					if(Creature* Arthas = GetCreatureInMap(ArthasGUID))
						Arthas->setFaction(1108);
				}

				StartVague_Timer = 80000;
			}
			else
				StartVague_Timer -= diff;
		}

		if(ActiveTimer)
		{
			if(Instance_Timer <= diff)
			{
				if(min_Timer < 1)
					if(Creature* corr = GetCreatureInMap(corrupteur_guid))
					{
						if(corr->isAlive())
							corr->ForcedDespawn();
					}

				min_Timer--;
				DoUpdateWorldState(WS_TIMER,min_Timer);
				Instance_Timer = (min_Timer == 0) ? DAY*MINUTE : 60000;
			}
			else
				Instance_Timer -= diff;
		}
	}
};

InstanceData* GetInstanceData_instance_culling_of_stratholme(Map* pMap)
{
    return new instance_culling_of_stratholme(pMap);
}

void AddSC_instance_culling_of_stratholme()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_culling_of_stratholme";
    newscript->GetInstanceData = &GetInstanceData_instance_culling_of_stratholme;
    newscript->RegisterSelf();
}
