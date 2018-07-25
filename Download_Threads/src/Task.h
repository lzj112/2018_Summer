//声明任务变量
// #ifndef TASK_H
// #define TASK_H
class Task 
{
public:
    char* pathName;         //请求下载的文件路径
    char* To;               //请求保存的路径
    int Id;                 //标识本任务下载文件的第几部分
    int clientFd;           //客户端socketfd
    int Size;               //文件大小
    int Location;           //服务端(客户端)开始读取(下载)的位置
    int Bytes;              //本部分需要读取(下载)多少字节
    int num;                //客户端指定了多少个线程 num+1 == id时读到文件完
    char buff[256];         //缓冲区
};

class Buff 
{
public:
    char* from;
    char* to;
    int num;    //指定用多少线程
};
// #endif