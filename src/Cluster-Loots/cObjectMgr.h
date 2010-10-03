#ifndef _COBJECTMGR_H
#define _COBJECTMGR_H

#include <map>
#include "LootMgr.h"

enum LootTable
{
	LOOT_CREATURE		= 0x00,
	LOOT_DISENCHANT		= 0x01,
	LOOT_FISH			= 0x02,
	LOOT_GAMEOBJECT		= 0x03,
	LOOT_ITEM			= 0x04,
	LOOT_MAIL			= 0x05,
	LOOT_MILL			= 0x06,
	LOOT_PICKPOCKET		= 0x07,
	LOOT_PROSPECT		= 0x08,
	LOOT_REFERENCE		= 0x09,
	LOOT_SKIN			= 0x0A,
	LOOT_SPELL			= 0x0B,
	MAX_LOOT_TABLE		= 0x0C,
};

static const char* TableName[MAX_LOOT_TABLE] = {
	"creature_loot_template",
	"disenchant_loot_template",
	"fishing_loot_template",
	"gameobject_loot_template",
	"item_loot_template",
	"mail_loot_template",
	"milling_loot_template",
	"pickpocketing_loot_template",
	"prospecting_loot_template",
	"reference_loot_template",
	"skinning_loot_template",
	"spell_loot_template"
};

class cObjectMgr
{
	public:
		void LoadLoots();
	private:
		void LoadLootTable(LootTable lt);
		LootTemplateMap m_LootTemplates[MAX_LOOT_TABLE];
};

#define sClusterObjectMgr MaNGOS::Singleton<cObjectMgr>::Instance()

#endif