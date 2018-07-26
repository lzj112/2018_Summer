#include "ThreadPool.h"
#include <fcntl.h>
#include <cstring>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/socket.h>
using namespace std;

Pool::Pool() : DEFAULT(10), running(true), poolInFree(DEFAULT)
{
    for (int i = 0; i < DEFAULT; i++) 
    {
        threads.push_back(std::thread(&Pool::performTask, this));
    }
}

void Pool::addTask(Task task) 
{
    syncQueue.Add(task);
}

void Pool::stopPool() 
{
   
    std::once_flag tmp;
    
    std::call_once(tmp, [this](){   //使任务队列唤醒所有线程
        syncQueue.stopAll();
    });

    std::call_once(tmp, [this](){   //等待所有线程退出
        running = false;
        for (auto& x : threads) 
        {
            x.join();
        }
    });
};

//使用pread函数,不移动文件指针,不造成竞争,不用上锁
void Pool::readFile(Task job) //读取文件,发送到客户端
{
    int fd = open(job.pathName, O_RDONLY);  //以只读方式打开文件
    if (fd == -1) 
    {
        cout << "打开文件" << job.pathName << "失败" << endl; 
        return ;
    }

    // off_t offset = lseek(fd, job.Location, SEEK_SET); //移动文件指针到本线程下载的开始
    // assert(offset != -1);
    int ret, id, sum, location;
    // char buff[256];
    sum = 0;
    ret = 0;
    location = job.Location;
    if (job.num == (job.Id + 1)) //防止最后一部分按Bytes读,有剩余
    {
        while (ret = pread(fd, job.buff, 255/*sizeof(job.buff)*/, location))  //读到文件尾 
        {
            // strcpy(job.buff, buff);
            job.ret = ret;
            int byte = send(job.clientFd, (void*)&job, sizeof(job), 0);
            assert(byte != 0);
            
            location += ret;
            job.writen += ret;
            memset(job.buff, 0, sizeof(job.buff));
            // memset(buff, 0, sizeof(buff));
        }
    }
    else //不是最后一部分的线程就按Bytes读取
    {
        while (sum < job.Bytes) 
        {
            ret = pread(fd, job.buff, 255/*sizeof(job.buff)*/, location);   //从Location开始读

            sum += ret;
            location += ret;    //pread不会改变文件指针
            job.ret = ret;
            // strcpy(job.buff, buff);
            int byte = send(job.clientFd, (void*)&job, sizeof(job), 0);
            assert(byte != 0);

            job.writen += ret;
            memset(job.buff, 0, sizeof(job.buff));
            // memset(buff, 0, sizeof(buff));
        }
    }
}

void Pool::performTask() //线程函数,循环等待获取任务
{
    Task job;
    char buff[256];
    int id;
    while (1) 
    {
        syncQueue.Take(job);    //没有取到任务即阻塞在此

        if (!running)       //如果线程池停止运转了,结束线程
        {
            break;
        }
        
        readFile(job);  //读取文件并发往客户端
        memset(&job, 0, sizeof(job));   //完成一次任务,清空
    }
}
