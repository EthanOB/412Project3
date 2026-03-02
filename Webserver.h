#define WEBSERVER_H
#include "Request.h"
#include <string>
#include "Request.h"
#include <string>

class Webserver{
    private:
        Request curRequest;
        int timeProcessing;
        bool open;

    public:
        bool isOpen();
        void acceptRequest(Request r);
        void tick();
        Request getCurRequest();
        Webserver();
};