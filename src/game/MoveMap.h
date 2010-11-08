#ifndef __MOVEMAP_H_
#define __MOVEMAP_H_

typedef std::map<uint32,dtNavMesh*> mmaps;
class MoveMapMgr
{
	public:
		MoveMapMgr() { m_mmaps.clear(); }
		~MoveMapMgr() {}
		void LoadAllMMaps();
		dtNavMesh* GetMMap(uint32 id);
	private:
		bool LoadMMap(uint32 id);
		mmaps m_mmaps;
};

#define sMMapMgr MaNGOS::Singleton<MoveMapMgr>::Instance()

#endif