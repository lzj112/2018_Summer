#include <iostream>
using namespace std;

int main() 
{
    int a, b, t;
    while (cin >> t) 
    {
        b = 0;
        while (t--) 
        {
            cin >> a;
            b += a;
        }
        cout << b << endl;
    }

    return 0;
}