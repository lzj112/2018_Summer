#include <iostream>
#include <queue>
#include <cstring>
#include <vector>
using namespace std;

char map[100][100];
bool pre[100][100] = {false};
int m, n;
int dir[8][2] = 
{
    {0, 1}, {0, -1}, {1, 0}, {-1, 0},
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
};

int count = 0;

bool check(int x, int y)
{
    if (x >= m || y >= n || x < 0 || y < 0 || pre[x][y] == true || map[x][y] == '*') 
    {
        return false;
    }
    return true;
}

int tx, ty;
bool flag = false;
void DFS(int x, int y) 
{
    for (int i = 0; i < 8; i++) 
    {
        tx = x + dir[i][0];
        ty = y + dir[i][1];
        flag = check(tx, ty);
        if (flag) 
        {
            pre[tx][ty] = true;
            DFS(tx, ty);
        }
        
    }               
}



int main() 
{
    while (cin >> m >> n) 
    {
        memset(map, 0, sizeof(map));
        memset(pre, 0, sizeof(pre));
        if (m == 0) 
        {
            break;
        }
        for (int i = 0; i < m; i++) 
        {
            for (int j = 0; j < n; j++) 
            {
                cin >> map[i][j];
            }
        }

        for (int i = 0; i < m; i++) 
        {
            for (int j = 0; j < n; j++) 
            {
                if (map[i][j] == '@' && pre[i][j] == false)
                {
        // cout << "tx===" << i << "  ty==" << j << ' ' << map[i][j] << endl;
                    count++;
                    DFS(i, j);
                }
            }
        }
        cout << count << endl;
        count = 0;
    }
}