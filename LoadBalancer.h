#ifndef LOADBALANCER_H
#define LOADBALANCER_H
#include "Webserver.h"
#include "Request.h"
#include <queue>
#include <vector>
#include <string>

/**
 * @file LoadBalancer.h
 * @brief Defines the LoadBalancer class responsible for managing a pool of
 *        Webserver instances and distributing incoming Request objects.
 */

/**
 * @class LoadBalancer
 * @brief Coordinates multiple web servers by queuing and dispatching requests,
 *        scaling the number of active servers up or down, and logging activity.
 */
class LoadBalancer{
    private:
        int numServers;       /**< number of active servers */
        std::vector<Webserver> servers; /**< list of server objects */
        std::vector<std::queue<Request>> requestQueues; /**< per-server request queues */
        std::vector<int> queuedTime; /**< cumulative time of queued requests for each server */
        std::vector<std::string> previousIPs; /**< history of seen IPs for throttling */
        std::vector<std::string> blockedIPs; /**< firewall rules by IP address */
        int minimumTime; /**< time threshold below which a server is considered idle */
        int maximumTime; /**< time threshold above which an additional server is needed */
        int maxServerCount; /**< highest number of servers seen (for logging) */
        int rejectedRequestCount; /**< count of requests dropped due to firewall/overload */
        int uptime; /**< tick counter for how long the balancer has run */
        int requestsProcessed; /**< total number of requests handed off to servers */
        int maxTaskTime; /**< maximum task duration observed (for logging) */
        int serverChangeWait; /**< cooldown ticks before another server change */
        int serverChangeWaitTime; /**< configured cooldown duration */
    public:
        /**
         * @brief Construct a load balancer with the given initial server count.
         * @param numServers Initial number of web servers to create.
         */
        LoadBalancer(int numServers);

        /**
         * @brief Retrieve the current number of active servers.
         * @return Number of servers.
         */
        int getServerCount();

        /**
         * @brief Enqueue a batch of requests into the appropriate server queues.
         * @param rq Queue containing the incoming requests.
         */
        void receiveRequestBatch(std::queue<Request> rq);

        /**
         * @brief Process a single request, assigning it to the smallest queue.
         * @param r The request to handle.
         */
        void receiveRequest(Request r);

        /**
         * @brief Advance the state of all servers by one tick and adjust capacity.
         */
        void tick();

        /**
         * @brief Check whether a request should be blocked based on IP rules.
         * @param r Request to evaluate.
         * @return True if allowed, false if blocked.
         */
        bool checkFirewall(Request r);

        /**
         * @brief Update IP history and add to firewall if an address is abusive.
         * @param r The request being inspected.
         */
        void checkPreviousIPs(Request r);

        /**
         * @brief Remove one server and redistribute its queued requests.
         */
        void takeDownServer();

        /**
         * @brief Add a new server and rebalance queues if necessary.
         */
        void bringUpServer();

        /**
         * @brief Determine whether load justifies spinning up another server.
         * @return True if another server should be added.
         */
        bool serverNeeded();

        /**
         * @brief Determine whether an existing server is underutilized.
         * @return True if a server can be removed.
         */
        bool serverIdle();

        /**
         * @brief Find the index of the queue with the fewest requests.
         * @return Index of smallest queue, or -1 if no servers.
         */
        int smallestQueue();

        /**
         * @brief Find the index of the queue with the most requests.
         * @return Index of largest queue, or -1 if no servers.
         */
        int largestQueue();

        /**
         * @brief Log initial state information when the balancer starts.
         */
        void StartLog();

        /**
         * @brief Log summary information when the balancer stops.
         */
        void EndLog();

        /**
         * @brief Internally log a message with optional console color.
         * @param message Text to log.
         * @param color Name of ANSI color code.
         */
        void log(std::string message, std::string color);

        /**
         * @brief Load timing thresholds from a configuration file.
         * @param filename Path to the config file.
         */
        void loadConfig(std::string filename);
};
#endif