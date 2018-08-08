#include <iostream>
#include <array>
using namespace std;

array<long long, 32768> count = {0};
long long DP(int n) 
{
    // cout << n << endl;
    if (n == 1) 
    {
        return 1;
    }
    if (n == 2) 
    {
        return 2;
    }
    // if (n == 3) 
    // {
    //     return 3;
    // }
    count[n] = DP(n - 1);// + DP(n - 2);// + DP(n - 3);
    return count[n];
}

int main() 
{
    int n;
    while (cin >> n) 
    {
        // n = n * 10;
        cout << DP(n) << endl;
    }   
    return 0;
}