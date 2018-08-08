#include <iostream>
#include <queue>
#include <cstring>
#include <vector>
using namespace std;

int s, n, m;

bool check(int a, int b)  
{
    if (b == a && ((a + b) == s)) 
    {
        return true;
    }
    return false;
}

int count;
void BFS(int a, int b) 
{
    queue<pair<int, int> > t;
    t.push(make_pair(a, b));
    pair<int, int> tmp;
    int tn, tm;

    while (!t.empty()) 
    {
        tmp = t.front();
        t.pop();
        
        if 


    }
    cout << "NO" << endl;
}

int main() 
{
    while (cin >> s >> n >> m) 
    {
        count = 0;
        if (s == 0 && n == 0 && m == 0) 
        {
            break;
        }
        BFS(0, 0);
    }
}