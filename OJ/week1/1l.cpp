#include <iostream>
#include <sstream>
using namespace std;

int main() 
{
    string t, tt;
    int flag = 0;
    while (getline(cin, t)) 
    {
        stringstream s;
        s << t;
        while (s >> tt)
        {
            tt[0] = tt[0] - 32;
            if (!flag) 
            {
                flag++;
                cout << tt;
            }
            else 
            {
                cout << ' ' << tt;
            }
        }
        cout << endl;
        flag = 0;
    }
}