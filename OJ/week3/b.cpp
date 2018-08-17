#include <iostream>
#include <array>
using namespace std;

array<long long, 32768> count = {0};
void DP() 
{
    for (int i = 1; i <= 3; i++) 
    {
        for (int j = i; j < 32768; j++) 
        {
            count[j] += count[j - i];
        }
    }
}

int main() 
{
    int n;
    count[0] = 1;
    DP();
    while (cin >> n) 
    {
        cout << count[n] << endl;
    }   
    return 0;
}