#ifndef REQUEST_H
#define REQUEST_H
#include <string>

/**
 * @file Request.h
 * @brief Represents a network request with source/destination IPs, processing time,
 *        and a job category.
 */

/**
 * @class Request
 * @brief Simple data container for attributes associated with a request.
 */
class Request {
    private:
        std::string IPin;   /**< source IP address */
        std::string IPout;  /**< destination IP address */
        int time;           /**< required processing time in ticks */
        char jobType;       /**< category of work ('P' or 'S', etc.) */

    public:
        /**
         * @brief Default construct an empty request.
         */
        Request();

        /**
         * @brief Construct a request with all properties specified.
         * @param IPin Source IP string.
         * @param IPout Destination IP string.
         * @param time Processing time.
         * @param jobType Category code.
         */
        Request(std::string IPin, std::string IPout, int time, char jobType);

        /**
         * @brief Get the source IP.
         * @return IPin string.
         */
        std::string getIPin() const;

        /**
         * @brief Get the destination IP.
         * @return IPout string.
         */
        std::string getIPout() const;

        /**
         * @brief Get the processing time requirement.
         * @return Time in ticks.
         */
        int getTime() const;

        /**
         * @brief Get the job category type.
         * @return Character representing category.
         */
        char getJobType() const;
};
#endif