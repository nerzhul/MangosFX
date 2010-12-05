#include "precompiled.h"
#include "pit_of_saron.h"

struct MANGOS_DLL_DECL instance_pit_of_saron : public InstanceData
{
    instance_pit_of_saron(Map* pMap) : InstanceData(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
	std::string strInstData;

	uint64 krick_Guid;
	uint64 ick_Guid;
	uint64 rimefang_Guid;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
		ick_Guid = 0;
		krick_Guid = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
			case 36477:
				krick_Guid = pCreature->GetGUID();
				break;
			case 36476:
				ick_Guid = pCreature->GetGUID();
				break;
			case 36661:
				rimefang_Guid = pCreature->GetGUID();
				break;
			default:
				break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
		{
			case 0:
			default:
				break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
			case DATA_KRICK:
				return krick_Guid;
			case DATA_ICK:
				return ick_Guid;
			case DATA_RIMEFANG:
				return rimefang_Guid;
        }
        return 0;
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

};

InstanceData* GetInstanceData_instance_pit_of_saron(Map* pMap)
{
    return new instance_pit_of_saron(pMap);
}

void AddSC_instance_pit_of_saron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_pit_of_saron";
    newscript->GetInstanceData = &GetInstanceData_instance_pit_of_saron;
    newscript->RegisterSelf();
}
