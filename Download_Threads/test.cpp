#include <iostream>
#include <thread>
#include <vector>
using namespace std;

void Func() 
{
    cout << "hello world" << endl;
}

int main() 
{
    vector<thread> tmp;
    // thread tmp[5];
 
    for (int i = 0; i < 5; i++) 
    {
        tmp.push_back(thread(Func));
        // tmp[i] = thread(Func);       这样初始化直接凉凉,[] at操作都不检查边界,这就不是插入操作
    }
    cout << tmp.size();
    
    for (auto& x : tmp) 
    {
        cout << "here" << endl;
    }
    for (auto x = tmp.begin(), end = tmp.end(); x != end; x++) 
    {
        x->join();
    }
    return 0;
}