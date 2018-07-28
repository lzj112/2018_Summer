#include "ThreadPool.h"
#include <fcntl.h>
#include <cstring>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/socket.h>
#include <sys/sendfile.h>
using namespace std;

Pool::Pool() : DEFAULT(10), running(true), poolInFree(DEFAULT)
{
    for (int i = 0; i < DEFAULT; i++) 
    {
        threads.push_back(std::thread(&Pool::performTask, this));
    }
}

void Pool::incThreads() 
{
    for (int i = 0; i < 5; i++) //添加五个线程
    {
        threads.push_back(std::thread(&Pool::performTask, this));   
    }
}

void Pool::addTask(Task task) 
{
    syncQueue.Add(task);
    if (poolInFree == 0 && syncQueue.get_Size() > 6) //如果空闲线程为0且尚有6个任务等待
    {
        incThreads();   //添加线程
    }
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
void Pool::sendNoBreak(Task job) //读取文件,发送到客户端
{
    // cout << "here is Nobreak" << endl;
    int fd = open(job.base.from, ios::binary | O_RDONLY);  //以只读方式打开文件
    if (fd == -1) 
    {
        cout << "打开文件" << job.base.from << "失败" << endl; 
        return ;
    }
    
    int ret, id, sum, location;
    sum = 0;
    ret = 0;
    location = job.inFo.Location;
    if (job.base.num == (job.inFo.Id + 1)) //防止最后一部分按Bytes读,有剩余
    {
        while (ret = pread(fd, job.buff, sizeof(job.buff), location))  //读到文件尾 
        {
            job.inFo.ret = ret;
            job.base.isBreak += ret;

            int byte = send(job.inFo.clientFd, (void*)&job, sizeof(job), 0);
            assert(byte != 0);
            
            location += ret;
            job.inFo.writen += ret;
            memset(job.buff, 0, sizeof(job.buff));
        }
    }
    else //不是最后一部分的线程就按Bytes读取
    {
        while (sum < job.inFo.Bytes) 
        {
            ret = pread(fd, job.buff, sizeof(job.buff), location);   //从Location开始读

            sum += ret;
            location += ret;    //pread不会改变文件指针
            job.inFo.ret = ret;
            job.base.isBreak += ret;
            

            int byte = send(job.inFo.clientFd, (void*)&job, sizeof(job), 0);
            assert(byte != 0);

            job.inFo.writen += ret;
            memset(job.buff, 0, sizeof(job.buff));
        }
    }
    close(fd);
}


void Pool::sendFromBreak(Task job) 
{
    cout << "here is FromBreak" << endl;
    int fd = open(job.base.from, ios::binary | O_RDONLY);  //以只读方式打开文件
    if (fd == -1) 
    {
        cout << "打开文件" << job.base.from << "失败" << endl; 
        return ;
    }
    
    int ret, id, sum, location;
    sum = 0;
    ret = 0;
    location = job.breakPoint[job.inFo.Id];
    job.base.isBreak = location;
    job.inFo.Bytes = job.inFo.Location + job.inFo.Bytes - job.breakPoint[job.inFo.Id];
    if (job.base.num == (job.inFo.Id + 1)) //防止最后一部分按Bytes读,有剩余
    {
        while (ret = pread(fd, job.buff, 255, location))  //读到文件尾 
        {
            job.inFo.ret = ret;
            job.base.isBreak += ret;

            int byte = send(job.inFo.clientFd, (void*)&job, sizeof(job), 0);
            assert(byte != 0);
            sleep(1);
            
            location += ret;
            job.inFo.writen += ret;
            memset(job.buff, 0, sizeof(job.buff));
        }
    }
    else //不是最后一部分的线程就按Bytes读取
    {
        while (sum < job.inFo.Bytes) 
        {
            ret = pread(fd, job.buff, 255, location);   //从Location开始读

            sum += ret;
            location += ret;    //pread不会改变文件指针
            job.inFo.ret = ret;
            job.base.isBreak += ret;

            int byte = send(job.inFo.clientFd, (void*)&job, sizeof(job), 0);
            assert(byte != 0);
            sleep(1);

            job.inFo.writen += ret;
            memset(job.buff, 0, sizeof(job.buff));
        }
    }
    close(fd);
}

void Pool::judge(Task job) //判断是否是断点续传
{
    if (job.base.isBreak == -1) 
    {
        sendNoBreak(job);
    }
    else if (job.base.isBreak == 1) 
    {
        sendFromBreak(job);
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

        my_Lock.lock();     //空闲线程-1
        poolInFree--;
        my_Lock.unlock();

        if (!running)       //如果线程池停止运转了,结束线程
        {
            break;
        }
        
        judge(job);  //读取文件并发往客户端

        memset(&job, 0, sizeof(job));   //完成一次任务,清空
        my_Lock.lock();
        poolInFree++;       //空闲线程+1
        my_Lock.unlock();

    }
}
