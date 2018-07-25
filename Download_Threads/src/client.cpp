#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <cstring>
#include "downLoad.h"
using namespace std;

const int PORT = 4096;
const char* IP = "127.0.0.1";
int main() 
{
    sockaddr_in cliaddress;
    memset(&cliaddress, 0, sizeof(cliaddress));
    cliaddress.sin_family = AF_INET;
    cliaddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &cliaddress.sin_addr);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    int ret = connect(sockfd, (sockaddr*)&cliaddress, sizeof(cliaddress));
    assert(ret != -1);

    downLoad cli(sockfd);
    cli.run();

}