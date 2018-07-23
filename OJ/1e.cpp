#include<iostream>
#include <iomanip>
using namespace std;

double t(char c, int a, int b) 
{
    if (c == '+') 
    {
        return a + b;
    }
    else if (c == '-') 
    {
        return a - b;
    }
    else if (c == '*') 
    {
        return a * b;
    }
    else 
    {
        return (double)a / (double)b;
    }

}

int main() 
{
    int n;
    int a, b;
    char c;
    cin >> n;
    double tmp;
    while (n--) 
    {
        cin >> c >> a >> b;
        tmp = t(c, a, b);
        // cout << tmp << ' ' << (int)tmp << endl;
        if ((tmp - (int)tmp) > 0 )
        {
            cout << fixed << setprecision(2) << t(c, a, b) << endl;
        }
        else 
        {
            cout << fixed << setprecision(0) << t(c, a, b) << endl;
        }
    }
}