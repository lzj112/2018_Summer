#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include "Task.h"
#include <thread>
#include <string>
#include <mutex>
#include <map>
#include <vector>

class downLoad 
{
private:
    Task buff;
    int sockFd;
    std::thread t;
    std::mutex my_lock;
    bool running;
    std::map<std::string, char*> record;    //记录文件名对应的保存断点的数组
    std::vector<std::thread> mergeThreads;    //多线程合并文件
public:
    downLoad(int fd);
    void recvFile();
    void jointFile(Task job);
    void run();
    void stop();
    void addRecord(Task job, int breakpoint);
    // void delRecord();
    void applyBreCon(Task& job); //查看有无断点 
    // void print();        
};

#endif
