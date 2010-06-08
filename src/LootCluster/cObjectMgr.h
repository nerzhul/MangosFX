#ifndef _COBJECTMGR_H
#define _COBJECTMGR_H

#include <ItemPrototype.h> // replace this
#include <QuestDef.h>
#include <Database/SQLStorage.h>
#include <map>

extern SQLStorage sItemStorage;
extern SQLStorage sCreatureStorage;
extern SQLStorage sGOStorage;

class cObjectMgr
{
	public:
		void LoadItemPrototypes();
		void LoadCreatureTemplates();
		void LoadGameobjectInfo();
		void LoadQuests();

		uint32 GetScriptId(const char *name);

		typedef std::vector<std::string> ScriptNameMap;
		typedef UNORDERED_MAP<uint32, Quest*> QuestMap;
		QuestMap            mQuestTemplates;
	private:
		static ItemPrototype const* GetItemPrototype(uint32 id) { return sItemStorage.LookupEntry<ItemPrototype>(id); }

		ScriptNameMap       m_scriptNames;
		
};

#define sClusterObjectMgr MaNGOS::Singleton<cObjectMgr>::Instance()

#endif