#include <iostream>
using namespace std;

int t(string s) 
{
    for (auto x : s) 
    {
        if(!(x >= '0' && x <= '9' || x >= 'a' && x <= 'z' || x >= 'A' && x <= 'Z' || x=='_'))
        {
            return 0;
        }
    }
    return 1;
}
int main() 
{
    string str;
    int n;
    int flag;
    cin >> n;
    while (n--) 
    {
        flag = 0;
        getline(cin, str);
        if (!(('a' <= str[0] && str[0] <= 'z') || ('A' <= str[0] && str[0] <= 'Z') || str[0] == '_')) 
        {
            flag = 0;
        }
        flag = t(str);
        if (flag) 
        {
            cout << "yes" << endl;
        }
        else 
        {
            cout << "no" << endl;
        }
    }
}