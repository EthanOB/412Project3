#ifndef LOADBALANCER_H
#define LOADBALANCER_H
#include "Webserver.h"
#include "Request.h"
#include <queue>
#include <vector>
#include <string>

// LoadBalancer class manages multiple web servers and distributes incoming requests among them.
// contains most of the functionality of the program since knowledge of all servers and request is needed.
class LoadBalancer{
    private:
        int numServers; // number of servers
        std::vector<Webserver> servers; // stores servers
        std::vector<std::queue<Request>> requestQueues; // stores next request for each server
        std::vector<int> queuedTime; // used to find which server to add requests to
        std::vector<std::string> previousIPs; // used to find which IPs to block
        std::vector<std::string> blockedIPs; // used for firewall
        int minimumTime; // used to find when to take down servers
        int maximumTime; // used to find when to bring up servers
        int maxServerCount; // used for logs
        int rejectedRequestCount; // used for logs
        int uptime; // used for logs
        int requestsProcessed; // used for logs
        int maxTaskTime; // used for logs
        int serverChangeWait; // used to prevent too many servers from being added or removed at once
        int serverChangeWaitTime; // used to set time to wait between server changes
    public:
        LoadBalancer(int numServers);
        int getServerCount();
        void receiveRequestBatch(std::queue<Request> rq);
        void receiveRequest(Request r); //recieves requests and adds to smallest queue
        void tick(); // ticks all servers
        bool checkFirewall(Request r); //checks request for blocked IPs
        void checkPreviousIPs(Request r); // checks request for IPs that have made many requests in the past
        void takeDownServer(); // takes down smallest server and redistributes requests to other servers
        void bringUpServer(); // brings up new server and redistributes requests to be balanced
        bool serverNeeded(); // checks if another server is needed
        bool serverIdle(); // checks if a server needs to be removed
        int smallestQueue(); // finds index of smallest queue for other functions
        int largestQueue(); // finds index of the largest queue for other functions
        void StartLog(); // logs number of servers, time of queues
        void EndLog(); // logs remaining requests, active servers, inactive servers, rejected requests, blocked IPs
        void log(std::string message, std::string color); // logs messages to the console with color.
        void loadConfig(std::string filename); // loads config file to set minimum and maximum times for servers.
};
#endif