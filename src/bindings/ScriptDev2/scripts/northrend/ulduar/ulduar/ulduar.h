/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ULDUAR_H
#define DEF_ULDUAR_H

enum
{
    MAX_ENCOUNTER               = 56,

    TYPE_LEVIATHAN              = 0,
    TYPE_IGNIS                  = 1,
    TYPE_RAZORSCALE             = 2,
    TYPE_XT002                  = 3,
    TYPE_ASSEMBLY               = 4,
    TYPE_KOLOGARN               = 5,
    TYPE_AURIAYA                = 6,
    TYPE_MIMIRON                = 7,
    TYPE_HODIR                  = 8,
    TYPE_THORIM                 = 9,
    TYPE_FREYA                  = 10,
    TYPE_VEZAX                  = 11,
    TYPE_YOGGSARON              = 12,
    TYPE_ALGALON                = 13,

	DATA_LEVIATHAN              = 14,
    DATA_IGNIS                  = 15,
    DATA_RAZORSCALE             = 16,
    DATA_XT002                  = 17,
    DATA_KOLOGARN               = 18,
    DATA_AURIAYA                = 19,
	DATA_STEELBREAKER           = 20,
    DATA_MOLGEIM                = 21,
    DATA_BRUNDIR                = 22,
    DATA_MIMIRON                = 23,
    DATA_HODIR                  = 24,
    DATA_THORIM                 = 25,
    DATA_FREYA                  = 26,
    DATA_VEZAX                  = 27,
    DATA_YOGGSARON              = 28,
    DATA_ALGALON                = 29,
	DATA_RIGHT_ARM              = 30,
	DATA_LEFT_ARM               = 31,
	DATA_SENTRY_1				= 32,
	DATA_SENTRY_2				= 33,
	DATA_SENTRY_3				= 34,
	DATA_SENTRY_4				= 35,
	DATA_FERAL_DEFENDER			= 36,
	DATA_YOGG_SARA				= 37,
	DATA_LEVIMKII				= 38,
	DATA_VX001					= 39,
	DATA_MIMIRONHEAD			= 40,

	DATA_IGNIS_ADDS				= 50,
	DATA_THORIM_BIGADD			= 51,
	DATA_THORIM_DOOR			= 52,
	DATA_THORIM_ORB				= 53,
	DATA_THORIM_ADDS			= 54,
	DATA_IGNIS_FIRE				= 55,
	DATA_YOGG_NUAGE				= 56,
	DATA_HODIR_FREEZE			= 57,

    NPC_LEVIATHAN               = 33113,
    NPC_IGNIS                   = 33118,
    NPC_RAZORSCALE              = 33186,
    NPC_XT002                   = 33293,
    NPC_STEELBREAKER            = 32867,
    NPC_MOLGEIM                 = 32927,
    NPC_BRUNDIR                 = 32857,
    NPC_KOLOGARN                = 32930,
	NPC_RIGHT_ARM               = 32934,
	NPC_LEFT_ARM                = 32933,
    NPC_AURIAYA                 = 33515,
	NPC_SANCTUM_SENTRY			= 34014,
	NPC_FERAL_DEFENDER			= 34035,
    NPC_MIMIRON                 = 33350,
    NPC_HODIR                   = 32845,
    NPC_THORIM                  = 32865,
    NPC_FREYA                   = 32906,
    NPC_VEZAX                   = 33271,
    NPC_YOGGSARON               = 33288,
    NPC_ALGALON                 = 32871,

	NPC_IRON_ASSEMBLAGE			= 33121,

	GO_KOLOGARN_BRIDGE			= 194232,
	GO_KOLOGARN_LOOT			= 195046,
	GO_KOLOGARN_LOOT_H			= 195047
};

struct MANGOS_DLL_DECL instance_ulduar;

struct MANGOS_DLL_DECL boss_steelbreakerAI: public ScriptedAI
{
	boss_steelbreakerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
		Reset();
    }

	ScriptedInstance* pInstance;
    uint32 phase;
	uint32 Disruption_Timer;
	bool m_bIsHeroic;

	MobEventTasks MobsTasks;
	void UpdateAI(const uint32 diff);
	void KilledUnit(Unit *who);
	void JustDied(Unit* Killer);
	void DamageTaken(Unit* pKiller, uint32 &damage);
	void UpdatePhase();
	void EnterCombat(Unit *who);
	void Reset();
};
struct MANGOS_DLL_DECL boss_runemaster_molgeimAI: public ScriptedAI
{
	boss_runemaster_molgeimAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
		Reset();
    }

	bool m_bIsHeroic;
    ScriptedInstance* pInstance;
	MobEventTasks MobsTasks;

    uint32 phase;
	uint32 Shield_Timer;
	uint32 RuneD_Timer;
	uint32 RuneS_Timer;

	void UpdateAI(const uint32 diff);
	void JustDied(Unit* Killer);
	void DamageTaken(Unit* pKiller, uint32 &damage);
	void UpdatePhase();
	void EnterCombat(Unit *who);
	void Reset();
};
struct MANGOS_DLL_DECL boss_stormcaller_brundirAI: public ScriptedAI
{
	boss_stormcaller_brundirAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		m_bIsHeroic = c->GetMap()->GetDifficulty();
		Reset();
    }

	MobEventTasks MobsTasks;

	bool m_bIsHeroic;
    ScriptedInstance* pInstance;
    uint32 phase;

	uint32 LightW_Timer;
	uint32 LightT_Timer;

	void UpdateAI(const uint32 diff);
	void JustDied(Unit* Killer);
	void DamageTaken(Unit* pKiller, uint32 &damage);
	void UpdatePhase();
	void EnterCombat(Unit *who);
	void Reset();
};

struct MANGOS_DLL_DECL instance_ulduar : public ScriptedInstance
{
    instance_ulduar(Map* pMap) : ScriptedInstance(pMap)
	{ 
		thisMap = pMap;
		Initialize(); 
	}

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string m_strInstData;

	Map* thisMap;

    uint64 m_uiLeviathanGUID;
    uint64 m_uiIgnisGUID;
    uint64 m_uiRazorscaleGUID;
    uint64 m_uiXT002GUID;
    uint64 m_auiAssemblyGUIDs[3];
    uint64 m_uiKologarnGUID;
    uint64 m_uiAuriayaGUID;
    uint64 m_uiMimironGUID;
    uint64 m_uiHodirGUID;
    uint64 m_uiThorimGUID;
    uint64 m_uiFreyaGUID;
    uint64 m_uiVezaxGUID;
    uint64 m_uiYoggSaronGUID;
    uint64 m_uiAlgalonGUID;
	uint64 m_uiRightArmGUID;
	uint64 m_uiLeftArmGUID;
	uint64 m_uiKologarnLootGUID;
	uint64 m_uiKologarnBridgeGUID;
	uint64 m_uiThorimBigAddGUID;
	uint64 m_uiThorimDoor;
	uint64 m_uiThorimLootGUID;
	uint64 m_uiIgnisFireGUID;
	uint64 m_uiYoggSaronSaraGUID;
	uint64 m_uiLeviMKIIGUID;
	uint64 m_uiVX001GUID;
	uint64 m_uiMimironHeadGUID;

	std::vector<Creature*> IgnisIronAdds;
	std::vector<Creature*> ThorimThunderOrbs;

	std::vector<Creature*> ThorimAdds;

	std::vector<Creature*> KologarnTrashs;
	std::vector<Creature*> HodirTrashs;
	std::vector<Creature*> HodirAdds;
	std::vector<Creature*> FreyaTrashs;
	std::vector<Creature*> MimironTrashs;
	std::vector<Creature*> YoggNuage;
	std::vector<Creature*> YoggAdds;

    void Initialize();    

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        }

        return false;
    }

    void OnCreatureCreate(Creature* pCreature);
    
    void OnObjectCreate(GameObject *pGo)
    {
		switch(pGo->GetEntry())
        {
			case GO_KOLOGARN_BRIDGE:
				m_uiKologarnBridgeGUID = pGo->GetGUID();
				pGo->SetGoState(GO_STATE_ACTIVE);
				break;
			case GO_KOLOGARN_LOOT:
			case GO_KOLOGARN_LOOT_H:
				m_uiKologarnLootGUID = pGo->GetGUID();
				break;
			case 8151: // Thorim Door
				m_uiThorimDoor = pGo->GetGUID();
				pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
				break;
			case 194312:
			case 194313:
				m_uiThorimLootGUID = pGo->GetGUID();
				break;
		}
    }


    void SetData(uint32 uiType, uint32 uiData);    

	void OnPlayerEnter(Player* plr)
	{
		if(plr->GetTeam() == ALLIANCE)
		{
			for(std::vector<Creature*>::const_iterator itr = HodirAdds.begin(); itr != HodirAdds.end(); ++itr)
				if((*itr)->getFaction() == 1802)
					((Unit*)(*itr))->SetPhaseMask(1,true);
				else
					(*itr)->ForcedDespawn(1000);
		}
		else
		{
			for(std::vector<Creature*>::const_iterator itr = HodirAdds.begin(); itr != HodirAdds.end(); ++itr)
				if((*itr)->getFaction() == 1801)
					((Unit*)(*itr))->SetPhaseMask(1,true);
				else
					(*itr)->ForcedDespawn(1000);
		}
	}

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case TYPE_LEVIATHAN:
                return m_uiLeviathanGUID;
            case TYPE_IGNIS:
                return m_uiIgnisGUID;
            case TYPE_RAZORSCALE:
                return m_uiRazorscaleGUID;
            case TYPE_XT002:
                return m_uiXT002GUID;
            case TYPE_KOLOGARN:
                return m_uiKologarnGUID;
			case DATA_LEFT_ARM:
                return m_uiLeftArmGUID;
			case DATA_RIGHT_ARM:
                return m_uiRightArmGUID;
            case TYPE_AURIAYA:
                return m_uiAuriayaGUID;
            case TYPE_MIMIRON:
                return m_uiMimironGUID;
            case TYPE_HODIR:
                return m_uiHodirGUID;
            case TYPE_THORIM:
                return m_uiThorimGUID;
            case TYPE_FREYA:
                return m_uiFreyaGUID;
            case TYPE_VEZAX:
                return m_uiVezaxGUID;
            case TYPE_YOGGSARON:
                return m_uiYoggSaronGUID;
            case TYPE_ALGALON:
                return m_uiAlgalonGUID;

            // Assembly of Iron
            case DATA_STEELBREAKER:
                return m_auiAssemblyGUIDs[0];
            case DATA_MOLGEIM:
                return m_auiAssemblyGUIDs[1];
            case DATA_BRUNDIR:
                return m_auiAssemblyGUIDs[2];
			case DATA_IGNIS_ADDS:
				return GetIgnisRandomAdd();
			case DATA_THORIM_BIGADD:
				return m_uiThorimBigAddGUID;
			case DATA_THORIM_DOOR:
				return m_uiThorimDoor;
			case DATA_IGNIS_FIRE:
				return m_uiIgnisFireGUID;
			case DATA_YOGG_SARA:
				return m_uiYoggSaronSaraGUID;
			case DATA_LEVIMKII:
				return m_uiLeviMKIIGUID;
			case DATA_VX001:
				return m_uiVX001GUID;
			case DATA_MIMIRONHEAD:
				return m_uiMimironHeadGUID;
        }

        return 0;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_LEVIATHAN:
            case TYPE_IGNIS:
            case TYPE_RAZORSCALE:
            case TYPE_XT002:
            case TYPE_ASSEMBLY:
            case TYPE_KOLOGARN:
            case TYPE_AURIAYA:
            case TYPE_MIMIRON:
            case TYPE_HODIR:
            case TYPE_THORIM:
            case TYPE_FREYA:
            case TYPE_VEZAX:
            case TYPE_YOGGSARON:
            case TYPE_ALGALON:
                return m_auiEncounter[uiType];
        }

        return 0;
    }

	void SetData64(uint32 uiData, uint64 d)
	{
		switch(uiData)
		{
			case DATA_IGNIS_FIRE:
				m_uiIgnisFireGUID = d;
				break;
		}
	}

	uint64 GetIgnisRandomAdd()
	{
		bool find_good = false;
		uint16 rand_add;
		uint16 force_out = 0;
		while(!find_good && force_out < 30)
		{
			rand_add = rand() % 20;
			uint16 nb_add = 0;
			for (std::vector<Creature*>::iterator itr = IgnisIronAdds.begin(); itr != IgnisIronAdds.end();++itr)
			{
				Creature *tmpAdd = *itr;
				if(nb_add == rand_add && tmpAdd && tmpAdd->isAlive())
					return tmpAdd->GetGUID();

				nb_add++;
			}
			force_out++;
		}
		return 0;
	}

	void FreeHodirAdd(float x, float y)
	{
		for(std::vector<Creature*>::const_iterator itr = HodirAdds.begin(); itr != HodirAdds.end(); ++itr)
		{
			if((*itr)->isAlive())
				if((*itr)->GetDistance2d(x,y) < 2.0f)
				{
					(*itr)->RemoveAurasDueToSpell(62297);
					return;
				}
		}
	}

	void FreezeAllHodirAdds()
	{
		for(std::vector<Creature*>::const_iterator itr = HodirAdds.begin(); itr != HodirAdds.end(); ++itr)
			if((*itr)->isAlive())
			{
				(*itr)->SetAuraStack(62297,(*itr),1);
				if(Unit* Hodir = Unit::GetUnit((*(*itr)), GetData64(TYPE_HODIR)))
					if(Hodir->isAlive())
						((Creature*)Hodir)->SummonCreature(33212,(*itr)->GetPositionX(),(*itr)->GetPositionY(),(*itr)->GetPositionZ(),0.0f,TEMPSUMMON_TIMED_DESPAWN,600000);
			}
	}

	const char* Save(){ return m_strInstData.c_str(); }

    void Load(const char* strIn)
    {
        if (!strIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(strIn);

        std::istringstream loadStream(strIn);

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            loadStream >> m_auiEncounter[i];

            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};
#endif
