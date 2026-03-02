/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the LoadBalancer methods declared in LoadBalancer.h.
 */

#include "LoadBalancer.h"
#include <iostream>
#include <fstream>
#include <ctime>

LoadBalancer::LoadBalancer(int numServers)
{
    this->numServers = numServers;
    for (int i = 0; i < numServers; i++)
    {
        servers.push_back(Webserver());
    }
    for (int i = 0; i < numServers; i++)
    {
        requestQueues.push_back(std::queue<Request>());
        queuedTime.push_back(0);
    }
    blockedIPs = std::vector<std::string>();
    previousIPs = std::vector<std::string>();

    loadConfig(".config");
    maxServerCount = numServers;
    rejectedRequestCount = 0;
    uptime = 0;
    serverChangeWait = 0;
    log("Load balancer started with " + std::to_string(numServers) + " servers", "white");
}
int LoadBalancer::getServerCount(){
    return numServers;
}
void LoadBalancer::receiveRequestBatch(std::queue<Request> rq){
    if(rq.empty()) return;
    log("Recieved " + std::to_string(rq.size()) + " requests at tick " + std::to_string(uptime), "white");
    while(!rq.empty()){
        receiveRequest(rq.front());
        rq.pop();
    }
}
void LoadBalancer::receiveRequest(Request r)
{
    if (checkFirewall(r))
    {
        checkPreviousIPs(r);
        int smallest = smallestQueue();
        requestQueues.at(smallest).push(r);
        queuedTime.at(smallest) += r.getTime();
    }
    else
    {
        rejectedRequestCount += 1;
    }
}
void LoadBalancer::tick()
{
    if(serverNeeded() && serverChangeWait == 0){
        bringUpServer();
        serverChangeWait = serverChangeWaitTime;
    }
    if(serverIdle() && serverChangeWait == 0){
        takeDownServer();
        serverChangeWait = serverChangeWaitTime;
    }
    for (int i = 0; i < numServers; i++)
    {
        if(servers.at(i).isOpen() && requestQueues.at(i).size() != 0){
            servers.at(i).acceptRequest(requestQueues.at(i).front());
            requestQueues.at(i).pop();
            requestsProcessed += 1;
        }
        servers.at(i).tick();
    }
    uptime += 1;
    if(serverChangeWait > 0){
        serverChangeWait -= 1;
    }
}
bool LoadBalancer::checkFirewall(Request r)
{
    for (int i = 0; i < blockedIPs.size(); i++)
    {
        if (r.getIPin() == blockedIPs.at(i) || r.getIPout() == blockedIPs.at(i))
        {
            return false;
        }
    }
    return true;
}
void LoadBalancer::checkPreviousIPs(Request r)
{
    int count = 0;
    for (int i = 0; i < previousIPs.size(); i++)
    {
        if (r.getIPin() == previousIPs.at(i))
        {
            count++;
        }
    }
    if (count > 50)
    {
        blockedIPs.push_back(r.getIPin());
        log("Blocked IP " + r.getIPin() + " for making too many requests at tick " + std::to_string(uptime), "red");
    }
    count = 0;
    for(int i = 0; i < previousIPs.size(); i++)
    {
        if (r.getIPout() == previousIPs.at(i))
        {
            count++;
        }
    }
    if (count > 50)    {
        blockedIPs.push_back(r.getIPout());
        log("Blocked IP " + r.getIPout() + " for making too many requests at tick " + std::to_string(uptime), "red");
    }
    previousIPs.push_back(r.getIPin());
    previousIPs.push_back(r.getIPout());
    while(previousIPs.size() > 1000)
    {
        previousIPs.erase(previousIPs.begin());
    }
}

void LoadBalancer::takeDownServer()
{
    if(numServers == 0) {
        return;
    }

    int smallest = smallestQueue();
    if(smallest < 0 || smallest >= numServers) return;

    std::queue<Request> tempQueue = requestQueues.at(smallest);
    if (!servers.at(smallest).isOpen())
    {
        tempQueue.push(servers.at(smallest).getCurRequest());
    }
    servers.erase(servers.begin() + smallest);
    requestQueues.erase(requestQueues.begin() + smallest);
    queuedTime.erase(queuedTime.begin() + smallest);
    numServers -= 1;
    while (!tempQueue.empty())
    {
        receiveRequest(tempQueue.front());
        tempQueue.pop();
    }
    log("Took down server " + std::to_string(smallest) + " at tick " + std::to_string(uptime), "yellow");
}
void LoadBalancer::bringUpServer()
{
    servers.push_back(Webserver());
    requestQueues.push_back(std::queue<Request>());
    queuedTime.push_back(0);
    numServers += 1;
    while (smallestQueue() == numServers - 1)
    {
        int largest = largestQueue();
        requestQueues.at(numServers - 1).push(requestQueues.at(largest).front());
        queuedTime.at(largest) -= requestQueues.at(largest).front().getTime();
        queuedTime.at(numServers - 1) += requestQueues.at(largest).front().getTime();
        requestQueues.at(largest).pop();
    }
    if(numServers > maxServerCount){
        maxServerCount = numServers;
    }
    log("Brought up server " + std::to_string(numServers - 1) + " at tick " + std::to_string(uptime), "green");
}
bool LoadBalancer::serverNeeded()
{
    if(numServers == 0) return false;
    int idx = smallestQueue();
    if (queuedTime.at(idx) > maximumTime)
    {
        return true;
    }
    return false;
}
bool LoadBalancer::serverIdle()
{
    if(numServers == 0) return false;
    int idx = largestQueue();
    if (queuedTime.at(idx) < minimumTime)
    {
        return true;
    }
    return false;
}
int LoadBalancer::smallestQueue()
{
    if(numServers == 0) return -1;
    int smallest = 0;
    for (int i = 1; i < numServers; i++)
    {
        if (requestQueues.at(i).size() < requestQueues.at(smallest).size())
        {
            smallest = i;
        }
    }
    return smallest;
}
int LoadBalancer::largestQueue()
{
    if(numServers == 0) return -1;
    int largest = 0;
    for (int i = 1; i < numServers; i++)
    {
        if (requestQueues.at(i).size() > requestQueues.at(largest).size())
        {
            largest = i;
        }
    }
    return largest;
}
void LoadBalancer::StartLog(){ // logs number of servers, time of queues
    log("Minimum time to take down server: " + std::to_string(minimumTime), "white");
    log("Maximum time to bring up server: " + std::to_string(maximumTime), "white");
    int totalTicks = 0;
    for(int i = 0; i < numServers; i++){
        totalTicks = queuedTime.at(i);
    }
    log("Total queued time of: " + std::to_string(totalTicks), "white");
    int totalRequests = 0;
    for(int i = 0; i < numServers; i++){
        totalRequests += requestQueues.at(i).size();
    }
    log("Total queued requests: " + std::to_string(totalRequests) + " with task times between 1 and " + std::to_string(maxTaskTime), "white");
    log("----------------------------------------------------------", "white");
}
void LoadBalancer::EndLog(){ // logs remaining requests, active servers, inactive servers, rejected requests, blocked IPs
    log("----------------------------------------------------------", "white");
    log("Load balancer stopped", "red");
    log("Current server count: " + std::to_string(numServers), "white");
    log("Inactive server count: " + std::to_string(maxServerCount - numServers), "white");
    log("Total time up: " + std::to_string(uptime), "white");
    log("Rejected requests: " + std::to_string(rejectedRequestCount), "white");
    std::string ips = "";
    for(int i = 0; i < blockedIPs.size(); i++){
        ips += " " + blockedIPs.at(i);
    }
    log("Blocked IPs: " + ips, "white");
    int remainingRequests = 0;
    for(int i = 0; i < numServers; i++){
        remainingRequests += requestQueues.at(i).size();
    }
    log("Remaining requests: " + std::to_string(remainingRequests), "white");
    log("Requests processed: " + std::to_string(requestsProcessed), "white");
}
void LoadBalancer::log(std::string message, std::string color)
{
    std::time_t now = std::time(nullptr);
    char buf[64];
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now)) == 0)
        buf[0] = '\0';

    std::string timestamp = "[" + std::string(buf) + "] ";

    std::string colorCode;

    if (color == "red") colorCode = "\033[31m";
    else if (color == "green") colorCode = "\033[32m";
    else if (color == "yellow") colorCode = "\033[33m";
    else if (color == "blue") colorCode = "\033[34m";
    else if (color == "magenta") colorCode = "\033[35m";
    else if (color == "cyan") colorCode = "\033[36m";
    else if (color == "white") colorCode = "\033[37m";
    else colorCode = "";

    if (!colorCode.empty())
        std::cout << timestamp << colorCode << message << "\033[0m\n";
    else
        std::cout << timestamp << message << "\n";

    std::ofstream logFile("loadbalancer.log", std::ios::app);
    if (logFile.is_open())
        logFile << timestamp << message << "\n";
}

void LoadBalancer::loadConfig(std::string filename)
{
    std::ifstream configFile(filename);
    if (!configFile.is_open())
    {
        log("Failed to open config file: " + filename, "red");
        return;
    }
    std::string line;
    while (std::getline(configFile, line))
    {
        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos)
        {
            continue; // skip invalid lines
        }
        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);
        if (key == "minTime")
        {
            minimumTime = std::stoi(value);
        }
        else if (key == "maxTime")
        {
            maximumTime = std::stoi(value);
        }
        else if(key == "serverChangeTime")
        {
            serverChangeWaitTime = std::stoi(value);
        }
        else if(key == "maxRequestTime")
        {
            maxTaskTime = std::stoi(value);
        }
    }
    configFile.close();
}