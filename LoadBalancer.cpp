#include "LoadBalancer.h"
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
}
int LoadBalancer::getServerCount(){
    return numServers;
}
void LoadBalancer::receiveRequestBatch(std::queue<Request> rq){
    log("Recieved " + std::to_string(rq.size()) + " requests", "white");
    while(!rq.empty()){
        receiveRequest(rq.front());
        rq.pop();
    }
}
void LoadBalancer::receiveRequest(Request r)
{
    checkPreviousIPs(r);
    if (checkFirewall(r))
    {
        int smallest = smallestQueue();
        requestQueues.at(smallest).push(r);
        queuedTime.at(smallest) += r.getTime();
    }
    else
    {
        rejectedRequestCount += 1;
        log("Blocked request from " + r.getIPin() + " to " + r.getIPout() + " of type " + r.getJobType() + " with expected time of " + std::to_string(r.getTime()), "red");
    }
}
void LoadBalancer::tick()
{
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
    if (count > 15)
    {
        blockedIPs.push_back(r.getIPin());
        log("Blocked IP " + r.getIPin() + " for making too many requests", "red");
    }
    for(int i = 0; i < previousIPs.size(); i++)
    {
        if (r.getIPout() == previousIPs.at(i))
        {
            count++;
        }
    }
    if (count > 15)    {
        blockedIPs.push_back(r.getIPout());
        log("Blocked IP " + r.getIPout() + " for making too many requests", "red");
    }
    previousIPs.push_back(r.getIPin());
    previousIPs.push_back(r.getIPout());
    if(previousIPs.size() > 500)
    {
        previousIPs.erase(previousIPs.begin(), previousIPs.begin() + 2);
    }
}

void LoadBalancer::takeDownServer()
{
    int smallest = smallestQueue();
    std::queue<Request> tempQueue = requestQueues.at(smallest);
    if (!servers.at(smallest).isOpen())
    {
        tempQueue.push(servers.at(smallest).getCurRequest());
    }
    servers.erase(servers.begin() + smallest);
    requestQueues.erase(requestQueues.begin() + smallest);
    queuedTime.erase(queuedTime.begin() + smallest);
    while (!tempQueue.empty())
    {
        receiveRequest(tempQueue.front());
        tempQueue.pop();
    }
    log("Took down server " + std::to_string(smallest), "yellow");
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
    log("Brought up server " + std::to_string(numServers - 1), "green");
}
bool LoadBalancer::serverNeeded()
{
    if (queuedTime.at(smallestQueue()) > maximumTime)
    {
        return true;
    }
    return false;
}
bool LoadBalancer::serverIdle()
{
    if (queuedTime.at(largestQueue()) < minimumTime)
    {
        return true;
    }
    return false;
}
int LoadBalancer::smallestQueue()
{
    int smallest = 0;
    for (int i = 0; i < numServers; i++)
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
    int largest = 0;
    for (int i = 0; i < numServers; i++)
    {
        if (requestQueues.at(i).size() > requestQueues.at(largest).size())
        {
            largest = i;
        }
    }
    return largest;
}
void LoadBalancer::StartLogStatus(){ // logs number of servers, time of queues
    log("Load balancer started with " + std::to_string(numServers) + "Servers", "white");
    log("Minimum time to take down server: " + std::to_string(minimumTime), "white");
    log("Maximum time to bring up server: " + std::to_string(maximumTime), "white");
    int totalTicks = 0;
    for(int i = 0; i < numServers; i++){
        totalTicks = queuedTime.at(i);
    }
    log("Total queued time of: " + std::to_string(totalTicks), "white");
}
void LoadBalancer::EndLogStatus(){ // logs remaining requests, active servers, inactive servers, rejected requests, blocked IPs
    int totalTicks = 0;
    for(int i = 0; i < numServers; i++){
        totalTicks = queuedTime.at(i);
    }
    log("Current server count: " + std::to_string(numServers), "white");
    log("Inactive server count: " + std::to_string(maxServerCount - numServers), "white");
    log("Total time up: " + std::to_string(uptime), "white");
    log("Rejected requests: " + std::to_string(rejectedRequestCount), "white");
    std::string ips = "";
    for(int i = 0; i < blockedIPs.size(); i++){
        ips += " " + blockedIPs.at(i);
    }
    log("Blocked IPs: " + ips, "white");
    log("Requests processed: " + std::to_string(requestsProcessed), "white");
}
void LoadBalancer::log(std::string message, std::string color)
{
    // open log file in append mode
    std::ofstream logFile("loadbalancer.log", std::ios::app);
    if (!logFile.is_open())
    {
        // cannot open log file, silently fail or could throw
        return;
    }

    // get current time as string
    std::time_t now = std::time(nullptr);
    char buf[64];
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now)) == 0)
    {
        buf[0] = '\0';
    }

    // write timestamp, color tag, and message
    logFile << "[" << buf << "] ";
    if (!color.empty())
    {
        logFile << "(" << color << ") ";
    }
    logFile << message << "\n";

    logFile.close();
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
    }
    configFile.close();
}