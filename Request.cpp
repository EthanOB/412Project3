#include "Request.h"

Request::Request() {
    IPin = "";
    IPout = "";
    time = 0;
    jobType = 'N';
}

Request::Request(std::string IPin, std::string IPout, int time, char jobType) {
    this->IPin = IPin;
    this->IPout = IPout;
    this->time = time;
    this->jobType = jobType;
}

std::string Request::getIPin() const {
    return IPin;
}

std::string Request::getIPout() const {
    return IPout;
}

int Request::getTime() const {
    return time;
}

char Request::getJobType() const {
    return jobType;
}