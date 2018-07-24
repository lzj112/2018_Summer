#include <iostream>
using namespace std;

int main() 
{
    int n;
    float a, b, c;
    cin >> n;
    while (n--) 
    {
        cin >> a >> b >> c;
        if ((a + b) > c && (a + c) > b) 
        {
            cout << "YES" << endl;
        }
        else 
        {
            cout << "NO" << endl;
        }
    }
}