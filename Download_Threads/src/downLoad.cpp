#include "downLoad.h"
#include <assert.h>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstring> 
#include <string>

using namespace std;

downLoad::downLoad(int fd) : sockFd(fd), running(true)
{
    int i = 0;
    char tmp[5][50];
    string t;
    ifstream fin("record");
    char c;
    while (fin >> t && fin >> tmp[i]) 
    {
        record.insert(make_pair(t, tmp[i++]));
    }
}

void downLoad::stop() 
{
    running = false;
    my_lock.~mutex();
    for (auto& x : mergeThreads) 
    {
        x.join();
    }
    close(sockFd);
}


void downLoad::addRecord(Task job, int breakpoint) 
{
    auto it = record.find(job.base.from);
    if (it == record.end()) 
    {
        // cout << "emmmm未知的错误发生了" << endl;
    }

    int fd  = open(it->second, O_WRONLY);
    lseek(fd, job.inFo.Location, SEEK_SET);

    string s;
    char p[20];
    s = to_string(breakpoint);

    strcpy(p, s.c_str());
    int len = strlen (p);
    p[len + 1] = '\n';
    p[len + 2] = '\0';
    write(fd, p, sizeof(p));
    close(fd);
}


void downLoad::saveRecord() //将record内容写入文件
{
    remove("record");
    ofstream fout("record");

    for (auto x : record) 
    {
        fout << x.first << ' ' << x.second << '\n';
    }
    cout << "save successful" << endl;
}

void downLoad::jointFile(Task job) 
{

    int fd = open(job.base.to, ios::binary |O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1) 
    { 
        // cout << "Errno ===== " <<errno << endl;
        // fd = open("/home/lzj/test/tmp", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
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
            break;
        }
        mergeThreads.push_back(thread(&downLoad::jointFile, this, job));
    }
}

void downLoad::applyBreCon(Task& job, string flag) 
{
    // char* fileName = new char[50];
    char fileName[50];
    strcpy(fileName, job.base.from);
    char* str = "_tmp";
    strcat(fileName, str);

    auto it = record.find(job.base.from);
    if (it == record.end()) //此前无记录
    {
        record.insert(make_pair(job.base.from, fileName));
        saveRecord();
        job.base.isBreak = -1;
        int fd = open(fileName, O_RDONLY | O_CREAT, S_IWUSR | S_IRUSR); //创建临时文件
        close(fd);
    }
    if (flag == "1")    //这里如果文件没有内容用户还输入1,会有问题尚未解决
    {
        job.base.isBreak = 1;   //有断点续传的文件  
        ifstream fin(it->second);
        int i = 0;
        while (fin >>  job.breakPoint[i++]) 
        {
            
        }
    }
    if (record.size() >= 5)     //只保留五次记录     
    {
        auto it = record.begin();
        remove(it->second);    //删除文件
        record.erase(record.begin());   //删除第一个记录
        // delete[] it->second;    //释放内存

        saveRecord();   //写入文件
    }

    // delete[] fileName;
}

void downLoad::run() 
{
    std::thread t(&downLoad::recvFile, this);
    t.detach();
    string flag;

    // signal(SIGINT, SIG_IGN);

    while (running) 
    {
        memset(&buff, 0, sizeof(buff));

        std::cout << "输入想要下载的文件名:" << std::endl;
        std::cin >> buff.base.from;
        std::cout << "输入想要保存的地址:" << std:: endl;
        std::cin >> buff.base.to;
        std::cout << "想要用多少个线程下载 : " << std:: endl;
        std::cin >> buff.base.num;
        std::cout << "是否从继续下载(1/0) : " << std::endl;
        std::cin >> flag;

        if (!strcmp(buff.base.from, "0") && !strcmp(buff.base.to, "0") && buff.base.num == 0) 
        {
            stop();
            break;
        }
        else 
        {
            applyBreCon(buff, flag);  //有记录就申请断点续传 没有就添加
            int ret = send(sockFd, (void*)&buff, sizeof(buff), 0);
        }
    }
    cout << "-------------------客户端退出---------------------" << endl;
}