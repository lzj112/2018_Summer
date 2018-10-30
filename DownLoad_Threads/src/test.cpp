#include <sys/timerfd.h>

#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

class test 
{
public:
    test() : a(10){

    }
    void show() 
    {
        int* p = &a;
        cout << *p << endl;
        cout << a << endl;
    }
private:
    int a;

};
int main(int argc, char*argv[]) {
    test tmp;
    tmp.show();

    return 0;
}