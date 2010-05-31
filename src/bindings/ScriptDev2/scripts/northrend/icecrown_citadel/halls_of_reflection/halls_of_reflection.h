#ifndef __INSTANCE_HOR_H
#define __INSTANCE_HOR_H

enum Types
{
	TYPE_FALRIC				=	0,
	TYPE_MARWYN				=	1,
	TYPE_LICHKING			=	2,
	TYPE_FACTIONLEADER_EV1	=	3,
	TYPE_FACTIONLEADER_EV2	=	4,
	TYPE_LICHKING_EVENT		=	5,

	TYPE_EVENT_FROSTMOURNE	=	10,
	TYPE_EVENT_ESCAPE		=	11,

	DATA_DOOR_MAIN			=	20,
	DATA_DOOR_LICHKING		=	21,

	DATA_RANDOM_WARRIOR		=	30,
	DATA_RANDOM_MAGE		=	31,
	DATA_RANDOM_PRIEST		=	32,
	DATA_RANDOM_HUNT		=	33,
	DATA_RANDOM_ROGUE		=	34,

	DATA_FACTION			=	40,

	WS_MAIN					=	4884,
	WS_VAGUE				=	4882,

	GOBJ_ICEWALL_LARGE		=	201910,
	GOBJ_ICEWALL_MEDIUM		=	201911,
};

enum EscapeSpells
{
	// jaina
	SPELL_BARRER_CHANNEL	=	76221,
	SPELL_ICE_BARRER		=	69787,
	SPELL_ICEBLOCK			=	69708,
	// sylvanas
	SPELL_DARK_ARROW		=	70194,
};

const static float LichKingEscapePos[7][3] = {
	{5552.6f,2262.5f,733.1f},		// Initial pos
	{5607.3f,2203.1f,731.4f},		// First step, cast Barrer
	{0.0f,0.0f,0.0f},		
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f},	
	{0.0f,0.0f,0.0f}		// Last Step, killed
};

// Shared IA
struct MANGOS_DLL_DECL HoR_LichKing_EscapeAI : public LibDevFSAI
{
    HoR_LichKing_EscapeAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		if(pInstance->GetData(DATA_FACTION) == ALLIANCE)
			DoCastMe(SPELL_ICEBLOCK);
		else
			DoCastMe(SPELL_DARK_ARROW);
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

#endif