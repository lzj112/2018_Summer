//声明任务变量
#ifndef TASK_H
#define TASK_H

class Buff //服务器与客户端的基本通信结构
{
public:
    char from[50];      //请求下载的文件
    char to[50];        //请求保存的路径
    int num;            //指定用多少线程 num+1 == id时读到文件完
};

class InFo  //下载过程所需信息
{
public:
    int Id;                     //标识本任务下载文件的第几部分
    int clientFd;               //客户端socketfd
    int Size;                   //文件大小
    int Location;               //服务端(客户端)开始读取(下载)的位置
    int writen;                 //客户端开始写的位置
    int ret;                    //服务器读取了多少字节
    int Bytes;                  //本部分需要读取(下载)多少字节
    int breakPoint;             //文件下载断点 用来实现断点续传
};

class Task  //任务
{
public:
    Buff base;
    InFo inFo;
    char buff[256];             //缓冲区
};

#endif