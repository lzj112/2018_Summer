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
#include <system_error>

using namespace std;

downLoad::downLoad(int fd) : sockFd(fd), running(true)
{
/*
是不是应该将i和t的定义声明放到while循环前
*/
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

bool downLoad::stop() 
{
    cout << "STOP!" << endl;
    running = false;
    my_lock.~mutex();
    
    //等待线程结束
    for (auto& x : mergeThreads) 
    {
        x.join();
    }
/*
清空mergeThreads, 可以把end的赋值放前面, 这样就不用每次都赋值一遍
*/
    // for (auto it = mergeThreads.begin(); it != mergeThreads.end(); it++) 
    // {
    //     it = mergeThreads.erase(it);
    // }
    close(sockFd);
    return true;
}


void downLoad::addRecord(DownloadMsg recvTmp) 
{
    auto it = record.find(recvTmp.body.base.from);
    if (it == record.end()) 
    {
        // cout << "emmmm未知的错误发生了" << endl;
    }

    int fd  = open(it->second, O_WRONLY);
    lseek(fd, recvTmp.body.inFo.Location, SEEK_SET);

    string s;
    char p[20];
    s = to_string(recvTmp.body.base.isBreak);

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
}

void downLoad::jointFile(DownloadMsg recvTmp) 
{

    int fd = open(recvTmp.body.base.to, ios::binary |O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd == -1) 
    { 
        // cout << "Errno ===== " <<errno << endl;
        // fd = open("/home/lzj/test/tmp", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    }
        
    pwrite(fd, recvTmp.body.buff, recvTmp.body.inFo.ret, recvTmp.body.inFo.writen);
    close(fd);

/*
lock_guard 是不是更好
*/
    my_lock.lock();
    addRecord(recvTmp);    //添加断点
    my_lock.unlock();
}

void downLoad::recvFile() //接收服务器发来的数据
{
/*
在使用前声明定义... 是为了防止使用前代码GG造成浪费么
*/
    DownloadMsg recvTmp;
    int sum, len, ret;
    bool flag = false;

    while (running) 
    {   
        sum = 0;
        ret = 0;
        memset(&recvTmp, 0, sizeof(recvTmp));
        while (sum < sizeof(MyHead)) //先收包头
        {
            int ret = recv(sockFd, (void*)(&recvTmp + sum), sizeof(MyHead), 0);
            if (ret == 0) 
            {
                std::cout << "与服务器断开连接1" << std::endl;
                flag = stop();
                break;
            }
            sum += ret;
        }
        sum = 0, len = recvTmp.head.packetLength;
        while (sum < len)  //再收包体
        {
            int ret = recv(sockFd, (void*)(&recvTmp.body + sum), sizeof(Task), 0);
            if (ret == 0) 
            {
                std::cout << "与服务器断开连接2" << std::endl;
                flag = stop();
                break;
            }
            sum += ret;
        }
        if (!flag)
        {
            mergeThreads.emplace_back([this, recvTmp] {
                jointFile(recvTmp);
            });
            // mergeThreads.push_back(thread(&downLoad::jointFile, this, recvTmp));
        }
    }
}

void downLoad::applyBreCon(RequestMsg& job, string flag) 
{
    char fileName[50];
    strcpy(fileName, job.buff.from);
    const char* str = "_tmp";
    strcat(fileName, str);

    auto it = record.find(job.buff.from);
    if (it == record.end()) //此前无记录
    {
        record.insert(make_pair(job.buff.from, fileName));
        saveRecord();
        job.head.serviceType = -1;  //普通下载请求
        int fd = open(fileName, O_RDONLY | O_CREAT, S_IWUSR | S_IRUSR); //创建临时文件
        close(fd);
    }
    else if (flag == "1")    //这里如果文件没有内容用户还输入1,会有问题尚未解决
    {
        job.head.serviceType = 1;   //有断点续传的文件  
        ifstream fin(it->second);
        int i = 0;
        while (fin >>  job.buff.breakPoint[i++]) 
        {
        }
    }
    else if (record.size() >= 5)     //只保留五次记录     
    {
        auto it = record.begin();
        remove(it->second);    //删除文件
        record.erase(record.begin());   //删除第一个记录
        saveRecord();   //写入文件
    }
}

void downLoad::turnTo(string a, string b, string c) 
{
    strcpy(request.buff.from, a.c_str());
    strcpy(request.buff.to, b.c_str());

    if (c[0] < '0' || c[0] > '9' && c[1] < '0' && c[1] > '9')
    {
        request.buff.num = 0;
    }
    else 
    {
        request.buff.num = stoi(c);
    }
}

void downLoad::run() 
{
    std::thread t(&downLoad::recvFile, this);
    t.detach();
    string flag;
    // signal(SIGINT, SIG_IGN);

    string fromTmp, toTmp, numTmp, flagTmp;
    while (running) 
    {
        memset(&request, 0, sizeof(request));
        request.head.serviceType = -1;  //普通下载请求

        std::cout << "输入想要下载的文件名:" << std::endl;
        std::cin >> fromTmp;
        std::cout << "输入想要保存的地址:" << std:: endl;
        std::cin >> toTmp;
        std::cout << "想要用多少个线程下载 : " << std:: endl;
        std::cin >> numTmp;
        std::cout << "是否从继续下载(1/0) : " << std::endl;
        std::cin >> flag;

        if (!running) 
        {
            break;
        }
        if (fromTmp.size() >49 || toTmp.size() > 49 || numTmp.size() > 2) 
        {
            std::cout << "输入有误" << endl;
            continue;
        }
        else 
        {
            turnTo(fromTmp, toTmp, numTmp);
        }
        if (!strcmp(request.buff.from, "0") && !strcmp(request.buff.to, "0")) 
        {
            stop();
            break;
        }
        else if (request.buff.num == 0) 
        {
            continue;
        }
        else 
        {
            if (!running) 
            {
                break;
            }
            applyBreCon(request, flag);  //有记录就申请断点续传 没有就添加
            request.head.packetLength = sizeof(request.buff);
            int ret = send(sockFd, (void*)&request, sizeof(request), 0);
            if (ret == 0) 
            {
                stop();
            }
        }
    }
    cout << "-------------------客户端退出---------------------" << endl;
}