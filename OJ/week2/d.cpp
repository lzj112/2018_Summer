#include <iostream>
#include <queue>
#include <cstring>
#include <vector>
#include <math.h>
using namespace std;

long long x;

bool flag = false;
bool check(long long t) 
{
    if (t % x)  
    {
        return false;
    }
    return true;
}

void BFS(int tx) 
{
    queue<long long> t;
    t.push(tx);

    while (!t.empty()) 
    {
        long long tmp = t.front();
        t.pop();

        if (check(tmp)) 
        {
            cout << tmp << endl;
            return ;
        }
        t.push(tmp * 10);
        t.push(tmp * 10 + 1);

    }
}

int main() 
{
    while (cin >> x && x != 0) 
    {
        BFS(1);
    }
}