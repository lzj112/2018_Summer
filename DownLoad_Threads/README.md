# 多线程下载模型

项目使用客户/服务器模式,使用多线程发送客户请求的文件
- 客户端发送请求下载的文件名(绝对路径),指定下载的线程数(可下载文本,图片,视频等,支持断点续传)
- 服务器使用 epoll, 将客户端发来的任务请求封装好添加到任务同步队列
- 同步队列使用 vector<Task> 存储
- 线程池每一个线程都在循环获取任务,任务队列无任务即阻塞
- 线程取到任务,执行`Pool::readFile`函数向客户端发送文件的部分
- 客户端收到服务端发来的部分文件写到同一个文件里拼接成一个完整的文件
- 添加定时器, 关闭那些长时间没有活动的 socket 连接


------------

### 主要数据结构 

> Task.h
    
    struct MyHead     //包头
    {
    int serviceType;        //服务类型 本项目目前只有下载文件, 断点续传
    unsigned int packetLength;    //数据包长度, 不含包头
    };

    struct Buff //服务器与客户端的基本通信结构
    {
    char from[50];      //请求下载的文件
    char to[50];        //请求保存的路径
    int num;            //指定用多少线程 num+1 == id时读到文件完
    int breakPoint[20];         //记录下载的每个线程的断点,也就是说最多支持20个线程下载
    int isBreak;        //断点
    };

    struct InFo  //下载过程所需信息
    {
    int Id;                     //标识本任务下载文件的第几部分
    int clientFd;               //客户端socketfd
    int Size;                   //文件大小
    int Location;               //服务端(客户端)开始读取(下载)的位置
    int writen;                 //客户端开始写的位置
    int ret;                    //服务器读取了多少字节
    int Bytes;                  //本部分需要读取(下载)多少字节
    };

    struct Task  //任务
    {
    Buff base;
    InFo inFo;
    char buff[4096];             //缓冲区
    };

    struct RequestMsg   //下载请求
    {
    MyHead head;
    Buff buff;
    };

    struct DownloadMsg  //下载消息体, 即目前只支持下载和断点续传服务
    {
    MyHead head;    //协议头
    Task body;      //协议体    
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
            {
                TaskSyncQueue::Add();
            }
        }
    }
}

Pool::performTask() //线程池线程函数,循环等待获取任务
{
    TaskQueue::Take();    //没有取到任务即阻塞在此 

    Pool::readFile();       //读取文件(该线程需要读取的部分)并发往客户端
}

//客户端
downLoad::run() //循环输入,send()
{
    downLoad::applyBreCon();    //send之前判断一下是否有断点记录,
    send();                     //没有就创建,有就拿到断点
}

downLoad::recvFile() //接收服务器发来的数据
{
    recv();
    mergeThreads.push_back(thread(&downLoad::jointFile, this, job));
    //recv到数据就开线程合并到同一个文件
}

downLoad::jointFile() 
{
    addRecord();    //添加断点
    //写入数据后,将新的断点写入文件
} 
```

| 头            |功能          |
| ------------ | ------------ |
|  Epoll.h     | 封装epoll    |
|  Task.h       | 主要的数据结构  |
|  TaskSyncQueue.h |任务同步队列   |
|  ThreadPool.h | 线程池  |
|downLoad.h     |负责客户端的发送接收|
|TimerClass.h    | 封装的定时器|
|TimerWheel.h   | 时间轮       |

----------
视频丢失数据
