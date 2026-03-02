/**
 * @file Webserver.cpp
 * @brief Implementation of Webserver methods, simulates server behavior.
 */

#include "Request.h"
#include <string>
#include "Webserver.h"

bool Webserver::isOpen()
{
    return open;
}

void Webserver::acceptRequest(Request r)
{
    curRequest = r;
    open = false;
}

void Webserver::tick()
{
    timeProcessing++;
    if (timeProcessing >= curRequest.getTime())
    {
        timeProcessing = 0;
        curRequest = Request();
        open = true;
    }
}

Request Webserver::getCurRequest(){
    return curRequest;
}

Webserver::Webserver()
{
    curRequest = Request();
    timeProcessing = 0;
    open = true;
}