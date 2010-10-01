#ifndef __CLUSTER_SOCKET_H_
#define __CLUSTER_SOCKET_H_

#include <Common.h>

class ClusterSocket : public ACE_Based::Runnable
{
	public:
		ClusterSocket();
		~ClusterSocket();
		void run();
};

#endif