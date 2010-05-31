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
	{0.0f,0.0f,0.0f},		// initial pos
	{0.0f,0.0f,0.0f},		
	{0.0f,0.0f,0.0f},		// barrer 1
	{0.0f,0.0f,0.0f},		// barrer 2
	{0.0f,0.0f,0.0f},		// barrer 3
	{0.0f,0.0f,0.0f},		// barrer 4
	{0.0f,0.0f,0.0f},		// end of the event
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