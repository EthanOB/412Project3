#ifndef REQUESTGENERATOR_H
#define REQUESTGENERATOR_H
#include "Request.h"
#include <queue>
#include <string>
#include <random>

// RequestGenerator class is responsible for generating random requests to be sent to the load balancer.
// Creates a queue of requeust that will be sent to the load balancer in the main file.
/**
 * @class RequestGenerator
 * @brief Generates network requests with various patterns including random bursts and attack simulations.
 * 
 * The RequestGenerator class is responsible for creating and managing a queue of Request objects.
 * It supports configurable request generation with limits on the number of requests and time intervals
 * to prevent overloading the load balancer. It can generate normal traffic patterns as well as
 * simulated attack bursts.
 * 
 * @note Uses std::random_device for random number generation across all randomization methods.
 */
/**
 * @file RequestGenerator.h
 * @brief Declaration of the RequestGenerator class used to produce
 *        simulated network traffic for testing the load balancer.
 */

class RequestGenerator {
    public:
    /**
     * @brief Constructs a RequestGenerator with default configuration.
     */
    RequestGenerator();

    /**
     * @brief Fills the server queue with a specified number of requests.
     * @param numRequests The number of requests to generate and queue.
     * @return A queue of Request objects ready to be processed.
     */
    std::queue<Request> fillServers(int numRequests);

    /**
     * @brief Generates a queue of requests with random parameters.
     * @return A queue of randomly generated Request objects.
     */
    std::queue<Request> generateRandomRequests();

    /**
     * @brief Generates a single request with random properties.
     * @return A Request object with randomly generated attributes.
     */
    Request generateRequest();

    /**
     * @brief Generates a random IP address string.
     * @return A randomly generated IP address as a string.
     */
    std::string generateRandomIP();

    /**
     * @brief Loads configuration parameters from a file.
     * @param filename The path to the configuration file.
     */
    void loadConfiguration(std::string filename);

    /**
     * @brief Determines if an attack burst should occur based on random probability.
     * @return True if an attack burst should be generated, false otherwise.
     */
    bool randomAttackBurst();

    /**
     * @brief Determines if a normal traffic burst should occur based on random probability.
     * @return True if a burst should be generated, false otherwise.
     */
    bool randomBurst();

    /**
     * @brief Generates a random request count within configured limits.
     * @return A random number of requests to generate.
     */
    int randomRequestCount();

    /**
     * @brief Generates a random time interval within configured limits.
     * @return A random time value in an unspecified unit.
     */
    int randomTime();

    /**
     * @brief Generates a random request category character.
     * @return A character representing a request category.
     */
    char randomCategory();

    private:
    /**
     * @brief Maximum number of requests allowed to prevent load balancer overload.
     */
    int maxRequests;

    /**
     * @brief Maximum time interval for request generation to prevent load balancer overload.
     */
    int maxTime;

    /**
     * @brief Random device used for generating random numbers across all methods.
     */
    std::random_device rd;
};
#endif