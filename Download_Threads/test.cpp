#include <iostream>
#include <thread>
using namespace std;


void Func() 
{
    cout << "hello world!";
}

int main() 
{
    thread t[3];
    for (int i = 0; i < 3; i++) 
    {
        t[i] = thread(Func);
    }
    for (int i = 0; i < 3; i++) 
    {
        t[i].join();
    }
}