#include <Policies/SingletonImp.h>
#include "AccountMgr.h"

INSTANTIATE_SINGLETON_1( AccountMgr );

AccountMgr::AccountMgr()
{
	AccountMap.clear();
}

bool AccountMgr::isRegistered(std::string str)
{
	if(AccountMap.find(str) != AccountMap.end())
		return true;

	return false;
}