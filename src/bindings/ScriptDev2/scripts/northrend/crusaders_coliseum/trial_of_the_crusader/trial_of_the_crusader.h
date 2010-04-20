#include "precompiled.h"

#define MAX_ENCOUNTER	5
enum Boss
{
	TYPE_Fjola_Lightbane		= 20,
	TYPE_Eydis_Darkbane			= 21,
	TYPE_CHAMPIONWAR			= 32,
	TYPE_CHAMPIONDRUID			= 33,
	TYPE_CHAMPIONHUNT			= 34,
	TYPE_CHAMPIONROGUE			= 35,
	TYPE_CHAMPIONWARLOCK		= 36,
	TYPE_CHAMPIONSHAMAN			= 37,
	TYPE_CHAMPIONPALADIN		= 38,
	TYPE_CHAMPIONPRIEST			= 39,
	TYPE_CHAMPIONDEATHKNIGHT	= 40,
	TYPE_CHAMPIONMAGE			= 41,
	TYPE_Icehowl				= 12,
	TYPE_Acidmaw				= 13,
	TYPE_Dreadscale				= 14,
	TYPE_GormoktheImpaler		= 15,
	TYPE_FLOOR					= 16,
	TYPE_DOOR					= 17,
	TYPE_RUNE					= 18,

	DATA_FORDRING				= 30,
	DATA_VARIAN					= 31,
	DATA_GARROSH				= 32,
	DATA_ANNOUNCER				= 33,
	TYPE_TRY					= 5,

	TYPE_EVENT_BEAST			= 0,
	TYPE_JARAXXUS				= 1,
	TYPE_CHAMPIONS				= 2,
	TYPE_VALKYRS				= 3,
	TYPE_ANUBARAK				= 4
};

static const uint32 Champion_Entries[14][2] = {
	{34470,34444}, // chaman heal
	{34469,34459}, // drood heal
	{34465,34445}, // pala heal
	{34466,34447}, // priest disci

	{34461,34458}, // DK
	{34471,34456}, // paladps
	{34472,34454}, // rogue
	{34475,34453},  // war

	{34460,34451}, // drood dps
	{34467,34448}, // hunt
	{34468,34449}, // mage
	{34473,34441}, // priest shadow
	{34463,34455}, // chaman elem
	{34474,34450}, // démo
};

static const float Champion_spawn[8][2][3] = {
	{{525.852f,123.875f,394.7f},{600.461f,156.371f,394.7f}},
	{{524.298f,131.855f,394.7f},{598.346f,154.355f,394.7f}},
	{{522.946f,141.160f,394.7f},{603.984f,150.482f,394.7f}},
	{{523.112f,148.032f,394.7f},{601.088f,141.702f,394.7f}},
	{{525.885f,134.643f,394.7f},{606.144f,136.472f,394.7f}},
	{{526.142f,142.087f,394.7f},{601.957f,134.034f,394.7f}},
	{{527.008f,130.824f,394.7f},{604.477f,129.186f,394.7f}},
	{{527.314f,139.130f,394.7f},{598.834f,127.735f,394.7f}}
};

static const float ValkyrPortal_spawn[4][3] = {
	{540.641f,	116.780f,	394.4f},
	{540.976f,	162.352f,	394.7f},
	{585.720f,	118.065f,	394.4f},
	{586.186f,	161.965f,	394.7f}
};

enum
{
	EVENT_TYPE_BEASTS_INTRO		= 1,
	EVENT_TYPE_BEASTS_JORM		= 2,
	EVENT_TYPE_BEASTS_YETI		= 3,
	EVENT_TYPE_JARAXXUS			= 4,
	EVENT_TYPE_JARAXXUS_OUTRO	= 5,
	EVENT_TYPE_CHAMPIONS		= 6,
	EVENT_TYPE_VALKYR			= 7,
	EVENT_TYPE_VALKYR_OUTRO		= 8,
	EVENT_TYPE_ANUB				= 9
};

struct MANGOS_DLL_DECL npc_toc10_announcerAI : public ScriptedAI
{
    npc_toc10_announcerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
		Difficult = c->GetMap()->GetDifficulty();
		Reset();
    }
    
    ScriptedInstance* pInstance;
	bool EventStarted;
	bool HeroicMode;
	uint32 Spawn_Timer;
	uint8 Try_Heroic;
	MobEventTasks Tasks;
	Difficulty Difficult;
	uint32 Event_Timer;
	uint8 Event_Type;
	uint8 Event_phase;
	uint32 check_Timer;
	Creature* Fordring, *Varian,*Garrosh,*Flopboum,*LichKing;
	uint32 team;
	uint32 ID[8];
	std::vector<uint32> Champions_Guid;

    void Reset();
    
	void StartEvent(Unit* pwho,uint32 type);

	void SpawnChampions();
	
    void UpdateAI(const uint32 diff);
    
	void UpdateWStates()
	{
		if(!HeroicMode)
			return;

		pInstance->DoUpdateWorldState(4390,1);
		Try_Heroic--;
		pInstance->DoUpdateWorldState(4389,Try_Heroic);
		pInstance->SetData(TYPE_TRY,Try_Heroic);				
	};
};
