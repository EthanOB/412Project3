#include "RequestGenerator.h"
#include <fstream>
#include <ctime>

RequestGenerator::RequestGenerator(){
    attackIP = generateRandomIP();
    loadConfiguration(".config");
}
std::queue<Request> RequestGenerator::fillServers(int numRequests){
    std::queue<Request> reqQueue = std::queue<Request>();
    for(int i = 0; i < numRequests; i++){
        reqQueue.push(generateRequest());
    }
    return reqQueue;
}
std::queue<Request> RequestGenerator::generateRandomRequests(){
    std::queue<Request> reqQueue = std::queue<Request>();
    int numReq = 0;
    if(randomAttack){
        numReq = maxRequests;
    }
    else{
        numReq = randomRequestCount();
    }
    for(int i = 0; i < numReq; i++){
        reqQueue.push(generateRequest());
    }
    return reqQueue;
}

Request RequestGenerator::generateRequest(){
    return Request(generateRandomIP(), generateRandomIP(), randomTime(), randomCategory());
}

std::string RequestGenerator::generateRandomIP(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(0, 255);

    return std::to_string(dist(gen)) + "." +
           std::to_string(dist(gen)) + "." +
           std::to_string(dist(gen)) + "." +
           std::to_string(dist(gen));
}

void RequestGenerator::loadConfiguration(std::string filename){

    std::ifstream configFile(filename);
    if (!configFile.is_open())
    {
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
        if (key == "maxRequests")
        {
            maxRequests = std::stoi(value);
        }
        else if (key == "maxRequestTime")
        {
            maxTime = std::stoi(value);
        }
    }
    configFile.close();
}
bool RequestGenerator::randomAttack(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(1, 100);
    if(dist(gen) <= 15){ // 15 of 100 messages will be from the attack IP address
        return true;
    }
    return false;
}
bool RequestGenerator::randomBurst(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(1, 100);
    if(dist(gen) == 1){ // 1 of 100 request queues will be bursts
        return true;
    }
    return false;
}

int RequestGenerator::randomRequestCount(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(floor(maxRequests * 0.05), floor(maxRequests * 0.25));
    return dist(gen);
}

int RequestGenerator::randomTime(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(1,maxTime);
    return dist(gen);
}

char RequestGenerator::randomCategory(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(1,2);
    if(dist(gen) == 1){
        return 'P';
    }
    else{
        return 'S';
    }
}
