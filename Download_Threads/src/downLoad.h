#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "Task.h"
#include <thread>

class downLoad 
{
private:
    Buff buff;
    int sockFd;
    std::thread t;
    bool running;
public:
    downLoad(int fd);
    void recvFile();
    void jointFile(Task job);
    void run();
    void stop();
};
#endif