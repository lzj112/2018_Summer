#include <iostream>
#include <queue>
#include <vector>
using namespace std;

int dir[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
bool pre[5][5] = {false};
int maze[5][5];

struct Node 
{
    int x, y;
    int id;
    Node* pre;
} path[100];

bool check(int i, int j) 
{
    if (i >= 0 && i < 5 && j >= 0 && j < 5 && maze[i][j] != 1 && pre[i][j] == false) 
    {
        return true;
    }
    return false;
}

int BFS()
{
   
    queue<Node> t;
    Node q;
    Node* ptr = &path[0];
    t.push(path[0]);
    bool flag = false;
    pre[0][0] = true; 
    int j = 1;

    while (!t.empty()) 
    {
        q = t.front();
        t.pop();
        ptr = &path[q.id];

        for (int i = 0; i < 4; i++) 
        {
            int tx = q.x + dir[i][0];
            int ty = q.y + dir[i][1];
            flag = check(tx, ty);
            if (flag) 
            { 
                path[j].x = tx, path[j].y = ty, path[j].id = j, path[j].pre = ptr;
                j++;

                if (tx == 4 && ty == 4) 
                {
                    return --j;
                }
                pre[tx][ty] = true;
                t.push(path[j-1]);
            }
        } 
    }
    return --j;
}

int main() 
{
    for (int i = 0; i < 5; i++) 
    {
        for (int j = 0; j < 5; j++) 
        {
            cin >> maze[i][j];
        }
    }
    path[0].x = 0;
    path[0].y = 0;
    path[0].id = 0;
    path[0].pre = NULL;

    int j = BFS();
    Node* ptr = &path[j];

    vector<Node *> tmp;
    for (;ptr;) 
    {
        tmp.push_back(ptr);
        ptr = ptr->pre;
    }
    vector<Node *>::reverse_iterator it = tmp.rbegin();
    // auto end = tmp.rend();
    for (; it != tmp.rend(); it++) 
    {
        cout << '(' << (*it)->x << ", " << (*it)->y << ")" << endl;
    }
}
