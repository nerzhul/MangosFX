#include "precompiled.h"
#include "forge_of_souls.h"

#define MAX_ENCOUNTER 2

/* Forge of Souls encounters:
0- Bronjahm, The Godfather of Souls
1- The Devourer of Souls
*/

struct instance_forge_of_souls : public InstanceData
{
    instance_forge_of_souls(Map* pMap) : InstanceData(pMap) {};

    uint64 uiBronjahm;
    uint64 uiDevourer;

    uint32 uiEncounter[MAX_ENCOUNTER];
    uint32 uiTeamInInstance;

    void Initialize()
    {
        uiBronjahm = 0;
        uiDevourer = 0;

        uiTeamInInstance = 0;

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            uiEncounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (uiEncounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        Map::PlayerList const &players = instance->GetPlayers();

        if (!players.isEmpty())
            if (Player* pPlayer = players.begin()->getSource())
                uiTeamInInstance = pPlayer->GetTeam();

        switch(pCreature->GetEntry())
        {
            case CREATURE_BRONJAHM:
                uiBronjahm = pCreature->GetGUID();
                break;
            case CREATURE_DEVOURER:
                uiDevourer = pCreature->GetGUID();
                break;
        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_BRONJAHM_EVENT:
                uiEncounter[0] = data;
                break;
            case DATA_DEVOURER_EVENT:
                uiEncounter[1] = data;
                break;
        }

        if (data == DONE)
            SaveToDB();
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case DATA_BRONJAHM_EVENT:    return uiEncounter[0];
            case DATA_DEVOURER_EVENT:    return uiEncounter[1];
            case DATA_TEAM_IN_INSTANCE:  return uiTeamInInstance;
        }

        return 0;
    }

    uint64 GetData64(uint32 identifier)
    {
        switch(identifier)
        {
            case DATA_BRONJAHM:         return uiBronjahm;
            case DATA_DEVOURER:         return uiBronjahm;
        }

        return 0;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << " " << uiEncounter[0] << " " << uiEncounter[1];

        OUT_SAVE_INST_DATA_COMPLETE;
        return saveStream.str();
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
        loadStream >> uiEncounter[0] >> uiEncounter[1];

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (uiEncounter[i] == IN_PROGRESS)
                uiEncounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_forge_of_souls(Map* pMap)
{
    return new instance_forge_of_souls(pMap);
}

void AddSC_instance_forge_of_souls()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_forge_of_souls";
    newscript->GetInstanceData = &GetInstanceData_instance_forge_of_souls;
    newscript->RegisterSelf();
}
