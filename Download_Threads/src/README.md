# 多线程下载
- 客户端发送请求下载的文件名(绝对路径),指定下载的线程数
- 服务器使用epoll,将客户端发来的任务请求封装好添加到任务同步队列
- 同步队列使用vector<Task>存储
- 线程池每一个线程都在循环获取任务,任务队列无任务即阻塞
- 线程取到任务,执行`Pool::readFile`函数向客户端发送文件的部分
- 客户端收到服务端发来的部分文件写到同一个文件里拼接成一个完整的文件


------------

### 重要数据结构 


    class Task           //任务
    {
    public:
        char pathName[20];         //请求下载的文件路径
        char To[20];               //请求保存的路径
        int Id;                 //标识本任务下载文件的第几部分
        int clientFd;           //客户端socketfd
        int Size;               //文件大小
        int Location;           //服务端(客户端)开始读取(下载)的位置
        int Bytes;              //本部分需要读取(下载)多少字节
        int num;                //客户端指定了多少个线程 num+1 == id时读到文件完
        char buff[256];         //缓冲区
    };
    
    class Buff          //服务器与客户端的通信结构
    {
    public:
        char from[20];
        char to[20];
        int num;    //指定用多少线程
    };

### 主要逻辑

```cpp

//服务器部分
Epoll::epoll_Run()  //循环 监听 
{
    Epoll::epollEt()    //有可读事件
    {
        void Epoll::assignedTask() //读取客户端的下载请求并封装任务
        {
            Pool::addTask();        //添加任务到任务队列
        }
    }
}

Pool::performTask() //线程池线程函数,循环等待获取任务
{
    TaskQueue::Take(job);    //没有取到任务即阻塞在此 

    Pool::readFile();       //读取文件(该线程需要读取的部分)并发往客户端
}

//客户端设计较为简单,主线程负责不停向服务器申请下载,而下面的接收线程每接收到一部分就写到同一个文件中的那一块
downLoad::recvFile()   
```

| 头  |功能   |
| ------------ | ------------ |
|  Epoll.h | 封装epoll  |
|  Task.h | 两个重要的数据结构  |
|  TaskSyncQueue.h |任务同步队列   |
|  ThreadPool.h | 线程池  |
|downLoad.h|负责客户端的发送接收|


----------
继续完善ing....
