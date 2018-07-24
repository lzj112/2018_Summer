#include <iostream>
#include <vector>
using namespace std;

void tt(vector<int> tmp, int m) 
{
    int len = tmp.size();
    if (m < tmp[0]) 
    {
        cout << m << ' ';
    }
    for (int i = 0; i < len; i++) 
    {
        if (i == 0)
        {
            cout << tmp[i];
        }
        else 
        {
            cout << ' ' << tmp[i];
        }
        if (m >= tmp[i] && m < tmp[i+1]) 
        {
            cout << ' ' << m;
        }
    }
    if (m > tmp[len -1 ]) 
    {
        cout <<  ' ' << m << endl;
    }
    else 
    {
        cout << endl;
    }

}

int main() 
{

    int n ,m, t;
    vector<int> tmp;
    while (cin >> n >> m) 
    {
        if (n == 0 && m == 0) 
        {
            break;
        }
        while (n--) 
        {
            cin >> t;
            tmp.push_back(t);
        }
        tt(tmp, m);
        tmp.clear();
    }
}