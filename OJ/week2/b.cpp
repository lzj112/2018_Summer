#include <iostream>
#include <queue>
#include <vector>
using namespace std;
int dir[2] = {1, -1};
bool pre[200000] = {false};
int n, k;

bool check(int x) 
{
    if (x >= 200000 || x < 0 || pre[x] == true) 
    {
        return false;
    }
    return true;
}

vector<pair<int, int> > timeCount;
queue<int> path;

void BFS() 
{
    
    timeCount.push_back(make_pair(n, -1));
    path.push(n);
    pre[n] = true;
    int tmp;
    bool flag = false;

    while (!path.empty()) 
    {
        tmp = path.front();
        path.pop();

        if (tmp == k) 
        {
            return ;
        }
        for (int i = 0; i < 2; i++) 
        {
            int t = tmp + dir[i];
            flag = check(t);
            if (flag) 
            {
                path.push(t);
                timeCount.push_back(make_pair(t, tmp));
                if (t == k) 
                {
                    return ;
                }
                pre[t] = true;
            }
            
        }
        int t = tmp * 2;
        flag = check(t);
        if (flag) 
        {
            path.push(t);
            timeCount.push_back(make_pair(t, tmp));
            if (t == k) 
            {
                return ;
            }
            pre[t] = true;
        }
    }

}

int main() 
{
    cin >> n >> k;
    BFS();
    vector<pair<int, int> >::reverse_iterator it = timeCount.rbegin();
    int flag = it->second;
    int count = 0;
    for (; it != timeCount.rend(); it++) 
    {
        if (flag == it->first) 
        {
            count++;
            flag = it->second;
        }
    }
    cout << count << endl;;

    return 0;
}