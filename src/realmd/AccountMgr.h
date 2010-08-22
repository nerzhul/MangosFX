#ifndef _ACCOUNTMGR_H
#define _ACCOUNTMGR_H

#include "Common.h"
#include "Policies/Singleton.h"

struct AccountData
{
	uint64 guid;
	std::string accShaPass;
	std::string lastIp;
};

class AccountMgr
{
	public:
		AccountMgr();
		~AccountMgr(){}
		bool isRegistered(std::string str);
	private:
		std::map<std::string,AccountData*> AccountMap;
};

#define sAccountMgr MaNGOS::Singleton<AccountMgr>::Instance()
#endif