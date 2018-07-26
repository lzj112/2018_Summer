#include "downLoad.h"
#include <assert.h>
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

void downLoad::jointFile(Task job) 
{

    int fd = open(job.To, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    assert(fd != -1);
    
    // job.buff[256] = '\0';
    // cout << "收到文件 from :" << job.Id << ' ' <<  job.writen << '\n' << job.buff << endl;
    // if (job.num != (job.Id + 1))
    pwrite(fd, job.buff, /*strlen(job.buff)*/job.ret/*255*/, job.writen);
    /*else 
    {
        cout << strlen(job.buff) << endl;
        cout << job.buff << endl;
         job.buff[strlen(job.buff)] = '\0';*/
        // pwrite(fd, job.buff, strlen(job.buff)  /*255*/, job.writen);
    // }  
}

void downLoad::recvFile() 
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

        jointFile(job);
    }
}

void downLoad::run() 
{
    std::thread t(&downLoad::recvFile, this);
    t.detach();
    
    while (1) 
    {
        memset(&buff, 0, sizeof(buff));

        std::cout << "输入想要下载的文件名:" << std::endl;
        std::cin >> buff.from;
        std::cout << "输入想要保存的地址:" << std:: endl;
        std::cin >> buff.to;
        std::cout << "想要用多少个线程下载 : " << std:: endl;
        std::cin >> buff.num;

//mock
        // strcpy(buff.from, "memory.c");
        // strcpy(buff.to, "/home/lzj/1.c");
        // buff.num = 5;
        send(sockFd, (void*)&buff, sizeof(buff), 0);
    }
}
