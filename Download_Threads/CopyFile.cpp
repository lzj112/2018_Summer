/*
Linux下c++　多线程文件拷贝
使用指定线程实现从文件的拷贝
*/
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <mutex>
#include <pthread.h>
using namespace std;

const int MAX_THREADS = 5;
mutex x;

typedef struct TAG_INFO 
{
    char* fromfile; //源地址
    char* tofile;   //目的地址
    int num;        //启动的第i+1个进程
} info;

//st_size的类型为__off_t 也就是long nit
int get_size(const char* filename) 
{
    struct stat st; //文件的信息
    memset(&st, 0, sizeof(st));
    stat(filename, &st);
    return st.st_size;
}

void* threadDL(void* param) 
{
    info info1 = *((info*)param);

    FILE* fin = fopen(info1.fromfile, "r+");
    FILE* fout = fopen(info1.tofile, "w+");

    int size = get_size(info1.fromfile);
    //将文件指针分别设置在每个线程要读和写的位置
    fseek(fin, size*(info1.num) / MAX_THREADS, SEEK_SET);
    fseek(fout, size*(info1.num) / MAX_THREADS, SEEK_SET);


    // int t = size / MAX_THREADS;
    char buff[1024] = {'\0'};
    int len = 0;
    int total = 0;
    while ((len = fread(buff, 1, sizeof(buff), fin)) > 0) 
    {
        // cout << "num === " << info1.num << "   buff == " << endl << buff << endl;
        fwrite(buff, 1, len, fout);
        memset(buff, 0, sizeof(buff));
        total += len;
        //如果读入的数据大于　文件总大小除线程总数　则停止读入，
        //因为每个线程要读或写的数据就等于文件总大小除线程总数
        //可能会多写入一些数据，　下一次写入时会覆盖多写入的数据，　所以不用担心
        if (total > size / MAX_THREADS) 
        {
            break;
        }

        
    }
    fclose(fin);
    fclose(fout);
}

int main(int argc, char* argv[]) 
{
    //先创建一个与文件1同样大小的文件
    creat(argv[2], 0777);
    truncate(argv[2], get_size(argv[1]));

    pthread_t pid[MAX_THREADS];
    info info1;
    int tmp;///////////////////////////////////
    //启动指定线程数的线程
    for (int i = 0; i < MAX_THREADS; i++) 
    {
        tmp = i;
        memset(&info1, 0, sizeof(info1));
        info1.fromfile = argv[1];
        info1.tofile = argv[2];
        info1.num = tmp;
        // cout << "tmp === " << tmp << endl;
        pthread_create(&pid[tmp], NULL, threadDL, (void*)&info1);
        sleep(1);
    }

    //等待线程结束
    for (int j = 0; j < MAX_THREADS; j++) 
    {
        //pthread_join不能用在创建进程的for循环中，否则创建的第一个进程会等待
        //第一个进程结束后创建第二个进程
        pthread_join(pid[j], NULL);
    }

    cout << "file copy success" << endl;
    return 0;
}
