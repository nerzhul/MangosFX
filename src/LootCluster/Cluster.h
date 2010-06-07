#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "Common.h"
#include "Policies/Singleton.h"

class Cluster
{
    public:
        Cluster();
        ~Cluster();
        int Run();
        static volatile uint32 m_masterLoopCounter;

    private:
        bool _StartDB();

        void _HookSignals();
        void _UnhookSignals();
        static void _OnSignal(int s);
};

#define sCluster MaNGOS::Singleton<Cluster>::Instance()


#endif