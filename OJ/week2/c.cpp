#include <iostream>
#include <queue>
#include <cstring>
#include <vector>
using namespace std;
bool pre[8] = {false};
char board[8][8];
int count = 0;
int pieces = 0;
int n, k;

bool check(int x, int y) 
{
    if (board[x][y] == '#' && pre[y] == false) 
    {
        return true;
    }
    return false;
}

bool flag = false;
void DFS(int x) 
{
    if (pieces == k) 
    {
        count++;
        return ;
    }
    for (int i = x; i < n; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            flag = check(i, j);
            if (flag) 
            {
                pre[j] = true;
                pieces++;
                DFS(i+1);
                pieces--;
                pre[j] = false;
            }
        }
    }
}


int main() 
{
    while (cin >> n >> k) 
    {
        if (n == -1 && k == -1) 
        {
            break;
        }
        memset(board, 0, sizeof(board));
        for (int i = 0; i < n; i++) 
        {
            for (int j = 0; j < n; j++) 
            {
                cin >> board[i][j];
            }
        }
        DFS(0);
        cout << count << endl;
        count = 0;
    }
}