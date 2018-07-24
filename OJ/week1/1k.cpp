#include <iostream>
#include <cmath>
using namespace std;

int t(int d) 
{
    for (int i = 2; i <= sqrt(d); i++) 
    {
        if (d % i == 0) 
        {
            return 0;
        }
    }
    return 1;
}
int main() 
{
    int x, y, q, flag;
    while (cin >> x >> y) 
    {
        flag = 0;
        if (x == 0 && y == 0)
        {
            break;
        }
       
        for (int i = x; i <= y; i++) 
        { 
            q = i * i + i + 41;
            if (!t(q)) 
            {
                cout << "Sorry" << endl;
                flag++;
                break;
            }
        }
        if (!flag)
            cout << "OK" << endl;
    }
}