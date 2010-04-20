#include "precompiled.h"

enum Event_Spells
{
	// Phase Givre

	SPELL_FROST_VRILLE		=	56925, // 4k dmg
	SPELL_AURA_BLIZZARD		=	29952, // une aura de blizzard pour le raid
	SPELL_BLIZZARD			=	64653,	// le blizzard
	SPELL_FROST_STORM		=	31249,
	SPELL_EXPLODE_FROST		=	64626, // 10k dmg en instantané
	SPELL_FROST_DEBUFF		=	4307,	// debuff sur une zone
	SPELL_FROST_ATTACK		=	68510, // dommages direct type dk

	// Phase Ombre
	SPELL_MENTAL_ATTACK		=	68040, // 6k dmg
	SPELL_SHADOW_AURA		=	41142, // +10% dmg
	SPELL_BANNIR			=	8994,
	SPELL_SHADOW_BOMB		=	63024, // explosion d'un joueur
	SPELL_NIGHTMARE			=	60947, // fear
	SPELL_ALL_SHADOW_BOLT	=	59975, // trait de l'ombre sur tous
	SPELL_SHADOW_BOLT		=	40876,

	// Phase Feu
	SPELL_ARMAGEDDON		=	45915, // explosion
	SPELL_FIRE_BOLT			=	62796,
	SPELL_FLAME_BREATH		=	64721,
	SPELL_FIRE_BOLT2		=	54249,
	SPELL_DOWN_LIFE			=	19367, // -35% de vie pour 15 min

	// All Phases
	SPELL_UP_DAMAGE_ON_TAR	=	39095,
	SPELL_UP_CC				=	31035,
	SPELL_SLEEP_TAR			=	36333,
	SPELL_CECITE			=	26108,
	SPELL_FRACASS_ARMOR		=	36836,

};

enum Event_Texts
{


};

struct MANGOS_DLL_DECL event_lich_king_AI : public ScriptedAI
{
	event_lich_king_AI(Creature *pCreature) : ScriptedAI(pCreature) 
	{
		Reset();
	}

	uint8 phase;
	uint32 phase_Timer;
	uint32 Enrage_Timer;

	// init frost
	uint32 Vrille_Timer;
	uint32 AuraFrost_Timer;
	uint32 Blizzard_Timer;
	uint32 Glacon_Timer;
	uint32 FrostExplode_Timer;
	uint32 FrostZone_Timer;
	uint32 FrostDirect_Timer;

	void Reset()
	{
		phase = 0;
		phase_Timer = 0; // TODO : changer suivant l'event

		// Frost Timers
		Vrille_Timer = 15000;
		AuraFrost_Timer = 17000;
		Blizzard_Timer = 8000;
		Glacon_Timer = 35000;
		FrostExplode_Timer = 45000;
		FrostZone_Timer = 37000;
		FrostDirect_Timer = 4000;
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!CanDoSomething())
            return;

		switch(phase)
		{
			case 0: // Event Phase
				phase = 1;
				break;
			case 1: // Frost Phase
				phase_Timer = 1200000;

				if(Vrille_Timer <= uiDiff)
				{
					DoCastVictim(SPELL_FROST_VRILLE);
					Vrille_Timer = urand(17000,22000);
				}
				else
					Vrille_Timer -= uiDiff;

				if(AuraFrost_Timer <= uiDiff)
				{
					DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_AURA_BLIZZARD);
					AuraFrost_Timer = urand(27000,35000);
				}
				else
					AuraFrost_Timer -= uiDiff;

				if(Blizzard_Timer <= uiDiff)
				{
					DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_AURA_BLIZZARD);
					Blizzard_Timer = urand(12000,25000);
				}
				else
					Blizzard_Timer -= uiDiff;

				if(Glacon_Timer <= uiDiff)
				{
					// todo : 6 cibles dans le raid
					Glacon_Timer = urand(18000,22000);
				}
				else
					Glacon_Timer -= uiDiff;

				if(FrostExplode_Timer <= uiDiff)
				{
					DoCastVictim(SPELL_AURA_BLIZZARD);
					FrostExplode_Timer = urand(40000,50000);
				}
				else
					FrostExplode_Timer -= uiDiff;

				if(FrostZone_Timer <= uiDiff)
				{
					DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_AURA_BLIZZARD);
					FrostZone_Timer = urand(17000,22000);
				}
				else
					FrostZone_Timer -= uiDiff;

				if(FrostDirect_Timer <= uiDiff)
				{
					// todo : 10 cibles dans le raid
					FrostDirect_Timer = urand(6000,8000);
				}
				else
					FrostDirect_Timer -= uiDiff;

				break;
			case 2: // Shadow Phase
				phase_Timer = 1200000;
				break;
			case 3: // Fire Phase
				phase_Timer = 60000;
				break;
			default :
				break;
		}
	}
};