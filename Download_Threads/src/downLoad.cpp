#include "downLoad.h"
#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

downLoad::downLoad(int sockfd) : fd(sockfd), running(true)
{

}

void downLoad::jointFile(Task job) 
{
    int fd = open(job.To, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    int bytes = 0;
    int location = job.Location;
    pwrite(fd, job.buff, 255, location);
}

void downLoad::recvFile() 
{
    Task job;
    while (running) 
    {
        int ret = recv(fd, (void*)&job, sizeof(job), 0);
        assert(ret < 0);
        if (ret == 0) 
        {
            std::cout << "与服务器断开连接" << std::endl;
        }
        jointFile(job);
        // std::thread t(jointFile, job);
    }
}

void downLoad::run() 
{
    std::thread t(recvFile);
    t.detach();

    char* from;
    char* to;
    int num;
    while (1) 
    {
        std::cout << "输入想要下载的文件名:" << std::endl;
        std::cin >> from;
        std::cout << "输入想要保存的地址:" << std:: endl;
        std::cin >> to;
        std::cout << "想要用多少个线程下载 : " << std:: endl;
        std::cin >> num;
        strcpy(buff.from, from);
        strcpy(buff.to, to);
        buff.num = num;

        send(fd, (void*)&buff, sizeof(buff), 0);
    }
}
