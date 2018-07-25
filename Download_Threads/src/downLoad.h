#include "Task.h"
#include <thread>

class downLoad 
{
private:
    Buff buff;
    int fd;
    std::thread t;
    bool running;
public:
    downLoad(int sockfd);
    void recvFile();
    void jointFile(Task job);
    void run();
};
