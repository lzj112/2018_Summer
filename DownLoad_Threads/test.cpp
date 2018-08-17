#include <sys/timerfd.h>
#include <iostream>
#include <time.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
using namespace std;

const int MAXNUM = 20;

int main(int argc, char *argv[])
{
    struct itimerspec new_value;
    struct timespec now;
    uint64_t exp;
    ssize_t s;

    int ret = clock_gettime(CLOCK_REALTIME, &now);
    assert(ret != -1);

    // 创建一个 CLOCK_REALTIME 绝对计时器
    new_value.it_value.tv_sec = /*now.tv_sec + */3; //第一次到期的时间
    new_value.it_value.tv_nsec = now.tv_nsec; 

    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_nsec = 0;

    int timefd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK); // 构建了一个 相对时间 定时器
    assert(timefd != -1);

    ret = timerfd_settime(timefd, 0/*TFD_TIMER_ABSTIME*/, &new_value, NULL);//启动定时器
    assert(ret != -1);

    cout << "timer started" << endl; // 定时器开启啦！


    int epollfd = epoll_create(1);  //创建epoll实例对象

    struct epoll_event ev;
    struct epoll_event events[MAXNUM];
    ev.data.fd = timefd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, timefd, &ev); //添加到epoll事件集合

    for (; ;) 
    {
        int num = epoll_wait(epollfd, events, MAXNUM, 0);
        assert(num >= 0);

        for (int i = 0; i < num; i++) 
        {
            if (events[i].events & EPOLLIN) 
            {
                //....处理其他事件
                if (events[i].data.fd == timefd) 
                {
                    s = read(events[i].data.fd, &exp, sizeof(uint64_t)); // 也就是说，如果fd不是非阻塞的,那么程序会阻塞在这里
                    assert(s == sizeof(uint64_t));
                    cout << "here is timer" << endl;
                }
            }
        }
    }

    close(timefd);
    close(epollfd);

    return 0;
}
