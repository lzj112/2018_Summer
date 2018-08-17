#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
using namespace std;


const int maxn = 100;
int arr[maxn][maxn];
int DP[maxn][maxn];

int main()
{
    int c, n;
    cin >> c;
    while (c--)
    {
        cin >> n;
        memset(arr, 0, sizeof(arr));
        memset(DP, 0, sizeof(DP));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                cin >> arr[i][j];
            }
        }
        for (int i = n - 1; i >= 0; i--)
        {
            for (int j = 0; j <= i; j++)
            {
                if (i == n - 1)
                    DP[i][j] = arr[i][j]; 
                else
                {
                    DP[i][j] = DP[i+1][j] > DP[i+1][j+1] ? DP[i+1][j] : DP[i+1][j+1];
                    DP[i][j] += arr[i][j];
                }
            }
        }
        cout << DP[0][0] << endl;
    }
    return 0;
}