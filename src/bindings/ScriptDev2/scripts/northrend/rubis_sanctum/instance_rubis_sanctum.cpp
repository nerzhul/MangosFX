#include "precompiled.h"
#include "rubis_sanctum.h"

struct MANGOS_DLL_DECL instance_rubis_sanctum : public ScriptedInstance
{
    instance_rubis_sanctum(Map* pMap) : ScriptedInstance(pMap) {Initialize();}

    std::string strInstData;
    uint32 m_auiEncounter[MAX_ENCOUNTER];

    uint64 m_uiBaltharusGUID;
	uint64 m_uiRagefireGUID;
    uint64 m_uiZarithrianGUID;
    uint64 m_uiHalionGUID;



    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiBaltharusGUID				= 0;
		m_uiRagefireGUID				= 0;
        m_uiZarithrianGUID				= 0;
        m_uiHalionGUID					= 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_ZARITHRIAN: 
				m_uiZarithrianGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
            case NPC_BALTHARUS: 
				m_uiBaltharusGUID = pCreature->GetGUID(); 
				AutoFreeze(pCreature);
				break;
            case NPC_RAGEFIRE: 
				m_uiRagefireGUID = pCreature->GetGUID(); 
				AutoFreeze(pCreature);
				break;
			case NPC_HALION:
				m_uiHalionGUID = pCreature->GetGUID();
				AutoFreeze(pCreature);
				break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        /*switch(pGo->GetEntry())
        {
           
        }*/
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
			case TYPE_RAGEFIRE:
			case TYPE_BALTHARUS:
			case TYPE_ZARITHRIAN:
			case TYPE_HALION:
				m_auiEncounter[uiType] = uiData;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[TYPE_BALTHARUS] << " " << m_auiEncounter[TYPE_RAGEFIRE] << " " << m_auiEncounter[TYPE_ZARITHRIAN] << " " << m_auiEncounter[TYPE_HALION];
            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[TYPE_BALTHARUS] >> m_auiEncounter[TYPE_RAGEFIRE] >> m_auiEncounter[TYPE_ZARITHRIAN] >> m_auiEncounter[TYPE_HALION];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_BALTHARUS:
			case TYPE_RAGEFIRE:
			case TYPE_ZARITHRIAN:
			case TYPE_HALION:
                return m_auiEncounter[uiType];

        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case TYPE_BALTHARUS:
                return m_uiBaltharusGUID;
			case TYPE_RAGEFIRE:
                return m_uiRagefireGUID;
			case TYPE_ZARITHRIAN:
                return m_uiZarithrianGUID;
			case TYPE_HALION:
                return m_uiHalionGUID;
        }
        return 0;
	}

	void Update(uint32 diff)
	{
	}
};

InstanceData* GetInstanceData_instance_rubis_sanctum(Map* pMap)
{
    return new instance_rubis_sanctum(pMap);
}

void AddSC_instance_rubis_sanctum()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_rubis_sanctum";
    pNewScript->GetInstanceData = &GetInstanceData_instance_rubis_sanctum;
    pNewScript->RegisterSelf();
}
