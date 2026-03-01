#define LOADBALANCER_H
#include "Webserver.h"
#include "Request.h"
#include <queue>
#include <vector>
#include <string>

class LoadBalancer{
    private:
        int numServers;
        std::vector<Webserver> servers;
        std::vector<std::queue<Request>> requestQueues;
        std::vector<int> queuedTime;
        std::vector<std::string> blockedIPs;
    public:
        LoadBalancer(int numServers);
        void receiveRequest(Request r);
        void tick();
        void checkFirewall(Request r);
        void takeDownServer();
        void bringUpServer();
        bool serverNeeded();
        bool serverIdle();

};