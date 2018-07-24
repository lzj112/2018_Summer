#include "ThreadPool.h"
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h> 
using namespace std;

Pool::Pool() : DEFAULT(10), running(true)
{
    for (int i = 0; i < DEFAULT; i++) 
    {
        freePool.push_back(std::thread(performTask));
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
        for (auto& x : freePool) 
        {
            x.join();
        }
    });
};

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
    int ret, id, sum;
    char buff[256];
    if (job.num == (job.Id + 1)) //防止最后一部分按Bytes读,有剩余
    {

    }
    else //不是最后一部分的线程就按Bytes读取
    {

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
        if (!running)
        {
            break;
        }
        
        readFile(job);  //读取文件并发往客户端
    }
}
