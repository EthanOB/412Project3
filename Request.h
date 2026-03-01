#define REQUEST_H
#include <string>

class Request {
    private:
        std::string IPin;
        std::string IPout;
        int time;
        char jobType;

    public:
        Request();
        Request(std::string IPin, std::string IPout, int time, char jobType);

        std::string getIPin() const;
        std::string getIPout() const;
        int getTime() const;
        char getJobType() const;
};