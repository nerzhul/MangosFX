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

const static float LichKingEscapePos[7][3] = {
	{5552.6f,2262.5f,733.1f},		// Initial pos
	{5607.3f,2203.1f,731.4f},		// First step, cast Barrer
	{5553.2,2102.653f,731.1f},		
	{5510.3f,1995.0f,735.3f},
	{5446.7f,1895.363f,748.8f},
	{5391.7f,1821.447f,758.8f},	
	{5282.1f,1705.4f,784.1f}		// Last Step, killed
};

const static float fLeadEscapePos[7][3] = {
	{5547.27f,2259.95f,733.1f},		// initial pos
	{5609.66f,2200.48f,731.62f},		
	{5562.22f,2120.75f,730.8f},		// barrer 1
	{5512.29f,2008.16f,733.94f},	// barrer 2
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
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

#endif