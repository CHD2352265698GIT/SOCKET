#include <iostream>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
#define SERV_PORT 3333          // 定义端口号
#define BACK_LOG 10             // 请求队列中允许的最大请求数
#define MAX_DATA_SIZE 1024 * 10 // 数据长度
#define SERVER_IP "10.0.8.2"    // 服务器IP地址
string receive_data = "hello, client\r\n\r\n";
int main()
{
    struct sockaddr_in server_sockaddr, client_sockaddr;
    int sin_size, recvbytes;
    int sockfd, client_fd;                    // socket描述符
    char buf[MAX_DATA_SIZE];                  // 传输的数据
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    if (sockfd == -1)
    {
        cout << "socket error" << endl;
        return -1;
    }
    cout << "socket success" << endl;
    server_sockaddr.sin_family = AF_INET;        // 设置协议族
    server_sockaddr.sin_port = htons(SERV_PORT); // 设置端口号
    // server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 设置IP地址
    inet_aton(SERVER_IP, &server_sockaddr.sin_addr);
    bzero(&server_sockaddr.sin_zero, 8); // 清零
    if (bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        return -1;
    }
    if ((listen(sockfd, BACK_LOG)) == -1) // 监听
    {
        perror("listen");
        return -1;
    }
    cout << "listen ..." << endl;
    if ((client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, (socklen_t *)&sin_size)) == -1)
    {
        perror("accept");
        return -1;
    }
    printf("accept success\n");
    while (1)
    {
        if ((recvbytes = recv(client_fd, buf, MAX_DATA_SIZE, 0)) == -1)
        {
            printf("recv: error\n");
            // return -1;
        }
        printf("recv data is:' %s '", buf);
        bzero(buf, MAX_DATA_SIZE);
        if (send(client_fd, "\r\n\r\n200", 7, 0) == -1)
        {
            printf("send: error\n");
            // exit(-1);
        }
    }
    close(sockfd);
    return 0;
}