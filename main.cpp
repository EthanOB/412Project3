/**
 * @file main.cpp
 * @brief Entry point for the simulation program, prompts user and runs load balancer.
 */

#include "Request.h"
#include "LoadBalancer.h"
#include "RequestGenerator.h"
#include <iostream>
#include <limits>

/**
 * @brief Prompt the user to enter an initial server count.
 *
 * Loops until a positive integer is provided. Handles invalid input by
 * clearing the stream state and discarding the rest of the line.
 *
 * @return A positive integer representing the desired number of servers.
 */
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

/**
 * @brief Prompt the user for how many simulation ticks to execute.
 *
 * Continues asking until a positive integer is entered, using the same
 * validation logic as getServerCount().
 *
 * @return The number of ticks to run the load balancer.
 */
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

/**
 * @brief Generate an occasional random event indicator.
 *
 * Returns a uniformly distributed integer between 0 and 100. The calling
 * code treats a value of 0 as the trigger for creating a new batch of
 * requests.
 *
 * @return Random integer in [0,100].
 */
int randomBatch() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(0, 100);
    return dist(gen);
}

int main(){
    RequestGenerator RQ;
    LoadBalancer LB = LoadBalancer(getServerCount());
    int tickMax = getTickCount();
    LB.receiveRequestBatch(RQ.fillServers(LB.getServerCount() * 100));
    LB.StartLog();
    for(int i = 0; i < tickMax; i++){
        if(randomBatch() == 0 && i != 0){
            LB.receiveRequestBatch(RQ.generateRandomRequests());
        }
        LB.tick();
    }
    LB.EndLog();
}