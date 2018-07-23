#include <iostream>
using namespace std;

int t(int a, int b) 
{
    int i;
    int t = a > b ? b : a;
    for (i= t; ; i++ ) 
    {
        if ( i % a == 0 && i % b ==0 )     
        {
            break;
        }
    }
    return i;
}
int main() 
{
    int a, b;
    while (cin >> a >> b) 
    {
        cout << t(a, b) << endl;
    }
}

