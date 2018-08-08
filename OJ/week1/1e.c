// #include<iostream>
// #include <iomanip>
// using namespace std;
/*
double t(char c, int a, int b) 
{
    if (c == '+') 
    {
        return a + b;
    }
    else if (c == '-') 
    {
        return a - b;
    }
    else if (c == '*') 
    {
        return a * b;
    }
    else 
    {
        return (double)a / (double)b;
    }

}

int main() 
{
    int n;
    int a, b;
    char c;
    cin >> n;
    getchar();
    double tmp;
    while (n--) 
    {
        cin >> c >> a >> b;
        tmp = t(c, a, b);
        if ((tmp - (int)tmp) > 0 )
        {
            cout << fixed << setprecision(2) << t(c, a, b) << endl;
        }
        else 
        {
            cout << fixed << setprecision(0) << t(c, a, b) << endl;
        }
    }
}
*/
#include <stdio.h>
int main() 
{
    char c;
    int a, b, n;
    scanf("%d", &n);
    getchar();
    while (n--) 
    {
        scanf("%c%d%d", &c, &a, &b);
        getchar();
        switch(c)
        {
            case '+':printf("%d\n", a + b);break;
            case '-':printf("%d\n", a - b);break;
            case '*':printf("%d\n", a * b);break;
            case '/':
            if ((a % b) != 0) 
            {
                printf("%.2lf\n", (double)a / (double)b);
            }
            else 
            {
                printf("%d\n", a / b);
            }
        }
    }
}