#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "Epoll.h"
using namespace std;

const char* IP = "127.0.0.1";
const int PORT = 4096;
int main() 
{
    /*
    封装这段
    */
    sockaddr_in serAddress;
    memset(&serAddress, 0, sizeof(serAddress));
    serAddress.sin_family = AF_INET;
    serAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &serAddress.sin_addr);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    int optval = 1;     //设置该套接字可以重新绑定端口
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(int));

    int ret = bind(sockfd, (sockaddr*)&serAddress, sizeof(serAddress));
    assert(ret != -1);

    ret = listen(sockfd, 5);
    
    Epoll epollServer(sockfd);
    epollServer.epoll_Run();
}