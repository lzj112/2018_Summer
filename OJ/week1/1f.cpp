#include <iostream>
#include <map>
// #include <>
using namespace std;

int main() 
{
    // map<char, int> t;
    int t;
    string str;
    int len, i, sum, n;
    cin >> n;
    while (n--)
    {
        while (cin >> str) 
        {
            t = 0;
            len = str.size();
            i = len - 1;
            while (len--) 
            {
                if (str[i] >= '0' && str[i] <= '9') 
                {
                    // t[str[i]]++;
                    t++;
                }
                i--;
            }
            cout << t << endl;
            // for (auto x : t) 
            // {
            
            //     sum += x.second;
            // }
            // cout << sum << endl;
            // sum = 0;
        }
    }
}
