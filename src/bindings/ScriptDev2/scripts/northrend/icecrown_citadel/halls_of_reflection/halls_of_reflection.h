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
	DATA_FROSTMOURNE		=	22,

	DATA_RANDOM_WARRIOR		=	30,
	DATA_RANDOM_MAGE		=	31,
	DATA_RANDOM_PRIEST		=	32,
	DATA_RANDOM_HUNT		=	33,
	DATA_RANDOM_ROGUE		=	34,

	DATA_FACTION			=	40,

	WS_MAIN					=	4884,
	WS_VAGUE				=	4882,
};

enum MiscConst
{
	MAX_TRASHSTEP			=	12
};

enum EscapeSpells
{
	// lich king
	SPELL_WINTER			=	69780,
	SPELL_PAIN_SUFFERING	=	74115,
	SPELL_FROSTMOURNE		=	70063,
	SPELL_ICEWALL			=	69768,
	// jaina
	SPELL_BARRER_CHANNEL	=	76221,
	SPELL_ICE_BARRER		=	69787,
	SPELL_ICEBLOCK			=	69708,
	// sylvanas
	SPELL_DARK_ARROW		=	70194,
};

enum EscapeNpcs
{
	NPC_GHOUL				=	36940,
	NPC_WITCHER				=	36941,
	NPC_ABOMINATION			=	37069,
};

const static float LichKingEscapePos[9][3] = {
	{5552.6f,2262.5f,733.1f},		// Initial pos
	{5614.3f,2194.1f,731.99f},		// First step, cast Barrer
	{5566.78f,2130.584f,730.96f},
	{5515.52f,2017.15f,732.969f},
	{5472.79f,1935.86f,742.21f},
	{5416.67f,1847.37f,756.22f},
	{5371.23f,1803.82f,760.63f},
	{5292.07f,1719.195f,780.1f},
	{5279.51f,1699.67f,785.28f}
};

const static float fLeadEscapePos[7][3] = {
	{5547.27f,2259.95f,733.1f},		// initial pos
	{5609.66f,2200.48f,731.62f},		
	{5566.12f,2129.15f,730.9f},		// barrer 1
	{5514.83f,2002.97f,734.44f},	// barrer 2
	{5437.53f,1882.67f,751.3f},		// barrer 3
	{5350.93f,1777.66f,765.7f},		// barrer 4
	{5276.12f,1696.1f,786.2f},		// end of the event
};

// Shared IA
struct MANGOS_DLL_DECL HoR_LichKing_EscapeAI : public LibDevFSAI
{
    HoR_LichKing_EscapeAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		me->SetReactState(REACT_PASSIVE);
    }

    void Reset()
    {
		if(pInstance->GetData(DATA_FACTION) == ALLIANCE)
			SetAuraStack(SPELL_ICEBLOCK,1,me,me,1);
		else
			SetAuraStack(SPELL_DARK_ARROW,1,me,me,1);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
		me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct MANGOS_DLL_DECL HoR_fLead_frostmourneAI : public LibDevFSAI
{
    HoR_fLead_frostmourneAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		me->SetReactState(REACT_PASSIVE);
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

#endif