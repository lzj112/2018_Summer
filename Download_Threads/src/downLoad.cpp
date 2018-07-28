#include "downLoad.h"
#include <assert.h>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstring> 

using namespace std;

downLoad::downLoad(int fd) : sockFd(fd), running(true)
{

}

void downLoad::stop() 
{
    running = false;
    my_lock.~mutex();
    for (auto x : record) 
    {
        delete[] x.second;
    }
    for (auto& x : mergeThreads) 
    {
        x.join();
    }
}


void downLoad::addRecord(Task job, int breakpoint) 
{
    auto it = record.find(job.base.from);
    if (it == record.end()) 
    {
        // cout << "emmmm未知的错误发生了" << endl;
    }
    ofstream fout(it->second);
    if (!fout) 
    {
        // cout << "未知的错误发生了" << endl;
    }
    else 
    {
        fout << endl;
        fout << breakpoint;
    }
}

void downLoad::jointFile(Task job) 
{

    int fd = open(job.base.to, ios::binary |O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1) 
    { 
        cout << "Errno ===== " <<errno << endl;
        fd = open("/home/lzj/test/tmp", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    }
        
    pwrite(fd, job.buff, job.inFo.ret, job.inFo.writen);
    close(fd);
    
    my_lock.lock();
    addRecord(job, job.base.isBreak);    //添加断点
    my_lock.unlock();
}

void downLoad::recvFile() //接收服务器发来的数据
{
    Task job;
    
    while (running) 
    {   
        memset(&job, 0, sizeof(job));

        int ret = recv(sockFd, (void*)&job, sizeof(job), 0);
        assert(ret >= 0);
        if (ret == 0) 
        {
            std::cout << "与服务器断开连接" << std::endl;
        }

        mergeThreads.push_back(thread(&downLoad::jointFile, this, job));
        // jointFile(job); //合并文件
    }
}

void downLoad::applyBreCon(Task& job) 
{
    char* fileName = new char[50];
    strcpy(fileName, job.base.from);
    char* str = "_tmp";
    strcat(fileName, str);

    auto it = record.find(job.base.from);
    if (it == record.end()) //此前无记录
    {
        record.insert(make_pair(job.base.from, fileName));
        job.base.isBreak = -1;
        int fd = open(fileName, O_RDONLY | O_CREAT, S_IWUSR | S_IRUSR); //创建临时文件
    }
    else 
    {
        job.base.isBreak = 1;   //有断点续传的文件  
        ifstream fin(it->second);
        int i = 0;
        while (!fin.eof()) 
        {
            fin >>  job.breakPoint[i++];
        }
    }

    if (record.size() >= 5)     //只保留五次记录     
    {
        auto it = record.begin();
        remove(it->second);    //删除文件
        record.erase(record.begin());   //删除第一个记录
        delete[] it->second;    //释放内存
    }
}

void downLoad::run() 
{
    std::thread t(&downLoad::recvFile, this);
    t.detach();
    
    signal(SIGINT, SIG_IGN);
    while (running) 
    {
        memset(&buff, 0, sizeof(buff));

        std::cout << "输入想要下载的文件名:" << std::endl;
        std::cin >> buff.base.from;
        std::cout << "输入想要保存的地址:" << std:: endl;
        std::cin >> buff.base.to;
        std::cout << "想要用多少个线程下载 : " << std:: endl;
        std::cin >> buff.base.num;

        if (!strcmp(buff.base.from, "0") && !strcmp(buff.base.to, "0") && buff.base.num == 0) 
        {
            stop();
        }

        applyBreCon(buff);  //有记录就申请断点续传 没有就添加
        send(sockFd, (void*)&buff, sizeof(buff), 0);
    }
    cout << "-------------------客户端退出---------------------" << endl;
}