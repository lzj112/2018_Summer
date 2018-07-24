#include <iostream>
using namespace std;

string tmp(string str) 
{
    int len = str.size() / 2;
    int j = str.size() - 1;
    int i = 0;
    while (len--) 
    {
        if (str[j--] != str[i++]) 
        {
            return string("no");
        }
    }
    return string("yes");
}

int main() 
{
    int n;
    string str;
    cin >> n;
    while (n--) 
    {
        cin >> str;
        cout << tmp(str) << endl;
    }

    return 0;
}
