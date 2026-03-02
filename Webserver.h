#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "Request.h"
#include <string>
#include "Request.h"
#include <string>

/**
 * @file Webserver.h
 * @brief Defines a simple web server that processes a single Request at a time.
 */

/**
 * @class Webserver
 * @brief Simulates a backend server that can accept one request and process it
 *        over multiple ticks.
 */
class Webserver{
    private:
        Request curRequest; /**< currently processing request */
        int timeProcessing; /**< ticks spent on the current request */
        bool open;          /**< whether the server is ready to accept a new request */

    public:
        /**
         * @brief Query whether the server is open for a new request.
         * @return True if open, false if busy.
         */
        bool isOpen();

        /**
         * @brief Accept a request and begin processing it immediately.
         * @param r The request to handle.
         */
        void acceptRequest(Request r);

        /**
         * @brief Advance the server's processing by one tick. Completes the
         *        request when its required time elapses.
         */
        void tick();

        /**
         * @brief Retrieve the request currently being processed.
         * @return A copy of the active Request.
         */
        Request getCurRequest();

        /**
         * @brief Default constructor; initializes server to open state.
         */
        Webserver();
};
#endif