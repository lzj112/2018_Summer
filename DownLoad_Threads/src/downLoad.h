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
    RequestMsg request;
    int sockFd;
    std::thread t;
/*
mutex资源尝试使用RAII吧
*/
    std::mutex my_lock;
    bool running;
    std::map<std::string, char*> record;    //记录文件名对应的保存断点的数组
    std::vector<std::thread> mergeThreads;    //多线程合并文件
    bool isStop;
public:
    downLoad(int fd);
    void recvFile();
    void jointFile(DownloadMsg recvTmp);
    void run();
    bool stop();
    void addRecord(DownloadMsg recvTmp);
    void applyBreCon(RequestMsg& job, std::string flag); //查看有无断点 
    void saveRecord();  
    void turnTo(std::string a, std::string b, std::string c);
    int recvFrom(int fd, void* ptr, int n);
};

#endif
