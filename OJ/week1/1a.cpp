#include <iostream>
#include <queue>
using namespace std;

int main() 
{
    priority_queue <char, vector<char>, greater<char> > tmp;
    char a, b, c;
    int flag = 0;
    while (cin >> a >> b >> c) 
    {
        // for (int i = 0; i < 3; i++) 
        // {
        //     cin >> c;
        //     tmp.push(c);
        // }
        tmp.push(a);
        tmp.push(b);
        tmp.push(c);
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