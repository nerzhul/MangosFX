#include "precompiled.h"

enum Spells
{
	// Dreamwalker
	SPELL_NIGHTMARE_PORTAL		=	72482,
	SPELL_EMERALD_VIGOR			=	70873,
	SPELL_DREAMWALKER_RAGE		=	71189,

	// adds
	// Archmages
	SPELL_FROSTBOLT_VOLLEY		=	70759,
	SPELL_MANA_VOID				=	71179,
	NPC_MANA_VOID				=	38068,
	SPELL_COLUMN_OF_FROST		=	70702,
	// Blazing Skeletons
	SPELL_LAY_WASTE				=	69325,
	SPELL_FIREBALL				=	70754,
	// Suppressors
	SPELL_SUPPRESSION			=	70588,
	// Blistering Zombies
	SPELL_CORROSION				=	70751,
	SPELL_ACID_BURST			=	70744,
	// Gluttonous Abominations
	SPELL_GUT_SPRAY				=	71283,
	// Rot
	NPC_ROT_WORM				=	37907,
	SPELL_FLESH_ROT				=	72963,
	
};

const static float SpawnLoc[6][3]=
{
    {4203.470215f, 2484.500000f, 364.872009f},  // 0 Valithria
    {4240.688477f, 2405.794678f, 364.868591f},  // 1 Valithria Room 1
    {4165.112305f, 2405.872559f, 364.872925f},  // 2 Valithria Room 2
    {4166.216797f, 2564.197266f, 364.873047f},  // 3 Valithria Room 3
    {4239.579102f, 2566.753418f, 364.868439f},  // 4 Valithria Room 4
};

void AddSC_ICC_DreamWalker()
{
}
