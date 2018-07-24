#include <iostream>
#include <vector>
#include <cmath>
using namespace std;


int qq(int t) 
{
    vector<int> a;
    a.push_back(1);
    int sum = 0;
    for (int i = 2; i <= sqrt(t); i++) 
    {
        if (! (t % i)) 
        {
            a.push_back(i);
        }
    }
    for (auto x : a) 
    {
        sum += x;
    }
    return sum;
}

int main() 
{
    int a, b, c;
    vector<int> tmp;
    int n, ret;
    cin >> n;
    while (n--) 
    {
        cin >> a >> b;
        ret = qq(a);
        if (ret != b) 
        {
            cout << "NO" << endl;
            continue;
        }
        ret = qq(b);
        if (ret != a) 
        {
            cout << "NO" << endl;
            continue;
        }
        cout << "YES" << endl;
    }
}