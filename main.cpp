#include "Request.h"
#include "LoadBalancer.h"
#include "RequestGenerator.h"
#include <iostream>
#include <limits>

int getServerCount() {
    int serverCount;

    while (true) {
        std::cout << "Enter the number of servers to create: ";

        if (std::cin >> serverCount && serverCount > 0) {
            return serverCount;
        }

        std::cout << "Invalid input. Please enter a positive integer.\n";

        std::cin.clear(); // clear error state
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int getTickCount() {
    int tickCount;

    while (true) {
        std::cout << "Enter the number of ticks to run the load balancer: ";

        if (std::cin >> tickCount && tickCount > 0) {
            return tickCount;
        }

        std::cout << "Invalid input. Please enter a positive integer.\n";

        std::cin.clear(); 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main(){
    RequestGenerator RQ = RequestGenerator();
    LoadBalancer LB = LoadBalancer(getServerCount());
    int tickMax = getTickCount();
    LB.receiveRequestBatch(RQ.fillServers(LB.getServerCount() * 100));
}