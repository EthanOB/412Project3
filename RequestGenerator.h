#ifndef REQUESTGENERATOR_H
#define REQUESTGENERATOR_H
#include "Request.h"
#include <queue>
#include <string>
#include <random>

// RequestGenerator class is responsible for generating random requests to be sent to the load balancer.
// Creates a queue of requeust that will be sent to the load balancer in the main file.
class RequestGenerator {
    private:
        int maxRequests; //used to limit overloading load balancer
        int maxTime; //used to limit overloading load balancer
        std::random_device rd; // used for random number generation
    public:
        RequestGenerator();
        std::queue<Request> fillServers(int numRequests);
        std::queue<Request> generateRandomRequests();
        Request generateRequest();
        std::string generateRandomIP();
        void loadConfiguration(std::string filename);
        bool randomAttackBurst();
        bool randomBurst();
        int randomRequestCount();
        int randomTime();
        char randomCategory();
};
#endif