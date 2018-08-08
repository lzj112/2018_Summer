//声明任务变量
#ifndef TASK_H
#define TASK_H


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
#endif