#include <iostream>
#include <queue>
using namespace std;

int main() 
{
    priority_queue <char, vector<char>, greater<char> > tmp;
    char c;
    int flag = 0;
    while (1) 
    {
        for (int i = 0; i < 3; i++) 
        {
            cin >> c;
            tmp.push(c);
        }
        while (!tmp.empty()) 
        {
            if (!flag)
                cout << tmp.top();
            else 
                cout << ' ' << tmp.top();
            tmp.pop();
            flag++;
        }
        cout << endl;
        flag = 0;
    }
}