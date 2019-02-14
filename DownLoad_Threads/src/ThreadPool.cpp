#include "ThreadPool.h"
#include <fcntl.h>
#include <cstring>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
using namespace std;

Pool::Pool() : running(true), poolInFree(MAX_NUMBER)
{
    for (int i = 0; i < MAX_NUMBER; i++)
    {
        /*
emplace_back比push_back更加简洁,快速,直接原地构造
*/
        threads.emplace_back([this] {
            performTask();
        });
        // threads.emplace_back(&Pool::performTask, this);
        // threads.push_back(std::thread(&Pool::performTask, this));
    }
}

void Pool::incThreads()
{
    for (int i = 0; i < 5; i++) //添加五个线程
    {
        threads.emplace_back([this] {
            performTask();
        });
        // threads.push_back(std::thread(&Pool::performTask, this));
    }
}

void Pool::addTask(DownloadMsg task)
{
    syncQueue.Add(task);
    if (poolInFree == 0 && syncQueue.get_Size() > 6) //如果空闲线程为0且尚有6个任务等待
    {
        incThreads(); //添加线程
    }
}

void Pool::stopPool()
{

    std::once_flag tmp;

    std::call_once(tmp, [this] { //使任务队列唤醒所有线程
        syncQueue.stopAll();
    });

    std::call_once(tmp, [this] { //等待所有线程退出
        running = false;
        for (auto &x : threads)
        {
            x.join();
        }
    });
};

int Pool::my_pread(int fd, int location, DownloadMsg &job)
{
    int ret = pread(fd, job.body.buff, sizeof(job.body.buff), location);
    // job.body.inFo.ret = ret;
    // job.body.base.isBreak += ret;
    // job.head.packetLength = sizeof(job.body);
    int size = job.body.base.isBreak;
    job =
        {
            .body.inFo.ret = ret,
            .body.base.isBreak = size + ret,
            .head.packetLength = sizeof(job.body),
        };
    int byte = send(job.body.inFo.clientFd, (void *)&job, sizeof(job), 0);
    if (byte == 0)
    {
        if (errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK)
            return -1;
    }
    location += ret;
    job.body.inFo.writen += ret;
    memset(job.body.buff, 0, sizeof(job.body.buff));
    return ret;
}
//使用pread函数,不移动文件指针,不造成竞争,不用上锁
void Pool::sendNoBreak(DownloadMsg job) //读取文件,发送到客户端
{
    int fd = open(job.body.base.from, ios::binary | O_RDONLY); //以只读方式打开文件
    if (fd == -1)
    {
        cout << "打开文件" << job.body.base.from << "失败" << endl;
        return;
    }

    int ret, id, sum, location;
    sum = 0;
    ret = 0;
    location = job.body.inFo.Location;
    if (job.body.base.num == (job.body.inFo.Id + 1)) //防止最后一部分按Bytes读,有剩余
    {
        do
        {
            ret = my_pread(fd, location, job);
        } while (ret != 'EOF');
    }
    else //不是最后一部分的线程就按Bytes读取
    {
        do
        {
            ret = my_pread(fd, location, job);
            sum += ret;
        } while (sum < job.body.inFo.Bytes);
    }
    close(fd);
}

void Pool::sendFromBreak(DownloadMsg job)
{
    int fd = open(job.body.base.from, ios::binary | O_RDONLY); //以只读方式打开文件
    if (fd == -1)
    {
        cout << "打开文件" << job.body.base.from << "失败" << endl;
        return;
    }

    int ret, id, sum, location;
    sum = 0;
    ret = 0;
    int tmp = job.body.base.breakPoint[job.body.inFo.Id];
    if (tmp)
    {
        location = tmp;
    }
    else
    {
        location = job.body.inFo.Location;
    }

    job.body.base.isBreak = location;
    job.body.inFo.Bytes = job.body.inFo.Location + job.body.inFo.Bytes - location;
    if (job.body.base.num == (job.body.inFo.Id + 1)) //防止最后一部分按Bytes读,有剩余
    {
        do
        {
            ret = my_pread(fd, location, job);
        } while (ret != 'EOF');
    }
    else //不是最后一部分的线程就按Bytes读取
    {
        do
        {
            ret = my_pread(fd, location, job);
            sum += ret;
        } while (sum < job.body.inFo.Bytes);
    }
    close(fd);
}
/*
上面两个函数在发送方面有重复的代码
应该考虑封装起来
*/

void Pool::judge(DownloadMsg job) //判断是否是断点续传
{
    if (job.body.base.isBreak != 1)
    {
        sendNoBreak(job);
    }
    else if (job.body.base.isBreak == 1)
    {
        sendFromBreak(job);
    }
}

void Pool::performTask() //线程函数,循环等待获取任务
{
    DownloadMsg job;
    char buff[256];
    int id;
    while (1)
    {
        syncQueue.Take(job); //没有取到任务即阻塞在此
        my_Lock.lock();
        poolInFree--; //空闲线程-1
        my_Lock.unlock();

        if (!running) //如果线程池停止运转了,结束线程
        {
            break;
        }

        judge(job); //读取文件并发往客户端

        memset(&job, 0, sizeof(job)); //完成一次任务,清空
        my_Lock.lock();
        poolInFree++; //空闲线程+1
        my_Lock.unlock();
    }
}
