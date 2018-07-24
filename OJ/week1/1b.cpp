#include <iostream>
#include <iomanip>
using namespace std;
const double PI = 3.1415926;

int main() 
{
    double r;
    
    while (cin >> r) 
    {
        cout << fixed << setprecision(3) << (PI * r * r * r * 4.0 / 3.0) << endl;
    }
}