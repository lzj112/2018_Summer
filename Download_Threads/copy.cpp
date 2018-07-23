#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <mutex>
#include <thread>
using namespace std;

const int MAX_THREADS = 5;
mutex g_lock;

typedef struct info 
{
    const char* from;
    const char* to;
    int id;
} info;

int get_size(const char* name) 
{
    struct stat s;
    int size = stat(name, &s);
    return s.st_size;
}

void Func(info info1) 
{
    ifstream fin;
    fin.open(info1.from, ios::binary);
    ofstream fout;
    fout.open(info1.to, ios::binary);

    int size = get_size(info1.from);
    //seekp:seek_put  seekg:seek_get  移动到第几个字节
    int location = size * info1.id / MAX_THREADS;
    fin.seekg(location);
    fout.seekp(location);
    
    string buff;
    int len = 0;
    int total = 0;

    while (fin.good() && !fin.eof()) //流是好的，未读完
    {
        getline(fin, buff);
        len = buff.size();
        total += len;

       /*g_lock.lock();
       cout << size << ' ' << info1.id << ' ' << len << endl;
       cout <<"location = " <<  location << endl;
        cout << "buff = " << buff << endl << endl;
        g_lock.unlock();
        */
        g_lock.lock();
        fout << buff; 
        fout << '\n';
        g_lock.unlock();

        if (total >= (size / MAX_THREADS)) 
        {
            break;
        }

    }
    fin.close();
    fout.close();
    g_lock.~mutex();
}

int main(int argc, char* argv[]) 
{
    creat(argv[2], 0777);
    truncate(argv[2], get_size(argv[1]));

    info info1;
    memset(&info1, 0, sizeof(info1));
    info1.from = argv[1];
    info1.to = argv[2];

    thread t[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) 
    {
        info1.id = i;
        t[i] = thread(Func, info1);
    }
    for (int i = 0; i < MAX_THREADS; i++) 
    {
        t[i].join();
    }

    return 0;
}