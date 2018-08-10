#include <iostream>
#include <functional>
using namespace std;

struct a 
{
    function<void(int, int) > f;
};

class c 
{

public:
    void setFunc();
    void show(int l, int r);
};

void c::show(int l, int r = 10) 
{
    cout << l << ' ' << r << endl;
}

void c::setFunc() 
{
    a* aTmp = new a;
    aTmp->f = bind(&c::show, *this, placeholders::_1, placeholders::_2);
    aTmp->f(1, 2);
}

int main() 
{
    c cTmp;
    cTmp.setFunc();
}