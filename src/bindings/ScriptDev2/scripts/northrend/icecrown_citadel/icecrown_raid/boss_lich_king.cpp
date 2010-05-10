#include "precompiled.h"

enum Spells
{
	//** phase 1,2,3
	SPELL_INFEST						=	70541,
	//** phase 1
	SPELL_SUMMON_SHAMBLING_HORROR		=	70372,
	SPELL_SUMMON_DRUDGE_GHOULS			=	70358,
	SPELL_NECROTIC_PLAGUE				=	70337,
	SPELL_PLAGUE_SIPHON					=	74074,
	// Heroic only
	SPELL_SUMMON_SHADOW_TRAP			=	73539,
	SPELL_PROC_SHADOW_TRAP				=	73529,
	// Shambling horror
	SPELL_SHOCKWAVE						=	72149,
	SPELL_ENRAGE						=	72143,
	
	//** phase 2,3
	SPELL_SOUL_REAPER					=	69409,
	SPELL_DEFILE						=	72754,
	
	//** phase 2
	SPELL_SUMMON_VALKYR					=	69037,
	NPC_VALKYR							=	36609,
	// Valkyr
	//-> Grab: Main target teleported to her
	SPELL_WINGS_OF_THE_DARMED			=	74352,
	// Heroic only for valkyr
	SPELL_LIFE_SIPHON					=	73783,
	
	//** phase 3
	SPELL_HARVEST_SOUL					=	68980,
	SPELL_VILE_SPIRITS					=	70498,
	// Vile Spirits
	SPELL_SPIRIT_BURST					=	70503,
	
	//** Frostmourne phase
	NPC_TERENAS_MENETHIL				=	36823,
	NPC_SPIRIT_WARDEN					=	36824,
	SPELL_HARVESTED_SOUL				=	74322, // for players
	// Teneras Menethil
	SPELL_LIGHTS_FAVOR					=	69382, // only in normal mode
	SPELL_RESTORE_SOUL					=	72595,
	// Spirit Warden
	SPELL_SOUL_RIP						=	69397,
	SPELL_DARK_HUNGER					=	69383,
	// Wicked spirits : to find
	
	//** Transitional phases
	SPELL_REMORSELESS_WINTER			=	68981,
	SPELL_PAIN_AND_SUFFERING			=	72133,
	SPELL_SUMMON_ICE_SPHERE				=	69103,
	SPELL_RAGING_SPIRIT					=	69200,
	SPELL_QUAKE							=	72262,
	// Ice sphere
	SPELL_ICE_PULSE						=	69099,
	SPELL_ICE_BURST						=	69108,
	// Raging Spirit
	SPELL_SOUl_SHRIEK					=	69242
};

void AddSC_ICC_LichKing()
{
}
